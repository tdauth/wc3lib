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

#include <kmenu.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kmenubar.h>
#include <klocale.h>

#include <QtGui>

#include "modulemenu.hpp"
#include "editor.hpp"

namespace wc3lib
{

namespace editor
{

ModuleMenu::ModuleMenu(QWidget *widget, class Editor *editor) : KMenu(tr("Module"), widget)
{
	QAction *action = editor->actionCollection()->action("terraineditor");
	this->addAction(action);
	this->m_actions << action;
	action = editor->actionCollection()->action("triggereditor");
	this->addAction(action);
	this->m_actions << action;
	action = editor->actionCollection()->action("soundeditor");
	this->addAction(action);
	this->m_actions << action;
	action = editor->actionCollection()->action("objecteditor");
	this->addAction(action);
	this->m_actions << action;
	action = editor->actionCollection()->action("campaigneditor");
	this->addAction(action);
	this->m_actions << action;
	action = editor->actionCollection()->action("aieditor");
	this->addAction(action);
	this->m_actions << action;
	action = editor->actionCollection()->action("modeleditor");
	this->addAction(action);
	this->m_actions << action;
	action = editor->actionCollection()->action("textureeditor");
	this->addAction(action);
	this->m_actions << action;
	action = editor->actionCollection()->action("objectmanager");
	this->addAction(action);
	this->m_actions << action;
	action = editor->actionCollection()->action("importmanager");
	this->addAction(action);
	this->m_actions << action;
}

}

}
