#ifndef WC3LIB_EDITOR_OGREENVIRONMENT_HPP
#define WC3LIB_EDITOR_OGREENVIRONMENT_HPP

#include <boost/scoped_ptr.hpp>

#include <kdemacros.h>

#include <Ogre.h>
#include <OGRE/Terrain/OgreTerrain.h>

#include "resource.hpp"

namespace wc3lib
{

namespace map
{

class Environment;

}

namespace editor
{

/**
 * \brief Combines \ref wc3lib::map::Environment and OGRE's terrain system to provide a 3D rendering of a Warcraft's map
 * environment.
 *
 * \note Shouldn't use any Qt or KDE classes except inheritance of \ref Resource. Keep it independent!
 *
 * \sa OgreMdlx, Texture
 *
 * http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Basic+Tutorial+3
 * http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Ogre+Terrain+Textures
 */
class KDE_EXPORT OgreEnvironment : public Resource
{
	public:
		/**
		 * Pointer for keeping a map's environment.
		 */
		typedef boost::scoped_ptr<map::Environment> EnvironmentPtr;

		OgreEnvironment(const KUrl& url, Type type);
		virtual ~OgreEnvironment();

		void load(map::Environment *environment);

		virtual void load() throw (Exception);
		virtual void reload() throw (Exception);
		virtual void save(const KUrl& url) const throw (Exception);

	private:
		Ogre::TerrainGlobalOptions *m_terrainGlobals;
		Ogre::Terrain *m_terrain;
};

}

}

#endif // OGREENVIRONMENT_H
