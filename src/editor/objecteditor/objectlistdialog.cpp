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

#include "objectlistdialog.hpp"
#include "sharedobjectdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectListDialog *ObjectListDialog::m_dialog = 0;

void ObjectListDialog::addObject()
{
	qDebug() << "Select";
}

ObjectListDialog::ObjectListDialog(MpqPriorityList *source, ObjectData *objectData, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_source(source), m_objectData(objectData)
{
	setupUi(this);

	connect(this->m_addPushButton, SIGNAL(triggered()), this, SLOT(addObject()));
}

void ObjectListDialog::load(const QStringList &objects)
{
	foreach (QString objectId, objects)
	{
		const QStringList list = objectId.split(':');
		const QString originalObjectId = list.size() >= 1 ? list[0] : "";
		const QString customObjectId = list.size() >= 2 ? list[1] : "";
		const QString name = this->objectData()->objectName(originalObjectId, customObjectId);
		//QIcon icon = this->source()->sharedData()->icon()
		QListWidgetItem *item = new QListWidgetItem(name);
		QString data = QString("%1:%2").arg(originalObjectId).arg(customObjectId);
		item->setData(Qt::UserRole, data);
		this->m_listWidget->addItem(item);
	}
}

QStringList ObjectListDialog::objects() const
{
	QStringList result;

	for (int i = 0; i < this->m_listWidget->count(); ++i)
	{
		const QString data = this->m_listWidget->item(i)->data(Qt::UserRole).toString();
		QStringList list = data.split(':');

		result.push_back(list.join(","));
	}

	return result;
}

int ObjectListDialog::getObjectIds(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId, ObjectData *objectData, SharedObjectData *sharedObjectData, const QString& label, QWidget* parent, Qt::WindowFlags f)
{
	const QString type = objectData->metaData()->value(fieldId, "type");
	const QString stringExt = objectData->metaData()->value(fieldId, "stringExt");
	const QString fieldValue = objectData->fieldValue(originalObjectId, customObjectId, fieldId);
	ObjectData *listObjectData = sharedObjectData->resolveByFieldType(type);

	if (listObjectData != 0)
	{
		const QString objects = objectData->fieldValue(originalObjectId, customObjectId, fieldId);
		QStringList objectList = objects.split(',');

		if (getObjectIds(objectList, objectData->source(), objectData, label, parent, f) == QDialog::Accepted)
		{
			objectData->modifyField(originalObjectId, customObjectId, fieldId, objectList.join(","));
		}
	}

	return 0;
}

int ObjectListDialog::getObjectIds(QStringList& result, MpqPriorityList* source, ObjectData* objectData, const QString& label, QWidget* parent, Qt::WindowFlags f)
{
	if (m_dialog == 0)
	{
		m_dialog = new ObjectListDialog(source, objectData, parent, f);
	}
	else
	{
		m_dialog->setParent(parent);
		m_dialog->setWindowFlags(f);
		m_dialog->setSource(source);
		m_dialog->setObjectData(objectData);
	}

	m_dialog->setWindowTitle(label);
	m_dialog->load(result);

	const int exitCode = m_dialog->exec();

	if (exitCode == QDialog::Accepted)
	{
		result = m_dialog->objects();
	}

	return exitCode;
}

}

}
