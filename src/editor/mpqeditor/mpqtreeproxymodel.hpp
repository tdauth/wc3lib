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

#ifndef WC3LIB_EDITOR_MPQTREEPROXYMODEL_HPP
#define WC3LIB_EDITOR_MPQTREEPROXYMODEL_HPP

#include <QSortFilterProxyModel>

namespace wc3lib
{

namespace editor
{

/**
 * \brief Orders tree items by type and name which means that directories are listed before files.
 */
class MpqTreeProxyModel : public QSortFilterProxyModel
{
	public:
		MpqTreeProxyModel(QObject *parent = 0);

	protected:
		virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
		virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

		bool filterAcceptsRowItself(int source_row, const QModelIndex &source_parent) const;
		bool hasAcceptedChildren(int source_row, const QModelIndex &source_parent) const;
};

}

}

#endif // WC3LIB_EDITOR_MPQTREEPROXYMODEL_HPP
