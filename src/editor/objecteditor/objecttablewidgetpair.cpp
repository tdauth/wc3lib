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

#include <KLineEdit>

#include "objecttablewidgetpair.hpp"
#include "objecteditor.hpp"
#include "objectvaluedialog.hpp"
#include "../mpqprioritylist.hpp"
#include "../metadata.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTableWidgetPair::ObjectTableWidgetPair(QTableWidget *tableWidget, ObjectEditorTab *tab, int row, const QString &originalObjectId, const QString &customObjectId, const QString &fieldId)
: QObject(tableWidget)
, m_tab(tab)
, m_originalObjectId(originalObjectId)
, m_customObjectId(customObjectId)
, m_fieldId(fieldId)
, m_descriptionItem(new QTableWidgetItem())
, m_valueItem(new QTableWidgetItem())
{
	const QString displayName = tab->objectData()->metaData()->value(fieldId, "displayName");
	// cut "
	descriptionItem()->setText(tab->source()->sharedData()->tr(displayName));
	descriptionItem()->setData(Qt::UserRole, fieldId);
	valueItem()->setData(Qt::UserRole, fieldId);

	descriptionItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); // not editable!
	valueItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable); // editable!
	//descriptionItem()->setText(this->metaData()->displayName().c_str());

	tableWidget->setItem(row, 0, descriptionItem());
	tableWidget->setItem(row, 1, valueItem());
}

ObjectTableWidgetPair::~ObjectTableWidgetPair()
{
}

void ObjectTableWidgetPair::edit()
{
	const QString label = QString(tr("%1:")).arg(this->descriptionItem()->text());
	QString result;

	if (ObjectValueDialog::show(result, this->originalObjectId(), this->customObjectId(), this->fieldId(), this->tab()->objectData(), label, this->tab()) == QDialog::Accepted)
	{
		this->descriptionItem()->setForeground(Qt::magenta);
		this->valueItem()->setForeground(Qt::magenta);
		valueItem()->setText(result);
	}
}

QString ObjectTableWidgetPair::defaultValue() const
{
	return this->tab()->objectData()->defaultFieldValue(this->originalObjectId(), this->fieldId());
}

void ObjectTableWidgetPair::reset()
{
	const QString dataValue = this->defaultValue();
	this->valueItem()->setText(dataValue);
	this->descriptionItem()->setForeground(Qt::black);
	this->valueItem()->setForeground(Qt::black);
	this->tab()->objectData()->resetField(this->originalObjectId(), this->customObjectId(), this->fieldId());
}

}

}
