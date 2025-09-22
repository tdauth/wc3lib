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
	// TODO Implement.

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

std::streamsize UnitAndItem::DroppedItemSet::read(InputStream &istream)
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::DroppedItemSet::write(OutputStream &ostream) const
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::InventoryItem::read(InputStream &istream)
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::InventoryItem::write(OutputStream &ostream) const
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::AbilityModification::read(InputStream &istream)
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::AbilityModification::write(OutputStream &ostream) const
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::NeutralPassiveBuildingOrItem::read(InputStream &istream)
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::NeutralPassiveBuildingOrItem::write(OutputStream &ostream) const
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::RandomUnitFromRandomGroup::read(InputStream &istream)
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::RandomUnitFromRandomGroup::write(OutputStream &ostream) const
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::RandomUnit::read(InputStream &istream)
{
	std::streamsize size = 0;

	return size;
}

std::streamsize UnitAndItem::RandomUnit::write(OutputStream &ostream) const
{
	std::streamsize size = 0;

	return size;
}

}

}
