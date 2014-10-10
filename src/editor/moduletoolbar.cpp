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

#include <QtGui>

#include <KAction>
#include <KActionCollection>

#include "moduletoolbar.hpp"
#include "editor.hpp"

namespace wc3lib
{

namespace editor
{

ModuleToolBar::ModuleToolBar(Module *module) : QToolBar(module), m_leftModuleSeparator(0), m_rightModuleSeparator(0)
{
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	//this->setMovable(false);
	//this->setToolButtonStyle(Qt::ToolButtonIconOnly);
	//this->setFloatable(true);

	if (module->hasEditor())
	{
		m_leftModuleSeparator = this->addSeparator();

		foreach (Module *mod, const_cast<const Editor*>(module->editor())->modules())
		{
			addModuleAction(mod);
		}

		connect(module->editor(), SIGNAL(createdModule(Module*)), this, SLOT(addModuleAction(Module*)));

		m_rightModuleSeparator = this->addSeparator();

		// test map tool button
		this->addAction(module->editor()->actionCollection()->action("testmap"));
	}
}

void ModuleToolBar::addCustomAction(QAction *action)
{
	if (this->m_leftModuleSeparator == 0)
	{
		this->addAction(action);
	}
	else
	{
		this->insertAction(this->m_leftModuleSeparator, action);
	}
}

void ModuleToolBar::addCustomSeparator()
{
	if (this->m_leftModuleSeparator == 0)
	{
		this->addSeparator();
	}
	else
	{
		this->insertSeparator(this->m_leftModuleSeparator);
	}
}

void ModuleToolBar::addModuleAction(Module *module)
{
	this->insertAction(m_rightModuleSeparator, const_cast<const Editor*>(this->module()->editor())->modulesActions()[module]);
}

#include "moc_moduletoolbar.cpp"

}

}
