/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#include "itemtreemodel.hpp"
#include "../itemdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ItemTreeModel::ItemTreeModel(MpqPriorityList *source, QWidget *window, QObject *parent) : ObjectTreeModel(source, window, parent)
{
	QStringList names;
	names << source->sharedData()->tr("WESTRING_IE_STANDARDITEMS", "WorldEditStrings");
	names << source->sharedData()->tr("WESTRING_IE_CUSTOMITEMS", "WorldEditStrings");

	insertRowFolders(names, 0);

	QStringList classes;
	classes << source->sharedData()->tr("WESTRING_ITEMCLASS_PERMANENT", "WorldEditStrings");
	classes << source->sharedData()->tr("WESTRING_ITEMCLASS_CHARGED", "WorldEditStrings");
	classes << source->sharedData()->tr("WESTRING_ITEMCLASS_POWERUP", "WorldEditStrings");
	classes << source->sharedData()->tr("WESTRING_ITEMCLASS_ARTIFACT", "WorldEditStrings");
	classes << source->sharedData()->tr("WESTRING_ITEMCLASS_PURCHASABLE", "WorldEditStrings");
	classes << source->sharedData()->tr("WESTRING_ITEMCLASS_CAMPAIGN", "WorldEditStrings");
	classes << source->sharedData()->tr("WESTRING_ITEMCLASS_MISCELLANEOUS", "WorldEditStrings");

	insertRowFolders(classes, 0, this->index(0, 0));
	insertRowFolders(classes, 0, this->index(1, 0));
}

ObjectTreeItem* ItemTreeModel::createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString &originalObjectId, const QString &customObjectId)
{
	const QModelIndex parentIndex = itemParent(objectData, originalObjectId, customObjectId);
	ObjectTreeItem *parent = item(parentIndex);
	insertRows(parent->children().count(), 1, parentIndex);

	ObjectTreeItem *item = parent->children().last();
	item->setObjectData(objectData);
	item->setObjectId(originalObjectId, customObjectId);

	if (customObjectId.isEmpty())
	{
		this->insertStandardItem(item);
	}
	else
	{
		this->insertCustomItem(item);
	}

	return item;
}

QModelIndex ItemTreeModel::itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	QModelIndex parent;

	if (customObjectId.isEmpty())
	{
		parent = index(0, 0);
	}
	else
	{
		parent = index(1, 0);
	}

	const QString itemClass = objectData->fieldValue(originalObjectId, customObjectId, "icla");

	if (itemClass == "Permanent")
	{
		return index(0, 0, parent);
	}
	else if (itemClass == "Charged")
	{
		return index(1, 0, parent);
	}
	else if (itemClass == "PowerUp")
	{
		return index(2, 0, parent);
	}
	else if (itemClass == "Artifact")
	{
		return index(3, 0, parent);
	}
	else if (itemClass == "Purchasable")
	{
		return index(4, 0, parent);
	}
	else if (itemClass == "Campaign")
	{
		return index(5, 0, parent);
	}
	else if (itemClass == "Miscellaneous")
	{
		return index(6, 0, parent);
	}

	qDebug() << "Unknown item class" << itemClass;

	return index(0, 0, parent);
}

}

}
