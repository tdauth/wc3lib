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

#include "unittreemodel.hpp"
#include "objectdata.hpp"
#include "unitdata.hpp"
#include "../mpqprioritylist.hpp"
#include "../warcraftiiishared.hpp"

namespace wc3lib
{

namespace editor
{

UnitTreeModel::UnitTreeModel(MpqPriorityList *source, QObject *parent)
: ObjectTreeModel(source, parent)
{
	// custom and standard items
	createObjects(source->sharedData().get());
}

void UnitTreeModel::load(MpqPriorityList *source, ObjectData *objectData, QWidget *window)
{
	UnitData *unitData = dynamic_cast<UnitData*>(objectData);

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (unitData->unitData() != 0 && !unitData->unitData()->isEmpty())
	{
		for (map::Slk::Table::size_type row = 1; row < unitData->unitData()->rows(); ++row)
		{
			const QString objectId = unitData->unitData()->value(row, "unitID");
			createItem(source, objectData, window, objectId, "");
		}
	}

	ObjectTreeModel::load(source, objectData, window);
}

ObjectTreeItem* UnitTreeModel::createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString& originalObjectId, const QString& customObjectId)
{
	UnitData *unitData = dynamic_cast<UnitData*>(objectData);
	const QModelIndex parentIndex = itemParent(unitData, originalObjectId, customObjectId);
	ObjectTreeItem *parent = item(parentIndex);
	insertRows(parent->children().count(), 1, parentIndex);

	ObjectTreeItem *item = parent->children().last();
	item->setObjectData(objectData);
	item->setObjectId(originalObjectId, customObjectId);

	const QString art = objectData->fieldValue(originalObjectId, customObjectId, "uico");
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


QModelIndex UnitTreeModel::itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	return unitTypeIndex(objectData, originalObjectId, customObjectId);
}

QModelIndex UnitTreeModel::objectsIndex(ObjectData* objectData, const QString& originalObjectId, const QString& customObjectId)
{
	if (customObjectId.isEmpty())
	{
		return this->index(0, 0);
	}

	return this->index(1, 0);
}

QModelIndex editor::UnitTreeModel::raceIndex(editor::ObjectData* objectData, const QString& originalObjectId, const QString& customObjectId)
{
	UnitData *unitData = boost::polymorphic_cast<UnitData*>(objectData);
	const QString race = unitData->unitData()->value(originalObjectId, "race");

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

QModelIndex UnitTreeModel::campaignIndex(ObjectData* objectData, const QString& originalObjectId, const QString& customObjectId)
{
	UnitData *unitData = boost::polymorphic_cast<UnitData*>(objectData);
	QString campaign;

	if (unitData->hasDefaultFieldValue(originalObjectId, "ucam"))
	{
		campaign = unitData->fieldValue(originalObjectId, customObjectId, "ucam");
	}

	if (campaign == "1")
	{
		return index(1, 0, raceIndex(objectData, originalObjectId, customObjectId));
	}

	return index(0, 0, raceIndex(objectData, originalObjectId, customObjectId));
}

QModelIndex editor::UnitTreeModel::unitTypeIndex(editor::ObjectData* objectData, const QString& originalObjectId, const QString& customObjectId)
{
	UnitData *unitData = boost::polymorphic_cast<UnitData*>(objectData);

	if (unitData->objectIsSpecial(originalObjectId, customObjectId))
	{
		return index(3, 0, campaignIndex(objectData, originalObjectId, customObjectId));
	}
	else if (unitData->objectIsBuilding(originalObjectId, customObjectId))
	{
		return index(2, 0, campaignIndex(objectData, originalObjectId, customObjectId));
	}
	else if (unitData->objectIsHero(originalObjectId, customObjectId))
	{
		return index(1, 0, campaignIndex(objectData, originalObjectId, customObjectId));
	}

	return index(0, 0, campaignIndex(objectData, originalObjectId, customObjectId));
}

void UnitTreeModel::createObjects(WarcraftIIIShared *shared)
{
	QStringList names;
	names << shared->tr("WESTRING_UE_STANDARDUNITS");
	names << shared->tr("WESTRING_UE_CUSTOMUNITS");

	insertRowFolders(names, 0);

	createRaces(shared, 0, this->index(0, 0));
	createRaces(shared, 0, this->index(1, 0));
}

void UnitTreeModel::createRaces(WarcraftIIIShared *shared, int row, QModelIndex parent)
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

void UnitTreeModel::createRace(WarcraftIIIShared *shared, int row, QModelIndex parent)
{
	QStringList names;
	names << shared->tr("WESTRING_MELEE");
	names << shared->tr("WESTRING_CAMPAIGN");

	insertRowFolders(names, row, parent);

	for (int i = 0; i < names.size(); ++i)
	{
		createMelee(shared, 0, this->index(i, 0 , parent));
	}
}

void UnitTreeModel::createMelee(WarcraftIIIShared *shared, int row, QModelIndex parent)
{
	QStringList names;
	names << shared->tr("WESTRING_UNITS");
	names << shared->tr("WESTRING_UTYPE_HEROES");
	names << shared->tr("WESTRING_UTYPE_BUILDINGS");
	names << shared->tr("WESTRING_UTYPE_SPECIAL");

	insertRowFolders(names, row, parent);
}

}

}
