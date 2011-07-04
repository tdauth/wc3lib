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

#include <QColor>

#include <KUrl>

#include <Ogre.h>

#include "resource.hpp"
#include "../mdlx.hpp"
#include "../core.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * This class can be used to display MDLX models by using the OGRE 3d rendering engine.
 * It maintains a single mesh instance in a scene which contains all converted data of the original
 * MDLX model.
 * Geosets are implemented as sub meshes.
 * Each MDLX instance can have its own team color and glow which is required for proper unit displays and model testings.
 * \todo Use inherited event functions of frame listener to apply animation track data (each model instance should have its own time marker for sequences).
 */
class OgreMdlx  : public Resource, public Ogre::FrameListener
{
	public:
		BOOST_SCOPED_ENUM_START(TeamColor)
		{
			Red,
			Blue,
			Teal,
			Purple,
			Yellow,
			Orange,
			Green,
			Pink,
			Gray,
			LightBlue,
			DarkGreen,
			Brown,
			Black,
			MaxTeamColors
		};
		BOOST_SCOPED_ENUM_END
		
		typedef std::map<const class mdlx::Geoset*, Ogre::ManualObject*> Geosets;
		typedef std::map<const class mdlx::Camera*, Ogre::Camera*> Cameras;
		
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
		
		typedef std::map<const class mdlx::CollisionShape*, struct CollisionShape*> CollisionShapes;
		typedef boost::scoped_ptr<mdlx::Mdlx> MdlxPtr;
		
		/**
		 * Converts team color enumeration value into Qt RGB color.
		 * \param teamColor Team color which is converted.
		 * \return Returns corresponding Qt RGB color.
		 * \sa teamColor(const QColor&)
		 */
		static QColor teamColor(const BOOST_SCOPED_ENUM(TeamColor) &teamColor);
		/**
		 * Converts Qt RGB color into team color enumeration value.
		 * \param color Qt RGB color which is converted.
		 * \return Returns corresponding team color enumeration value.
		 * \sa teamColor(const enum TeamColor&)
		 */
		static BOOST_SCOPED_ENUM(TeamColor) teamColor(const QColor &color);
		/**
		 * Changes camera's \p ogreCamera to settings of camera \p camera.
		 * Might be used by camera actions in any editor which have to view a specific camera.
		 */
		static void updateCamera(const class mdlx::Camera &camera, Ogre::Camera *ogreCamera);

		OgreMdlx(const KUrl &url, class ModelView *modelView);
		virtual ~OgreMdlx();

		const MdlxPtr& mdlx() const;
		class ModelView* modelView() const;
		Ogre::SceneNode* sceneNode() const;

		const Cameras& cameras() const;
		const CollisionShapes& collisionShapes() const;

		void setTeamColor(BOOST_SCOPED_ENUM(TeamColor) teamColor);
		BOOST_SCOPED_ENUM(TeamColor) teamColor() const;
		void setTeamGlow(BOOST_SCOPED_ENUM(TeamColor) teamGlow);
		BOOST_SCOPED_ENUM(TeamColor) teamGlow() const;

		/**
		* Loads and analyses all data of corresponding MDLX model and refreshes displayed OGRE mesh.
		*/
		virtual void load() throw (class Exception, class Ogre::Exception);

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
		virtual void save(const KUrl &url, const QString &format = "mdx") throw (class Exception);
		
		QString namePrefix() const;

	protected:
		typedef std::pair<const class Node*, Ogre::Node*> NodePairType;

		/**
		 * Searches in member list of \p block for member \p member and returns its corresponding id.
		 * Id is always its index in member list (starting with 0).
		 */
		mdlx::long32 mdlxId(const mdlx::GroupMdxBlockMember &member, const mdlx::GroupMdxBlock *block) const;
		/**
		 * Sometimes models are loaded from local directories which do also contain their required textures, attachments, particle emitter effects etc.
		 * In that case you can't load those required files from \ref MpqPriorityList (\ref Editor -> \ref ModelView::editor()). Hence you must load them from the local filesystem using the model's directory URL as root directory.
		 * This function checks whether URL \p url exists in one of the MPQ archives using \ref MpqPriorityList::findFile and if not it replaces the given URL parameter by a local URL using \ref OgreMdlx::url()'s directory as root directory.
		 * \param url URL which is checked for and might be replaced by its local version using model's resource directory URL as root directory.
		 * \param showMessage If this value is true it will show a message dialog if file does not exist in one of the MPQ archives.
		 * \return Returns true if file does not exist in one of the MPQ archives and you have to try the local URL.
		 */
		bool useDirectoryUrl(KUrl &url, bool showMessage = false) const;

		Ogre::TexturePtr createTexture(const class mdlx::Texture &texture) throw (class Exception);
		Ogre::MaterialPtr createMaterial(const class mdlx::Material &material) throw (class Exception);
		/**
		* Creates manual object for specified geoset.
		*/
		Ogre::ManualObject* createGeoset(const class mdlx::Geoset &geoset) throw (class Exception);

		Ogre::Camera* createCamera(const class mdlx::Camera &camera) throw (class Exception);

		/**
		 * Collision shapes are required for "hit tests".
		 */
		CollisionShape* createCollisionShape(const class mdlx::CollisionShape &collisionShape) throw (class Exception);
		
		Ogre::Node* createNode(const class mdlx::Node &node);

		/**
		* Creates all necessary OGRE nodes with correct inheritane and returns the resulting map with all nodes and objects.
		* @todo Allocate objects by using type information (Object::type).
		*/
		std::map<const class mdlx::Node*, Ogre::Node*> setupInheritance(const std::list<const class mdlx::Node*> &nodes);

		MdlxPtr m_mdlx;
		class ModelView *m_modelView;
		Ogre::SceneNode *m_sceneNode;

		std::map<const class mdlx::Texture*, Ogre::TexturePtr> m_textures;
		std::map<const class mdlx::Material*, Ogre::MaterialPtr> m_materials;
		Geosets m_geosets;
		Cameras m_cameras;
		CollisionShapes m_collisionShapes;

		std::map<const class mdlx::Node*, Ogre::Node*> m_nodes;
		std::map<const class mdlx::Bone*, Ogre::Bone*> m_bones;

		class GlobalSequence *m_globalSequence; /// Current global sequence which is played.

		// these members are required for dynamic team glow and color settings
		BOOST_SCOPED_ENUM(TeamColor) m_teamColor;
		BOOST_SCOPED_ENUM(TeamColor) m_teamGlow;
		std::list<Ogre::TexturePtr> m_teamColorTextures;
		std::list<Ogre::TexturePtr> m_teamGlowTextures;
};

inline QColor OgreMdlx::teamColor(const BOOST_SCOPED_ENUM(TeamColor) &teamColor)
{
	switch (teamColor)
	{
		case TeamColor::Red:
			return QColor(Qt::red);
		
		case TeamColor::Blue:
			return QColor(Qt::blue);
			
		case TeamColor::Teal:
			return QColor(0x1CB619);
			
		case TeamColor::Purple:
			return QColor(0x800080);
			
		case TeamColor::Yellow:
			return QColor(Qt::yellow);
			
		case TeamColor::Orange:
			return QColor(0xFF8000);
			
		case TeamColor::Green:
			return QColor(Qt::green);
			
		case TeamColor::Pink:
			return QColor(0xFF80C0);
			
		case TeamColor::Gray:
			return QColor(0xC0C0C0);
			
		case TeamColor::LightBlue:
			return QColor(0x0080FF);
			
		case TeamColor::DarkGreen:
			return QColor(0x106246);
			
		case TeamColor::Brown:
			return QColor(0x804000);
			
		case TeamColor::Black:
			return QColor(Qt::black);
	}
	
	return QColor(Qt::red);
}

inline BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) OgreMdlx::teamColor(const QColor &color)
{
	if (color == Qt::red)
		return TeamColor::Red;
	else if (color == Qt::blue)
		return TeamColor::Blue;
	else if (color == 0x1CB619)
		return TeamColor::Teal;
	else if (color == 0x800080)
		return TeamColor::Purple;
	else if (color == Qt::yellow)
		return TeamColor::Yellow;
	else if (color == 0xFF8000)
		return TeamColor::Orange;
	else if (color == Qt::green)
		return TeamColor::Green;
	else if (color == 0xFF80C0)
		return TeamColor::Pink;
	else if (color == 0xC0C0C0)
		return TeamColor::Gray;
	else if (color == 0x0080FF)
		return TeamColor::LightBlue;
	else if (color == 0x106246)
		return TeamColor::DarkGreen;
	else if (color == 0x804000)
		return TeamColor::Brown;
	else if (color == Qt::black)
		return TeamColor::Black;
	
	return TeamColor::Red;
}

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

inline BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) OgreMdlx::teamColor() const
{
	return this->m_teamColor;
}

inline BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) OgreMdlx::teamGlow() const
{
	return this->m_teamGlow;
}

/// Global type cast function.
inline Ogre::Real ogreReal(mdlx::float32 value)
{
	return boost::numeric_cast<Ogre::Real>(value);
}

/// Global type cast function.
inline Ogre::Vector3 ogreVector3(const mdlx::VertexData &vertexData)
{
	return Ogre::Vector3(vertexData.x, vertexData.y, vertexData.z);
}

}

}

#endif
