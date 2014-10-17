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

#include "abilitytreemodel.hpp"
#include "abilitydata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

AbilityTreeModel::AbilityTreeModel(MpqPriorityList *source, QObject *parent) : ObjectTreeModel(source, parent)
{
	createObjects(source->sharedData().get());
}

ObjectTreeItem* AbilityTreeModel::createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString &originalObjectId, const QString &customObjectId)
{
	const QModelIndex parentIndex = itemParent(objectData, originalObjectId, customObjectId);
	ObjectTreeItem *parent = item(parentIndex);
	insertRows(parent->children().count(), 1, parentIndex);

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

QModelIndex AbilityTreeModel::itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	return abilityTypeIndex(objectData, originalObjectId, customObjectId);
}


QModelIndex AbilityTreeModel::objectsIndex(ObjectData* objectData, const QString& originalObjectId, const QString& customObjectId)
{
	if (customObjectId.isEmpty())
	{
		return this->index(0, 0);
	}

	return this->index(1, 0);
}

QModelIndex AbilityTreeModel::raceIndex(editor::ObjectData* objectData, const QString& originalObjectId, const QString& customObjectId)
{
	AbilityData *abilityData = boost::polymorphic_cast<AbilityData*>(objectData);
	const QString race = abilityData->abilityData()->value(originalObjectId, "race");

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
	else if (race == "demon")
	{
		return index(4, 0, objectsIndex(objectData, originalObjectId, customObjectId));
	}
	else
	{
		return index(5, 0, objectsIndex(objectData, originalObjectId, customObjectId));
	}

	return index(0, 0, objectsIndex(objectData, originalObjectId, customObjectId));
}

QModelIndex AbilityTreeModel::abilityTypeIndex(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	AbilityData *abilityData = boost::polymorphic_cast<AbilityData*>(objectData);

	if (abilityData->objectIsUnit(originalObjectId, customObjectId))
	{
		return index(0, 0, raceIndex(objectData, originalObjectId, customObjectId));
	}
	else if (abilityData->objectIsItem(originalObjectId, customObjectId))
	{
		return index(2, 0, raceIndex(objectData, originalObjectId, customObjectId));
	}
	else if (abilityData->objectIsHero(originalObjectId, customObjectId))
	{
		return index(1, 0, raceIndex(objectData, originalObjectId, customObjectId));
	}

	return index(0, 0, raceIndex(objectData, originalObjectId, customObjectId));
}

void AbilityTreeModel::createObjects(WarcraftIIIShared *shared)
{
	QStringList names;
	names << shared->tr("WESTRING_AE_STANDARDABILS");
	names << shared->tr("WESTRING_AE_CUSTOMABILS");

	insertRowFolders(names, 0);

	createRaces(shared, 0, this->index(0, 0));
	createRaces(shared, 0, this->index(1, 0));
}

void AbilityTreeModel::createRaces(WarcraftIIIShared *shared, int row, QModelIndex parent)
{
	QStringList names;
	names << shared->tr("WESTRING_RACE_HUMAN");
	names << shared->tr("WESTRING_RACE_ORC");
	names << shared->tr("WESTRING_RACE_NIGHTELF");
	names << shared->tr("WESTRING_RACE_UNDEAD");
	names << shared->tr("WESTRING_RACE_DEMON");
	names << shared->tr("WESTRING_RACE_NEUTRAL");

	insertRowFolders(names, row, parent);

	for (int i = 0; i < names.size(); ++i)
	{
		createRace(shared, 0, this->index(i, 0 , parent));
	}
}

void AbilityTreeModel::createRace(WarcraftIIIShared *shared, int row, QModelIndex parent)
{
	QStringList names;
	names << shared->tr("WESTRING_UNITS");
	names << shared->tr("WESTRING_UTYPE_HEROES");
	names << shared->tr("WESTRING_ITEMS");

	insertRowFolders(names, row, parent);
}

}

}
