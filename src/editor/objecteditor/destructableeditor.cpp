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

#include "destructableeditor.hpp"
#include "objecttreeview.hpp"
#include "objecttreeitem.hpp"
#include "destructabletreemodel.hpp"
#include "../metadata.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

DestructableEditor::DestructableEditor(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent) : ObjectEditorTab(source, objectData, "destructableditor", objectEditor, parent)
{
	setupUi();
}

DestructableEditor::~DestructableEditor()
{
}

ObjectTreeModel* DestructableEditor::createTreeModel()
{
	return new DestructableTreeModel(this->source(), this, this);
}

void DestructableEditor::onSwitchToMap(Map *map)
{
}

void DestructableEditor::onNewObject()
{
}

}

}
