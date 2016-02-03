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

#include "doodadtreemodel.hpp"
#include "../destructabledata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

DoodadTreeModel::DoodadTreeModel(MpqPriorityList *source, QObject *parent) : ObjectTreeModel(source, parent)
{
	createObjects(source->sharedData().get());
}

ObjectTreeItem* DoodadTreeModel::createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString &originalObjectId, const QString &customObjectId)
{
	const QModelIndex parentIndex = itemParent(objectData, originalObjectId, customObjectId);
	ObjectTreeItem *parent = item(parentIndex);
	insertRows(parent->children().count(), 1, parentIndex);

	ObjectTreeItem *item = parent->children().last();
	item->setObjectData(objectData);
	item->setObjectId(originalObjectId, customObjectId);
	item->setIcon(objectData->objectIcon(originalObjectId, customObjectId, window));

	if (customObjectId.isEmpty())
	{
		this->insertStandardItem(item);
	}
	else
	{
		qDebug() << "Insert custom ability item";
		this->insertCustomItem(item);
	}

	return item;
}

QModelIndex DoodadTreeModel::itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	return objectsIndex(objectData, originalObjectId, customObjectId);
}

QModelIndex DoodadTreeModel::objectsIndex(ObjectData* objectData, const QString& originalObjectId, const QString &customObjectId)
{
	if (customObjectId.isEmpty())
	{
		return this->index(0, 0);
	}

	return this->index(1, 0);
}

void DoodadTreeModel::createObjects(WarcraftIIIShared *shared)
{
	QStringList names;
	names << shared->tr("WESTRING_DE_STANDARDDOODS");
	names << shared->tr("WESTRING_DE_CUSTOMDOODS");

	insertRowFolders(names, 0);
}

}

}
