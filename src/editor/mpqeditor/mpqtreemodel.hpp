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

#ifndef WC3LIB_EDITOR_MPQTREEMODEL_HPP
#define WC3LIB_EDITOR_MPQTREEMODEL_HPP

#include <QAbstractItemModel>

#include "../../mpq.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class MpqTreeItem;

/**
 * \brief Tree model for MPQ archives and corresponding files.
 *
 * This model allows you to list several MPQ archives with their contained files in a tree form.
 * It uses columns for additional information such as the file sizes (compressed and uncompressed), extended attributes etc.
 * Besides it supports icons for folders and MIME types.
 */
class MpqTreeModel : public QAbstractItemModel
{
	public:
		typedef QList<MpqTreeItem*> FileItems;

		MpqTreeModel(QObject *parent = 0);
		virtual ~MpqTreeModel();

		virtual QVariant data(const QModelIndex& index, int role) const override;
		virtual int columnCount(const QModelIndex& parent) const override;
		virtual int rowCount(const QModelIndex& parent) const override;
		virtual QModelIndex parent(const QModelIndex& child) const override;
		virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

		virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
		virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

		MpqTreeItem* item(const QModelIndex &index) const;

		/**
		 * Adds a new MPQ archive to the model.
		 * All contained files from \p entries will be listed as tree items.
		 */
		void addArchive(mpq::Archive *archive, const mpq::Listfile::Entries &entries);
		void removeArchive(mpq::Archive *archive);

		void setSource(MpqPriorityList *source);
		MpqPriorityList* source() const;

		/**
		 * \return Returns all top level MPQ archive items.
		 *
		 * @{
		 */
		const FileItems& topLevelItems() const;
		FileItems& topLevelItems();
		/**
		 * @}
		 */

	private:
		MpqPriorityList *m_source;
		FileItems m_topLevelItems;
};

inline void MpqTreeModel::setSource(MpqPriorityList* source)
{
	this->m_source = source;
}

inline MpqPriorityList* MpqTreeModel::source() const
{
	return this->m_source;
}

inline const MpqTreeModel::FileItems& MpqTreeModel::topLevelItems() const
{
	return this->m_topLevelItems;
}

inline MpqTreeModel::FileItems& MpqTreeModel::topLevelItems()
{
	return this->m_topLevelItems;
}

}

}

#endif // WC3LIB_EDITOR_MPQTREEMODEL_HPP
