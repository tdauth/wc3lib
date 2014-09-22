/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <QtGui>

#include "weathertreemodel.hpp"
#include "weatherdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

WeatherTreeModel::WeatherTreeModel(MpqPriorityList* source, QObject* parent): ObjectTreeModel(source, parent)
{
	QStringList names;
	names << source->sharedData()->tr("WESTRING_UE_STANDARDUNITS");
	names << source->sharedData()->tr("WESTRING_UE_CUSTOMUNITS");

	insertRowFolders(names, 0);
}

void WeatherTreeModel::load(MpqPriorityList* source, ObjectData* objectData, QWidget* window)
{
	WeatherData *weatherData = dynamic_cast<WeatherData*>(objectData);

	// add all entries from "Weather.slk" to standard weather effects in Unit Editor
	if (!weatherData->weather()->isEmpty())
	{
		for (map::Slk::Table::size_type row = 1; row < weatherData->weather()->slk().rows(); ++row)
		{
			const QString objectId = weatherData->weather()->value(row, "effectID");
			createItem(source, objectData, window, objectId, "");
		}
	}

	ObjectTreeItem::Children folders = this->folders();

	foreach (ObjectTreeItem *item, folders)
	{
		item->setCollapsed(source, window);
		item->setFolderText(tr("%1 (%2)").arg(item->text(false)).arg(item->countNonFolderItems()));
	}

	ObjectTreeModel::load(source, objectData, window);
}

ObjectTreeItem* WeatherTreeModel::createItem(MpqPriorityList* source, ObjectData* objectData, QWidget* window, const QString& originalObjectId, const QString& customObjectId)
{
	WeatherData *weatherData = dynamic_cast<WeatherData*>(objectData);
	const QModelIndex parentIndex = itemParent(weatherData, originalObjectId, customObjectId);
	ObjectTreeItem *parent = item(parentIndex);
	qDebug() << "Count before:" << parent->children().count();
	insertRows(parent->children().count(), 1, parentIndex);
	qDebug() << "Count after:" << parent->children().count();

	ObjectTreeItem *item = parent->children().last();
	item->setObjectData(objectData);
	item->setObjectId(originalObjectId, customObjectId);

	const QString art = objectData->fieldValue(originalObjectId, customObjectId, "texf");
	item->setIcon(objectData->source()->sharedData()->icon(art, window));

	if (customObjectId.isEmpty())
	{
		this->insertStandardItem(item);
	}
	else
	{
		this->insertCustomItem(item);
	}

	return item;
}

QModelIndex WeatherTreeModel::itemParent(ObjectData* objectData, const QString& originalObjectId, const QString& customObjectId)
{
	if (customObjectId.isEmpty())
	{
		return index(0, 0);
	}

	return index(1, 0);
}

}

}
