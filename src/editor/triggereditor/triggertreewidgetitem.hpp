/***************************************************************************
 *   Copyright (C) 2013 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_TRIGGEREDITOR_TRIGGERTREEWIDGETITEM_HPP
#define WC3LIB_EDITOR_TRIGGEREDITOR_TRIGGERTREEWIDGETITEM_HPP

#include <QTreeWidget>

#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class TriggerTreeWidgetItem : public QTreeWidgetItem
{
	public:
		TriggerTreeWidgetItem(map::Trigger *trigger, QTreeWidgetItem *parent, const QStringList& strings = QStringList(), int type = Type);

		map::Trigger* trigger() const;

	private:
		map::Trigger *m_trigger;;
};

inline map::Trigger* TriggerTreeWidgetItem::trigger() const
{
	return this->m_trigger;
}

}

}

#endif // TRIGGERTREEWIDGETITEM_H
