/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include <QtGui>

#include "windowsmenu.hpp"
#include "editor.hpp"

namespace wc3lib
{

namespace editor
{

WindowsMenu::WindowsMenu(Module *module) : QMenu(module)
{
	if (module->hasEditor())
	{
		foreach (Map *map, const_cast<const Editor*>(module->editor())->maps())
		{
			addMapAction(map);
		}

		connect(module->editor(), SIGNAL(openedMap(Map*)), this, SLOT(addMapAction(Map*)));
		connect(module->editor(), SIGNAL(aboutToCloseMap(Map*)), this, SLOT(removeMapAction(Map*)));
	}
}

void WindowsMenu::retranslateUi()
{
	this->setTitle(module()->source()->sharedData()->tr("WESTRING_MENU_WINDOW"));
}

void WindowsMenu::addMapAction(Map *map)
{
	if (module()->hasEditor())
	{
		this->addAction(const_cast<const Editor*>(module()->editor())->mapActions()[map]);
	}
}

void WindowsMenu::removeMapAction(Map* map)
{
	if (module()->hasEditor())
	{
		this->removeAction(const_cast<const Editor*>(module()->editor())->mapActions()[map]);
	}
}

#include "moc_windowsmenu.cpp"

}

}
