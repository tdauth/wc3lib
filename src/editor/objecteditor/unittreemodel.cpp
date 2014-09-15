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

UnitTreeModel::UnitTreeModel(QObject *parent)
: ObjectTreeModel(parent)
, m_standardObjectsItem(0)
, m_customObjectsItem(0)
, m_humanItem(0)
, m_humanMeleeItem(0)
, m_humanCampaignItem(0)
, m_humanMeleeUnitsItem(0)
, m_humanMeleeBuildingsItem(0)
, m_humanMeleeHeroesItem(0)
, m_humanMeleeSpecialItem(0)
, m_orcItem(0)
, m_orcUnitsItem(0)
, m_orcBuildingsItem(0)
, m_orcHeroesItem(0)
, m_orcSpecialItem(0)
, m_orcCampaignItem(0)
, m_undeadItem(0)
, m_undeadUnitsItem(0)
, m_undeadBuildingsItem(0)
, m_undeadHeroesItem(0)
, m_undeadSpecialItem(0)
, m_undeadCampaignItem(0)
, m_nightElfItem(0)
, m_nightelfUnitsItem(0)
, m_nightelfBuildingsItem(0)
, m_nightelfHeroesItem(0)
, m_nightelfSpecialItem(0)
, m_nightelfCampaignItem(0)
, m_neutralNagaItem(0)
, m_neutralNagaUnitsItem(0)
, m_neutralNagaBuildingsItem(0)
, m_neutralNagaHeroesItem(0)
, m_neutralNagaSpecialItem(0)
, m_neutralNagaCampaignItem(0)
, m_neutralHostileItem(0)
, m_neutralHostileUnitsItem(0)
, m_neutralHostileBuildingsItem(0)
, m_neutralHostileHeroesItem(0)
, m_neutralHostileSpecialItem(0)
, m_neutralHostileCampaignItem(0)
, m_neutralPassiveItem(0)
, m_neutralPassiveUnitsItem(0)
, m_neutralPassiveBuildingsItem(0)
, m_neutralPassiveHeroesItem(0)
, m_neutralPassiveSpecialItem(0)
, m_neutralPassiveCampaignItem(0)
{
}

void UnitTreeModel::load(MpqPriorityList *source, ObjectData *objectData, QWidget *window)
{
	UnitData *unitData = dynamic_cast<UnitData*>(objectData);

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (!unitData->unitData()->isEmpty())
	{
		for (map::Slk::Table::size_type row = 1; row < unitData->unitData()->slk().rows(); ++row)
		{
			const QString objectId = unitData->unitData()->value(row, "unitID");
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

ObjectTreeItem* UnitTreeModel::createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString& originalObjectId, const QString& customObjectId)
{
	UnitData *unitData = dynamic_cast<UnitData*>(objectData);
	ObjectTreeItem *parent = itemParent(unitData, originalObjectId, customObjectId);
	ObjectTreeItem *item = new ObjectTreeItem(objectData, originalObjectId, customObjectId, parent);
	parent->appendChild(item);

	beginInsertRows(parent->modelIndex(this), item->row(), item->row());

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

	endInsertRows();

	return item;
}

ObjectTreeItem* UnitTreeModel::itemParent(UnitData *unitData, const QString &originalObjectId, const QString &customObjectId)
{
	const QString race = unitData->unitData()->value(originalObjectId, "race");
	qDebug() << "Race:" << race;
	QString campaign;
	QString special = unitData->unitUi()->value(originalObjectId, "special");

	if (unitData->hasDefaultFieldValue(originalObjectId, "ucam"))
	{
		campaign = unitData->fieldValue(originalObjectId, customObjectId, "ucam");
	}

	if (customObjectId.isEmpty())
	{
		if (race == "human")
		{
			if (campaign == "1")
			{
				return humanCampaignItem(unitData);
			}
			else
			{
				if (special == "1")
				{
					return humanMeleeSpecialItem(unitData);
				}
				else if (unitData->objectIsBuilding(originalObjectId, customObjectId))
				{
					return humanMeleeBuildingsItem(unitData);
				}
				else if (unitData->objectIsHero(originalObjectId, customObjectId))
				{
					return humanMeleeHeroesItem(unitData);
				}
				else
				{
					return humanMeleeUnitsItem(unitData);
				}
			}
		}
		/*
		else if (race == "orc")
		{
			if (special == "1")
			{
				return orcSpecialItem(unitData);
			}
			else if (campaign == "1")
			{
				return orcCampaignItem(unitData);
			}
			else if (unitData->objectIsBuilding(originalObjectId, customObjectId))
			{
				return orcBuildingsItem(unitData);
			}
			else if (unitData->objectIsHero(originalObjectId, customObjectId))
			{
				return orcHeroesItem(unitData);
			}
			else
			{
				return orcUnitsItem(unitData);
			}
		}
		else if (race == "nightelf")
		{
			if (special == "1")
			{
				return nightelfSpecialItem(unitData);
			}
			else if (campaign == "1")
			{
				return nightelfCampaignItem(unitData);
			}
			else if (unitData->objectIsBuilding(originalObjectId, customObjectId))
			{
				return nightelfBuildingsItem(unitData);
			}
			else if (unitData->objectIsHero(originalObjectId, customObjectId))
			{
				return nightelfHeroesItem(unitData);
			}
			else
			{
				return nightelfUnitsItem(unitData);
			}
		}
		else if (race == "undead")
		{
			if (special == "1")
			{
				return undeadSpecialItem(unitData);
			}
			else if (campaign == "1")
			{
				return undeadCampaignItem(unitData);
			}
			else if (unitData->objectIsBuilding(originalObjectId, customObjectId))
			{
				return undeadBuildingsItem(unitData);
			}
			else if (unitData->objectIsHero(originalObjectId, customObjectId))
			{
				return undeadHeroesItem(unitData);
			}
			else
			{
				return undeadUnitsItem(unitData);
			}
		}
		else if (race == "other" || race == "critters" || race == "commoner")
		{
			if (special == "1")
			{
				return neutralPassiveSpecialItem(unitData);
			}
			else if (campaign == "1")
			{
				return neutralPassiveCampaignItem(unitData);
			}
			else if (unitData->objectIsBuilding(originalObjectId, customObjectId))
			{
				return neutralPassiveBuildingsItem(unitData);
			}
			else if (unitData->objectIsHero(originalObjectId, customObjectId))
			{
				return neutralPassiveHeroesItem(unitData);
			}
			else
			{
				return neutralPassiveUnitsItem(unitData);
			}
		}
		else if (race == "demon" || race == "creeps")
		{
			if (special == "1")
			{
				return neutralHostileSpecialItem(unitData);
			}
			else if (campaign == "1")
			{
				return neutralHostileCampaignItem(unitData);
			}
			else if (unitData->objectIsBuilding(originalObjectId, customObjectId))
			{
				return neutralHostileBuildingsItem(unitData);
			}
			else if (unitData->objectIsHero(originalObjectId, customObjectId))
			{
				return neutralHostileHeroesItem(unitData);
			}
			else
			{
				return neutralHostileUnitsItem(unitData);
			}
		}
		else if (race == "naga")
		{
			if (special == "1")
			{
				return neutralNagaSpecialItem(unitData);
			}
			else if (campaign == "1")
			{
				return neutralNagaCampaignItem(unitData);
			}
			else if (unitData->objectIsBuilding(originalObjectId, customObjectId))
			{
				return neutralNagaBuildingsItem(unitData);
			}
			else if (unitData->objectIsHero(originalObjectId, customObjectId))
			{
				return neutralNagaHeroesItem(unitData);
			}
			else
			{
				return neutralNagaUnitsItem(unitData);
			}
		}
		*/
		else
		{
			return standardObjectsItem(unitData);
		}
	}
	else
	{
		return customObjectsItem(unitData);
	}
}

ObjectTreeItem* UnitTreeModel::standardObjectsItem(UnitData *unitData)
{
	if (this->m_standardObjectsItem == 0)
	{
		qDebug() << "Constructing standard objects";
		this->m_standardObjectsItem = new ObjectTreeItem(unitData, unitData->source()->sharedData()->tr("WESTRING_UE_STANDARDUNITS"));
		this->insertTopLevelItem(this->m_standardObjectsItem);
	}

	return this->m_standardObjectsItem;
}

ObjectTreeItem* UnitTreeModel::customObjectsItem(UnitData* unitData)
{
	if (this->m_customObjectsItem == 0)
	{
		this->m_customObjectsItem = new ObjectTreeItem(unitData, unitData->source()->sharedData()->tr("WESTRING_UE_CUSTOMUNITS"));
		this->insertTopLevelItem(this->m_customObjectsItem);
	}

	return this->m_customObjectsItem;
}

ObjectTreeItem* UnitTreeModel::humanItem(UnitData* unitData)
{
	if (this->m_humanItem == 0)
	{
		qDebug() << "Constructing human";
		this->m_humanItem = new ObjectTreeItem(unitData, unitData->source()->sharedData()->tr("WESTRING_RACE_HUMAN"), standardObjectsItem(unitData));
		standardObjectsItem(unitData)->appendChild(this->m_humanItem);
	}

	return this->m_humanItem;
}

ObjectTreeItem* UnitTreeModel::humanMeleeItem(UnitData* unitData)
{
	if (this->m_humanMeleeItem == 0)
	{
		this->m_humanMeleeItem = new ObjectTreeItem(unitData, unitData->source()->sharedData()->tr("WESTRING_MELEE"), humanItem(unitData));
		humanItem(unitData)->appendChild(this->m_humanMeleeItem);
	}

	return this->m_humanMeleeItem;
}

ObjectTreeItem* UnitTreeModel::humanCampaignItem(UnitData* unitData)
{
	if (this->m_humanCampaignItem == 0)
	{
		this->m_humanCampaignItem = new ObjectTreeItem(unitData, unitData->source()->sharedData()->tr("WESTRING_CAMPAIGN"), humanItem(unitData));
		humanItem(unitData)->appendChild(this->m_humanCampaignItem);
	}

	return this->m_humanCampaignItem;
}

ObjectTreeItem* UnitTreeModel::humanMeleeUnitsItem(UnitData* unitData)
{
	if (this->m_humanMeleeUnitsItem == 0)
	{
		this->m_humanMeleeUnitsItem = new ObjectTreeItem(unitData, unitData->source()->sharedData()->tr("WESTRING_MENU_OM_UNITS"), humanMeleeItem(unitData));
		humanMeleeItem(unitData)->appendChild(this->m_humanMeleeUnitsItem);
	}

	return this->m_humanMeleeUnitsItem;
}

ObjectTreeItem* UnitTreeModel::humanMeleeBuildingsItem(UnitData* unitData)
{
	if (this->m_humanMeleeBuildingsItem == 0)
	{
		this->m_humanMeleeBuildingsItem = new ObjectTreeItem(unitData, unitData->source()->sharedData()->tr("WESTRING_UTYPE_BUILDINGS"), humanMeleeItem(unitData));
		humanMeleeItem(unitData)->appendChild(this->m_humanMeleeBuildingsItem);
	}

	return this->m_humanMeleeBuildingsItem;
}

ObjectTreeItem* UnitTreeModel::humanMeleeHeroesItem(UnitData* unitData)
{
	if (this->m_humanMeleeHeroesItem == 0)
	{
		this->m_humanMeleeHeroesItem = new ObjectTreeItem(unitData, unitData->source()->sharedData()->tr("WESTRING_UTYPE_HEROES"), humanMeleeItem(unitData));
		humanMeleeItem(unitData)->appendChild(this->m_humanMeleeHeroesItem);
	}

	return this->m_humanMeleeHeroesItem;
}

ObjectTreeItem* UnitTreeModel::humanMeleeSpecialItem(UnitData* unitData)
{
	if (this->m_humanMeleeSpecialItem == 0)
	{
		this->m_humanMeleeSpecialItem = new ObjectTreeItem(unitData, unitData->source()->sharedData()->tr("WESTRING_UTYPE_SPECIAL"), humanMeleeItem(unitData));
		humanMeleeItem(unitData)->appendChild(this->m_humanMeleeSpecialItem);
	}

	return this->m_humanMeleeSpecialItem;
}

ObjectTreeItem* UnitTreeModel::orcItem(UnitData *unitData)
{
	if (this->m_orcItem == 0)
	{
		qDebug() << "Constructing orc";
		this->m_orcItem = new ObjectTreeItem(unitData, unitData->source()->sharedData()->tr("WESTRING_RACE_ORC"), standardObjectsItem(unitData));
		standardObjectsItem(unitData)->appendChild(this->m_orcItem);
	}

	return this->m_orcItem;
}


}

}
