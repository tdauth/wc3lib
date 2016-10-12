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

#include <QDebug>

#include "objecttreesortfilterproxymodel.hpp"
#include "objecttreemodel.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTreeSortFilterProxyModel::ObjectTreeSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

bool ObjectTreeSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
	const bool matchesItem = sourceModel()->data(index).toString().contains(filterRegExp());
	ObjectTreeModel *treeModel = dynamic_cast<ObjectTreeModel*>(sourceModel());

	//qDebug() << "Match: " << filterRegExp().pattern() << " with data " << sourceModel()->data(index).toString();

	// check recursively for folders
	if (treeModel->item(index)->isFolder() && !matchesItem)
	{
		for (int row = 0; row < treeModel->item(index)->children().count(); ++row)
		{
			if (this->filterAcceptsRow(row, index))
			{
				return true;
			}
		}
	}

	return matchesItem;
}

bool ObjectTreeSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	// TODO exclude head lines custom abilities and standard abilities! And exclude Heroes Items Units?

	return sourceModel()->data(left, Qt::DisplayRole).toString() < sourceModel()->data(right, Qt::DisplayRole).toString();
}

}

}
