/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_OGREENVIRONMENT_HPP
#define WC3LIB_EDITOR_OGREENVIRONMENT_HPP

#include <boost/scoped_ptr.hpp>

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
class OgreEnvironment : public Resource
{
	public:
		/**
		 * Pointer for keeping a map's environment.
		 */
		typedef boost::scoped_ptr<map::Environment> EnvironmentPtr;

		OgreEnvironment(const QUrl &url, Type type);
		virtual ~OgreEnvironment();

		void load(map::Environment *environment);

		virtual void load();
		virtual void reload();
		virtual void save(const QUrl &url) const;

	private:
		Ogre::TerrainGlobalOptions *m_terrainGlobals;
		Ogre::Terrain *m_terrain;
};

}

}

#endif // OGREENVIRONMENT_H
