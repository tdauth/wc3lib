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

#include "misctreemodel.hpp"
#include "miscdata.hpp"
#include "../mpqprioritylist.hpp"
#include "../warcraftiiishared.hpp"

namespace wc3lib
{

namespace editor
{

MiscTreeModel::MiscTreeModel(MpqPriorityList *source, QObject *parent)
: ObjectTreeModel(source, parent)
{
	// custom and standard items
	QStringList names;
	names << "standard";
	names << "custom";

	insertRowFolders(names, 0);
}

void MiscTreeModel::load(MpqPriorityList *source, ObjectData *objectData, QWidget *window)
{
	MiscData *miscData = dynamic_cast<MiscData*>(objectData);

	if (miscData != 0)
	{
		createItem(source, objectData, window, "miscGame", "");
		createItem(source, objectData, window, "customv0", "");
		createItem(source, objectData, window, "customv1", "");
		createItem(source, objectData, window, "meleev0", "");
	}

	ObjectTreeModel::load(source, objectData, window);
}

ObjectTreeItem* MiscTreeModel::createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString& originalObjectId, const QString& customObjectId)
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

QModelIndex MiscTreeModel::itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	if (customObjectId.isEmpty())
	{
		return this->index(0, 0);
	}

	return this->index(1, 0);
}

}

}
