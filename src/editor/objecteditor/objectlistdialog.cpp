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

void ObjectListDialog::removeObject()
{
	QList<QListWidgetItem*> selection = this->m_listWidget->selectedItems();

	if (!selection.isEmpty())
	{
		int oldRow = selection.first()->listWidget()->row(selection.first());
		this->m_listWidget->removeItemWidget(selection.first());
		QStringList list = this->objects();
		list.removeAt(oldRow);
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

ObjectListDialog::ObjectListDialog(MpqPriorityList *source, ObjectData *objectData, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_source(source), m_objectData(objectData)
{
	setupUi(this);

	connect(this->m_addPushButton, SIGNAL(clicked(bool)), this, SLOT(addObject()));
	connect(this->m_removePushButton, SIGNAL(clicked(bool)), this, SLOT(removeObject()));
	connect(this->m_moveUpPushButton, SIGNAL(clicked(bool)), this, SLOT(moveObjectUp()));
	connect(this->m_moveDownPushButton, SIGNAL(clicked(bool)), this, SLOT(moveObjectDown()));
}

void ObjectListDialog::load(const QStringList &objects)
{
	this->m_listWidget->clear();

	foreach (QString objectId, objects)
	{
		const QStringList list = objectId.split(':');
		const QString originalObjectId = list.size() >= 1 ? list[0] : "";
		const QString customObjectId = list.size() >= 2 ? list[1] : "";
		QString name = this->objectData()->objectName(originalObjectId, customObjectId);

		if (name.isEmpty())
		{
			name = QString("%1:%2").arg(originalObjectId).arg(customObjectId);
		}

		QIcon icon = this->objectData()->objectIcon(originalObjectId, customObjectId, this);
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

int ObjectListDialog::getObjectIds(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId, ObjectData *objectData, SharedObjectData *sharedObjectData, const QString& label, QWidget* parent, Qt::WindowFlags f)
{
	const QString type = objectData->metaData()->value(fieldId, "type");
	const QString stringExt = objectData->metaData()->value(fieldId, "stringExt");
	const QString fieldValue = objectData->fieldValue(originalObjectId, customObjectId, fieldId);
	ObjectData *listObjectData = sharedObjectData->resolveByFieldType(type);

	if (listObjectData != 0)
	{
		if (listObjectData->metaData() == 0)
		{
			/*
			* Indicate loading by changing the cursor to busy.
			* The process of loading object data might take quite some time.
			*/
			QCursor cursor = parent->cursor();
			cursor.setShape(Qt::BusyCursor);
			parent->setCursor(cursor);

			try
			{
				listObjectData->load(parent);
			}
			catch (const Exception &e)
			{
				QMessageBox::warning(parent, tr("Error"), e.what());

				return QDialog::Rejected;
			}

			cursor = parent->cursor();
			cursor.setShape(Qt::ArrowCursor);
			parent->setCursor(cursor);
		}

		const QString objects = objectData->fieldValue(originalObjectId, customObjectId, fieldId);
		qDebug() << "Objects:" << objects;
		QStringList objectList = objects.split(',');

		if (getObjectIds(objectList, listObjectData->source(), listObjectData, label, parent, f) == QDialog::Accepted)
		{
			objectData->modifyField(originalObjectId, customObjectId, fieldId, objectList.join(","));
		}
	}
	else
	{
		qDebug() << "Missing object data for field type" << type;
	}

	return QDialog::Rejected;
}

int ObjectListDialog::getObjectIds(QStringList& result, MpqPriorityList* source, ObjectData* objectData, const QString& label, QWidget* parent, Qt::WindowFlags f)
{
	if (m_dialog != 0)
	{
		delete m_dialog;
		m_dialog = 0;
	}

	m_dialog = new ObjectListDialog(source, objectData, parent, f);
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
