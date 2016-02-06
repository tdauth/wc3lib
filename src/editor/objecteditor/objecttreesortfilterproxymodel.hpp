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

#ifndef WC3LIB_EDITOR_OBJECTTREESORTFILTERPROXYMODEL_HPP
#define WC3LIB_EDITOR_OBJECTTREESORTFILTERPROXYMODEL_HPP

#include <QSortFilterProxyModel>

namespace wc3lib
{

namespace editor
{

/**
 * \brief This sort filter proxy model allows to filter an object tree model for a certain expression.
 * The default QSortFilterProxyModel does not support filtering trees properly. For example if the filter expression matches a child item
 * of another item if the parent item does not match nothing will be shown.
 * This class passes filter expressions down for folder items in the tree model and looks for matching child items.
 * If at least one child item of a folder item matches, it won't be hidden.
 *
 * Additionally it supports alphabetical order.
 */
class ObjectTreeSortFilterProxyModel : public QSortFilterProxyModel
{
	public:
		ObjectTreeSortFilterProxyModel(QObject *parent = 0);

	protected:
		virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

		virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

}

}

#endif
