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

#include <QtGui>

#include "objecttablewidgetpair.hpp"
#include "objecteditor.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTableWidgetPair::ObjectTableWidgetPair(QTableWidget *tableWidget, ObjectEditorTab *tab, const map::Slk::Cell &objectId, const map::Slk::Cell &fieldId)
: QObject(tableWidget)
, m_tab(tab)
, m_descriptionItem(new QTableWidgetItem())
, m_valueItem(new QTableWidgetItem())
, m_objectId(objectId)
, m_fieldId(fieldId)
{
	descriptionItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); // not editable!
	valueItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable); // editable!
	//descriptionItem()->setText(this->metaData()->displayName().c_str());
}

ObjectTableWidgetPair::~ObjectTableWidgetPair()
{
	delete m_descriptionItem;
	delete m_valueItem;
}

void ObjectTableWidgetPair::update()
{
	//if (data() != 0)
	//{
		valueItem()->setData(Qt::DisplayRole, valueToVariant(customValue()));

		if (!isDefault())
		{
			descriptionItem()->setForeground(Qt::magenta);
			valueItem()->setForeground(Qt::magenta);
		}
		else
		{
			descriptionItem()->setForeground(Qt::black);
			valueItem()->setForeground(Qt::black);
		}

	//}
}

}

}
