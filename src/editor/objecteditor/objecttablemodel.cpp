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

#include "objecttablemodel.hpp"
#include "objectdata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTableModel::ObjectTableModel(QObject* parent) : QAbstractItemModel(parent), m_objectData(0)
{
}

QVariant ObjectTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	const QString fieldId = objectData()->metaData()->value(m_itemsByRow[index.row()], "ID");

	switch (role)
	{
		case Qt::DisplayRole:
		{
			if (index.column() == 0)
			{
				/*
				* In Frozen Throne there is a category for each value.
				*/
				QString category;

				if (objectData()->metaData()->hasValue(fieldId, "category"))
				{
					category = objectData()->metaData()->value(fieldId, "category");
					category = objectData()->source()->sharedData()->worldEditData()->value("ObjectEditorCategories", category);
					category = objectData()->source()->sharedData()->tr(category).remove('&');
				}

				const QString displayName = objectData()->metaData()->value(fieldId, "displayName");
				const QString displayText = objectData()->source()->sharedData()->tr(displayName);

				return tr("%1").arg(displayText);
			}
			else if (index.column() == 1)
			{
				return objectData()->fieldReadableValue(originalObjectId(), customObjectId(), fieldId);
			}
		}

		case Qt::UserRole:
		{
			return m_itemsByRow[index.row()];
		}

		case Qt::ForegroundRole:
		{
			if (objectData()->isFieldModified(originalObjectId(), customObjectId(), fieldId))
			{
				return Qt::magenta;
			}

			return Qt::black;
		}
	}

	return QVariant();
}

int ObjectTableModel::columnCount(const QModelIndex &parent) const
{
	return 2;
}

int ObjectTableModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		return 0;
	}

	return m_itemsByRow.count();
}

QModelIndex ObjectTableModel::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

QModelIndex ObjectTableModel::index(int row, int column, const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		return QModelIndex();
	}

	return createIndex(row, column, 0);
}

QVariant ObjectTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			if (section == 0 && objectData() != 0)
			{
				return objectData()->source()->sharedData()->tr("WESTRING_NAME");
			}
			else if (section == 1 && objectData() != 0)
			{
				return objectData()->source()->sharedData()->tr("WESTRING_VALUE");
			}
		}
	}

	return QVariant();
}

void ObjectTableModel::load(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	if (m_objectData != 0)
	{
		disconnect(m_objectData, 0, this, 0);
	}

	beginRemoveColumns(QModelIndex(), 0, 1);
	endRemoveColumns();
	beginRemoveRows(QModelIndex(), 0, m_itemsByRow.count());
	endRemoveRows();
	m_itemsByRow.clear();
	m_itemsByField.clear();

	m_objectData = objectData;
	m_originalObjectId = originalObjectId;
	m_customObjectId = customObjectId;
	int rows = 0;

	for (map::Slk::Table::size_type row = 1; row < objectData->metaData()->slk().rows(); ++row)
	{
		const QString fieldId = objectData->metaData()->value(row, "ID");

		if (objectData->hideField(this->originalObjectId(), this->customObjectId(), fieldId))
		{
			qDebug() << "Hide row with field id" << fieldId << "and description" << objectData->source()->sharedData()->tr(objectData->metaData()->value(fieldId, "displayName"));
		}
		else
		{
			rows++;
		}
	}

	beginInsertColumns(QModelIndex(), 0, 1);
	endInsertColumns();

	beginInsertRows(QModelIndex(), 0, rows);

	/*
	 * Order field IDs by their sort column and add them in the correct order.
	 */
	typedef QMultiMap<QString, QString> OrderedFieldIds;
	OrderedFieldIds fieldIds;

	for (map::Slk::Table::size_type i = 1; i < this->objectData()->metaData()->slk().rows() - 1; ++i)
	{
		const QString fieldId = this->objectData()->metaData()->value(i + 1, "ID");

		if (!this->objectData()->hideField(this->originalObjectId(), this->customObjectId(), fieldId))
		{
			QString sort;

			/*
			 * Some meta data might not have the "sort" column
			 */
			if (this->objectData()->metaData()->hasValue(fieldId, "sort"))
			{
				sort = this->objectData()->metaData()->value(fieldId, "sort");
			}
			else
			{
				qDebug() << "Missing column \"sort\" for field" << i;
			}

			fieldIds.insert(sort, fieldId);
		}
	}

	int row = 0;

	for (OrderedFieldIds::const_iterator iterator = fieldIds.begin(); iterator != fieldIds.end(); ++iterator, ++row)
	{
		const QString fieldId = iterator.value();
		m_itemsByRow.insert(row, fieldId);
		m_itemsByField.insert(fieldId, row);
	}

	endInsertRows();

	connect(objectData, SIGNAL(fieldModification(QString,QString,QString)), this, SLOT(modifyField(QString,QString,QString)));
	connect(objectData, SIGNAL(modificationReset(QString,QString,QString)), this, SLOT(resetField(QString,QString,QString)));
	m_objectData = objectData;
}

void ObjectTableModel::modifyField(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId)
{
	emit dataChanged(index(m_itemsByField[fieldId], 0), index(m_itemsByField[fieldId], 0));
}

void ObjectTableModel::resetField(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId)
{
	emit dataChanged(index(m_itemsByField[fieldId], 0), index(m_itemsByField[fieldId], 0));
}

}

}
