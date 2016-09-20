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

#include "watereditor.hpp"
#include "watertreemodel.hpp"
#include "../waterdata.hpp"

namespace wc3lib
{

namespace editor
{

WaterEditor::WaterEditor(MpqPriorityList* source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget* parent, Qt::WindowFlags f) : ObjectEditorTab(source, objectData, objectEditor, parent, f)
{
	setupUi();
}

WaterEditor::~WaterEditor()
{
}

void WaterEditor::onSwitchToMap(Map *map)
{
}

void WaterEditor::onNewObject()
{
}

ObjectTreeModel* WaterEditor::createTreeModel()
{
	return new WaterTreeModel(this->source(), this, this);
}

WaterData* WaterEditor::waterData() const
{
	return boost::polymorphic_cast<WaterData*>(objectData());
}

QString WaterEditor::name() const
{
	return tr("Water Editor");
}

}

}
