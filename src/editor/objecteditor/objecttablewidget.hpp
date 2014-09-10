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

#ifndef WC3LIB_EDITOR_OBJECTABLEWIDGET_HPP
#define WC3LIB_EDITOR_OBJECTABLEWIDGET_HPP

#include <QTableWidget>
#include <QLinkedList>

#include "../map.hpp"
#include "../metadata.hpp"

namespace wc3lib
{

namespace editor
{

class ObjectEditorTab;
class ObjectTableWidgetPair;

/**
 * \brief Table widget of the object editor which lists all values of the currently selected object.
 *
 * Provides a table with two columns. The first column lists the descriptions of all fields of the currently selected object.
 * The second column lists the values of the fields of the currently selected object.
 * Double clicking a row allows to modify one single field using a \ref ObjectValueDialog dialog.
 *
 * All rows can be accessed using \ref pairs() which returns a hash using the field ID as key and the pair of cells as value.
 */
class ObjectTableWidget : public QTableWidget
{
	Q_OBJECT

	public:
		/**
		 * All pairs are hashed by the ID of the field.
		 * The IDs of all fields can be found in the corresponding meta data file (\ref wc3lib::editor::ObjectEMetaData::metaData()).
		 */
		typedef QHash<QString, ObjectTableWidgetPair*> Pairs;

		/**
		 * Creates a new table widget using the tab \p parent as parent widget and reference for all field data.
		 */
		ObjectTableWidget(ObjectEditorTab *parent);

		ObjectEditorTab* tab() const;

		Pairs& pairs();
		const Pairs& pairs() const;

	protected:
		ObjectEditorTab *m_tab;
		Pairs m_pairs;

		QMenu *m_contextMenu;
		QAction *m_modifyField;
		QAction *m_resetField;

	private slots:
		/**
		 * Items are not edited directly like usual table items. Usually a simple dialog with OK and cancel button is shown.
		 * This functions handles all edit requests!
		 */
		void editItem(QTableWidgetItem *item);
		void customContextMenuRequested(QPoint pos);
		void modifyField();
		void resetField();
};

inline ObjectEditorTab* ObjectTableWidget::tab() const
{
	return this->m_tab;
}

inline ObjectTableWidget::Pairs& ObjectTableWidget::pairs()
{
	return m_pairs;
}

inline const ObjectTableWidget::Pairs& ObjectTableWidget::pairs() const
{
	return m_pairs;
}

}

}

#endif
