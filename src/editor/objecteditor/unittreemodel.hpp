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

#ifndef WC3LIB_EDITOR_UNITTREEMODEL_HPP
#define WC3LIB_EDITOR_UNITTREEMODEL_HPP

#include "objecttreemodel.hpp"

namespace wc3lib
{

namespace editor
{

class UnitData;

/**
 * \brief The concrete tree model for units.
 * \ingroup objectdata
 */
class UnitTreeModel : public ObjectTreeModel
{
	public:
		UnitTreeModel(QObject *parent = 0);

		virtual void load(MpqPriorityList *source, ObjectData *objectData, QWidget *window) override;
		virtual ObjectTreeItem* createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString& originalObjectId, const QString& customObjectId) override;

		ObjectTreeItem* standardObjectsItem(UnitData *unitData);
		ObjectTreeItem* customObjectsItem(UnitData *unitData);
		ObjectTreeItem* humanItem(UnitData *unitData);
		ObjectTreeItem* humanMeleeItem(UnitData *unitData);
		ObjectTreeItem* humanCampaignItem(UnitData *unitData);
		ObjectTreeItem* humanMeleeUnitsItem(UnitData *unitData);
		ObjectTreeItem* humanMeleeBuildingsItem(UnitData *unitData);
		ObjectTreeItem* humanMeleeHeroesItem(UnitData *unitData);
		ObjectTreeItem* humanMeleeSpecialItem(UnitData *unitData);
		ObjectTreeItem* orcItem(UnitData *unitData);
		ObjectTreeItem* orcMeleeItem(UnitData *unitData);
		ObjectTreeItem* orcCampaignItem(UnitData *unitData);
		ObjectTreeItem* orcMeleeUnitsItem(UnitData *unitData);
		ObjectTreeItem* orcMeleeBuildingsItem(UnitData *unitData);
		ObjectTreeItem* orcMeleeHeroesItem(UnitData *unitData);
		ObjectTreeItem* orcMeleeSpecialItem(UnitData *unitData);
		ObjectTreeItem* undeadItem(UnitData *unitData);
		ObjectTreeItem* undeadMeleeItem(UnitData *unitData);
		ObjectTreeItem* undeadCampaignItem(UnitData *unitData);
		ObjectTreeItem* undeadMeleeUnitsItem(UnitData *unitData);
		ObjectTreeItem* undeadMeleeBuildingsItem(UnitData *unitData);
		ObjectTreeItem* undeadMeleeHeroesItem(UnitData *unitData);
		ObjectTreeItem* undeadMeleeSpecialItem(UnitData *unitData);
		ObjectTreeItem* nightelfItem(UnitData *unitData);
		ObjectTreeItem* nightelfMeleeItem(UnitData *unitData);
		ObjectTreeItem* nightelfCampaignItem(UnitData *unitData);
		ObjectTreeItem* nightelfMeleeUnitsItem(UnitData *unitData);
		ObjectTreeItem* nightelfMeleeBuildingsItem(UnitData *unitData);
		ObjectTreeItem* nightelfMeleeHeroesItem(UnitData *unitData);
		ObjectTreeItem* nightelfMeleeSpecialItem(UnitData *unitData);
		ObjectTreeItem* neutralNagaItem(UnitData *unitData);
		ObjectTreeItem* neutralNagaUnitsItem(UnitData *unitData);
		ObjectTreeItem* neutralNagaBuildingsItem(UnitData *unitData);
		ObjectTreeItem* neutralNagaHeroesItem(UnitData *unitData);
		ObjectTreeItem* neutralNagaSpecialItem(UnitData *unitData);
		ObjectTreeItem* neutralNagaCampaignItem(UnitData *unitData);
		ObjectTreeItem* neutralHostileItem(UnitData *unitData);
		ObjectTreeItem* neutralHostileUnitsItem(UnitData *unitData);
		ObjectTreeItem* neutralHostileBuildingsItem(UnitData *unitData);
		ObjectTreeItem* neutralHostileHeroesItem(UnitData *unitData);
		ObjectTreeItem* neutralHostileSpecialItem(UnitData *unitData);
		ObjectTreeItem* neutralHostileCampaignItem(UnitData *unitData);
		ObjectTreeItem* neutralPassiveItem(UnitData *unitData);
		ObjectTreeItem* neutralPassiveUnitsItem(UnitData *unitData);
		ObjectTreeItem* neutralPassiveBuildingsItem(UnitData *unitData);
		ObjectTreeItem* neutralPassiveHeroesItem(UnitData *unitData);
		ObjectTreeItem* neutralPassiveSpecialItem(UnitData *unitData);
		ObjectTreeItem* neutralPassiveCampaignItem(UnitData *unitData);

	protected:
		virtual ObjectTreeItem* itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId) override;

	private:
		ObjectTreeItem *m_standardObjectsItem;
		ObjectTreeItem *m_customObjectsItem;
		ObjectTreeItem *m_humanItem;
		ObjectTreeItem *m_humanMeleeItem;
		ObjectTreeItem *m_humanCampaignItem;
		ObjectTreeItem *m_humanMeleeUnitsItem;
		ObjectTreeItem *m_humanMeleeBuildingsItem;
		ObjectTreeItem *m_humanMeleeHeroesItem;
		ObjectTreeItem *m_humanMeleeSpecialItem;
		ObjectTreeItem *m_orcItem;
		ObjectTreeItem *m_orcMeleeItem;
		ObjectTreeItem *m_orcCampaignItem;
		ObjectTreeItem *m_orcMeleeUnitsItem;
		ObjectTreeItem *m_orcMeleeBuildingsItem;
		ObjectTreeItem *m_orcMeleeHeroesItem;
		ObjectTreeItem *m_orcMeleeSpecialItem;
		ObjectTreeItem *m_undeadItem;
		ObjectTreeItem *m_undeadCampaignItem;
		ObjectTreeItem *m_undeadMeleeItem;
		ObjectTreeItem *m_undeadMeleeUnitsItem;
		ObjectTreeItem *m_undeadMeleeBuildingsItem;
		ObjectTreeItem *m_undeadMeleeHeroesItem;
		ObjectTreeItem *m_undeadMeleeSpecialItem;
		ObjectTreeItem *m_nightelfItem;
		ObjectTreeItem *m_nightelfCampaignItem;
		ObjectTreeItem *m_nightelfMeleeItem;
		ObjectTreeItem *m_nightelfMeleeUnitsItem;
		ObjectTreeItem *m_nightelfMeleeBuildingsItem;
		ObjectTreeItem *m_nightelfMeleeHeroesItem;
		ObjectTreeItem *m_nightelfMeleeSpecialItem;
		ObjectTreeItem *m_neutralNagaItem;
		ObjectTreeItem *m_neutralNagaUnitsItem;
		ObjectTreeItem *m_neutralNagaBuildingsItem;
		ObjectTreeItem *m_neutralNagaHeroesItem;
		ObjectTreeItem *m_neutralNagaSpecialItem;
		ObjectTreeItem *m_neutralNagaCampaignItem;
		ObjectTreeItem *m_neutralHostileItem;
		ObjectTreeItem *m_neutralHostileUnitsItem;
		ObjectTreeItem *m_neutralHostileBuildingsItem;
		ObjectTreeItem *m_neutralHostileHeroesItem;
		ObjectTreeItem *m_neutralHostileSpecialItem;
		ObjectTreeItem *m_neutralHostileCampaignItem;
		ObjectTreeItem *m_neutralPassiveItem;
		ObjectTreeItem *m_neutralPassiveUnitsItem;
		ObjectTreeItem *m_neutralPassiveBuildingsItem;
		ObjectTreeItem *m_neutralPassiveHeroesItem;
		ObjectTreeItem *m_neutralPassiveSpecialItem;
		ObjectTreeItem *m_neutralPassiveCampaignItem;
};

}

}

#endif // WC3LIB_EDITOR_UNITTREEMODEL_HPP
