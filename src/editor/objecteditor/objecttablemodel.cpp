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
#include "../objectdata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTableModel::ObjectTableModel(QObject* parent) : QAbstractItemModel(parent), m_objectData(0), m_showRawData(false)
{
}

QVariant ObjectTableModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	if (objectData()->metaData()->hasValue(m_itemsByRow[index.row()], "ID"))
	{
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

						if (!category.isEmpty())
						{
							if (objectData()->source()->sharedData()->worldEditData()->hasValue("ObjectEditorCategories", category))
							{
								category = objectData()->source()->sharedData()->worldEditData()->value("ObjectEditorCategories", category);
								category = objectData()->source()->sharedData()->tr(category).remove('&');
							}
							else
							{
								qDebug() << "Missing category entry for category" << category;
							}
						}
					}

					if (objectData()->metaData()->hasValue(fieldId, "displayName"))
					{
						const QString displayName = objectData()->metaData()->value(fieldId, "displayName");
						const QString displayText = objectData()->source()->sharedData()->tr(displayName);

						if (!category.isEmpty())
						{
							if (!showRawData())
							{
								return tr("%1 - %2").arg(category).arg(displayText);
							}

							return tr("%1 - %2 (%3)").arg(category).arg(fieldId).arg(displayText);
						}
						else
						{
							if (!showRawData())
							{
								return tr("%1").arg(displayText);
							}

							return tr("%1 (%2)").arg(fieldId).arg(displayText);
						}
					}
					else
					{
						qDebug() << "Missing \"displayName\" for field" << fieldId;

						return fieldId;
					}
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
	}
	else
	{
		qDebug() << "Missing ID column for row" << m_itemsByRow[index.row()] << "with number" << index.row();
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

void ObjectTableModel::load(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId, QWidget *widget)
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

	for (map::Slk::Table::size_type row = 1; row < objectData->metaData()->rows(); ++row)
	{
		const QString fieldId = objectData->metaData()->value(row, "ID");

		/*
		 * Skip empty rows.
		 */
		if (!fieldId.isEmpty())
		{
			if (objectData->hideField(this->originalObjectId(), this->customObjectId(), fieldId))
			{
				qDebug() << "Hide row with field id" << fieldId << "and description" << objectData->source()->sharedData()->tr(objectData->metaData()->value(fieldId, "displayName"));
			}
			else
			{
				/*
				 * Make sure object data for field is loaded.
				 */
				const QString fieldType = objectData->metaData()->value(row, "type");

				ObjectData *fieldObjectData = this->objectData()->source()->sharedData()->sharedObjectData()->resolveByFieldType(fieldType);

				if (fieldObjectData != 0)
				{
					fieldObjectData->loadOnRequest(widget);
				}

				m_itemsByRow.insert(rows, fieldId);
				m_itemsByField.insert(fieldId, rows);

				rows++;
			}
		}
	}

	beginInsertColumns(QModelIndex(), 0, 1);
	endInsertColumns();

	beginInsertRows(QModelIndex(), 0, rows - 1);
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

int ObjectTableModel::row(const QString& fieldId) const
{
	return this->m_itemsByField[fieldId];
}

}

}
