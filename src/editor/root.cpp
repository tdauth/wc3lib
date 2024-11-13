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

#include <QtCore>

#include "root.hpp"
#include "config.h"

namespace wc3lib
{

namespace editor
{

/*
 * Define as static constants due to initialization order.
 */
#ifdef DEBUG
 const char* Root::defaultOgreCfg = "./ogre.cfg";
 const char* Root::defaultPluginsCfg = "./plugins.cfg";
#else
#ifdef Q_OS_UNIX
 const char* Root::defaultOgreCfg = "/usr/etc/wc3lib/ogre.cfg";
 const char* Root::defaultPluginsCfg = "/usr/etc/wc3lib/plugins.cfg";
#else // TODO set Windows paths
 const char* Root::defaultOgreCfg = "./ogre.cfg";
 const char* Root::defaultPluginsCfg = "./plugins.cfg";
#endif
#endif

Root::Root()
: Ogre::Root(Root::defaultPluginsCfg, Root::defaultOgreCfg)
, m_ogreCfg(Root::defaultOgreCfg)
, m_pluginsCfg(Root::defaultPluginsCfg)
{
#ifdef DEBUG
	qDebug() << "Using debug paths: " << m_pluginsCfg.c_str() << " and " << m_ogreCfg.c_str();
#endif
}

Root::Root(const char *ogreCfg, const char *pluginsCfg) :
 Ogre::Root(ogreCfg, pluginsCfg)
, m_ogreCfg(ogreCfg)
, m_pluginsCfg(pluginsCfg)
{
}

bool Root::configure()
{
	// configure
	// Show the configuration dialog and initialise the system
	//if(!restoreConfig())
	//{
		if (showConfigDialog(NULL)) {
			initialise(false);

			return true;
		}
	//}

	return false;
}

}

}
