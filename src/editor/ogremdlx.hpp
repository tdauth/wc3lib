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

#ifndef WC3LIB_EDITOR_OGREMDLX_HPP
#define WC3LIB_EDITOR_OGREMDLX_HPP

#include <map>
#include <list>

#include <Ogre.h>

#include "resource.hpp"
#include "../mdlx.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace editor
{

class ModelView;

/**
 * This class can be used to display MDLX models by using the OGRE 3d rendering engine.
 * It maintains a single mesh instance in a scene which contains all converted data of the original
 * MDLX model.
 * Geosets are implemented as sub meshes.
 * Each MDLX instance can have its own team color and glow which is required for proper unit displays and model testings.
 * \todo Use inherited event functions of frame listener to apply animation track data (each model instance should have its own time marker for sequences).
 */
class OgreMdlx : public Resource, public Ogre::FrameListener
{
	public:
		typedef std::map<const class mdlx::Texture*, Ogre::TexturePtr> Textures;
		typedef std::map<const class mdlx::Material*, Ogre::MaterialPtr> Materials;
		typedef std::map<const class mdlx::Geoset*, Ogre::ManualObject*> Geosets;
		typedef std::map<const class mdlx::Geoset*, mdlx::long32> GeosetIds;
		typedef std::map<const class mdlx::Geoset*, Ogre::Mesh*> GeosetMeshes;
		typedef std::map<const class mdlx::Camera*, Ogre::Camera*> Cameras;
		typedef std::map<const class mdlx::Sequence*, Ogre::Animation*> Sequences;

		/**
		 * This structure is required for model's collision shapes which either can be boxes or spheres.
		 * One instance should be created per collision shape of the corresponding \ref mdlx::Mdlx instance.
		 */
		struct CollisionShape
		{
			CollisionShape() : box(0), shape(mdlx::CollisionShape::Shape::Box)
			{
			}

			~CollisionShape()
			{
				if (shape == mdlx::CollisionShape::Shape::Box)
					delete box;
				else
					delete sphere;
			}

			union
			{
				Ogre::AxisAlignedBox *box;
				Ogre::Sphere *sphere;
			};

			BOOST_SCOPED_ENUM(mdlx::CollisionShape::Shape) shape;
		};

		typedef std::map<const mdlx::CollisionShape*, struct CollisionShape*> CollisionShapes;
		typedef std::map<const mdlx::Bone*, Ogre::Bone*> Bones;
		typedef boost::scoped_ptr<mdlx::Mdlx> MdlxPtr;

		/**
		 * Changes camera's \p ogreCamera to settings of camera \p camera.
		 * Might be used by camera actions in any editor which have to view a specific camera.
		 */
		static void updateCamera(const mdlx::Camera &camera, Ogre::Camera *ogreCamera);

		OgreMdlx(const KUrl &url, ModelView *modelView);
		virtual ~OgreMdlx();

		virtual void clear() throw ();

		const MdlxPtr& mdlx() const;
		ModelView* modelView() const;
		Ogre::SceneNode* sceneNode() const;

		const Cameras& cameras() const;
		const CollisionShapes& collisionShapes() const;

		void setTeamColor(BOOST_SCOPED_ENUM(TeamColor) teamColor) throw (Exception);
		BOOST_SCOPED_ENUM(TeamColor) teamColor() const;
		void setTeamGlow(BOOST_SCOPED_ENUM(TeamColor) teamGlow) throw (Exception);
		BOOST_SCOPED_ENUM(TeamColor) teamGlow() const;

		Ogre::String geosetName(const mdlx::Geoset &geoset, mdlx::long32 id) const;
		Ogre::String sequenceName(const mdlx::Geoset &geoset, const mdlx::Sequence &sequence) const;

		/**
		 * Loads and analyses all data of corresponding MDLX model and refreshes displayed OGRE mesh.
		 * If \ref modelView() is based on
		 */
		virtual void load() throw (class Exception);
		virtual void reload() throw (Exception);

		/**
		 * Provides serialization functionality of the corresponding \ref mdlx::Mdlx instance or the OGRE scene.
		 * Detects output format by file extension (no matter what case).
		 * Supported formats:
		 * <ul>
		 * <li>MDX</li>
		 * <li>MDL</li>
		 * </ul>
		 * Stores file at \p url.
		 * \param format If this value is empty format will be detected automatically by it's destination's extension. If no valid extension could be detected default format is used: MDX.
		 * \throw Exception Is thrown when file could not be stored in \p url.
		 */
		virtual void save(const KUrl &url, const QString &format) const throw (class Exception);
		virtual void save(const KUrl &url) const throw (Exception)
		{
			save(url, "mdx");
		}

		QString namePrefix() const;

	protected:
		typedef std::pair<const class Node*, Ogre::Node*> NodePairType;

		/**
		 * Sometimes models are loaded from local directories which do also contain their required textures, attachments, particle emitter effects etc.
		 * In that case you can't load those required files from \ref MpqPriorityList (\ref Editor -> \ref ModelView::editor()). Hence you must load them from the local filesystem using the model's directory URL as root directory.
		 * This function checks whether URL \p url exists in one of the MPQ archives using \ref MpqPriorityList::findFile and if not it replaces the given URL parameter by a local URL using \ref OgreMdlx::url()'s directory as root directory.
		 * \param url URL which is checked for and might be replaced by its local version using model's resource directory URL as root directory.
		 * \param showMessage If this value is true it will show a message dialog if file does not exist in one of the MPQ archives.
		 * \return Returns true if file does not exist in one of the MPQ archives and you have to try the local URL.
		 */
		bool useDirectoryUrl(KUrl &url, bool showMessage = false) const;

		Ogre::Node* createNode(const mdlx::Node &node);
		//void createNodeAnimatedProperties(const mdlx::Node &node) const;

		//template<std::size_t size>
		//std::list<Ogre::VertexAnimationTrack*> createAnimatedProperties(const mdlx::MdlxAnimatedProperties<size> &properties) const;

		Ogre::TexturePtr createTexture(const mdlx::Texture &texture, mdlx::long32 id);
		Ogre::MaterialPtr createMaterial(const mdlx::Material &material, mdlx::long32 id);
		/**
		* Creates manual object for specified geoset.
		*/
		Ogre::ManualObject* createGeoset(const mdlx::Geoset &geoset, mdlx::long32 id);

		Ogre::Camera* createCamera(const mdlx::Camera &camera, mdlx::long32 id);

		/**
		 * Collision shapes are required for "hit tests".
		 */
		CollisionShape* createCollisionShape(const mdlx::CollisionShape &collisionShape, mdlx::long32 id);

		Ogre::Skeleton* createSkeleton(const Ogre::String &name);
		Ogre::Bone* createBone(const mdlx::Bone &bone, mdlx::long32 id);



		/**
		* Creates all necessary OGRE nodes with correct inheritane and returns the resulting map with all nodes and objects.
		* @todo Allocate objects by using type information (Object::type).
		*/
		std::map<const mdlx::Node*, Ogre::Node*> setupInheritance(const std::list<const mdlx::Node*> &nodes);

		MdlxPtr m_mdlx;
		ModelView *m_modelView;
		Ogre::SceneNode *m_sceneNode;

		Textures m_textures;
		Materials m_materials;
		Geosets m_geosets;
		GeosetIds m_geosetIds;
		GeosetMeshes m_geosetMeshes;
		Sequences m_sequences;
		Cameras m_cameras;
		CollisionShapes m_collisionShapes;
		Bones m_bones;

		std::map<const class mdlx::Node*, Ogre::Node*> m_nodes;

		// these members are required for dynamic team glow and color settings
		BOOST_SCOPED_ENUM(TeamColor) m_teamColor;
		BOOST_SCOPED_ENUM(TeamColor) m_teamGlow;
		std::list<Ogre::TexturePtr> m_teamColorTextures;
		std::list<Ogre::TexturePtr> m_teamGlowTextures;
};

inline const OgreMdlx::MdlxPtr& OgreMdlx::mdlx() const
{
	return this->m_mdlx;
}

inline class ModelView* OgreMdlx::modelView() const
{
	return this->m_modelView;
}

inline Ogre::SceneNode* OgreMdlx::sceneNode() const
{
	return this->m_sceneNode;
}

inline const OgreMdlx::Cameras& OgreMdlx::cameras() const
{
	return this->m_cameras;
}

inline const OgreMdlx::CollisionShapes& OgreMdlx::collisionShapes() const
{
	return this->m_collisionShapes;
}

inline BOOST_SCOPED_ENUM(TeamColor) OgreMdlx::teamColor() const
{
	return this->m_teamColor;
}

inline BOOST_SCOPED_ENUM(TeamColor) OgreMdlx::teamGlow() const
{
	return this->m_teamGlow;
}

inline Ogre::String OgreMdlx::geosetName(const mdlx::Geoset &geoset, mdlx::long32 id) const
{
	return Ogre::String((boost::format("%1%.Geoset%2%") % namePrefix().toUtf8().constData() % id).str().c_str());
}

inline Ogre::String OgreMdlx::sequenceName(const mdlx::Geoset &geoset, const mdlx::Sequence &sequence) const
{
	GeosetIds::const_iterator iterator = m_geosetIds.find(&geoset);

	if (iterator == m_geosetIds.end())
		throw Exception();

	return geosetName(geoset, iterator->second) + " - " + sequence.name();
}
/*
template<std::size_t size>
std::list<Ogre::VertexAnimationTrack*> createAnimatedProperties(const mdlx::MdlxAnimatedProperties<size> &properties) const
{
	const mdlx::Sequence *sequence = boost::polymorphic_cast<mdlx::Sequence*>(this->mdlx()->node(properties.globalSequenceId()));
	std::list<Ogre::VertexAnimationTrack*> results;

	BOOST_FOREACH(GeosetMeshes::const_reference ref, this->m_geosetMeshes)
	{
		const Ogre::Animation *animation = ref.second->getAnimation(sequenceName(*ref.first, sequence->name()));

		switch (properties.lineType())
		{
			case mdlx::LineType::DontInterpolate:
				qDebug() << "Dont interpolate is not supported.";

				break;

			case mdlx::LineType::Linear:
				animation->setInterpolationMode(Ogre::Animation::IM_LINEAR);

				break

			case mdlx::LineType::Hermite:
				qDebug() << "Hermite is not supported.";

				break;

			case mdlx::LineType::Bezier:
				animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);

				break;
		}

		Ogre::VertexAnimationTrack *track = animation->createVertexTrack(0);
		results.push_back(track);

		BOOST_FOREACH(mdlx::MdlxAnimatedProperties<size>::Properties::const_reference property, properties)
		{
			// TODO handle inTan and outTan if line type > 1
			const Ogre::HardwareVertexBufferSharedPtr vertexBuffer = HardwareBufferManager::getSingleton().createVertexBuffer(size * sizeof(mdlx::float32), size, Ogre::HardwareBuffer::HBU_STATIC);

			boost::scoped_array<mdlx::float32> buffer(new mdlx::float32[property.values().size()]);
			mdlx::long32 i = 0;

			BOOST_FOREACH(mdlx::MdlxAnimatedProperty<size>::Values::const_reference value, property.values())
			{
				buffer[i] = value;
				++i;
			}

			const std::size_t bufferSize = size * sizeof(mdlx::float32);
			vertexBuffer->writeData(0, bufferSize, buffer.get());

			const Ogre::VertexMorphKeyFrame *keyFrame = track->createVertexMorphKeyFrame(property.frame());
			keyFrame->setVertexBuffer(vertexBuffer);
		}
	}

	return results;
}
*/

}

}

#endif
