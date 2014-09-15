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
				result.push_back(child);
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

void ObjectTreeModel::clearCustomItems()
{
	foreach (ObjectTreeItem *item, this->m_customItems)
	{
		delete item;
	}

	this->m_customItems.clear();
}

}

}
