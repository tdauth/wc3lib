/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "weathereditor.hpp"
#include "weathertreemodel.hpp"
#include "weatherdata.hpp"

namespace wc3lib
{

namespace editor
{

WeatherEditor::WeatherEditor(MpqPriorityList* source, QWidget* parent, Qt::WindowFlags f) : ObjectEditorTab(source, new WeatherData(source), parent, f)
{
}

WeatherEditor::~WeatherEditor()
{
}

void WeatherEditor::onSwitchToMap(Map *map)
{
}

void WeatherEditor::onNewObject()
{
}

void WeatherEditor::onRenameObject()
{
}

void WeatherEditor::onDeleteObject()
{
}

void WeatherEditor::onResetObject()
{
}

void WeatherEditor::onResetAllObjects()
{
}

void WeatherEditor::onExportAllObjects()
{
}

void WeatherEditor::onImportAllObjects()
{
}

void WeatherEditor::onCopyObject()
{
}

void WeatherEditor::onPasteObject()
{
}

void WeatherEditor::onShowRawData(bool show)
{
}

void WeatherEditor::activateFolder(ObjectTreeItem* item)
{
}

void WeatherEditor::activateObject(ObjectTreeItem* item)
{
}

ObjectTreeModel* WeatherEditor::createTreeModel()
{
	return new WeatherTreeModel(this->source(), this);
}

WeatherData* WeatherEditor::weatherData() const
{
	return boost::polymorphic_cast<WeatherData*>(objectData());
}

QString WeatherEditor::name() const
{
	return tr("Weather Editor");
}

}

}
