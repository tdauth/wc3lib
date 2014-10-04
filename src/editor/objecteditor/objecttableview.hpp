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

#ifndef WC3LIB_EDITOR_OBJECTABLEVIEW_HPP
#define WC3LIB_EDITOR_OBJECTABLEVIEW_HPP

#include <QTableView>
#include <QSortFilterProxyModel>
#include <QLinkedList>

#include "../map.hpp"
#include "../metadata.hpp"

namespace wc3lib
{

namespace editor
{

class ObjectEditorTab;
class ObjectTableModel;

/**
 * \brief Table view of the object editor which lists all values of the currently selected object.
 *
 * Provides a table with two columns. The first column lists the descriptions of all fields of the currently selected object.
 * The second column lists the values of the fields of the currently selected object.
 * Double clicking a row allows to modify one single field using a \ref ObjectValueDialog dialog.
 *
 * All rows can be accessed using \ref pairs() which returns a hash using the field ID as key and the pair of cells as value.
 *
 * \ingroup objectdata
 */
class ObjectTableView : public QTableView
{
	Q_OBJECT

	public slots:
		void modifyField();
		void resetField();

	public:
		/**
		 * Creates a new table widget using the tab \p parent as parent widget and reference for all field data.
		 */
		ObjectTableView(ObjectEditorTab *parent);

		ObjectEditorTab* tab() const;
		QSortFilterProxyModel* sortFilterProxyModel() const;
		ObjectTableModel* tableModel() const;

		/**
		 * Selects the row with field ID \p fieldId.
		 */
		void selectField(const QString &fieldId);

	protected:
		ObjectEditorTab *m_tab;

		QMenu *m_contextMenu;
		QAction *m_modifyField;
		QAction *m_resetField;

	private slots:
		/**
		 * Items are not edited directly like usual table items. Usually a simple dialog with OK and cancel button is shown.
		 * This functions handles all edit requests!
		 */
		void editItem(const QModelIndex &index);
		void resetItem(const QModelIndex &index);
		void customContextMenuRequested(QPoint pos);
};

inline ObjectEditorTab* ObjectTableView::tab() const
{
	return this->m_tab;
}

inline QSortFilterProxyModel* ObjectTableView::sortFilterProxyModel() const
{
	return boost::polymorphic_cast<QSortFilterProxyModel*>(this->model());
}

inline ObjectTableModel* ObjectTableView::tableModel() const
{
	return boost::polymorphic_cast<ObjectTableModel*>(sortFilterProxyModel()->sourceModel());
}

}

}

#endif
