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

#ifndef WC3LIB_EDITOR_WINDOWSMENU_HPP
#define WC3LIB_EDITOR_WINDOWSMENU_HPP

#include <boost/cast.hpp>

#include <QMenu>

#include "platform.hpp"
#include "module.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief The menu called "Windows" in the World Editor which also lists all open maps.
 * \sa ModuleMenu
 */
class WindowsMenu : public QMenu
{
	Q_OBJECT

	public:
		WindowsMenu(Module *module);

		Module* module() const;

		void retranslateUi();

	protected slots:
		void addMapAction(Map *map);
		void removeMapAction(Map *map);
};

inline Module* WindowsMenu::module() const
{
	return boost::polymorphic_cast<Module*>(parent());
}

}

}

#endif // WC3LIB_EDITOR_WINDOWSMENU_HPP
