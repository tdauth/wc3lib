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

#ifndef WC3LIB_EDITOR_MODULEMENU_HPP
#define WC3LIB_EDITOR_MODULEMENU_HPP

#include <boost/cast.hpp>

#include <KMenu>

#include "module.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Menu entry which lists actions for all Editor modules.
 * Entries are created via signal when modules are created.
 * Should never be used without an \ref Editor instance.
 * \sa WindowsMenu
 */
class ModuleMenu : public KMenu
{
	Q_OBJECT

	public:
		ModuleMenu(Module *module);

		Module* module() const;

	protected slots:
		void addModuleAction(Module *module);
};

inline Module* ModuleMenu::module() const
{
	return boost::polymorphic_cast<Module*>(parent());
}

}

}

#endif
