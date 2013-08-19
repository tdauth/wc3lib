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

#include "importmanager.hpp"

namespace wc3lib
{

namespace editor
{

ImportManager::ImportManager(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : Module(source, parent, f)
{
}

void ImportManager::createFileActions(class KMenu *menu)
{
}

void ImportManager::createEditActions(class KMenu *menu)
{
}

void ImportManager::createMenus(class KMenuBar *menuBar)
{
}

void ImportManager::createWindowsActions(class WindowsMenu *menu)
{
}

void ImportManager::createToolButtons(class KToolBar *toolBar)
{
}

class SettingsInterface* ImportManager::settings()
{
	/// @todo FIXME
	return 0;
}

void ImportManager::onSwitchToMap(Map* map)
{
}

}

}
