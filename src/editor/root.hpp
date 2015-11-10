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

#ifndef WC3LIB_EDITOR_ROOT_HPP
#define WC3LIB_EDITOR_ROOT_HPP

#include <kdemacros.h>

#include <Ogre.h>

namespace wc3lib
{

namespace editor
{

/**
 * \brief Wrapper class for Ogre::Root which handles the configuration paths.
 *
 * Use \ref configure() to make sure the renderer system is configured.
 */
class KDE_EXPORT Root : public Ogre::Root
{
	public:
		/**
		 * The default file path to the ogre.cfg file.
		 * On Unix systems this is "/etc/wc3lib/ogre.cfg".
		 */
		static const char* defaultOgreCfg;
		/**
		 * The default file path to the plugins.cfg file.
		 * On Unix systems this is "/etc/wc3lib/plugins.cfg".
		 */
		static const char* defaultPluginsCfg;

		/**
		 * Creates a new root object using the default file paths \ref defaultOgreCfg and \ref defaultPluginsCfg.
		 */
		Root();

		/**
		 * Restores the configuration of the root object. If it is not possible to restore a former configuration it shows a configuration dialog.
		 * \return Returns true if the configuration succeeded and the user did not cancel the dialog.
		 */
		bool configure();

	private:
		Ogre::String m_ogreCfg;
		Ogre::String m_pluginsCfg;
};

}

}

#endif
