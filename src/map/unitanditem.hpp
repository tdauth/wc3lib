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

		class DroppedItemSet : public Format
		{
			/*
			 * int: number "d" of dropable items
			"d" times dropable items structures:
			char[4]: item ID ([00 00 00 00]h = none)
			this can also be a random item id (see below)
			int: % chance to be dropped
			*/
			virtual std::streamsize read(InputStream &istream);
			virtual std::streamsize write(OutputStream &ostream) const;
		};

		typedef boost::ptr_vector<DroppedItemSet> DroppedItemSetContainer;

		class InventoryItem : public Format
		{
			/*
int: inventory slot (this is the actual slot - 1, so 1 => 0)
char[4]: item id (as in ItemData.slk) 0x00000000 = none
this can also be a random item id (see below)
			*/
			virtual std::streamsize read(InputStream &istream);
			virtual std::streamsize write(OutputStream &ostream) const;
		};

		typedef boost::ptr_vector<InventoryItem> InventoryItemContainer;

		class AbilityModification : public Format
		{
			/*
char[4]: ability id (as in AbilityData.slk)
int: active for autocast abilities, 0 = no, 1 = active
int: level for hero abilities
			*/
			virtual std::streamsize read(InputStream &istream);
			virtual std::streamsize write(OutputStream &ostream) const;
		};

		typedef boost::ptr_vector<AbilityModification> AbilityModificationContainer;

		class NeutralPassiveBuildingOrItem : public Format
		{
			/*
byte[3]: level of the random unit/item,-1 = any (this is actually interpreted as a 24-bit number)
  byte: item class of the random item, 0 = any, 1 = permanent ... (this is 0 for units)
  r is also 0 for non random units/items so we have these 4 bytes anyway (even if the id wasn't uDNR or iDNR)
			*/
			virtual std::streamsize read(InputStream &istream);
			virtual std::streamsize write(OutputStream &ostream) const;
		};

		class RandomUnitFromRandomGroup : public Format
		{
			/*
int: unit group number (which group from the global table)
  int: position number (which column of this group)
  the column should of course have the item flag set (in the w3i) if this is a random item
			*/
			virtual std::streamsize read(InputStream &istream);
			virtual std::streamsize write(OutputStream &ostream) const;
		};

		class RandomUnit : public Format
		{
		/*
		 * char[4]: unit id (as in UnitUI.slk)
this can also be a random unit id (see below)
int: percentual chance of choice
*/
			virtual std::streamsize read(InputStream &istream);
			virtual std::streamsize write(OutputStream &ostream) const;
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
		int32 playerNumber() const;
		byte unknown0() const;
		byte unknown1() const;
		int32 hitPoints() const;
		int32 manaPoints() const;
		int32 itemTableIndex() const;

	protected:
		id m_typeId;
		int32 m_variation;
		Vertex3d<float32> m_position;
		float32 m_angle;
		Vertex3d<float32> m_scale;
		id m_typeId2;
		Flags m_flags;
		int32 m_playerNumber;
		byte m_unknown0;
		byte m_unknown1;
		int32 m_hitPoints; // -1 default
		int32 m_manaPoints; // -1 default, 0 unit doesn't have mana
		int32 m_itemTableIndex; // -1 no item table used
		DroppedItemSetContainer m_droppedItemSets;
		int32 m_gold;
		float32 m_targetAcquisition;
		int32 m_heroLevel;
		int32 m_strength;
		int32 m_agility;
		int32 m_intelligence;
		InventoryItemContainer m_inventoryItems;
		AbilityModificationContainer m_abilityModifications;

		// random data
		NeutralPassiveBuildingOrItem m_neutralPassiveBuildingOrItem;
		RandomUnitFromRandomGroup m_randomUnitFromRandomGroup;
		RandomUnitContainer m_randomUnits;

		int32 m_customColor;
		int32 m_waygateDestination;
		int32 m_creationNumber;
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

}

}

#endif
