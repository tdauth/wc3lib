/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#include "doodadeditor.hpp"
#include "objecttreeview.hpp"
#include "objecttreeitem.hpp"
#include "doodadtreemodel.hpp"
#include "../metadata.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

DoodadEditor::DoodadEditor(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent, Qt::WindowFlags f) : ObjectEditorTab(source, objectData, "doodadeditor", objectEditor, parent, f)
{
	setupUi();
}

DoodadEditor::~DoodadEditor()
{
}

ObjectTreeModel* DoodadEditor::createTreeModel()
{
	return new DoodadTreeModel(this->source(), this, this);
}

void DoodadEditor::onSwitchToMap(Map *map)
{
}

void DoodadEditor::onNewObject()
{
}

}

}
