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

#ifndef WC3LIB_MAP_UNITANDITEM_HPP
#define WC3LIB_MAP_UNITANDITEM_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

// TODO Adapt to specs for Reforged and TFT https://github.com/ChiefOfGxBxL/WC3MapTranslator
class UnitAndItem : public Format
{
	public:
		enum class Flags : byte
		{
			Invisible = 0,
			Visibile = 1,
			Normal = 2,
			OutsideCameraBounds = 3 // (set to 3 when it's outside the camerabounds?)
		};

		class DropableItem : public Format
		{
			public:
				virtual std::streamsize read(InputStream &istream);
				virtual std::streamsize write(OutputStream &ostream) const;

				id itemId() const;
				int32 dropChance() const;

			private:
				id m_itemId;
				int32 m_dropChance;
		};

		typedef boost::ptr_vector<DropableItem> DropableItemContainer;

		class DroppedItemSet : public Format
		{
			public:
				virtual std::streamsize read(InputStream &istream);
				virtual std::streamsize write(OutputStream &ostream) const;

				const DropableItemContainer& dropableItems() const;

			private:
				DropableItemContainer m_dropableItems;
		};

		typedef boost::ptr_vector<DroppedItemSet> DroppedItemSetContainer;

		class InventoryItem : public Format
		{
			public:
				virtual std::streamsize read(InputStream &istream);
				virtual std::streamsize write(OutputStream &ostream) const;

				int32 inventorySlot() const;
				id itemId() const;

			private:
				int32 m_inventorySlot;
				id m_itemId;
		};

		typedef boost::ptr_vector<InventoryItem> InventoryItemContainer;

		class AbilityModification : public Format
		{
			public:
				virtual std::streamsize read(InputStream &istream);
				virtual std::streamsize write(OutputStream &ostream) const;

				id abilityId() const;
				int32 active() const;
				int32 level() const;

			private:
				id m_abilityId;
				int32 m_active;
				int32 m_level;
		};

		typedef boost::ptr_vector<AbilityModification> AbilityModificationContainer;

		class NeutralPassiveBuildingOrItem : public Format
		{
			public:
				virtual std::streamsize read(InputStream &istream);
				virtual std::streamsize write(OutputStream &ostream) const;

				int32 level() const;
				byte itemClass() const;

			private:
				int32 m_level; // 24 bit int
				byte m_itemClass;
		};

		class RandomUnitFromRandomGroup : public Format
		{
			public:
				virtual std::streamsize read(InputStream &istream);
				virtual std::streamsize write(OutputStream &ostream) const;

				int32 unitGroupNumber() const;
				int32 positionNumber() const;

			private:
				int32 m_unitGroupNumber;
				int32 m_positionNumber;
		};

		class RandomUnit : public Format
		{
			public:
				virtual std::streamsize read(InputStream &istream);
				virtual std::streamsize write(OutputStream &ostream) const;

				id unitId() const;
				int32 chance() const;

			private:
				id m_unitId;
				int32 m_chance;
		};

		typedef boost::ptr_vector<RandomUnit> RandomUnitContainer;

		virtual std::streamsize read(InputStream &istream);
		virtual std::streamsize write(OutputStream &ostream) const;

		id typeId() const;
		int32 variation() const;
		const Vertex3d<float32>& position() const;
		float32 angle() const;
		const Vertex3d<float32>& scale() const;
		id typeId2() const;
		Flags flags() const;
		int32 unknownX() const;
		int32 playerNumber() const;
		byte unknown0() const;
		byte unknown1() const;
		int32 hitPoints() const;
		int32 manaPoints() const;
		int32 itemTableIndex() const;

	protected:
		id m_typeId;// iDNR can be used for random item, and uDNR for random unit
		int32 m_variation;
		Vertex3d<float32> m_position;
		float32 m_angle;
		Vertex3d<float32> m_scale;
		id m_typeId2;
		Flags m_flags;
		int32 m_unknownX;
		int32 m_playerNumber;
		byte m_unknown0;
		byte m_unknown1;
		int32 m_hitPoints; // -1 default
		int32 m_manaPoints; // -1 default, 0 unit doesn't have mana
		int32 m_itemTableIndex; // -1 no item table used
		DroppedItemSetContainer m_droppedItemSets;
		int32 m_gold;
		float32 m_targetAcquisition; // -1 = normal, -2 = camp
		int32 m_heroLevel;
		int32 m_strength;
		int32 m_agility;
		int32 m_intelligence;
		InventoryItemContainer m_inventoryItems;
		AbilityModificationContainer m_abilityModifications;

		// random data
		// TODO Use variant to store one of the three.
		NeutralPassiveBuildingOrItem m_neutralPassiveBuildingOrItem;
		RandomUnitFromRandomGroup m_randomUnitFromRandomGroup;
		RandomUnitContainer m_randomUnits;

		int32 m_customColor;
		int32 m_waygateDestination;
		int32 m_creationNumber;

		int32 m_unknown2;
};

inline id UnitAndItem::typeId() const
{
	return m_typeId;
}

inline int32 UnitAndItem::variation() const
{
	return m_variation;
}

inline const Vertex3d<float32>& UnitAndItem::position() const
{
	return m_position;
}

inline float32 UnitAndItem::angle() const
{
	return m_angle;
}

inline const Vertex3d<float32>& UnitAndItem::scale() const
{
	return m_scale;
}

inline id UnitAndItem::typeId2() const
{
	return m_typeId2;
}

inline UnitAndItem::Flags UnitAndItem::flags() const
{
	return m_flags;
}

inline int32 UnitAndItem::unknownX() const
{
	return m_unknownX;
}

inline int32 UnitAndItem::playerNumber() const
{
	return m_playerNumber;
}

inline byte UnitAndItem::unknown0() const
{
	return m_unknown0;
}

inline byte UnitAndItem::unknown1() const
{
	return m_unknown1;
}

inline int32 UnitAndItem::hitPoints() const
{
	return m_hitPoints;
}

inline int32 UnitAndItem::manaPoints() const
{
	return m_manaPoints;
}

inline int32 UnitAndItem::itemTableIndex() const
{
	return m_itemTableIndex;
}

inline id UnitAndItem::DropableItem::itemId() const
{
	return m_itemId;
}

inline int32 UnitAndItem::DropableItem::dropChance() const
{
	return m_dropChance;
}

inline const UnitAndItem::DropableItemContainer& UnitAndItem::DroppedItemSet::dropableItems() const
{
	return m_dropableItems;
}

inline int32 UnitAndItem::InventoryItem::inventorySlot() const
{
	return m_inventorySlot;
}

inline id UnitAndItem::InventoryItem::itemId() const
{
	return m_itemId;
}

inline id UnitAndItem::AbilityModification::abilityId() const
{
	return m_abilityId;
}

inline int32 UnitAndItem::AbilityModification::active() const
{
	return m_active;
}

inline int32 UnitAndItem::AbilityModification::level() const
{
	return m_level;
}

inline int32 UnitAndItem::NeutralPassiveBuildingOrItem::level() const
{
	return m_level;
}

inline byte UnitAndItem::NeutralPassiveBuildingOrItem::itemClass() const
{
	return m_itemClass;
}

inline int32 UnitAndItem::RandomUnitFromRandomGroup::unitGroupNumber() const
{
	return m_unitGroupNumber;
}

inline int32 UnitAndItem::RandomUnitFromRandomGroup::positionNumber() const
{
	return m_positionNumber;
}

inline id UnitAndItem::RandomUnit::unitId() const
{
	return m_unitId;
}

inline int32 UnitAndItem::RandomUnit::chance() const
{
	return m_chance;
}

}

}

#endif
