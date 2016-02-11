/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#include "upgradetreemodel.hpp"
#include "../upgradedata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

UpgradeTreeModel::UpgradeTreeModel(MpqPriorityList *source, QWidget *window, QObject *parent) : ObjectTreeModel(source, window, parent)
{
	createObjects(source->sharedData().get());
}

ObjectTreeItem* UpgradeTreeModel::createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString &originalObjectId, const QString &customObjectId)
{
	UpgradeData *upgradeData = dynamic_cast<UpgradeData*>(objectData);
	const QModelIndex parentIndex = itemParent(upgradeData, originalObjectId, customObjectId);
	ObjectTreeItem *parent = item(parentIndex);
	qDebug() << "Count before:" << parent->children().count();
	insertRows(parent->children().count(), 1, parentIndex);
	qDebug() << "Count after:" << parent->children().count();

	ObjectTreeItem *item = parent->children().last();
	item->setObjectData(objectData);
	item->setObjectId(originalObjectId, customObjectId);

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

QModelIndex UpgradeTreeModel::itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	return raceIndex(objectData, originalObjectId, customObjectId);
}

QModelIndex UpgradeTreeModel::objectsIndex(ObjectData* objectData, const QString& originalObjectId, const QString& customObjectId)
{
	if (customObjectId.isEmpty())
	{
		return this->index(0, 0);
	}

	return this->index(1, 0);
}

QModelIndex UpgradeTreeModel::raceIndex(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	const QString race = objectData->fieldValue(originalObjectId, customObjectId, "grac");

	qDebug() << "Has race " << race;

	if (race == "human")
	{
		return index(0, 0, objectsIndex(objectData, originalObjectId, customObjectId));
	}
	else if (race == "orc")
	{
		return index(1, 0, objectsIndex(objectData, originalObjectId, customObjectId));
	}
	else if (race == "nightelf")
	{
		return index(2, 0, objectsIndex(objectData, originalObjectId, customObjectId));
	}
	else if (race == "undead")
	{
		return index(3, 0, objectsIndex(objectData, originalObjectId, customObjectId));
	}
	else
	{
		return index(4, 0, objectsIndex(objectData, originalObjectId, customObjectId));
	}

	return index(0, 0, objectsIndex(objectData, originalObjectId, customObjectId));
}

void UpgradeTreeModel::createObjects(WarcraftIIIShared *shared)
{
	QStringList names;
	names << shared->tr("WESTRING_GE_STANDARDUPGRS");
	names << shared->tr("WESTRING_GE_CUSTOMUPGRS");

	insertRowFolders(names, 0);

	createRaces(shared, 0, this->index(0, 0));
	createRaces(shared, 0, this->index(1, 0));
}

void UpgradeTreeModel::createRaces(WarcraftIIIShared *shared, int row, QModelIndex parent)
{
	QStringList names;
	names << shared->tr("WESTRING_RACE_HUMAN");
	names << shared->tr("WESTRING_RACE_ORC");
	names << shared->tr("WESTRING_RACE_NIGHTELF");
	names << shared->tr("WESTRING_RACE_UNDEAD");
	names << shared->tr("WESTRING_RACE_NEUTRAL");

	insertRowFolders(names, row, parent);
}

}

}
