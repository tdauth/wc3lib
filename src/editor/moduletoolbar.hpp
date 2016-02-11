/***************************************************************************
 *   Copyright (C) 2012 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_MODULETOOLBAR_HPP
#define WC3LIB_EDITOR_MODULETOOLBAR_HPP

#include <boost/cast.hpp>

#include <QToolBar>

#include <kdemacros.h>

#include "module.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Tool bar which contains tool buttons for all Editor modules.
 * Entries are created via signal when modules are created.
 * Other than \ref ModuleMenu this can and has to be used by modules if there is no Editor instance.
 * \sa ModuleMenu
 */
class KDE_EXPORT ModuleToolBar : public QToolBar
{
	Q_OBJECT

	public:
		ModuleToolBar(Module *module);

		void addCustomAction(QAction *action);
		void addCustomSeparator();

		Module* module() const;

	protected slots:
		void addModuleAction(Module *module);

	private:
		QAction *m_leftModuleSeparator;
		QAction *m_rightModuleSeparator;
};

inline Module* ModuleToolBar::module() const
{
	return boost::polymorphic_cast<Module*>(this->parent());
}

}

}

#endif
