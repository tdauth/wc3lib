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

#include <boost/scoped_ptr.hpp>
#include <boost/bimap.hpp>

#include <QDebug>

#include <kdemacros.h>

#include <Ogre.h>

#include "resource.hpp"
#include "platform.hpp"
#include "collisionshape.hpp"
#include "../mdlx.hpp"

#include <boost/foreach.hpp> // NOTE include foreach after Qt stuff because of namespace bug

namespace wc3lib
{

namespace editor
{

class ModelView;
class OgreMdlxEntity;

/**
 * \brief This class can be used to display MDLX models by using the OGRE 3D rendering engine.
 * It maintains multiple mesh instances which contains all converted data of the original
 * MDLX model's geosets.
 *
 * The mesh instances are created in the scene of a corresponding \ref ModelView.
 *
 * It can be accessed using \ref modelView().
 *
 * When loading an MDX or an MDL file using \ref load() it creates instances of Ogre::Mesh with all the geometry
 * from the Geosets.
 *
 * Besides textures, lights, cameras etc. are converted.
 *
 * The parent scene node can which is created automatically on loading the model can be accessed using \ref sceneNode().
 *
 *
 * To create a movable entity of geometry use \ref createEntity(). Entities might allow better performance when using multiple
 * objects (for example units) which shared the same geometry.
 *
 *
 * Each MDLX instance can have its own team color and glow which is required for proper unit displays and model testings.
 *
 * To change the team color and team glow dynamically you can use \ref setTeamColor() and \ref setTeamGlow().
 * \todo This should be moved to the entity!
 *
 *
 * \todo Use inherited event functions of frame listener to apply animation track data (each model instance should have its own time marker for sequences).
 * \note Team color and glow textures have to be reloaded when team color or glow is changed. Reloading their images doesn't work in OGRE!
 * \todo Replace TextureIds, GeosetIds and SequenceIds (all objects which do not have specified their id in MDX files) by std::vector since you know for sure that their ids are in a strict order!
 * \todo Maybe instead of two maps you could uses one with a pair of MDLX and OGRE objects.
 */
class KDE_EXPORT OgreMdlx : public Resource, public Ogre::FrameListener
{
	public:
		typedef boost::scoped_ptr<mdlx::Mdlx> MdlxPtr;

		/**
		 * All nodes are identified by their corresponding unique ID.
		 */
		typedef std::map<mdlx::long32, const mdlx::Node*> Nodes;

		typedef std::map<mdlx::long32, const mdlx::GeosetAnimation*> GeosetAnimations;

		typedef std::map<const mdlx::Texture*, Ogre::TexturePtr> Textures;
		typedef std::map<mdlx::long32, const mdlx::Texture*> TextureIds;
		typedef std::map<const mdlx::Material*, Ogre::MaterialPtr> Materials;
		/**
		 * Required by geosets.
		 */
		typedef std::map<mdlx::long32, const mdlx::Material*> MaterialIds;
		/**
		 * Geoset IDs have to be stored in both directions since many members need access to geosets by their ID as well as you need the ID of a geoset sometimes.
		 */
		typedef boost::bimap<const mdlx::Geoset*, mdlx::long32> GeosetIds;
		typedef std::map<const mdlx::Geoset*, Ogre::MeshPtr> Geosets;
		typedef std::map<const mdlx::Camera*, Ogre::Camera*> Cameras;
		typedef std::map<const mdlx::Sequence*, Ogre::Animation*> Sequences;
		typedef std::map<mdlx::long32, const mdlx::Sequence*> SequenceIds;

		typedef std::map<const mdlx::CollisionShape*, CollisionShape*> CollisionShapes;
		typedef std::map<const mdlx::Bone*, Ogre::Bone*> Bones;

		typedef std::list<mdlx::long32> TeamColorTextures;
		typedef std::list<Ogre::TextureUnitState*> TeamColorTextureUnitStates;

		/**
		 * Changes camera's \p ogreCamera to settings of camera \p camera.
		 * Might be used by camera actions in any editor which have to view a specific camera.
		 */
		static void updateCamera(const mdlx::Camera &camera, Ogre::Camera *ogreCamera);

		OgreMdlx(const KUrl &url, ModelView *modelView);
		virtual ~OgreMdlx();

		virtual void clear() throw ();

		const MdlxPtr& mdlx() const;
		/**
		 * \return Returns the corresponding model view which the scene node does belong to.
		 */
		ModelView* modelView() const;
		/**
		 * \return Returns the corresponding scene node which is the parent node of all OGRE instances from this particular model.
		 */
		Ogre::SceneNode* sceneNode() const;

		const GeosetIds& geosetIds() const;
		const Geosets& geosets() const;
		const Cameras& cameras() const;
		const CollisionShapes& collisionShapes() const;

		/**
		 * Changes the current team color to \p teamColor.
		 * This updates all team color textures.
		 */
		void setTeamColor(TeamColor teamColor);
		TeamColor teamColor() const;
		void setTeamGlow(TeamColor teamGlow);
		TeamColor teamGlow() const;

		Ogre::String textureName(mdlx::long32 id) const;
		Ogre::String geosetName(mdlx::long32 id) const;

		/**
		 * Loads and analyses all data of corresponding MDLX model and refreshes displayed OGRE mesh.
		 * If \ref modelView() is based on
		 */
		virtual void load() override;
		virtual void reload() override;

		/**
		 * Provides serialization functionality of the corresponding \ref mdlx::Mdlx instance or the OGRE scene.
		 * Detects output format by file extension (no matter what case).
		 * Supported formats:
		 * <ul>
		 * <li>MDX - "mdx"</li>
		 * <li>MDL - "mdl"</li>
		 * <li>OGRE MESH - "mesh"</li>
		 * </ul>
		 * Stores file at \p url.
		 * \param url The URL where the model is saved to.
		 * \param format If this value is empty format will be detected automatically by it's destination's extension. If no valid extension could be detected default format is used: MDX.
		 * \throw Exception Is thrown when file could not be stored in \p url.
		 */
		virtual void save(const KUrl &url, const QString &format) const;
		virtual void save(const KUrl &url) const override
		{
			save(url, "");
		}

		QString namePrefix() const;

		/**
		 * \return Returns a new entity of Geoset geometry to which animations can be applied.
		 * \note Take care of deletion!
		 */
		OgreMdlxEntity* createEntity(const Ogre::String &name);

	protected:
		typedef std::map<const mdlx::Node*, Ogre::Node*> NodeInheritance;
		typedef NodeInheritance::value_type NodePairType;

		/**
		 * Sometimes models are loaded from local directories which do also contain their required textures, attachments, particle emitter effects etc.
		 * In that case you can't load those required files from \ref MpqPriorityList (\ref Editor -> \ref ModelView::editor()). Hence you must load them from the local filesystem using the model's directory URL as root directory.
		 * This function checks whether URL \p url exists in one of the MPQ archives using \ref MpqPriorityList::exists and if not it replaces the given URL parameter by a local URL using \ref OgreMdlx::url()'s directory as root directory.
		 * \param url URL which is checked for and might be replaced by its local version using model's resource directory URL as root directory.
		 * \param showMessage If this value is true it will show a message dialog if file does not exist in one of the MPQ archives.
		 * \return Returns true if file does not exist in one of the MPQ archives and you have to try the local URL.
		 */
		bool useDirectoryUrl(KUrl &url, bool showMessage = false) const;

		Ogre::Node* createNode(const mdlx::Node &node);
		void createNodeAnimatedProperties(const mdlx::Node &node, Ogre::Node *ogreNode);

		template<std::size_t size>
		Ogre::TransformKeyFrame* createAnimatedPropertyKeyFrame(Ogre::NodeAnimationTrack *track, const mdlx::MdlxAnimatedProperty<size> &property) const;
		template<std::size_t size>
		void createAnimatedProperties(Ogre::Node *node, unsigned short trackId, const mdlx::MdlxAnimatedProperties<size> &properties);

		void createTextures();
		Ogre::TexturePtr createTexture(const mdlx::Texture &texture, mdlx::long32 id);
		Ogre::TextureUnitState* createLayer(Ogre::Pass *pass, const mdlx::Layer &layer);
		void createMaterials();
		Ogre::MaterialPtr createMaterial(const mdlx::Material &material, mdlx::long32 id);
		void createGeosets();
		/**
		 * Creates manual object for specified geoset.
		 */
		Ogre::ManualObject* createGeoset(const mdlx::Geoset &geoset, mdlx::long32 id);

		void createCameras();
		Ogre::Camera* createCamera(const mdlx::Camera &camera, mdlx::long32 id);

		void createCollisionShapes();
		/**
		 * Collision shapes are required for "hit tests".
		 */
		CollisionShape* createCollisionShape(const mdlx::CollisionShape &collisionShape, mdlx::long32 id);

		Ogre::SkeletonPtr createSkeleton(const Ogre::String &name);
		Ogre::Bone* createBone(const mdlx::Bone &bone, mdlx::long32 id);



		/**
		* Creates all necessary OGRE nodes with correct inheritane and returns the resulting map with all nodes and objects.
		* @todo Allocate objects by using type information (Object::type).
		*/
		NodeInheritance setupInheritance(const std::list<const mdlx::Node*> &nodes);

		MdlxPtr m_mdlx;
		ModelView *m_modelView;
		Ogre::SceneNode *m_sceneNode;

		Nodes m_nodes;
		GeosetAnimations m_geosetAnimations;
		Textures m_textures;
		TextureIds m_textureIds;
		Materials m_materials;
		MaterialIds m_materialIds;
		Geosets m_geosets;
		GeosetIds m_geosetIds;
		Sequences m_sequences;
		SequenceIds m_sequenceIds;
		Cameras m_cameras;
		CollisionShapes m_collisionShapes;
		Bones m_bones;

		NodeInheritance m_nodeInheritance;

		// these members are required for dynamic team glow and color settings
		TeamColor m_teamColor;
		TeamColor m_teamGlow;
		TeamColorTextures m_teamColorTextures;
		TeamColorTextureUnitStates m_teamColorTextureUnitStates;
		TeamColorTextures m_teamGlowTextures;
		TeamColorTextureUnitStates m_teamGlowTextureUnitStates;
};

inline const OgreMdlx::MdlxPtr& OgreMdlx::mdlx() const
{
	return this->m_mdlx;
}

inline ModelView* OgreMdlx::modelView() const
{
	return this->m_modelView;
}

inline Ogre::SceneNode* OgreMdlx::sceneNode() const
{
	return this->m_sceneNode;
}

inline const OgreMdlx::GeosetIds& OgreMdlx::geosetIds() const
{
	return this->m_geosetIds;
}

inline const OgreMdlx::Geosets& OgreMdlx::geosets() const
{
	return this->m_geosets;
}

inline const OgreMdlx::Cameras& OgreMdlx::cameras() const
{
	return this->m_cameras;
}

inline const OgreMdlx::CollisionShapes& OgreMdlx::collisionShapes() const
{
	return this->m_collisionShapes;
}

inline TeamColor OgreMdlx::teamColor() const
{
	return this->m_teamColor;
}

inline TeamColor OgreMdlx::teamGlow() const
{
	return this->m_teamGlow;
}

inline Ogre::String OgreMdlx::textureName(mdlx::long32 id) const
{
	return Ogre::String((boost::format("%1%.Texture%2%") % namePrefix().toStdString() % id).str().c_str());
}

inline Ogre::String OgreMdlx::geosetName(mdlx::long32 id) const
{
	return Ogre::String((boost::format("%1%.Geoset%2%") % namePrefix().toStdString() % id).str().c_str());
}

template<std::size_t size>
inline Ogre::TransformKeyFrame* OgreMdlx::createAnimatedPropertyKeyFrame(Ogre::NodeAnimationTrack *track, const mdlx::MdlxAnimatedProperty<size> &property) const
{
	return 0;
}

template<>
inline Ogre::TransformKeyFrame* OgreMdlx::createAnimatedPropertyKeyFrame(Ogre::NodeAnimationTrack *track, const mdlx::Translation &translation) const
{
	Ogre::TransformKeyFrame *keyFrame = track->createNodeKeyFrame(translation.frame());
	keyFrame->setTranslate(ogreVertex<Ogre::Vector3>(translation.values()));

	return keyFrame;
}

template<>
inline Ogre::TransformKeyFrame* OgreMdlx::createAnimatedPropertyKeyFrame(Ogre::NodeAnimationTrack *track, const mdlx::Rotation &rotation) const
{
	Ogre::TransformKeyFrame *keyFrame = track->createNodeKeyFrame(rotation.frame());
	keyFrame->setRotation(ogreVertex<Ogre::Quaternion>(rotation.values()));

	return keyFrame;
}

template<std::size_t size>
inline void OgreMdlx::createAnimatedProperties(Ogre::Node *node, unsigned short trackId, const mdlx::MdlxAnimatedProperties<size> &properties)
{
	if (properties.properties().empty())
	{
		return;
	}

	if (properties.globalSequenceId() == mdlx::noneId)
	{
		qDebug() << "Properties have none id!";

		return;
	}

	const mdlx::Sequence *sequence = m_sequenceIds[properties.globalSequenceId()];
	// create a track to animate the camera's node
	Ogre::NodeAnimationTrack *track = m_sequences[sequence]->createNodeTrack(trackId, node);

        // create a new animation state to track this
       // mAnimState = mSceneMgr->createAnimationState("CameraTrack");

	BOOST_FOREACH(typename mdlx::MdlxAnimatedProperties<size>::Properties::const_reference property, properties.properties())
	{
		createAnimatedPropertyKeyFrame<size>(track, property);
	}
}

}

}

#endif
