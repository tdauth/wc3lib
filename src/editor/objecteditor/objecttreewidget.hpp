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

#ifndef WC3LIB_EDITOR_OBJECTREEWIDGET_HPP
#define WC3LIB_EDITOR_OBJECTREEWIDGET_HPP

#include <QTreeWidget>
#include <QMenu>

namespace wc3lib
{

namespace editor
{

class ObjectEditorTab;
/**
 * Object tree widgets can be used to display game/campaign/map objects in a tree style divided into groups (e. g. their races, players etc.)
 *
 */
class ObjectTreeWidget : public QTreeWidget
{
	Q_OBJECT

	public:
		ObjectTreeWidget(ObjectEditorTab *parent = 0, Qt::WindowFlags f = 0);

		ObjectEditorTab* tab() const;
		QMenu* contextMenu() const;

	private:
		ObjectEditorTab *m_tab;

		QMenu *m_contextMenu;

	private slots:
		void customContextMenuRequested(QPoint pos);
};

inline ObjectEditorTab* ObjectTreeWidget::tab() const
{
	return this->m_tab;
}

inline QMenu* ObjectTreeWidget::contextMenu() const
{
	return this->m_contextMenu;
}

}

}

#endif
