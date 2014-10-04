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

#include <QtGui>

#include <KMessageBox>
#include <KFileDialog>

#include "itemeditor.hpp"
#include "objecttreeview.hpp"
#include "objecttreeitem.hpp"
#include "itemtreemodel.hpp"
#include "../metadata.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

ItemEditor::ItemEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : ObjectEditorTab(source, new ItemData(source, parent), parent, f)
{
	setupUi();
}

ItemEditor::~ItemEditor()
{
}

ObjectTreeModel* ItemEditor::createTreeModel()
{
	return new ItemTreeModel(this->source(), this);
}

void ItemEditor::onSwitchToMap(Map *map)
{
}

void ItemEditor::onNewObject()
{
}

void ItemEditor::onRenameObject()
{
}

void ItemEditor::onDeleteObject()
{
}

void ItemEditor::onResetObject()
{
}

void ItemEditor::onResetAllObjects()
{
}

void ItemEditor::onExportAllObjects()
{
}

void ItemEditor::onImportAllObjects()
{
}

void ItemEditor::onCopyObject()
{
}

void ItemEditor::onPasteObject()
{
}

void ItemEditor::onShowRawData(bool show)
{
	/*
	if (show)
	{
		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->standardItems().begin(); iterator != this->treeWidget()->standardItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, QString("%1 (%2)").arg(iterator.key().first).arg(unitName));
		}

		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->customItems().begin(); iterator != this->treeWidget()->customItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, QString("%1:%2 (%3)").arg(iterator.key().first).arg(iterator.key().second).arg(unitName));
		}
	}
	else
	{
		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->standardItems().begin(); iterator != this->treeWidget()->standardItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, unitName);
		}

		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->customItems().begin(); iterator != this->treeWidget()->customItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, unitName);
		}
	}
	*/
}

void ItemEditor::activateObject(ObjectTreeItem *item)
{
	qDebug() << "Activated" << item->originalObjectId();
	qDebug() << "with custom ID" << item->customObjectId();
}

void ItemEditor::activateFolder(ObjectTreeItem *item)
{
}

}

}