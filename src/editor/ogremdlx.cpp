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

#include <QtCore>
#include <QtGui>

#include <KMessageBox>
#include <KLocale>
#include <KUrl>
#include <KTemporaryFile>

#include <OgreCodec.h>

#include "ogremdlx.hpp"
#include "modelview.hpp"
#include "texture.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

void OgreMdlx::updateCamera(const class mdlx::Camera &camera, Ogre::Camera *ogreCamera)
{
	ogreCamera->setPosition(ogreVector3(camera.position()));
	ogreCamera->setFOVy(Ogre::Radian(camera.fieldOfView()));
	ogreCamera->setFarClipDistance(camera.farClip());
	ogreCamera->setNearClipDistance(camera.nearClip());
	ogreCamera->setDirection(ogreVector3(camera.target()));
}

OgreMdlx::OgreMdlx(const KUrl &url, class ModelView *modelView) : Resource(url, Resource::Type::Model), m_modelView(modelView), m_sceneNode(0), m_teamColor(TeamColor::Red), m_teamGlow(TeamColor::Red)
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
		Ogre::SceneNode::ObjectIterator iterator = this->m_sceneNode->getAttachedObjectIterator();

		while (iterator.hasMoreElements())
		{
			Ogre::MovableObject *movableObject = iterator.current()->second;
			this->modelView()->sceneManager()->destroyMovableObject(movableObject);
			//this->m_sceneNode->detachObject(movableObject);
			//delete movableObject;

			iterator.getNext();
		}

		this->modelView()->sceneManager()->destroySceneNode(m_sceneNode);
		//delete m_sceneNode;
	}

	BOOST_FOREACH(CollisionShapes::reference value, m_collisionShapes)
		delete value.second;
}

void OgreMdlx::setTeamColor(BOOST_SCOPED_ENUM(TeamColor) teamColor) throw (Exception)
{
	if (!source()->teamColorTexture(teamColor)->hasOgre())
		source()->teamColorTexture(teamColor)->loadOgre();

	BOOST_FOREACH(Ogre::TexturePtr tex, this->m_teamColorTextures)
	{
		try
		{
			tex->loadImage(*source()->teamColorTexture(teamColor)->ogre().data());
		}
		catch (Ogre::Exception &exception)
		{
			throw Exception(exception.what());
		}
	}

	this->m_teamColor = teamColor; // exception safety
}

void OgreMdlx::setTeamGlow(BOOST_SCOPED_ENUM(TeamColor) teamGlow) throw (Exception)
{
	if (!source()->teamGlowTexture(teamGlow)->hasOgre())
		source()->teamColorTexture(teamGlow)->loadOgre();

	BOOST_FOREACH(Ogre::TexturePtr tex, this->m_teamGlowTextures)
	{
		try
		{
			tex->loadImage(*source()->teamGlowTexture(teamGlow)->ogre().data());
		}
		catch (Ogre::Exception &exception)
		{
			throw Exception(exception.what());
		}
	}

	this->m_teamGlow = teamGlow; // exception safety
}

void OgreMdlx::load() throw (Exception)
{
	QString tmpFile;

	if (source()->download(url(), tmpFile, modelView()))
		return;

	std::ios_base::openmode openmode = std::ios_base::in;
	bool isMdx = boost::filesystem::path(tmpFile.toUtf8().constData()).extension() == ".mdx";

	if (isMdx)
		openmode |= std::ios_base::binary;

	ifstream ifstream(tmpFile.toUtf8().constData(), openmode);

	MdlxPtr model(new mdlx::Mdlx());

	std::streamsize size = isMdx ? size = model->readMdx(ifstream) : size = model->readMdl(ifstream);

	//model->textures()->textures().size(); // TEST
	qDebug() << "Size: " << size;

	this->mdlx()->textures()->members().size(); // TEST
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME); // TEST

	try
	{
		this->m_sceneNode = this->m_modelView->sceneManager()->getRootSceneNode()->createChildSceneNode(mdlx()->model()->name());

		this->modelView()->camera()->setAutoTracking(true, this->m_sceneNode); // camera follows ogre mdlx automatically

		// create textures
		mdlx::long32 id = 0;

		BOOST_FOREACH(mdlx::Textures::Members::const_reference member, this->mdlx()->textures()->members())
		{
			const mdlx::Texture *texture = boost::polymorphic_cast<const mdlx::Texture*>(&member);
			this->m_textures[texture] = this->createTexture(*texture, id);
			++id;
		}

		// create materials
		id = 0;

		BOOST_FOREACH(mdlx::Materials::Members::const_reference member, this->mdlx()->materials()->members())
		{
			const mdlx::Material *material = boost::polymorphic_cast<const mdlx::Material*>(&member);
			this->m_materials[material] = this->createMaterial(*material, id);
			++id;
		}

		// create geosets
		id = 0;

		BOOST_FOREACH(mdlx::Geosets::Members::const_reference member, this->mdlx()->geosets()->members())
		{
			const mdlx::Geoset *geoset = boost::polymorphic_cast<const mdlx::Geoset*>(&member);
			this->m_geosets[geoset] = this->createGeoset(*geoset, id);
			++id;
		}

		// create cameras
		id = 0;

		BOOST_FOREACH(mdlx::Cameras::Members::const_reference member, this->mdlx()->cameras()->members())
		{
			const mdlx::Camera *camera = boost::polymorphic_cast<const mdlx::Camera*>(&member);
			this->m_cameras[camera] = this->createCamera(*camera, id);
			++id;
		}

		id = 0;

		BOOST_FOREACH(mdlx::CollisionShapes::Members::const_reference member, this->mdlx()->collisionShapes()->members())
		{
			const mdlx::CollisionShape *collisionShape = boost::polymorphic_cast<const mdlx::CollisionShape*>(&member);
			this->m_collisionShapes[collisionShape] = this->createCollisionShape(*collisionShape, id);
			++id;
		}
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

void OgreMdlx::reload() throw (Exception)
{
	clear();
	load();
}


void OgreMdlx::save(const KUrl &url, const QString &format) const throw (class Exception)
{
	QString realFormat = format;

	if (realFormat.isEmpty())
	{
		QString extension(QFileInfo(realFormat).suffix().toLower());

		if (extension == "mdx")
			realFormat = "mdx";
		else if (extension == "mdl")
			realFormat = "mdl";
		// default format
		else
			realFormat = "mdx";
	}

	KTemporaryFile tmpFile;

	if (realFormat == "mdx" || realFormat == "mdl")
	{
		std::ios_base::openmode openmode = std::ios_base::out;
		bool isMdx;

		if (realFormat == "mdx")
		{
			isMdx = true;
			openmode |= std::ios_base::binary;
		}
		else
			isMdx = false;

		ofstream ofstream(tmpFile.fileName().toUtf8().constData(), openmode);

		if (!ofstream)
			throw Exception(boost::format(_("Error when opening file \"%1%\".")) % tmpFile.fileName().toUtf8().constData());

		std::streamsize size;

		if (isMdx)
			size = mdlx()->writeMdx(ofstream);
		else
			size = mdlx()->writeMdl(ofstream);

		KMessageBox::information(modelView(), i18n("Wrote %1 file \"%2\" successfully.\nSize: %3.", isMdx ? i18n("MDX") : i18n("MDL"), tmpFile.fileName(), sizeStringBinary(size).c_str()));
	}
	else if (realFormat == "mesh")
	{
		boost::scoped_ptr<Ogre::MeshSerializer> serializer(new Ogre::MeshSerializer());
		QList<KUrl> files;

		BOOST_FOREACH(Geosets::const_reference value, m_geosets)
		{
			Ogre::ManualObject *manualObject = value.second;
			std::ostringstream sstream;
			sstream << manualObject->getName();
			sstream << "Mesh";
			const Ogre::MeshPtr mesh = manualObject->convertToMesh(sstream.str());

			const QFileInfo info(tmpFile.fileName());
			KUrl geosetUrl(info.absoluteDir().absolutePath());
			QString fileName(info.baseName() + sstream.str().c_str());
			fileName += '.' + info.completeSuffix();
			geosetUrl.addPath(fileName);
			qDebug() << "Geoset URL: " << geosetUrl.toLocalFile();
			serializer->exportMesh(mesh.getPointer(), geosetUrl.toLocalFile().toUtf8().constData());
			KUrl destination(url);
			destination.addPath(fileName);

			if (!source()->upload(geosetUrl.toLocalFile(), destination, modelView()))
				throw Exception(boost::format(_("Error while uploading file \"%1%\" to destination \"%2%\".")) % geosetUrl.toEncoded().constData() % destination.toEncoded().constData());
		}

		return;
	}
	else
		throw Exception(boost::format(_("Format \"%1%\" is not supported.")) % realFormat.toUtf8().constData());


	if (!source()->upload(tmpFile.fileName(), url, modelView()))
		throw Exception(boost::format(_("Error while uploading file \"%1%\" to destination \"%2%\".")) % tmpFile.fileName().toUtf8().constData() % url.toEncoded().constData());

	/*
	TODO
	typedef std::pair<const mdlx::Geoset*, Ogre::ManualObject*> EntryType;


	BOOST_FOREACH(EntryType entry, this->m_geosets)
		;

	Ogre::MeshSerializer serializer = new MeshSerializer();
	*/


//	serializer.ExportMesh(Target.ConvertToMesh(MeshName), "Ucgen.mesh");
}

Ogre::Node* OgreMdlx::createNode(const class mdlx::Node &node)
{
	Ogre::Node *result = 0;

	if (node.type() & mdlx::Node::Type::Helper)
		return 0;

	if (node.type() & mdlx::Node::Type::Bone)
		return 0;

	if (node.type() & mdlx::Node::Type::Light)
		return 0;

	if (node.type() & mdlx::Node::Type::EventObject)
		return 0;

	if (node.type() & mdlx::Node::Type::Attachment)
		return 0;

	if (node.type() & mdlx::Node::Type::ParticleEmitter)
		return 0;

	if (node.type() & mdlx::Node::Type::CollisionShape)
		return 0;

	if (node.type() & mdlx::Node::Type::RibbonEmitter)
		return 0;

	return result;
}

QString OgreMdlx::namePrefix() const
{
	return this->m_mdlx->model()->name();
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

Ogre::TexturePtr OgreMdlx::createTexture(const class mdlx::Texture &texture, mdlx::long32 id) throw (class Exception)
{
	//texture.parent()->members().size();
	//qDebug() << "First address " << this->mdlx()->textures() << " and second address " << texture.parent();
	//abort();
	Ogre::TexturePtr tex =
	this->m_modelView->root()->getTextureManager()->create((boost::format("%1%.Texture%2%") % namePrefix().toUtf8().constData() % id).str().c_str(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::Image *sourceImage = 0;
	boost::scoped_ptr<Texture> textureResource;

	if (texture.replaceableId() != mdlx::ReplaceableId::None)
	{
		KUrl url("ReplaceableTextures");

		switch (texture.replaceableId())
		{
			case mdlx::ReplaceableId::TeamColor:
			{
				sourceImage = source()->teamColorTexture(teamColor())->ogre().data();
				this->m_teamColorTextures.push_back(tex);

				break;
			}

			case mdlx::ReplaceableId::TeamGlow:
			{
				sourceImage = source()->teamGlowTexture(teamColor())->ogre().data();
				this->m_teamGlowTextures.push_back(tex);

				break;
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
		KUrl url(texturePath);
		textureResource.reset(new Texture(url));
		textureResource->setSource(source());
		textureResource->loadOgre();
		sourceImage = textureResource->ogre().data();
	}

	if (sourceImage != 0)
	{
		tex->loadImage(*sourceImage);

		/// \todo Apply texture properties:
		switch (texture.wrapping())
		{
			case mdlx::Texture::Wrapping::WrapWidth:
				KMessageBox::error(this->modelView(), i18n("Unsupported texture wrapping type:\nWrapWidth."));

				break;

			case mdlx::Texture::Wrapping::WrapHeight:
				KMessageBox::error(this->modelView(), i18n("Unsupported texture wrapping type:\nWrapHeight."));

				break;

			case mdlx::Texture::Wrapping::Both:
				KMessageBox::error(this->modelView(), i18n("Unsupported texture wrapping type:\nBoth."));

				break;
		}
	}

	return tex;
}

Ogre::MaterialPtr OgreMdlx::createMaterial(const class mdlx::Material &material, mdlx::long32 id) throw (class Exception)
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

	BOOST_FOREACH(mdlx::Layers::Members::const_reference member, material.layers()->members())
	{
		const mdlx::Layer &layer = dynamic_cast<const mdlx::Layer&>(member);
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

		Ogre::TextureUnitState *textureUnitState = pass->createTextureUnitState((boost::format("%1%.Texture%2%") % namePrefix().toUtf8().constData() % layer.textureId()).str().c_str(), layer.coordinatesId());

		//textureUnitState->setTextureFiltering();
		//textureUnitState->setAlphaOperation(Ogre::LBX_MODULATE_X4);
		//float32	alpha() const; TODO set alpha!
		technique->setLightingEnabled(false); // default value

		switch (layer.filterMode())
		{
			case mdlx::Layer::FilterMode::Transparent:
				pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
				//textureUnitState->setIsAlpha(true);

				break;

			case mdlx::Layer::FilterMode::Blend:
				pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
				// TODO FIXME
				//pass->setSceneBlending(Ogre::SBT_TRANSPARENT_COLOUR);
				//textureUnitState->setColourOperation(Ogre::LBO_ALPHA_BLEND);

				break;

			case mdlx::Layer::FilterMode::Additive:
				pass->setSceneBlending(Ogre::SBT_ADD);
				//textureUnitState->setColourOperation(Ogre::LBO_ADD);

				break;

			case mdlx::Layer::FilterMode::AddAlpha:
				//pass->setSceneBlending(Ogre::SceneBlendType::SBT);
				break;

			case mdlx::Layer::FilterMode::Modulate:
				pass->setSceneBlending(Ogre::SBT_MODULATE);
				//textureUnitState->setColourOperation(Ogre::LBO_MODULATE);

				break;

			case mdlx::Layer::FilterMode::Modulate2x:
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
				technique->setCullingMode(Ogre::CULL_NONE);

			if (layer.shading() & mdlx::Layer::Shading::Unfogged)
				qDebug() << "Material: shading type \"Unfogged\" is not supported.";

			if (layer.shading() & mdlx::Layer::Shading::NoDepthTest)
				qDebug() << "Material: shading type \"NoDepthTest\" is not supported.";

			if (layer.shading() & mdlx::Layer::Shading::NoDepthSet)
				qDebug() << "Material: shading type \"NoDepthSet\" is not supported.";
		}

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
		planeEnt->setMaterialName(boost::str(boost::format("%1%.Material0") % namePrefix().toUtf8().constData()).c_str());
		planeEnt->setCastShadows(false);
		this->m_sceneNode->attachObject(planeEnt);
		test = true;
	}
	*/
	// END TEST

	return mat;
}

Ogre::ManualObject* OgreMdlx::createGeoset(const class mdlx::Geoset &geoset, mdlx::long32 id) throw (class Exception)
{
	qDebug() << "Creating geoset";
	Ogre::ManualObject *object = this->modelView()->sceneManager()->createManualObject((boost::format("%1%.Geoset%2%") % namePrefix().toUtf8().constData() % id).str().c_str());
	//object->setKeepDeclarationOrder(true);
	qDebug() << "Creating geoset";

	// get corresponding material by id

	const class mdlx::Material *material = 0;
	mdlx::long32 materialId = 0;

	BOOST_FOREACH(mdlx::Materials::Members::const_reference member, this->m_mdlx->materials()->members())
	{
		if (materialId == geoset.materialId())
		{
			material = boost::polymorphic_cast<const mdlx::Material*>(&member);

			break;
		}

		++materialId;
	}

	if (materialId != geoset.materialId())
		throw Exception(boost::format(_("Missing material %1%")) % id);


	object->setRenderQueueGroupAndPriority(object->getRenderQueueGroup(), boost::numeric_cast<Ogre::ushort>(material->priorityPlane())); // set priority by corresponding material -> TODO should be applied for material?

	// increase processor performance
	object->estimateVertexCount(geoset.vertices()->members().size());
	object->estimateIndexCount(geoset.vertices()->members().size());
	object->begin(this->m_materials[material]->getName(), Ogre::RenderOperation::OT_TRIANGLE_LIST);
	// build vertices
	mdlx::Vertices::Members::const_iterator vertexIterator = geoset.vertices()->members().begin();
	mdlx::Normals::Members::const_iterator normalIterator = geoset.normals()->members().begin();
	mdlx::TextureVertices::Members::const_iterator textureVertexIterator = geoset.textureVertices()->members().begin();
	Ogre::uint32 index = 0;
	//qDebug() << "TVertices " << geoset.textureVertices()->textureVertices().size() << "{";
	//qDebug() << "Normals " << geoset.normals()->normals().size() << "(";
	//qDebug() << "Vertices " << geoset.vertices()->vertices().size() << "(";

	while (vertexIterator != geoset.vertices()->members().end())
	{
		//qDebug() << "Adding vertex (" << (*vertexIterator)->vertexData().x << "|" << (*vertexIterator)->vertexData().y << "|" << (*vertexIterator)->vertexData().z << ")";
		//qDebug() << "Adding normal (" << (*normalIterator)->vertexData().x << "|" << (*normalIterator)->vertexData().y << "|" << (*normalIterator)->vertexData().z << ")";
		//qDebug() << "Adding texture coordinates (" << (*textureVertexIterator)->x() << "|" << (*textureVertexIterator)->y() << ")";
		const mdlx::Vertex &vertex = dynamic_cast<const mdlx::Vertex&>(*vertexIterator);
		const mdlx::Normal &normal = dynamic_cast<const mdlx::Normal&>(*normalIterator);
		const mdlx::TextureVertex &textureVertex = dynamic_cast<const mdlx::TextureVertex&>(*textureVertexIterator);
		object->position(ogreVector3(vertex.vertexData()));
		object->normal(ogreVector3(normal.vertexData()));
		object->textureCoord(ogreVector2(textureVertex));
		//object->colour(1.0 - this->modelView()->viewPort()->getBackgroundColour().r, 1.0 - this->modelView()->viewPort()->getBackgroundColour().g, 1.0 - this->modelView()->viewPort()->getBackgroundColour().b, 1.0 - this->modelView()->viewPort()->getBackgroundColour().a);
		//object->index(index);

		++vertexIterator;
		++normalIterator;
		++textureVertexIterator;
		++index;
	}

	qDebug() << "Built " << index << " vertices.";

	if (geoset.primitiveTypes()->members().size() != geoset.primitiveSizes()->members().size())
		throw Exception((boost::format(_("Primitives error: Types size (%1%) and sizes size (%2%) are not equal.")) % geoset.primitiveTypes()->members().size()) % geoset.primitiveSizes()->members().size());

	// build primitives
	mdlx::PrimitiveTypes::Members::const_iterator pTypeIterator = geoset.primitiveTypes()->members().begin();
	mdlx::PrimitiveSizes::Members::const_iterator pSizeIterator = geoset.primitiveSizes()->members().begin();
	mdlx::PrimitiveVertices::Members::const_iterator pVertexIterator = geoset.primitiveVertices()->members().begin();

	while (pTypeIterator != geoset.primitiveTypes()->members().end())
	{
		const mdlx::PrimitiveType &type = dynamic_cast<const mdlx::PrimitiveType&>(*pTypeIterator);
		const mdlx::PrimitiveSize &size = dynamic_cast<const mdlx::PrimitiveSize&>(*pSizeIterator);

		if (type.type() == mdlx::PrimitiveType::Type::Triangles)
		{
			for (mdlx::long32 i = 0; i < size.value(); i += 3)
			{
				Ogre::uint32 indices[3];

				for (mdlx::long32 size = 0; size < 3; ++size)
				{
					const mdlx::PrimitiveVertex &vertex = dynamic_cast<const mdlx::PrimitiveVertex&>(*pVertexIterator);
					indices[size] = boost::numeric_cast<Ogre::uint32>(vertex.value());
					++pVertexIterator;
				}

				object->triangle(indices[0], indices[1], indices[2]);
			}
		}
		else
		{
			KMessageBox::error(this->modelView(), i18n("Unsupported primitive type:\n%1", type.type()));

			/// \todo build other primitives (other than triangles)
			for (mdlx::long32 i = 0; i < size.value(); ++i)
				++pVertexIterator; /// \todo triangles have 3 vertices, how much?
		}

		++pTypeIterator;
		++pSizeIterator;
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

	this->m_sceneNode->attachObject(object);

	return object;
}

Ogre::Camera* OgreMdlx::createCamera(const class mdlx::Camera &camera, mdlx::long32 id) throw (class Exception)
{
	Ogre::Camera *ogreCamera = this->modelView()->sceneManager()->createCamera((boost::format("%1%.Camera.%2%") % namePrefix().toUtf8().constData() % camera.name()).str().c_str());

	updateCamera(camera, ogreCamera);

	this->m_sceneNode->attachObject(ogreCamera);

	return ogreCamera;
}

OgreMdlx::CollisionShape* OgreMdlx::createCollisionShape(const class mdlx::CollisionShape &collisionShape, mdlx::long32 id) throw (class Exception)
{
	CollisionShape *cs = new CollisionShape();
	cs->shape = collisionShape.shape();

	if (collisionShape.shape() == mdlx::CollisionShape::Shape::Box)
		cs->box = new Ogre::AxisAlignedBox(ogreVector3(collisionShape.vertexData()), ogreVector3(collisionShape.boxVertexData()));
	// sphere
	else
		cs->sphere = new Ogre::Sphere(ogreVector3(collisionShape.vertexData()), ogreReal(collisionShape.boundsRadius()));

	return cs;
}

std::map<const class mdlx::Node*, Ogre::Node*> OgreMdlx::setupInheritance(const std::list<const class mdlx::Node*> &initialNodes)
{
	std::list<const class mdlx::Node*> resultingNodes(initialNodes);
	std::map<const class mdlx::Node*, Ogre::Node*> nodes;

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
