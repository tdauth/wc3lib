/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#include "objectlistwidget.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

void ObjectListWidget::addObject()
{
	qDebug() << "Select";
}

ObjectListWidget::ObjectListWidget(MpqPriorityList *source, ObjectData *objectData, QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f), m_source(source), m_objectData(objectData)
{
	setupUi(this);

	connect(this->m_addPushButton, SIGNAL(triggered()), this, SLOT(addObject()));
}

void ObjectListWidget::load(ObjectIds &objects)
{
	foreach (ObjectData::ObjectId objectId, objects)
	{
		const QString name = this->objectData()->objectName(objectId.first, objectId.second);
		//QIcon icon = this->source()->sharedData()->icon()
		QListWidgetItem *item = new QListWidgetItem(name);
		QString data = QString("%1:%2").arg(objectId.first).arg(objectId.second);
		item->setData(Qt::UserRole, data);
		this->m_listWidget->addItem(item);
	}
}

ObjectListWidget::ObjectIds ObjectListWidget::objects() const
{
	ObjectIds result;

	for (int i = 0; i < this->m_listWidget->count(); ++i)
	{
		const QString data = this->m_listWidget->item(i)->data(Qt::UserRole).toString();
		QStringList list = data.split(':');

		result.push_back(ObjectData::ObjectId(list[0], list[1]));
	}

	return result;
}

}

}
