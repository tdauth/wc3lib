/*
 *
 */

#include "ogreenvironment.hpp"
#include "../map/environment.hpp"

namespace wc3lib
{

namespace editor
{

OgreEnvironment::OgreEnvironment(const KUrl& url, Resource::Type type): Resource(url, type)
{
	//m_terrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
}

OgreEnvironment::~OgreEnvironment()
{

}

void OgreEnvironment::load(map::Environment* environment)
{

}

void OgreEnvironment::load() throw (Exception)
{

}

void OgreEnvironment::reload() throw (Exception)
{

}

void OgreEnvironment::save(const KUrl& url) const throw (Exception)
{

}

}

}
