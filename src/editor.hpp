/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_HPP
#define WC3LIB_EDITOR_HPP

namespace wc3lib
{
/**
 * \page editorsection Editor module
 * The editor module contains many classes to emulate the original World Editor created by Blizzard Entertainment.
 *
 * It's based on some plugins which can be used without the whole editor module functionality, as well.
 *
 * Include file \ref editor.hpp and use namspace \ref wc3lib::edtor to use this module.
 *
 * Resource classes derived from \ref editor::Resource can be used to load Warcraft III date like textures (\ref editor::Texture), models (\ref editor::OgreMdlx) or maps (\ref editor::Map).
 *
 * Those wrapper classes provide load and save functionality based on the KIO API and considering \ref editor::MpqPriorityList entries.
 *
 * \section pluginssection Plugins
 * There is various plug-ins which allow you to load and save BLP textures in Qt/KDE GUIs and OGRE scenes and to access MPQ archives via KDE's KIO slave module:
 * <ul>
 * <li>\ref editor::BlpCodec - OGRE BLP texture support</li>
 * <li>\ref editor::MpqSlave - KDE I/O slave for MPQ archives</li>
 * <li>\ref editor::BlpIOPlugin - Qt image format support for BLP textures</li>
 * </ul>
 *
 * \section modulessection World Editor Modules
 * The editor module provides some sub modules which offer all features of one single original World Editor module:
 * <ul>
 * <li>\subpage triggereditorsection </li>
 * <li>\subpage objecteditorsection </li>
 * <li>\subpage mpqeditorsection </li>
 * </ul>
 *
 * \namespace wc3lib::editor
 * \brief \ref editorsection
 */
}

#include "editor/editor.hpp"
#include "editor/listfilesdialog.hpp"
#include "editor/modelview.hpp"
#include "editor/module.hpp"
#include "editor/modulemenu.hpp"
#include "editor/moduletoolbar.hpp"
#include "editor/mpqprioritylist.hpp"
#include "editor/newmapdialog.hpp"
#include "editor/ogremdlx.hpp"
#include "editor/ogremdlxentity.hpp"
#include "editor/polygonmodecombobox.hpp"
#include "editor/renderstatswidget.hpp"
#include "editor/resource.hpp"
#include "editor/settings.hpp"
#include "editor/settingsinterface.hpp"
#include "editor/sourcesdialog.hpp"
#include "editor/splashscreen.hpp"
#include "editor/warcraftiiishared.hpp"
#include "editor/windowsmenu.hpp"

// modules
#include "editor/Plugin_BlpCodec/blpcodec.hpp"
#include "editor/modeleditor/modeleditor.hpp"
#include "editor/modeleditor/modeleditorview.hpp"
#include "editor/mpqeditor/mpqeditor.hpp"

#endif
