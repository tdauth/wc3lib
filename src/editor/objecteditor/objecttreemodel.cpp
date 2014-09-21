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

#include "objecttreemodel.hpp"
#include "objectdata.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTreeModel::ObjectTreeModel(QObject *parent) : QAbstractItemModel(parent)
{
}

ObjectTreeModel::~ObjectTreeModel()
{
	qDeleteAll(this->m_topLevelItems);
}

ObjectTreeItem::Children ObjectTreeModel::folders()
{
	QStack<ObjectTreeItem*> items;

	foreach (ObjectTreeItem *item, this->m_topLevelItems)
	{
		items.push(item);
	}

	ObjectTreeItem::Children result;

	while (!items.isEmpty())
	{
		ObjectTreeItem *item = items.pop();
		result.push_back(item);

		foreach (ObjectTreeItem *child, item->children())
		{
			if (child->isFolder())
			{
				items.push(child);
			}
		}
	}

	return result;
}

QVariant ObjectTreeModel::data(const QModelIndex &index, int role) const
{

     if (!index.isValid())
     {
         return QVariant();
     }

     ObjectTreeItem *item = static_cast<ObjectTreeItem*>(index.internalPointer());

     switch (role)
     {
	     case Qt::UserRole:
	     {
		     return QString("%1:%2").arg(item->originalObjectId()).arg(item->customObjectId());
	     }

	     case Qt::DisplayRole:
	     {
		     return item->text(false);
	     }

	     case Qt::DecorationRole:
	     {
		     return item->icon();
	     }

	     case Qt::ForegroundRole:
	     {
		     return item->foreground();
	     }
     }

     return QVariant();
}

int ObjectTreeModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

int ObjectTreeModel::rowCount(const QModelIndex &parent) const
{
	ObjectTreeItem *parentItem = 0;

	if (!parent.isValid())
	{
		return m_topLevelItems.size();
	}
	else
	{
		parentItem = static_cast<ObjectTreeItem*>(parent.internalPointer());
	}

	return parentItem->children().size();
}

QModelIndex ObjectTreeModel::parent(const QModelIndex &index) const
{
	 if (!index.isValid())
	 {
		return QModelIndex();
	 }

	ObjectTreeItem *childItem = static_cast<ObjectTreeItem*>(index.internalPointer());
	ObjectTreeItem *parentItem = childItem->parent();

	if (parentItem == 0)
	{
		return QModelIndex();
	}

	int parentRow = 0;

	/*
	 * If it is a top level item we have to calculate the row manually.
	 */
	if (parentItem->parent() == 0)
	{
		parentRow = m_topLevelItems.indexOf(parentItem);

		if (parentRow == -1)
		{
			qDebug() << "Missing top level item:" << parentItem->text(false);
		}
	}
	else
	{
		parentRow = parentItem->row();
	}

	return createIndex(parentRow, 0, parentItem);
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent) || column != 0)
	{
		return QModelIndex();
	}

	ObjectTreeItem *parentItem;

	if (!parent.isValid())
	{
		parentItem = 0;
	}
	else
	{
		parentItem = static_cast<ObjectTreeItem*>(parent.internalPointer());
	}

	ObjectTreeItem *childItem = 0;

	if (parentItem != 0)
	{
		childItem = parentItem->child(row);
	}
	else if (row >= 0 && row < m_topLevelItems.size())
	{
		childItem = m_topLevelItems.at(row);
	}
	else
	{
		qDebug() << "Error error error:" << row;
	}

	if (childItem != 0)
	{
		return createIndex(row, column, childItem);
	}
	else
	{
		return QModelIndex();
	}
}

QVariant ObjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// no header data available
	return QVariant();
}

bool ObjectTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
	qDebug() << "Insert rows!";
	ObjectTreeItem *parentItem = item(parent);
	ObjectTreeItem::Children children = parentItem->children();

	for (int i = row; i < count; ++i)
	{
		ObjectTreeItem *item = new ObjectTreeItem(parentItem->objectData(), "", "", parentItem);

		if (i >= children.size())
		{
			children.push_back(item);
		}
		else
		{
			children[i] = item;
		}
	}

	parentItem->setChildren(children);


	return true;
}

bool ObjectTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
	qDebug() << "Remove rows!";
	ObjectTreeItem *parentItem = item(parent);
	qDebug() << "Parent:" << parentItem->text(false);
	qDebug() << "Rows:" << parentItem->children().count();
	qDebug() << "Row + count:" << row + count;

	if (row + count > parentItem->children().count())
	{
		return false;
	}

	beginRemoveRows(parent, row, count);

	for (int i = row; i < count; ++i)
	{
		delete parentItem->children().takeAt(i);
	}


	endRemoveRows();

	return true;
}

void ObjectTreeModel::clearCustomItems()
{
	foreach (ObjectTreeItem *item, this->m_customItems)
	{
		delete item;
	}

	this->m_customItems.clear();
}

ObjectTreeItem* ObjectTreeModel::item(const QModelIndex &index) const
{
	return (ObjectTreeItem*)(index.internalPointer());
}

void ObjectTreeModel::load(MpqPriorityList* source, ObjectData *objectData, QWidget *window)
{
	connect(objectData, SIGNAL(objectCreation(const QString&, const QString&)), this, SLOT(createObject(const QString&, const QString&)));
	connect(objectData, SIGNAL(objectRemoval(const QString&, const QString&)), this, SLOT(removeObject(const QString&, const QString&)));
	connect(objectData, SIGNAL(objectReset(const QString&, const QString&)), this, SLOT(resetObject(const QString&, const QString&)));
	connect(objectData, SIGNAL(fieldModification(const QString &, const QString &, const QString &)), this, SLOT(modifyField(const QString&, const QString&, const QString&)));
}

void ObjectTreeModel::createObject(const QString &originalObjectId, const QString &customObjectId)
{
	ObjectData *objectData = dynamic_cast<ObjectData*>(sender());
	qDebug() << "Object data:" << objectData;
	qDebug() << "Original object:" << originalObjectId;
	qDebug() << "Custom object:" << customObjectId;
	// TODO insert item
	// createItem should emit all signals for adding new rows
	if (objectData != 0)
	{
		ObjectTreeItem *item = this->createItem(objectData->source(), objectData, 0, originalObjectId, customObjectId);
	}
}

void ObjectTreeModel::removeObject(const QString &originalObjectId, const QString &customObjectId)
{
	ObjectData *objectData = dynamic_cast<ObjectData*>(sender());
	qDebug() << "Object data:" << objectData;
	qDebug() << "Original object:" << originalObjectId;
	qDebug() << "Custom object:" << customObjectId;
	// TODO insert item
	// createItem should emit all signals for adding new rows
	if (objectData != 0)
	{
		ObjectTreeItem *item = this->item(originalObjectId, customObjectId);
		ObjectTreeItem *parent = item->parent();
		QModelIndex parentIndex;
		int row = 0;

		qDebug() << "Item:" << item->text(false);
		qDebug() << "Custom item:" <<  this->m_customItems.find(ObjectId(originalObjectId, customObjectId)).value()->text(false);

		if (parent != 0)
		{
			parentIndex = this->parent(item->modelIndex(this));
			row = item->row();
		}
		else
		{
			row = this->topLevelRow(item);
		}

		removeRows(row, 1, parentIndex);

	}
}

void ObjectTreeModel::resetObject(const QString& originalObjectId, const QString& customObjectId)
{
	ObjectTreeItem *item = this->item(originalObjectId, customObjectId);

	if (item != 0)
	{
		QModelIndex index = item->modelIndex(this);

		emit dataChanged(index, index);
	}
	else
	{
		qDebug() << "Missing" << originalObjectId << customObjectId;
	}
}

void ObjectTreeModel::modifyField(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId)
{
	ObjectData *objectData = boost::polymorphic_cast<ObjectData*>(QObject::sender());
	ObjectTreeItem *item = this->item(originalObjectId, customObjectId);

	if (item != 0)
	{
		ObjectTreeItem *parent = this->itemParent(objectData, originalObjectId, customObjectId);
		QModelIndex index = item->modelIndex(this);

		/*
		 * Signal that data have changed.
		 * For example if the name has changed it will be updated automatically in the view.
		 */
		emit dataChanged(index, index);

		/*
		 * If the item would have a new parent it has been moved.
		 * For example it would have a new parent if the race or unit type has been changed.
		 */
		if (item->parent() != parent)
		{
			qDebug() << "Move the row!!";
			beginMoveRows(index, item->row(), item->row(), parent->modelIndex(this), parent->children().size());
			endMoveRows();

		}
	}
	else
	{
		qDebug() << "Missing" << originalObjectId << customObjectId;
	}
}

}

}
