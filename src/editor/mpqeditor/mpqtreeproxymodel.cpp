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

#include "mpqtreeproxymodel.hpp"
#include "mpqtreemodel.hpp"
#include "mpqtreeitem.hpp"

namespace wc3lib
{

namespace editor
{

MpqTreeProxyModel::MpqTreeProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

bool MpqTreeProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	MpqTreeModel *sourceModel = boost::polymorphic_cast<MpqTreeModel*>(this->sourceModel());

	const MpqTreeItem *leftItem = sourceModel->item(left);
	const MpqTreeItem *rightItem = sourceModel->item(right);
	QVariant leftData = sourceModel->data(left, Qt::DisplayRole);
	QVariant rightData = sourceModel->data(right, Qt::DisplayRole);

	if (leftItem != nullptr && rightItem != nullptr)
	{
		// folders always have a higher priority than regular files
		if (leftItem->isFolder() && !rightItem->isFolder())
		{
			//qDebug() << "Comparing names but left is folder" << leftItem->filePath().toUpper() << left.row() << rightItem->filePath().toUpper() << right.row();

			return true;
		}
		else if (!leftItem->isFolder() && rightItem->isFolder())
		{
			//qDebug() << "Comparing names but right is folder" << leftItem->filePath().toUpper() << left.row() << rightItem->filePath().toUpper() << right.row();

			return false;
		}

		// compare case insensitive otherwise big letters will be at a different position
		//qDebug() << "Comparing names" << leftItem->name().toUpper() << left.row() << rightItem->name().toUpper() << right.row();
		//qDebug() << "Comparing names" << leftItem->filePath().toUpper() << left.row() << rightItem->filePath().toUpper() << right.row();

		//return QString::compare(leftItem->name(), rightItem->name(), Qt::CaseInsensitive);
		return QString::compare(leftItem->filePath(), rightItem->filePath(), Qt::CaseInsensitive) < 0;
	}
	else
	{
		qDebug() << "Did not find one of the items.";
	}

	/*
	if (leftData.type() == QVariant::String && rightData.type() == QVariant::String)
	{
		// compare case insensitive otherwise big letters will be at a different position
		return leftItem->name().toUpper() < rightItem->name().toUpper();
	}
	else
	{
		qDebug() << "No string data!";
	}
	*/

	return QSortFilterProxyModel::lessThan(left, right);
}

bool MpqTreeProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if (filterAcceptsRowItself(source_row, source_parent))
	{
		return true;
	}

	// accept if any of the parents is accepted on it's own merits
	QModelIndex parent = source_parent;

	while (parent.isValid())
	{
		if (filterAcceptsRowItself(parent.row(), parent.parent()))
		{
			return true;
		}

		parent = parent.parent();
	}

	// accept if any of the children is accepted on it's own merits
	if (hasAcceptedChildren(source_row, source_parent))
	{
		return true;
	}

	return false;
}

bool MpqTreeProxyModel::filterAcceptsRowItself(int source_row, const QModelIndex &source_parent) const
{
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool MpqTreeProxyModel::hasAcceptedChildren(int source_row, const QModelIndex &source_parent) const
{
	QModelIndex item = sourceModel()->index(source_row,0,source_parent);

	if (!item.isValid())
	{
		return false;
	}

	//check if there are children
	int childCount = item.model()->rowCount(item);

	if (childCount == 0)
	{
		return false;
	}

	for (int i = 0; i < childCount; ++i)
	{
		if (filterAcceptsRowItself(i, item))
		{
			return true;
		}

		//recursive call -> NOTICE that this is depth-first searching, you're probably better off with breadth first search...
		if (hasAcceptedChildren(i, item))
		{
			return true;
		}
	}

	return false;
}

}

}
