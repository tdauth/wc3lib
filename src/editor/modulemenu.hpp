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

#include <QList>

#include <kmenu.h>

namespace wc3lib
{

namespace editor
{

class ModuleMenu : public KMenu
{
	public:
		ModuleMenu(QWidget *widget, class Editor *editor);
		/**
		* Holds all modules actions (without separators).
		* Required by class Module.
		*/
		const QList<class QAction*>& actions() const;

	protected:
		QList<class QAction*> m_actions;
};

inline const QList<class QAction*>& ModuleMenu::actions() const
{
	return this->m_actions;
}

}

}

#endif
