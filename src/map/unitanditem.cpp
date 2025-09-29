/***************************************************************************
 *   Copyright (C) 2025 by Tamino Dauth                                    *
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

#include "unitanditem.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize UnitAndItem::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_typeId, size);
	wc3lib::read(istream, m_variation, size);
	size += m_position.read(istream);
	wc3lib::read(istream, m_angle, size);
	size += m_scale.read(istream);
	wc3lib::read(istream, m_typeId2, size);
	wc3lib::read<byte>(istream, (byte&)m_flags, size);
	wc3lib::read(istream, m_playerNumber, size);
	wc3lib::read(istream, m_unknown0, size);
	wc3lib::read(istream, m_unknown1, size);
	wc3lib::read(istream, m_hitPoints, size);
	wc3lib::read(istream, m_manaPoints, size);
	wc3lib::read(istream, m_itemTableIndex, size);
	int32 numberOfDroppedItemSets = 0;
	wc3lib::read(istream, numberOfDroppedItemSets, size);

	for (int32 i = 0; i < numberOfDroppedItemSets; i++)
	{
		std::unique_ptr<DroppedItemSet> droppedItemSet(new DroppedItemSet());
		size += droppedItemSet->read(istream);
		m_droppedItemSets.push_back(std::move(droppedItemSet));
	}

	wc3lib::read(istream, m_gold, size);
	wc3lib::read(istream, m_targetAcquisition, size);
	wc3lib::read(istream, m_heroLevel, size);
	wc3lib::read(istream, m_strength, size);
	wc3lib::read(istream, m_agility, size);
	wc3lib::read(istream, m_intelligence, size);

	int32 numberOfInventoryItems = 0;
	wc3lib::read(istream, numberOfInventoryItems, size);

	for (int32 i = 0; i < numberOfInventoryItems; i++)
	{
		std::unique_ptr<InventoryItem> inventoryItem(new InventoryItem());
		size += inventoryItem->read(istream);
		m_inventoryItems.push_back(std::move(inventoryItem));
	}

	int32 numberOfAbilityModifications = 0;
	wc3lib::read(istream, numberOfAbilityModifications, size);

	for (int32 i = 0; i < numberOfAbilityModifications; i++)
	{
		std::unique_ptr<AbilityModification> abilityModification(new AbilityModification());
		size += abilityModification->read(istream);
		m_abilityModifications.push_back(std::move(abilityModification));
	}

	int32 randomUnitAndItemFlag = 0;
	wc3lib::read(istream, randomUnitAndItemFlag, size);

	if (randomUnitAndItemFlag == 0)
	{
		size += m_neutralPassiveBuildingOrItem.read(istream);
	}
	else if (randomUnitAndItemFlag == 1)
	{
		size += m_randomUnitFromRandomGroup.read(istream);
	}
	else if (randomUnitAndItemFlag == 2)
	{
		int32 randomUnitsSize = 0;
		wc3lib::read(istream, randomUnitsSize, size);

		for (int32 i = 0; i < randomUnitsSize; i++)
		{
			std::unique_ptr<RandomUnit> randomUnit(new RandomUnit());
			size += randomUnit->read(istream);
			m_randomUnits.push_back(std::move(randomUnit));
		}
	}
	else
	{
		throw std::runtime_error("Unknown flag.");
	}

	wc3lib::read(istream, m_customColor, size);
	wc3lib::read(istream, m_waygateDestination, size);
	wc3lib::read(istream, m_creationNumber, size);
	wc3lib::read(istream, m_unknown2, size);

	return size;
}

std::streamsize UnitAndItem::write(OutputStream& ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, typeId(), size);
	wc3lib::write(ostream, variation(), size);
	size += position().write(ostream);
	wc3lib::write(ostream, angle(), size);
	size += scale().write(ostream);
	wc3lib::write(ostream, typeId2(), size);
	wc3lib::write<byte>(ostream, static_cast<byte>(flags()), size);
	wc3lib::write(ostream, unknown0(), size);
	wc3lib::write(ostream, unknown1(), size);
	wc3lib::write(ostream, playerNumber(), size);
	wc3lib::write(ostream, hitPoints(), size);
	wc3lib::write(ostream, manaPoints(), size);
	wc3lib::write(ostream, itemTableIndex(), size);
	// TODO Implement.

	return size;
}

std::streamsize UnitAndItem::DropableItem::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_itemId, size);
	wc3lib::read(istream, m_dropChance, size);

	return size;
}

std::streamsize UnitAndItem::DropableItem::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, itemId(), size);
	wc3lib::write(ostream, dropChance(), size);

	return size;
}

std::streamsize UnitAndItem::DroppedItemSet::read(InputStream &istream)
{
	std::streamsize size = 0;
	int32 numberOfDropableItems = 0;
	wc3lib::read(istream, numberOfDropableItems, size);

	for (int32 i = 0; i < numberOfDropableItems; i++)
	{
		std::unique_ptr<DropableItem> dropableItem(new DropableItem());
		size += dropableItem->read(istream);
		m_dropableItems.push_back(std::move(dropableItem));
	}

	return size;
}

std::streamsize UnitAndItem::DroppedItemSet::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, static_cast<int32>(dropableItems().size()), size);

	for (std::size_t i = 0; i < dropableItems().size(); i++)
	{
		size += dropableItems()[i].write(ostream);
	}

	return size;
}

std::streamsize UnitAndItem::InventoryItem::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_inventorySlot, size);
	wc3lib::read(istream, m_itemId, size);

	return size;
}

std::streamsize UnitAndItem::InventoryItem::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, inventorySlot(), size);
	wc3lib::write(ostream, itemId(), size);

	return size;
}

std::streamsize UnitAndItem::AbilityModification::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_abilityId, size);
	wc3lib::read(istream, m_active, size);
	wc3lib::read(istream, m_level, size);

	return size;
}

std::streamsize UnitAndItem::AbilityModification::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, abilityId(), size);
	wc3lib::write(ostream, active(), size);
	wc3lib::write(ostream, level(), size);

	return size;
}

std::streamsize UnitAndItem::NeutralPassiveBuildingOrItem::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_level, size, 3); // 24 bit
	wc3lib::read(istream, m_itemClass, size);

	return size;
}

std::streamsize UnitAndItem::NeutralPassiveBuildingOrItem::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, level(), size, 3); // 24 bit
	wc3lib::write(ostream, itemClass(), size);

	return size;
}

std::streamsize UnitAndItem::RandomUnitFromRandomGroup::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_unitGroupNumber, size);
	wc3lib::read(istream, m_positionNumber, size);

	return size;
}

std::streamsize UnitAndItem::RandomUnitFromRandomGroup::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, unitGroupNumber(), size);
	wc3lib::write(ostream, positionNumber(), size);

	return size;
}

std::streamsize UnitAndItem::RandomUnit::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_unitId, size);
	wc3lib::read(istream, m_chance, size);

	return size;
}

std::streamsize UnitAndItem::RandomUnit::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, unitId(), size);
	wc3lib::write(ostream, chance(), size);

	return size;
}

}

}
