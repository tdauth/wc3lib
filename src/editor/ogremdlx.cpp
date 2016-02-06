/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <algorithm>

#include <QtCore>
#include <QtGui>

#include <KUrl>

#include <OgreCodec.h>

#include "ogremdlx.hpp"
#include "modelview.hpp"
#include "texture.hpp"
#include "mpqprioritylist.hpp"
#include "ogremdlxentity.hpp"

namespace wc3lib
{

namespace editor
{

void OgreMdlx::updateCamera(const mdlx::Camera &camera, Ogre::Camera *ogreCamera)
{
	ogreCamera->setPosition(ogreVector3(camera.position()));
	ogreCamera->setFOVy(Ogre::Radian(camera.fieldOfView()));
	ogreCamera->setFarClipDistance(camera.farClip());
	ogreCamera->setNearClipDistance(camera.nearClip());
	ogreCamera->setDirection(ogreVector3(camera.target()));
}

OgreMdlx::OgreMdlx(const KUrl &url, ModelView *modelView) : Resource(url, Resource::Type::Model), m_modelView(modelView), m_sceneNode(0), m_teamColor(TeamColor::Red), m_teamGlow(TeamColor::Red)
{
}

OgreMdlx::~OgreMdlx()
{
	clear();
}

void OgreMdlx::clear() throw ()
{
	m_mdlx.reset();

	if (m_sceneNode != 0)
	{
		destroySceneNode(this->m_sceneNode);
	}

	BOOST_FOREACH(Textures::reference value, m_textures)
	{
		if (value.first->replaceableId() == mdlx::ReplaceableId::None && !value.second.isNull()) // don't ever removed textures with replaceable ids since they're shared by the resource's source, if there had been an error on loading the texture, pointer could be null!
		{
			qDebug() << "Remove texture " << value.second->getName().c_str();
			this->m_modelView->root()->getTextureManager()->remove(value.second->getName());
		}
	}

	m_textures.clear();

	BOOST_FOREACH(CollisionShapes::reference value, m_collisionShapes)
	{
		delete value.second;
	}

	m_collisionShapes.clear();

	// TODO clear all
}

void OgreMdlx::setTeamColor(TeamColor teamColor)
{
	BOOST_FOREACH(TeamColorTextureUnitStates::reference state, this->m_teamColorTextureUnitStates)
	{
		try
		{
			if (!source()->sharedData()->teamColorTexture(teamColor)->hasOgreTexture())
			{
				source()->sharedData()->teamColorTexture(teamColor)->loadOgreTexture();
			}

			state->setTexture(source()->sharedData()->teamColorTexture(teamColor)->ogreTexture());
		}
		catch (Ogre::Exception &exception)
		{
			throw Exception(exception.what());
		}
	}

	this->m_teamColor = teamColor;
}

void OgreMdlx::setTeamGlow(TeamColor teamGlow)
{
	BOOST_FOREACH(TeamColorTextureUnitStates::reference state, this->m_teamGlowTextureUnitStates)
	{
		try
		{
			if (!source()->sharedData()->teamGlowTexture(teamGlow)->hasOgreTexture())
			{
				source()->sharedData()->teamGlowTexture(teamGlow)->loadOgreTexture();
			}

			state->setTexture(source()->sharedData()->teamGlowTexture(teamGlow)->ogreTexture());
		}
		catch (Ogre::Exception &exception)
		{
			throw Exception(exception.what());
		}
	}

	this->m_teamGlow = teamGlow;
}

void OgreMdlx::load()
{
	QString tmpFile;

	if (!source()->download(url(), tmpFile, modelView()))
	{
		throw Exception(boost::format(_("Unable to download file from URL \"%1%\".")) % url().toLocalFile().toUtf8().constData());
	}

	std::ios_base::openmode openmode = std::ios_base::in;
	const bool isMdx = boost::filesystem::path(tmpFile.toStdString()).extension() == ".mdx";

	if (isMdx)
	{
		openmode |= std::ios_base::binary;
	}

	ifstream ifstream(tmpFile.toStdString(), openmode);

	MdlxPtr model(new mdlx::Mdlx());
	bool result = false;

	if (isMdx)
	{
		this->source()->removeTempFile(tmpFile);

		throw Exception(_("MDX is currently not supported."));
	}
	else
	{
		mdlx::MdlGrammar grammar;
		result = grammar.parse(ifstream, *model);
	}

	this->source()->removeTempFile(tmpFile);

	if (result)
	{
		this->m_mdlx.swap(model);

		Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME); // TEST

		try
		{
			this->m_sceneNode = this->m_modelView->sceneManager()->getRootSceneNode()->createChildSceneNode(mdlx()->model().name());

			// NOTE if you enable this, make sure auto tracking is removed when scene node is deleted
			//this->modelView()->camera()->setAutoTracking(true, this->m_sceneNode); // camera follows ogre mdlx automatically

			// build nodes
			BOOST_FOREACH(mdlx::Mdlx::Attachments::const_reference ref, this->mdlx()->attachments())
			{
				m_nodes[ref.objectId()] = &ref;
			}


			BOOST_FOREACH(mdlx::Mdlx::Bones::const_reference ref, this->mdlx()->bones())
			{
				m_nodes[ref.objectId()] = &ref;
			}

			BOOST_FOREACH(mdlx::Mdlx::CollisionShapes::const_reference ref, this->mdlx()->collisionShapes())
			{
				m_nodes[ref.objectId()] = &ref;
			}

			BOOST_FOREACH(mdlx::Mdlx::Events::const_reference ref, this->mdlx()->events())
			{
				m_nodes[ref.objectId()] = &ref;
			}

			BOOST_FOREACH(mdlx::Mdlx::Helpers::const_reference ref, this->mdlx()->helpers())
			{
				m_nodes[ref.objectId()] = &ref;
			}

			BOOST_FOREACH(mdlx::Mdlx::Lights::const_reference ref, this->mdlx()->lights())
			{
				m_nodes[ref.objectId()] = &ref;
			}

			BOOST_FOREACH(mdlx::Mdlx::ParticleEmitter2s::const_reference ref, this->mdlx()->particleEmitter2s())
			{
				m_nodes[ref.objectId()] = &ref;
			}

			BOOST_FOREACH(mdlx::Mdlx::ParticleEmitters::const_reference ref, this->mdlx()->particleEmitters())
			{
				m_nodes[ref.objectId()] = &ref;
			}

			BOOST_FOREACH(mdlx::Mdlx::RibbonEmitters::const_reference ref, this->mdlx()->ribbonEmitters())
			{
				m_nodes[ref.objectId()] = &ref;
			}

			mdlx::long32 id = 0;

			BOOST_FOREACH(mdlx::Mdlx::GeosetAnimations::const_reference ref, this->mdlx()->geosetAnimations())
			{
				m_geosetAnimations[id] = &ref;
				++id;
			}

			this->createTextures();
			this->createMaterials();
			this->createGeosets();
			this->createCameras();
			this->createCollisionShapes();

			id = 0;

			BOOST_FOREACH(mdlx::Mdlx::Sequences::const_reference ref, this->mdlx()->sequences())
			{
				Ogre::Animation* animation = modelView()->sceneManager()->createAnimation(Ogre::String(ref.name()), Ogre::Real(ref.length()));
				animation->setInterpolationMode(Ogre::Animation::IM_LINEAR);

				//animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);

				m_sequences[&ref] = animation;
				m_sequenceIds[id] = &ref;

				++id;
			}

			/*
			* FIXME, fix root bone creation
			id = 0;

			BOOST_FOREACH(mdlx::Bones::Members::const_reference member, this->mdlx()->bones()->members())
			{
				const mdlx::Bone *bone = boost::polymorphic_cast<const mdlx::Bone*>(&member);
				this->createBone(*bone, id); // values are assigned to map in function due to recursive calls

				++id;
			}
			*/

			// WORKAROUND - each skeleton needs at least one root bone
			id = 0;
			qDebug() << "Before workaround";

			BOOST_FOREACH(Geosets::reference ref, this->m_geosets)
			{
				Ogre::Skeleton *skeleton = ref.second->getSkeleton().get();
				bool hasRootBone = false;

				if (skeleton->getNumBones() > 0)
				{
					Ogre::Skeleton::BoneIterator iterator = skeleton->getBoneIterator();


					while (iterator.hasMoreElements() && !hasRootBone)
					{
						Ogre::Bone *bone = iterator.getNext();

						if (bone == 0)
						{
							qDebug() << "empty bone in geoset " << id;
						}
						else
						{
							qDebug() << "Not empty bone!";
						}

						if (bone != 0 && bone->getParent() == 0)
						{
							hasRootBone = true;
						}
					}
				}

				/*
				if (!hasRootBone)
				{
					ref.second->getSkeleton()->createBone((boost::format("%1%.RootBone.%2%") % namePrefix().toStdString() % geosetName(id)).str().c_str());

					qDebug() << "Geoset " << id << " has no bones!";
				}

				qDebug() << "root bone: " << ref.second->getSkeleton()->getRootBone()->getName().c_str();

				++id;
				*/
			}

			qDebug() << "After workaround";
		}
		catch (Ogre::Exception &exception)
		{
			throw Exception(exception.what());
		}

		Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL); // TEST
		// get new objects
		/*
		std::list<const class Node*> nodes = this->m_mdlx->nodes();

		BOOST_FOREACH(const class Node *node, nodes)
		{
			if (this->m_nodes.find(node) != this->m_nodes.end())
				nodes.remove(node);
		}
		*/

		// setup nodes of new objects (inheritance)
		/*
		std::map<const class Node*, Ogre::Node*> newNodes(this->setupInheritance(nodes));

		BOOST_FOREACH(NodePairType nodePair, newNodes)
			this->m_nodes.insert(nodePair);
		*/


		// refresh model
		/*
		class Ogre::MeshManager *meshManager = Ogre::MeshManager::getSingletonPtr();

		if (this->m_mesh.isNull())
			this->m_mesh = meshManager->createManual(this->m_mdlx->model()->name(), "MDLX");

		//else if (this->m_mesh->
		/// @todo bounds radius, minimum and maximum extent are for Warcraft rendering only?
		this->m_mesh->_setBoundingSphereRadius(this->m_mdlx->model()->boundsRadius());
		this->m_mesh->_setBounds(Ogre::AxisAlignedBox(
			this->m_mdlx->model()->minimumExtent().x,
			this->m_mdlx->model()->minimumExtent().y,
			this->m_mdlx->model()->minimumExtent().z,
			this->m_mdlx->model()->maximumExtent().x,
			this->m_mdlx->model()->maximumExtent().y,
			this->m_mdlx->model()->maximumExtent().z
			)
		);
		*/

		// refresh sequences

		//* 	createAnimation (const String &name, Real length)


		// refresh global sequences
		// Global sequences are played for all sequences!
		/*
		std::size_t i = 0;

		BOOST_FOREACH(const class GlobalSequence *globalSequence, this->m_mdlx->globalSequences()->globalSequences())
		{
			if (this->m_nodes.find(static_cast<const class Object*>(globalSequence)) == this->m_nodes.end())
				this->m_nodes[globalSequence] = this->m_mesh->createAnimation(boost::str(boost::format(_("Global Sequence - %1%")) % i), globalSequence->duration());
		}
		*/

		// refresh geosets
		/// ...
		// std::size_t i = 0;

		//BOOST_FOREACH(const class Geoset *geoset, this->m_mdlx->geosets()->geosets())
		//{
		//	const class Geoset &rGeoset = *geoset;
		//	Ogre::SceneNode *node = sceneManager.createSceneNode();
		//	node->attachObject(this->createGeoset(rGeoset));
		//}
			/*
			std::map<const class Geoset*, Ogre::MeshPtr>::iterator iterator = this->m_geosets.find(geoset);
			Ogre::MeshPtr geosetMesh;
			std::string name;

			if (iterator == this->m_geosets.end())
			{
				name = boost::str(boost::format(_("Geoset - %1%")) % i);
				geosetMesh = meshManager->createManual(boost::str(boost::format(_("%1% - %2%")) % this->m_mdlx->model()->name() % name), "MDLX");
				this->m_geosets[geoset] = geosetMesh;
			}
			else
			{
				geosetMesh = iterator->second;
				name = geosetMesh->getName();
			}

			geosetMesh->_setBoundingSphereRadius(geoset->boundsRadius());
			geosetMesh->_setBounds(Ogre::AxisAlignedBox(
				geoset->minimumExtent().x,
				geoset->minimumExtent().y,
				geoset->minimumExtent().z,
				geoset->maximumExtent().x,
				geoset->maximumExtent().y,
				geoset->maximumExtent().z
				)
			);

			//Ogre::SubMesh *subMesh = this->m_mesh->createSubMesh(name.c_str());
			std::string verticesPoseName = boost::str(boost::format(_("%1% - Vertices Pose")) % name);
			class Ogre::Pose *verticesPose = geosetMesh->createPose(i + 1, verticesPoseName.c_str());
			std::size_t j = 0;

			BOOST_FOREACH(const class Vertex *vertex, geoset->vertices()->vertices())
			{
				verticesPose->addVertex(j, Ogre::Vector3(vertex->vertexData().x, vertex->vertexData().y, vertex->vertexData().z));
				++j;
			}

			std::string normalsPoseName = boost::str(boost::format(_("%1% - Normals Pose")) % name);
			class Ogre::Pose *normalsPose = geosetMesh->createPose(i + 1, normalsPoseName.c_str());
			j = 0;

			BOOST_FOREACH(const class Normal *normal, geoset->normals()->normals())
			{
				normalsPose->addVertex(j, Ogre::Vector3(normal->vertexData().x, normal->vertexData().y, normal->vertexData().z));
				++j;
			}

			// primitives data
			// vertex group data

			//subMesh->vertexData()
			//subMesh->vertexData()void 	setMaterialName (const String &matName)
			*/
			/*
			std::list<const class Bone*> geosetBones;

			// Bones
			BOOST_FOREACH(const class Bone *bone, this->m_mdlx->bones()->bones())
			{
				if (geoset == this->m_mdlx->boneGeoset(*bone))
				{
					geosetBones.push_back(bone);

					Ogre::Node *node = this->m_nodes[bone];

					// hasn't already been created by parent
					if (!bone->hasParent() || this->m_bones.find(bone) != this->m_bones.end())
					{
						this->m_bones[bone] = geosetMesh->getSkeleton()->createBone(bone->name());
					}
					// else, has already been created

					Ogre::Bone *ogreBone = this->m_bones[bone];

					std::list<const class Node*> children = this->m_mdlx->children(*bone);

					BOOST_FOREACH(const class Node *node, children)
					{
						const class Bone *childBone = static_cast<const class Bone*>(node);
						Ogre::Bone *ogreChildBone = geosetMesh->getSkeleton()->createBone(childBone->name());
						ogreBone->addChild(ogreChildBone);
					}
				}
			}
			*/

		//}

		// apply object inhertance modifiers with pivot points
		/// @todo Consider pivot points and the fact that this function is called for refreshment not initial placement.
		/*
		BOOST_FOREACH(NodePairType nodePair, this->m_nodes)
		{
			const class Node *node = nodePair.first;
			Ogre::Node *node = nodePair.second;
			long32 currentSequenceId = 0; /// @todo Set by animation?
			long32 currentFrame = 0; /// @todo Set by animation?

			// If the object has pivot point it uses its own orientation and not the parent's one.
			const class PivotPoint *pivotPoint = this->m_mdlx->objectPivotPoint(*object);

			// Ogre: Note that rotations are oriented around the node's origin.
			if (pivotPoint == 0)
			{
				node->setInheritOrientation(true);
			}
			else
			{
				node->setInheritOrientation(false);
				node->setOrientation(0.0, /// @todo Real w???, aren't same as pivot points?
					pivotPoint->x(),
					pivotPoint->y(),
					pivotPoint->z()
					);
			}

			if (object->hasParent())
			{
				if (object->inheritsTranslation() && (!object->translations()->hasGlobalSequence() || object->translations()->globalSequenceId() == currentSequenceId))
				{
					BOOST_FOREACH(const class MdlxTranslation *translation, object->translations()->mdlxTranslations())
					{
						if (translation->frame() == currentFrame)
						{
							switch (object->translations()->lineType())
							{
								case DontInterpolate:
									node->translate(translation->vertexData().x, translation->vertexData().y, translation->vertexData().z, Ogre::Node::TS_PARENT);

									break;

								case Linear:
									/// @todo FIXME

									break;

								case Hermite:
									/// @todo FIXME

									break;

								case Bezier:
									/// @todo FIXME

									break;
							}
						}
					}
				}

				if (object->inheritsRotation() && (!object->rotations()->hasGlobalSequence() || object->rotations()->globalSequenceId() == currentSequenceId))
				{
					BOOST_FOREACH(const class MdlxRotation *rotation, object->rotations()->mdlxRotations())
					{
						if (rotation->frame() == currentFrame)
						{
							switch (object->rotations()->lineType())
							{
								case DontInterpolate:
									node->rotate(Ogre::Quaternion(rotation->quaternionData().a, rotation->quaternionData().b, rotation->quaternionData().c, rotation->quaternionData().d), Ogre::Node::TS_PARENT);

									break;

								case Linear:
									/// @todo FIXME

									break;

								case Hermite:
									/// @todo FIXME

									break;

								case Bezier:
									/// @todo FIXME

									break;
							}
						}
					}
				}

				if (object->inheritsScaling() && (!object->scalings()->hasGlobalSequence() || object->scalings()->globalSequenceId() == currentSequenceId))
				{
					node->setInheritScale(true);

					BOOST_FOREACH(const class MdlxScaling *scaling, object->scalings()->mdlxScalings())
					{
						if (scaling->frame() == currentFrame)
						{
							switch (object->scalings()->lineType())
							{
								case DontInterpolate:
									node->scale(Ogre::Vector3(scaling->vertexData().x, scaling->vertexData().y, scaling->vertexData().z));

									break;

								case Linear:
									/// @todo FIXME

									break;

								case Hermite:
									/// @todo FIXME

									break;

								case Bezier:
									/// @todo FIXME

									break;
							}
						}
					}
				}
			}
		}
		*/
	}
}

void OgreMdlx::reload()
{
	clear();
	load();
}

void OgreMdlx::save(const KUrl &url, const QString &format) const
{
	QString realFormat = format;

	if (realFormat.isEmpty())
	{
		QString extension(QFileInfo(url.toLocalFile()).suffix().toLower());

		qDebug() << "Extension: " << QFileInfo(url.toLocalFile()).suffix().toLower();

		if (extension == "mdx")
		{
			realFormat = "mdx";
		}
		else if (extension == "mdl")
		{
			realFormat = "mdl";
		}
		else if (extension == "mesh")
		{
			realFormat = "mesh";
		}
		// default format
		else
		{
			realFormat = "mdx";
		}
	}

	if (realFormat == "mdx" || realFormat == "mdl")
	{
		QTemporaryFile tmpFile;

		if (!tmpFile.open()) // creates file
		{
			throw Exception(_("Unable to create temporary file."));
		}

		std::ios_base::openmode openmode = std::ios_base::out;
		bool isMdx;

		if (realFormat == "mdx")
		{
			isMdx = true;
			openmode |= std::ios_base::binary;
		}
		else
		{
			isMdx = false;
		}

		ofstream ofstream(tmpFile.fileName().toStdString(), openmode);

		if (!ofstream)
		{
			throw Exception(boost::format(_("Error when opening file \"%1%\".")) % tmpFile.fileName().toStdString());
		}

		bool result = false;

		if (isMdx)
		{
			throw Exception(_("MDX is not supported."));
		}
		else
		{
			mdlx::MdlGenerator generator;
			result = generator.generate(ofstream, *this->mdlx());
		}

		if (result)
		{
			if (!source()->upload(tmpFile.fileName(), url, modelView()))
			{
				throw Exception(boost::format(_("Error while uploading file \"%1%\" to destination \"%2%\".")) % tmpFile.fileName().toStdString() % url.toEncoded().constData());
			}

			QMessageBox::information(modelView(), QObject::tr("Done"), QObject::tr("Wrote %1 file \"%2\" successfully.").arg(isMdx ? QObject::tr("MDX") : QObject::tr("MDL")).arg(url.toEncoded().constData()));
		}
	}
	else if (realFormat == "mesh")
	{
		boost::scoped_ptr<Ogre::MeshSerializer> serializer(new Ogre::MeshSerializer());
		QList<KUrl> files;

		if (!source()->mkdir(url, modelView()))
		{
			throw Exception(boost::format(_("Unable to create directory \"%1%\"")) % url.toEncoded().constData());
		}

		BOOST_FOREACH(Geosets::const_reference value, m_geosets)
		{
			GeosetIds::left_map::const_iterator it = this->geosetIds().left.find(value.first);

			if (it == this->geosetIds().left.end())
				throw Exception();

			QTemporaryFile tmpFile;

			if (!tmpFile.open())
			{
				throw Exception();
			}

			ostringstream sstream;
			sstream << "Geoset" << it->second << ".mesh";

			serializer->exportMesh(value.second.get(), tmpFile.fileName().toStdString());

			const QString fileName(sstream.str().c_str());
			KUrl destination(url);
			destination.addPath(fileName);
			qDebug() << "File name: " << tmpFile.fileName();
			qDebug() << "destination: " << destination.toLocalFile();

			if (!source()->upload(tmpFile.fileName(), destination, modelView()))
			{
				throw Exception(boost::format(_("Error while uploading file \"%1%\" to destination \"%2%\".")) % tmpFile.fileName().toStdString() % destination.toEncoded().constData());
			}
		}

		QMessageBox::information(modelView(), QObject::tr("Done"), QObject::tr("Wrote MESH file \"%1\" successfully.").arg(url.toEncoded().constData()));
	}
	else
	{
		throw Exception(boost::format(_("Format \"%1%\" is not supported.")) % realFormat.toStdString());
	}

	/*
	TODO
	typedef std::pair<const mdlx::Geoset*, Ogre::ManualObject*> EntryType;


	BOOST_FOREACH(EntryType entry, this->m_geosets)
		;

	Ogre::MeshSerializer serializer = new MeshSerializer();
	*/


//	serializer.ExportMesh(Target.ConvertToMesh(MeshName), "Ucgen.mesh");
}

OgreMdlxEntity* OgreMdlx::createEntity(const Ogre::String &name)
{
	return new OgreMdlxEntity(name, this, this->modelView()->sceneManager());
}

Ogre::Node* OgreMdlx::createNode(const mdlx::Node &node)
{
	Ogre::Node *result = 0;

	if (node.type() & mdlx::Node::Type::Helper)
	{
		return 0;
	}

	if (node.type() & mdlx::Node::Type::Bone)
	{
		return 0;
	}

	if (node.type() & mdlx::Node::Type::Light)
	{
		return 0;
	}

	if (node.type() & mdlx::Node::Type::EventObject)
	{
		return 0;
	}

	if (node.type() & mdlx::Node::Type::Attachment)
	{
		return 0;
	}

	if (node.type() & mdlx::Node::Type::ParticleEmitter)
	{
		return 0;
	}

	if (node.type() & mdlx::Node::Type::CollisionShape)
	{
		return 0;
	}

	if (node.type() & mdlx::Node::Type::RibbonEmitter)
	{
		return 0;
	}

	return result;
}

void OgreMdlx::createNodeAnimatedProperties(const mdlx::Node &node, Ogre::Node *ogreNode)
{
	createAnimatedProperties(ogreNode, 0, node.translations());
	createAnimatedProperties(ogreNode, 1, node.scalings());
	createAnimatedProperties(ogreNode, 2, node.rotations());
}

QString OgreMdlx::namePrefix() const
{
	return this->mdlx()->model().name();
}

/*
bool OgreMdlx::useDirectoryUrl(KUrl &url, bool showMessage) const
{
	if (!QFileInfo(this->modelView()->editor()->findFile(url).toLocalFile()).exists())
	{
		KUrl newUrl = this->url().directory();
		newUrl.addPath(QFileInfo(url.toLocalFile()).filePath());

		if (showMessage)
			KMessageBox::information(this->modelView(), i18n("No valid texture resource was found at \"%1\". Trying directory URL \"%2\".", url.toLocalFile(), newUrl.toLocalFile()));

		qDebug() << i18n("No valid texture resource was found at \"%1\". Trying directory URL \"%2\".", url.toLocalFile(), newUrl.toLocalFile());
		url = newUrl;

		return true;
	}

	return false;
}
*/

void OgreMdlx::createTextures()
{
	// create textures
	mdlx::long32 id = 0;

	BOOST_FOREACH(mdlx::Mdlx::Textures::const_reference ref, this->mdlx()->textures())
	{
		this->m_textures[&ref] = this->createTexture(ref, id);
		this->m_textureIds[id] = &ref;
		++id;
	}
}

Ogre::TexturePtr OgreMdlx::createTexture(const mdlx::Texture &texture, mdlx::long32 id)
{
	//texture.parent()->members().size();
	//qDebug() << "First address " << this->mdlx()->textures() << " and second address " << texture.parent();
	//abort();

	QScopedPointer<Texture> textureResource;
	Ogre::Image *sourceImage = 0;

	if (texture.replaceableId() != mdlx::ReplaceableId::None)
	{
		KUrl url("ReplaceableTextures");

		switch (texture.replaceableId())
		{
			case mdlx::ReplaceableId::TeamColor:
			{
				try
				{
					if (!source()->sharedData()->teamColorTexture(teamColor())->hasOgreTexture())
					{
						source()->sharedData()->teamColorTexture(teamColor())->loadOgreTexture();
					}
				}
				catch (Exception &exception)
				{
					std::cerr << boost::format(_("Warning: Unable to load team color %1%")) % static_cast<mdlx::long32>(teamColor()) << std::endl;
				}

				// NOTE this is only required since createTexture() is called again in setTeamColor() and setTeamGlow()
				if (std::find(this->m_teamColorTextures.begin(), this->m_teamColorTextures.end(), id) == this->m_teamColorTextures.end())
					this->m_teamColorTextures.push_back(id);

				return source()->sharedData()->teamColorTexture(teamColor())->ogreTexture();
			}

			case mdlx::ReplaceableId::TeamGlow:
			{
				try
				{
					if (!source()->sharedData()->teamGlowTexture(teamGlow())->hasOgreTexture())
					{
						source()->sharedData()->teamGlowTexture(teamGlow())->loadOgreTexture();
					}
				}
				catch (Exception &exception)
				{
					std::cerr << boost::format(_("Warning: Unable to load team glow %1%")) % static_cast<int>(teamGlow()) << std::endl;
				}

				// NOTE this is only required since createTexture() is called again in setTeamColor() and setTeamGlow()
				if (std::find(this->m_teamGlowTextures.begin(), this->m_teamGlowTextures.end(), id) == this->m_teamGlowTextures.end())
				{
					this->m_teamGlowTextures.push_back(id);
				}

				return source()->sharedData()->teamGlowTexture(teamColor())->ogreTexture();
			}

			case mdlx::ReplaceableId::Cliff:
				url.addPath("Cliff/Cliff0.blp");

				break;

			case mdlx::ReplaceableId::LordaeronTree:
				url.addPath("LordaeronTree/LordaeronSummerTree.blp");

				break;

			case mdlx::ReplaceableId::AshenvaleTree:
				url.addPath("AshenvaleTree/AshenTree.blp");

				break;

			case mdlx::ReplaceableId::BarrensTree:
				url.addPath("BarrensTree/BarrensTree.blp");

				break;

			case mdlx::ReplaceableId::NorthrendTree:
				url.addPath("NorthrendTree/NorthTree.blp");

				break;

			case mdlx::ReplaceableId::MushroomTree:
				url.addPath("Mushroom/MushroomTree.blp");

				break;
		}
	}
	else
	{
		QString texturePath(texture.texturePath());
		// converts \ to / on Unix systems
#ifdef Q_OS_UNIX
		texturePath.replace('\\', '/');
#elif defined Q_OS_WIN32
		texturePath.replace('/', '\\');
#else
#warning Unsupported OS?
#endif
		textureResource.reset(new Texture(KUrl(texturePath))); // we need a resource to use MpqPriorityList on loading!!!
		textureResource->setSource(source());

		try
		{
			textureResource->loadOgre();
		}
		catch (Exception &exception) // NOTE catch all exceptions, not loading texture file shouldn't stop loading model!
		{
			std::cerr << boost::format(_("Warning: %1%")) % exception.what() << std::endl;
		}

		sourceImage = textureResource->ogre().data();
	}

	Ogre::TexturePtr tex =
	this->m_modelView->root()->getTextureManager()->create(textureName(id), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	if (sourceImage != 0)
	{
		tex->loadImage(*sourceImage);

		/// \todo Apply texture properties:
		switch (texture.wrapping())
		{
			case mdlx::Texture::Wrapping::WrapWidth:
			{
				QMessageBox::warning(this->modelView(), QObject::tr("Warning"), QObject::tr("Unsupported texture wrapping type:\nWrapWidth."));

				break;
			}

			case mdlx::Texture::Wrapping::WrapHeight:
			{
				QMessageBox::warning(this->modelView(), QObject::tr("Warning"), QObject::tr("Unsupported texture wrapping type:\nWrapHeight."));

				break;
			}

			case mdlx::Texture::Wrapping::Both:
			{
				QMessageBox::warning(this->modelView(), QObject::tr("Warning"), QObject::tr("Unsupported texture wrapping type:\nBoth."));

				break;
			}
		}
	}
	else
		qDebug() << "No source image data";

	return tex;
}

Ogre::TextureUnitState* OgreMdlx::createLayer(Ogre::Pass *pass, const mdlx::Layer &layer)
{
	const mdlx::Texture *texture = m_textureIds[layer.textureId()];

	Ogre::String textureName = this->textureName(layer.textureId());

	if (texture->replaceableId() == mdlx::ReplaceableId::TeamColor)
	{
		if (!this->source()->sharedData()->teamColorTexture(teamColor())->ogreTexture().isNull())
		{
			textureName = this->source()->sharedData()->teamColorTexture(teamColor())->ogreTexture()->getName();
		}
	}
	else if (texture->replaceableId() == mdlx::ReplaceableId::TeamGlow)
	{
		if (!this->source()->sharedData()->teamColorTexture(teamGlow())->ogreTexture().isNull())
		{
			textureName = this->source()->sharedData()->teamGlowTexture(teamGlow())->ogreTexture()->getName();
		}
	}

	Ogre::TextureUnitState *state = pass->createTextureUnitState(textureName, layer.coordinatesId());

	if (texture->replaceableId() == mdlx::ReplaceableId::TeamColor)
	{
		this->m_teamColorTextureUnitStates.push_back(state);
	}
	else if (texture->replaceableId() == mdlx::ReplaceableId::TeamGlow)
	{
		this->m_teamGlowTextureUnitStates.push_back(state);
	}

	//textureUnitState->setTextureFiltering();
	//textureUnitState->setAlphaOperation(Ogre::LBX_MODULATE_X4);
	//float32	alpha() const; TODO set alpha!
	//technique->setLightingEnabled(false); // default value

	switch (layer.filterMode())
	{
		case mdlx::Layer::FilterMode::Transparent:
			qDebug() << "Missing support for filter mode transparent!";

			break;

		case mdlx::Layer::FilterMode::Blend:
			state->setColourOperationEx(Ogre::LBX_BLEND_TEXTURE_ALPHA);

			break;

		case mdlx::Layer::FilterMode::Additive:
			state->setColourOperationEx(Ogre::LBX_ADD);

			break;

		case mdlx::Layer::FilterMode::AddAlpha:
			qDebug() << "Missing support for filter mode add alpha!";

			break;

		case mdlx::Layer::FilterMode::Modulate:
			state->setColourOperationEx(Ogre::LBX_MODULATE);

			break;

		case mdlx::Layer::FilterMode::Modulate2x:
			state->setColourOperationEx(Ogre::LBX_MODULATE_X2);

			break;
	}

	if (layer.shading() == mdlx::Layer::Shading::Unshaded)
	{
	}
	else
	{
		if (layer.shading() & mdlx::Layer::Shading::SphereEnvironmentMap)
			qDebug() << "Material: shading type \"SphereEnvironmentMap\" is not supported.";

		if (layer.shading() & mdlx::Layer::Shading::Unknown0)
			qDebug() << "Material: shading type \"Unknown0\" is not supported.";

		if (layer.shading() & mdlx::Layer::Shading::Unknown1)
			qDebug() << "Material: shading type \"Unknown1\" is not supported.";

		if (layer.shading() & mdlx::Layer::Shading::TwoSided)
			;
			//technique->setCullingMode(Ogre::CULL_NONE);

		if (layer.shading() & mdlx::Layer::Shading::Unfogged)
			qDebug() << "Material: shading type \"Unfogged\" is not supported.";

		if (layer.shading() & mdlx::Layer::Shading::NoDepthTest)
			qDebug() << "Material: shading type \"NoDepthTest\" is not supported.";

		if (layer.shading() & mdlx::Layer::Shading::NoDepthSet)
			qDebug() << "Material: shading type \"NoDepthSet\" is not supported.";
	}

	return state;
}

void OgreMdlx::createMaterials()
{
	// create materials
	mdlx::long32 id = 0;

	BOOST_FOREACH(mdlx::Mdlx::Materials::const_reference ref, this->mdlx()->materials())
	{
		this->m_materials[&ref] = this->createMaterial(ref, id);
		this->m_materialIds[id] = &ref;
		++id;
	}
}

Ogre::MaterialPtr OgreMdlx::createMaterial(const mdlx::Material &material, mdlx::long32 id)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create((boost::format("%1%.Material%2%") % namePrefix().toUtf8().constData() % id).str().c_str(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME); // material->mdlx()->model()->name()

	// properties
	/*
	ConstantColor,
	SortPrimsFarZ,
	FullResolution,
	PriorityPlane <long>,
	*/
	if (material.renderMode() & mdlx::Material::RenderMode::ConstantColor)
		;

	if (material.renderMode() & mdlx::Material::RenderMode::SortPrimitivesFarZ)
		;

	if (material.renderMode() & mdlx::Material::RenderMode::SortPrimitivesNearZ)
		;

	if (material.renderMode() & mdlx::Material::RenderMode::FullResolution)
		;

	//material->prio
	//mat->set

	// layers do have texture id!!!
	// use Ogre::Technique?

	mat->removeAllTechniques(); // there's always one default technique

	BOOST_FOREACH(mdlx::Material::Layers::const_reference ref, material.layers())
	{
		Ogre::Technique *technique = mat->createTechnique();
		technique->removeAllPasses(); // there shouldn't be any default pass, anyway, we want to make sure
		Ogre::Pass *pass = technique->createPass();
		//pass->setFog (bool overrideScene, FogMode mode=FOG_NON
		// TEST
		Ogre::Material::TechniqueIterator iterator = mat->getTechniqueIterator();
		int i = 0;

		while (iterator.hasMoreElements())
		{
			iterator.getNext();
			++i;
		}

		qDebug() << "We have " << i << " techniques.";
		// TEST END

		technique->setLightingEnabled(false); // default value
		createLayer(pass, ref);

		/*
		Texture animation stuff
		BOOST_FOREACH(const class mdlx::TextureId *textureId, layer->textureIds())
		{
			//textureId->frame()
		}
		*/
	}

	// TEST BLOCK
	/*
	static bool test = false;

	if (!test)
	{
		qDebug() << "Creating plane!!!!!!!!!!!!!!!!!!!!!!!";
		Ogre::Plane plane;
		plane.normal = Ogre::Vector3::UNIT_Y;
		plane.d = 0;
		this->m_modelView->root()->getMeshManager()->createPlane("floor", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 256.0f, 256.0f, 10, 10, true, 1, 50.0f, 50.0f, Ogre::Vector3::UNIT_Z);
		Ogre::Entity* planeEnt = this->m_modelView->sceneManager()->createEntity("plane", "floor");
		planeEnt->setMaterialName(boost::str(boost::format("%1%.Material0") % namePrefix().toStdString()).c_str());
		planeEnt->setCastShadows(false);
		this->m_sceneNode->attachObject(planeEnt);
		test = true;
	}
	*/
	// END TEST

	return mat;
}

void OgreMdlx::createGeosets()
{
	// create geosets
	mdlx::long32 id = 0;

	BOOST_FOREACH(mdlx::Mdlx::Geosets::const_reference ref, this->mdlx()->geosets())
	{
		Ogre::ManualObject *mo = this->createGeoset(ref, id);
		this->m_geosetIds.left.insert(std::make_pair(&ref, id));
		Ogre::MeshPtr mesh = mo->convertToMesh(Ogre::String(geosetName(id) + ".mesh"));
		this->m_geosets[&ref] = mesh;
		//mesh->load();

		// TODO one skeleton can be shared by multiple meshes
		Ogre::SkeletonPtr skeleton = this->createSkeleton(Ogre::String(mdlx()->model().name()) + "Skeleton" + geosetName(id));

		qDebug() << "Skeleton name: " << skeleton->getName().c_str();

		mesh->_notifySkeleton(skeleton);
		mesh->setSkeletonName(skeleton->getName());

		qDebug() << "Skeleton name 2: " << mesh->getSkeleton()->getName().c_str();
		skeleton->createBone((boost::format("%1%.%2%.RootBone") % namePrefix().toUtf8().constData() % geosetName(id)).str().c_str(), boost::numeric_cast<unsigned short>(id)); // we need one root bone to create all other bones

		/*
		mdlx::long32 seqId = 0;

		BOOST_FOREACH(mdlx::Sequences::Members::const_reference member, this->mdlx()->sequences()->members())
		{
			const Ogre::Animation *animation = skeleton->createAnimation(sequenceName(*geoset, *boost::polymorphic_cast<const mdlx::Sequence*>(&member)), Ogre::Real(boost::polymorphic_cast<const mdlx::Sequence*>(&member)->length()));
			// TODO assign animation to member
			++seqId;
		}
		*/

		// TODO attach or create entities
		//this->modelView()->sceneManager()->attachObject(mesh);
		//mesh->load(); // Notify -Mesh object that it has been loaded


		++id;
	}
}

Ogre::ManualObject* OgreMdlx::createGeoset(const mdlx::Geoset &geoset, mdlx::long32 id)
{
	qDebug() << "Creating geoset";
	Ogre::ManualObject *object = this->modelView()->sceneManager()->createManualObject(geosetName(id));
	//object->setKeepDeclarationOrder(true);
	qDebug() << "Creating geoset";

	// get corresponding material by id
	// TODO built up binary tree before

	const mdlx::Material *material = 0;

	MaterialIds::const_iterator iterator = m_materialIds.find(geoset.materialId());

	if (iterator == m_materialIds.end())
	{
		throw Exception(boost::format(_("Material %1% not found for geoset %2%")) % geoset.materialId() % id);
	}

	material = iterator->second;

	// FIXME negative underflow
	//object->setRenderQueueGroupAndPriority(object->getRenderQueueGroup(), boost::numeric_cast<Ogre::ushort>(material->priorityPlane())); // set priority by corresponding material -> TODO should be applied for material?

	// increase processor performance
	object->estimateVertexCount(geoset.vertices().size());
	object->estimateIndexCount(geoset.vertices().size());
	object->begin(this->m_materials[material]->getName(), Ogre::RenderOperation::OT_TRIANGLE_LIST);
	// build vertices
	mdlx::Geoset::Vertices::const_iterator vertexIterator = geoset.vertices().begin();
	mdlx::Geoset::Normals::const_iterator normalIterator = geoset.normals().begin();
	mdlx::Geoset::TextureVertices::const_iterator textureVertexIterator = geoset.textureVertices().begin();
	Ogre::uint32 index = 0;
	//qDebug() << "TVertices " << geoset.textureVertices()->textureVertices().size() << "{";
	//qDebug() << "Normals " << geoset.normals()->normals().size() << "(";
	//qDebug() << "Vertices " << geoset.vertices()->vertices().size() << "(";

	while (vertexIterator != geoset.vertices().end() && normalIterator != geoset.normals().end() &&  textureVertexIterator != geoset.textureVertices().end())
	{
		//qDebug() << "Adding vertex (" << (*vertexIterator)->vertexData().x << "|" << (*vertexIterator)->vertexData().y << "|" << (*vertexIterator)->vertexData().z << ")";
		//qDebug() << "Adding normal (" << (*normalIterator)->vertexData().x << "|" << (*normalIterator)->vertexData().y << "|" << (*normalIterator)->vertexData().z << ")";
		//qDebug() << "Adding texture coordinates (" << (*textureVertexIterator)->x() << "|" << (*textureVertexIterator)->y() << ")";
		const mdlx::VertexData &vertex = *vertexIterator;
		const mdlx::VertexData &normal = *normalIterator;
		const Vertex2d<float32> &textureVertex = *textureVertexIterator;
		object->position(ogreVector3(vertex));
		object->normal(ogreVector3(normal));
		object->textureCoord(ogreVector2(textureVertex));
		//object->colour(1.0 - this->modelView()->viewPort()->getBackgroundColour().r, 1.0 - this->modelView()->viewPort()->getBackgroundColour().g, 1.0 - this->modelView()->viewPort()->getBackgroundColour().b, 1.0 - this->modelView()->viewPort()->getBackgroundColour().a);
		//object->index(index);

		++vertexIterator;
		++normalIterator;
		++textureVertexIterator;
		++index;
	}

	qDebug() << "Built " << index << " vertices.";

	// build primitives
	mdlx::Geoset::Faces::const_iterator facesIterator = geoset.faces().begin();

	while (facesIterator != geoset.faces().end())
	{
		const mdlx::Faces &faces = *facesIterator;

		switch (faces.type())
		{
			case mdlx::Faces::Type::Triangles:
			{
				for (mdlx::long32 i = 0; i < faces.vertices().size(); i += 3)
				{
					Ogre::uint32 indices[3];

					for (mdlx::long32 size = 0; size < 3; ++size)
					{
						indices[size] = boost::numeric_cast<Ogre::uint32>(faces.vertices()[i + size]);
					}

					object->triangle(indices[0], indices[1], indices[2]);
				}

				qDebug() << "Built " << faces.vertices().size() << " triangles.";

				break;
			}
			default:
			{
				QMessageBox::warning(this->modelView(), QObject::tr("Warning"), QObject::tr("Unsupported primitive type:\n%1").arg(static_cast<mdlx::long32>(faces.type())));

				/// \todo build other primitives (other than triangles)
				for (mdlx::long32 i = 0; i < faces.vertices().size(); ++i)
				{
				}
			}

		}

		++facesIterator;
	}

	object->end();

	// set bounds
	/// \todo Set Bounding radius (hit test?)
	//object->setBoundingSphereRadius(geoset->boundsRadius());
	/*object->setBoundingBox(Ogre::AxisAlignedBox(
		geoset.minimumExtent().x,
		geoset.minimumExtent().y,
		geoset.minimumExtent().z,
		geoset.maximumExtent().x,
		geoset.maximumExtent().y,
		geoset.maximumExtent().z
		)
	);*/

	return object;
}

void OgreMdlx::createCameras()
{
	// create cameras
	mdlx::long32 id = 0;

	BOOST_FOREACH(mdlx::Mdlx::Cameras::const_reference ref, this->mdlx()->cameras())
	{
		this->m_cameras[&ref] = this->createCamera(ref, id);
		++id;
	}
}

Ogre::Camera* OgreMdlx::createCamera(const mdlx::Camera &camera, mdlx::long32 id)
{
	Ogre::Camera *ogreCamera = this->modelView()->sceneManager()->createCamera((boost::format("%1%.Camera.%2%") % namePrefix().toStdString() % camera.name()).str().c_str());

	updateCamera(camera, ogreCamera);

	this->m_sceneNode->attachObject(ogreCamera);

	return ogreCamera;
}

void OgreMdlx::createCollisionShapes()
{
	mdlx::long32 id = 0;

	BOOST_FOREACH(mdlx::Mdlx::CollisionShapes::const_reference ref, this->mdlx()->collisionShapes())
	{
		this->m_collisionShapes[&ref] = this->createCollisionShape(ref, id);
		++id;
	}
}

CollisionShape* OgreMdlx::createCollisionShape(const mdlx::CollisionShape &collisionShape, mdlx::long32 id)
{
	CollisionShape *cs = new CollisionShape();
	cs->shape = collisionShape.shape();

	if (collisionShape.shape() == mdlx::CollisionShape::Shape::Box)
	{
		cs->box = new Ogre::AxisAlignedBox(ogreVector3(collisionShape.vertices()[0]), ogreVector3(collisionShape.vertices()[1]));
	}
	// sphere
	else
	{
		cs->sphere = new Ogre::Sphere(ogreVector3(collisionShape.vertices()[0]), ogreReal(collisionShape.boundsRadius()));
	}

	return cs;
}


Ogre::SkeletonPtr OgreMdlx::createSkeleton(const Ogre::String &name)
{
	return Ogre::SkeletonManager::getSingleton().create(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}

Ogre::Bone* OgreMdlx::createBone(const mdlx::Bone &bone, mdlx::long32 id)
{
	Ogre::Bone *ogreBone = 0;

	Bones::iterator iterator = m_bones.find(&bone);

	if (iterator == m_bones.end())
	{
		bool createdChild = false;

		if (bone.hasParent())
		{
			const mdlx::Bone *parentBone = dynamic_cast<const mdlx::Bone*>(this->m_nodes[bone.parentId()]);

			if (parentBone != 0 && bone.geosetId() == parentBone->geosetId()) // TODO support different geoset ids
			{
				Ogre::Bone *parent = createBone(*parentBone, bone.parentId());

				if (parent != 0)
				{
					qDebug() << "create child with id " << id;
					ogreBone = parent->createChild(boost::numeric_cast<unsigned short>(id));
					//ogreBone->setName((boost::format("%1%.Bone.%2%") % namePrefix().toStdString() % bone.name()).str().c_str());
					//ogreBone->setInheritsTranslation(bone.inheritsTranslation());
					//ogreBone->setInheritsRotation(bone.inheritsRotation());
					//ogreBone->setInheritScale(bone.inheritsScaling());

					createdChild = true;
				}
			}
		}

		if (!createdChild)
		{
			qDebug() << "Geoset id " << bone.geosetId();

			if (bone.geosetId() != mdlx::noneId) // TODO support Multiple
			{
				GeosetIds::right_const_iterator iterator = this->geosetIds().right.find(bone.geosetId());

				if (iterator != this->geosetIds().right.end())
				{
					const mdlx::Geoset *geoset = boost::polymorphic_cast<const mdlx::Geoset*>(iterator->second);
					Geosets::const_iterator iterator = this->geosets().find(geoset);
					const Ogre::Mesh *mesh = iterator->second.get();
					qDebug() << "Mesh: " << mesh->getName().c_str();
					qDebug() << "Skeleton: " << mesh->getSkeleton()->getName().c_str();
					qDebug() << "Bone name: " << bone.name();

					// need at least 1 bone to get root bone
					//if (mesh->getSkeleton()->getNumBones() > 0 && mesh->getSkeleton()->getRootBone() != 0)
					//	throw Exception(boost::format(_("There's already a root bone in geoset %1%!")) % bone.geosetId());

					//ogreBone = mesh->getSkeleton()->createBone((boost::format("%1%.Bone.%2%") % namePrefix().toStdString() % bone.name()).str().c_str(), boost::numeric_cast<unsigned short>(id));
					ogreBone = mesh->getSkeleton()->getRootBone()->createChild(boost::numeric_cast<unsigned short>(id));
					qDebug() << "Created bone " << id;
				}
				else
				{
					qDebug() << "Didn't find geoset with id " << bone.geosetId() << "!";
				}
			}
			else
			{
				qDebug() << "Multiple geosets aren't supported yet!";
			}
		}

		if (bone.geosetAnimationId() != mdlx::noneId)
		{
			const mdlx::GeosetAnimation *geosetAnim = this->m_geosetAnimations[bone.geosetAnimationId()];
			qDebug() << "We have geoset animation " << bone.geosetAnimationId();
			//const Ogre::Animation *animation = ogreBone->createAnimation((boost::format("GeosetAnim%1%") % bone.geosetAnimId()).c_str());
		}

		if (ogreBone != 0)
		{
			m_bones[&bone] = ogreBone;
			createNodeAnimatedProperties(bone, ogreBone);
		}
	}
	else
	{
		ogreBone = iterator->second;
	}

	return ogreBone;
}

OgreMdlx::NodeInheritance OgreMdlx::setupInheritance(const std::list<const mdlx::Node*> &initialNodes)
{
	std::list<const mdlx::Node*> resultingNodes(initialNodes);
	OgreMdlx::NodeInheritance nodes;

	/*
	BOOST_FOREACH(const class Node *node, resultingNodes)
	{
		// is parent
		if (!node->hasParent())
		{
			nodes[node] = new Ogre::Node(node->name());
			resultingNodes.remove(node);

			continue;
		}

		bool contained = false;
		const class Node *parent = this->m_mdlx->objectNode(*node);

		BOOST_FOREACH(const class Node *node, resultingNodes)
		{
			if (node == parent)
			{
				contained = true;

				break;
			}
		}

		// create as child, parent is already in map!
		if (!contained)
		{
			nodes[node] = nodes[parent]->createChild(node->name());
			resultingNodes.remove(node);
		}
	}
	*/

	return nodes;
}

}

}
