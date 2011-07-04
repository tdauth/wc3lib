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

namespace wc3lib
{

namespace editor
{

class ObjectTableWidget : public QTableWidget
{
	public:
		ObjectTableWidget(QWidget *parent, const QLinkedList<map::MetaData*> &metaData);
		
		/**
		 * Updates table by reading new data for corresponding meta data descriptions from list \p data.
		 * \todo Use ObjectTableWidgetPair::void setData(map::Data *data); to assign new data.
		 */
		void update(const QLinkedList<map::Data*> &data);
		
		const QLinkedList<class ObjectTableWidgetPair*> pairs() const;
		const QLinkedList<class ObjectTableWidgetPair*> pairs(map::Section section) const;
	
	protected:
		/**
		 * Items are not edited directly like usual table items. Usually a simple dialog with OK and cancel button is shown.
		 */
		virtual void editItem(QTableWidgetItem *item);
		
		QLinkedList<class ObjectTableWidgetPair*> m_pairs;
};

}

}

#endif
