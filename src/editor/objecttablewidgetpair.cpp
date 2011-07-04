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

ObjectTableWidgetPair::ObjectTableWidgetPair(QTableWidget *tableWidget, int row, map::MetaData *metaData) : QObject(tableWidget), m_metaData(metaData), m_data(0), m_descriptionItem(new QTableWidgetItem()), m_valueItem(new QTableWidgetItem()), m_row(row)
{
	tableWidget->setItem(row, 0, descriptionItem());
	tableWidget->setItem(row, 1, valueItem());
	descriptionItem()->setText(this->metaData()->displayName().c_str());
}

ObjectTableWidgetPair::~ObjectTableWidgetPair()
{
	tableWidget()->removeRow(row());
	delete m_descriptionItem;
	delete m_valueItem;
}

void ObjectTableWidgetPair::update()
{
	if (data() != 0)
	{
		valueItem()->setData(Qt::DisplayRole, ObjectEditor::valueToVariant(customValue()));
		
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
		
	}
}

}

}
