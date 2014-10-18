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

#include "weathertreemodel.hpp"
#include "../weatherdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

WeatherTreeModel::WeatherTreeModel(MpqPriorityList *source, QObject *parent): ObjectTreeModel(source, parent)
{
	QStringList names;
	names << tr("Standard Weather Effects");
	names << tr("Custom Weather Effects");

	insertRowFolders(names, 0);
}

ObjectTreeItem* WeatherTreeModel::createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString &originalObjectId, const QString &customObjectId)
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
	item->setIcon(objectData->objectIcon(originalObjectId, customObjectId, window));

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

QModelIndex WeatherTreeModel::itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	if (customObjectId.isEmpty())
	{
		return index(0, 0);
	}

	return index(1, 0);
}

}

}
