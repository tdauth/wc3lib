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
 * The editor module contains many classes to emulate the original World Editor for Warcraft III created by Blizzard Entertainment.
 *
 * It's based on some plugins which can be used without the whole editor module functionality, as well.
 *
 * Include file \ref editor.hpp and use namspace \ref wc3lib::editor to use this module.
 *
 * Resource classes derived from \ref editor::Resource can be used to load Warcraft III date like textures (\ref editor::Texture), models (\ref editor::OgreMdlx) or maps (\ref editor::Map).
 *
 * Those wrapper classes provide load and save functionality based on the API of \ref editor::MpqPriorityList.
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
 * \section appsection Applications
 * The editor module brings its own applications based on the editor libraries:
 * <ul>
 * <li>wc3ability - Standalone GUI ability editor based on \ref AbilityEditor .</li>
 * <li>wc3editor - World Editor GUI emulation based on \ref Editor .</li>
 * <li>wc3model - Standalone GUI model editor based on \ref ModelEditor .</li>
 * <li>wc3mpq - Standalone GUI MPQ editor based on \ref MpqEditor .</li>
 * <li>wc3object - Standalone GUI object editor based on \ref ObjectEditor .</li>
 * <li>wc3texture - Standalone GUI texture object editor based on \ref TextureEditor .</li>
 * <li>wc3trigger - Standalone GUI trigger editor based on \ref TriggerEditor .</li>
 * <li>wc3unit - Standalone GUI unit editor based on \ref UnitEditor .</li>
 * </ul>
 *
 * The applications are installed on the target platform and can be started as regular applications.
 * For Unix systems .desktop files are distributed as well.
 *
 * \namespace wc3lib::editor
 * \brief \ref editorsection
 */
}

// Check flags first
#include "config.h"

#include "editor/abilitydata.hpp"
#include "editor/buffdata.hpp"
#include "editor/destructabledata.hpp"
#include "editor/doodaddata.hpp"
#include "editor/editor.hpp"
#include "editor/itemdata.hpp"
#include "editor/listfilesdialog.hpp"
#include "editor/map.hpp"
#include "editor/metadata.hpp"
#include "editor/miscdata.hpp"
#include "editor/module.hpp"
#include "editor/modulemenu.hpp"
#include "editor/moduletoolbar.hpp"
#include "editor/mpqprioritylist.hpp"
#include "editor/newmapdialog.hpp"
#include "editor/objectdata.hpp"
#ifdef MDLX
#include "editor/collisionshape.hpp"
#include "editor/modelview.hpp"
#include "editor/ogremdlx.hpp"
#include "editor/ogremdlxentity.hpp"
#endif
#if defined(MDLX) || defined(USE_OGREBLP)
#include "editor/ogreenvironment.hpp"
#include "editor/environment.hpp"
#include "editor/polygonmodecombobox.hpp"
#include "editor/renderstatswidget.hpp"
#include "editor/root.hpp"
#endif
#include "editor/resource.hpp"
#include "editor/settings.hpp"
#include "editor/settingsinterface.hpp"
#include "editor/sharedobjectdata.hpp"
#include "editor/skindata.hpp"
#include "editor/sourcesdialog.hpp"
#include "editor/splashscreen.hpp"
#include "editor/teamcolordialog.hpp"
#include "editor/texture.hpp"
#include "editor/unitdata.hpp"
#include "editor/upgradedata.hpp"
#include "editor/warcraftiiishared.hpp"
#include "editor/waterdata.hpp"
#include "editor/weatherdata.hpp"
#include "editor/windowsmenu.hpp"

#endif
