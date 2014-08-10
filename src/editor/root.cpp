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
#include "../config.h"

namespace wc3lib
{

namespace editor
{

/*
 * Define as static constants due to initialization order.
 */
#ifdef DEBUG
static const char* ogreCfg = "ogre.cfg";
static const char* pluginsCfg = "plugins.cfg";
#else
#ifdef Q_OS_UNIX
static const char* ogreCfg = "/etc/wc3lib/ogre.cfg";
static const char* pluginsCfg = "/etc/wc3lib/plugins.cfg";
#else // TODO set Windows paths
static const char* ogreCfg = "ogre.cfg";
static const char* pluginsCfg = "plugins.cfg";
#endif
#endif

Root::Root()
: Ogre::Root(pluginsCfg, ogreCfg)
, m_ogreCfg(ogreCfg)
, m_pluginsCfg(pluginsCfg)
{
#ifdef DEBUG
	qDebug() << "Using debug paths: " << pluginsCfg << " and " << ogreCfg;
#endif
}

bool Root::configure()
{
	// configure
	// Show the configuration dialog and initialise the system
	if(!restoreConfig())
	{
		return showConfigDialog();
	}

	return true;
}

}

}
