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
#include "../sharedobjectdata.hpp"
#include "../mpqprioritylist.hpp"
#include "objectvaluedialog.hpp"

namespace wc3lib
{

namespace editor
{

ObjectListDialog *ObjectListDialog::m_dialog = 0;

void ObjectListDialog::addObject()
{
	const QString fieldId = this->fieldId().fieldId();
	const QString type = this->objectData()->metaData()->value(fieldId, "type");

	if (objectData()->fieldTypeIsLiteralList(type))
	{
		const QString fieldType = this->objectData()->fieldLiteralTypeFromListType(type);
		const QString stringExt = objectData()->metaData()->value(fieldId, "stringExt");
		const QString maxValue = objectData()->metaData()->hasValue(fieldId, "maxValue") ? objectData()->metaData()->value(fieldId, "maxValue") : "";
		const QString minValue = objectData()->metaData()->hasValue(fieldId, "minValue") ? objectData()->metaData()->value(fieldId, "minValue") : "";
		const QString fieldValue = "";
		const QString fieldReadableValue = "";
		QString result;

		if (ObjectValueDialog::getValue(result, fieldType, fieldValue, fieldReadableValue, stringExt, maxValue, minValue, this->objectData(), tr("test"), this) == QDialog::Accepted)
		{
			QStringList objects = this->objects();
			objects.prepend(result);
			this->load(objects);
		}
	}
	else
	{
	}
}

void ObjectListDialog::editObject()
{
	QList<QListWidgetItem*> selection = this->m_listWidget->selectedItems();

	if (!selection.isEmpty())
	{
		const QString fieldId = this->fieldId().fieldId();
		const QString type = this->objectData()->metaData()->value(fieldId, "type");

		if (objectData()->fieldTypeIsLiteralList(type))
		{
			const QString fieldType = this->objectData()->fieldLiteralTypeFromListType(type);
			const QString stringExt = objectData()->metaData()->value(fieldId, "stringExt");
			const QString maxValue = objectData()->metaData()->hasValue(fieldId, "maxValue") ? objectData()->metaData()->value(fieldId, "maxValue") : "";
			const QString minValue = objectData()->metaData()->hasValue(fieldId, "minValue") ? objectData()->metaData()->value(fieldId, "minValue") : "";
			const QString fieldValue = selection.first()->data(Qt::UserRole).toString();
			const QString fieldReadableValue = selection.first()->text();
			QString result;

			if (ObjectValueDialog::getValue(result, fieldType, fieldValue, fieldReadableValue, stringExt, maxValue, minValue, this->objectData(), tr("test"), this) == QDialog::Accepted)
			{
				selection.first()->setData(Qt::UserRole, result);
				selection.first()->setText(result);
			}
		}
		else
		{
		}
	}
}

void ObjectListDialog::removeObject()
{
	QList<QListWidgetItem*> selection = this->m_listWidget->selectedItems();

	if (!selection.isEmpty())
	{
		int oldRow = selection.first()->listWidget()->row(selection.first());
		this->m_listWidget->removeItemWidget(selection.first());
		QStringList list = this->objects();
		list.removeAt(oldRow);
		qDebug() << "Remove item at" << oldRow;
		this->load(list);
	}
}

void ObjectListDialog::moveObjectUp()
{
	QList<QListWidgetItem*> selection = this->m_listWidget->selectedItems();

	if (!selection.isEmpty())
	{
		int oldRow = selection.first()->listWidget()->row(selection.first());

		if (oldRow > 0)
		{
			QStringList list = this->objects();
			list.swap(oldRow, oldRow - 1);
			this->load(list);
		}
	}
}

void ObjectListDialog::moveObjectDown()
{
	QList<QListWidgetItem*> selection = this->m_listWidget->selectedItems();

	if (!selection.isEmpty())
	{
		int oldRow = selection.first()->listWidget()->row(selection.first());

		if (oldRow < this->m_listWidget->count() - 1)
		{
			QStringList list = this->objects();
			list.swap(oldRow, oldRow + 1);
			this->load(list);
		}
	}
}

ObjectListDialog::ObjectListDialog(MpqPriorityList *source, ObjectData *objectData, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_source(source), m_objectData(objectData), m_fieldTypeObjectData(0)
{
	setupUi(this);

	connect(this->m_addPushButton, SIGNAL(clicked(bool)), this, SLOT(addObject()));
	connect(this->m_editPushButton, SIGNAL(clicked(bool)), this, SLOT(editObject()));
	connect(this->m_removePushButton, SIGNAL(clicked(bool)), this, SLOT(removeObject()));
	connect(this->m_moveUpPushButton, SIGNAL(clicked(bool)), this, SLOT(moveObjectUp()));
	connect(this->m_moveDownPushButton, SIGNAL(clicked(bool)), this, SLOT(moveObjectDown()));
	connect(this->m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editObject()));
}

void ObjectListDialog::load(const QStringList &objects)
{
	qDebug() << "Loading objects with size" << objects.size();
	this->m_listWidget->clear();
	const QString fieldId = this->fieldId().fieldId();
	const QString type = this->objectData()->metaData()->value(fieldId, "type");


	if (this->objectData()->fieldTypeIsObjectList(type))
	{
		foreach (QString objectId, objects)
		{
			const QStringList list = objectId.split(':');
			const QString originalObjectId = list.size() >= 1 ? list[0] : "";
			const QString customObjectId = list.size() >= 2 ? list[1] : "";
			QString name = this->fieldTypeObjectData()->objectName(originalObjectId, customObjectId);

			if (name.isEmpty())
			{
				name = QString("%1:%2").arg(originalObjectId).arg(customObjectId);
			}

			QIcon icon = this->fieldTypeObjectData()->objectIcon(originalObjectId, customObjectId, this);
			QListWidgetItem *item = new QListWidgetItem(icon, name);
			QString data;

			if (!customObjectId.isEmpty())
			{
				data = QString("%1:%2").arg(originalObjectId).arg(customObjectId);
			}
			else
			{
				data = originalObjectId;
			}

			item->setData(Qt::UserRole, data);
			this->m_listWidget->addItem(item);
		}
	}
	else if (this->objectData()->fieldTypeIsLiteralList(type))
	{
		foreach (QString value, objects)
		{
			QListWidgetItem *item = new QListWidgetItem(value);
			item->setData(Qt::UserRole, value);
			this->m_listWidget->addItem(item);
		}
	}
	else
	{
		qDebug() << "Unsupported type" << type;
	}
}

QStringList ObjectListDialog::objects() const
{
	QStringList result;

	for (int i = 0; i < this->m_listWidget->count(); ++i)
	{
		const QString data = this->m_listWidget->item(i)->data(Qt::UserRole).toString();

		result.push_back(data);
	}

	return result;
}

int ObjectListDialog::getObjectIds(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level, ObjectData *objectData, SharedObjectData *sharedObjectData, const QString& label, QWidget* parent, Qt::WindowFlags f)
{
	const QString type = objectData->metaData()->value(fieldId, "type");
	const QString stringExt = objectData->metaData()->value(fieldId, "stringExt");
	ObjectData *fieldTypeObjectData = objectData;

	if (objectData->fieldTypeIsObjectList(type))
	{
		ObjectData *listObjectData = sharedObjectData->resolveByFieldType(type);

		if (listObjectData != 0)
		{
			if (!listObjectData->loadOnRequest(parent))
			{
				return QDialog::Rejected;
			}

			fieldTypeObjectData = listObjectData;
		}
		else
		{
			return QDialog::Rejected;
		}
	}

	const QString objects = objectData->fieldValue(originalObjectId, customObjectId, fieldId, level);
	qDebug() << "Objects:" << objects;
	/*
	 * Empty objects should not generate one single empty object.
	 */
	QStringList objectList = objects.isEmpty() ? QStringList() : objects.split(',');

	if (getObjectIds(objectList, originalObjectId, customObjectId, fieldId, level, objectData->source(), objectData, fieldTypeObjectData, label, parent, f) == QDialog::Accepted)
	{
		const QString value = objectList.join(",");
		objectData->modifyField(originalObjectId, customObjectId, fieldId, value, level);
	}

	return QDialog::Rejected;
}

int ObjectListDialog::getObjectIds(QStringList& result, const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level, MpqPriorityList* source, ObjectData* objectData, ObjectData* fieldTypeObjectData, const QString& label, QWidget* parent, Qt::WindowFlags f)
{
	if (m_dialog != 0)
	{
		delete m_dialog;
		m_dialog = 0;
	}

	m_dialog = new ObjectListDialog(source, objectData, parent, f);
	m_dialog->setWindowTitle(label);
	m_dialog->setFieldTypeObjectData(fieldTypeObjectData);
	m_dialog->setOriginalObjectId(originalObjectId);
	m_dialog->setCustomObjectId(customObjectId);
	m_dialog->setFieldId(ObjectData::FieldId(fieldId, level));
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
