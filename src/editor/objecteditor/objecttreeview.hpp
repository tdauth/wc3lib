/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_OBJECTREEVIEW_HPP
#define WC3LIB_EDITOR_OBJECTREEVIEW_HPP

#include <QTreeView>
#include <QMenu>

#include "../../map.hpp"
#include "objecttreeitem.hpp"

namespace wc3lib
{

namespace editor
{

class ObjectEditorTab;

/**
 * \brief A tree view for a list of standard and custom objects.
 *
 * Object tree views can be used to display game/campaign/map objects in a tree style divided into groups (e. g. their races, players etc.).
 * It allows selecting and therefore activating specific objects of the list as well as resetting them.
 *
 * \ingroup objectdata
 */
class ObjectTreeView : public QTreeView
{
	Q_OBJECT

	public:
		ObjectTreeView(ObjectEditorTab *parent = 0, Qt::WindowFlags f = 0);

		ObjectEditorTab* tab() const;
		QMenu* contextMenu() const;

		/**
		 * \return Returns true if only folders and no objects are selected.
		 */
		bool onlyFoldersSelected() const;
		/**
		 * \return Returns true if at least one custom object is selected.
		 */
		bool isCustomObjectSelected() const;

	private:
		ObjectEditorTab *m_tab;
		QMenu *m_contextMenu;

	private slots:
		void customContextMenuRequested(QPoint pos);
		void updateActions(const QItemSelection &selected, const QItemSelection &deselected);
		void expandItem(const QModelIndex &index);
		void collapseItem(const QModelIndex &index);
		void orderBySection(int logicalIndex);
};

inline ObjectEditorTab* ObjectTreeView::tab() const
{
	return this->m_tab;
}

inline QMenu* ObjectTreeView::contextMenu() const
{
	return this->m_contextMenu;
}

inline bool ObjectTreeView::onlyFoldersSelected() const
{
	const QItemSelectionModel *selection = this->selectionModel();

	foreach (QModelIndex index, selection->selectedIndexes())
	{
		const ObjectTreeItem *item = static_cast<const ObjectTreeItem*>(index.internalPointer());

		/*
		 * At least one selected item must be a non folder item.
		 */
		if (item->children().count() == 0 || !item->originalObjectId().isEmpty())
		{
			return false;
		}
	}

	return true;
}

inline bool editor::ObjectTreeView::isCustomObjectSelected() const
{
	const QItemSelectionModel *selection = this->selectionModel();

	foreach (QModelIndex index, selection->selectedIndexes())
	{
		const ObjectTreeItem *item = static_cast<const ObjectTreeItem*>(index.internalPointer());

		/*
		 * At least one selected item must be a custom object item.
		 */
		if (!item->customObjectId().isEmpty())
		{
			return true;
		}
	}

	return false;
}

}

}

#endif
