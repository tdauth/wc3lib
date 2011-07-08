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
#include "metadata.hpp"

namespace wc3lib
{

namespace editor
{

class ObjectTableWidget : public QTableWidget
{
	public:
		typedef QVector<QVariant> Entries;
		typedef QLinkedList<class ObjectTableWidgetPair*> Pairs;
		
		ObjectTableWidget(QWidget *parent, const MetaData *metaData);
		
		Pairs& pairs();
		const Pairs& pairs() const;
		Pairs pairs(BOOST_SCOPED_ENUM(map::Section) section);
	
	protected:
		/**
		 * Items are not edited directly like usual table items. Usually a simple dialog with OK and cancel button is shown.
		 * This functions handles all edit requests!
		 */
		virtual void editItem(QTableWidgetItem *item);
		
		Pairs m_pairs;
};

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
