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

#include "mpqtreeproxymodel.hpp"
#include "mpqtreemodel.hpp"
#include "mpqtreeitem.hpp"

namespace wc3lib
{

namespace editor
{

MpqTreeProxyModel::MpqTreeProxyModel(QObject* parent) : QSortFilterProxyModel(parent)
{
}

bool MpqTreeProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	/*
	MpqTreeModel *sourceModel = boost::polymorphic_cast<MpqTreeModel*>(this->sourceModel());

	MpqTreeItem *leftItem = sourceModel->item(mapToSource(left));
	MpqTreeItem *rightItem = sourceModel->item(mapToSource(right));

	if (!leftItem->isFolder() && rightItem->isFolder())
	{
		return true;
	}
	*/

	return QSortFilterProxyModel::lessThan(left, right);
}

}

}
