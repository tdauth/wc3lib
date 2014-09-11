/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include "customobjectscollection.hpp"
#include "customobjects.hpp"

namespace wc3lib
{

namespace map
{

CustomObjectsCollection::CustomObjectsCollection() : m_units(0), m_items(0), m_destructibles(0), m_doodads(0), m_abilities(0), m_buffs(0), m_upgrades(0)
{
}

std::streamsize CustomObjectsCollection::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_version, size);

	if (this->version() != latestFileVersion())
	{
		std::cerr << boost::format(_("W3O: Unknown version %1%, expected %2%.")) % this->version() % latestFileVersion() << std::endl;
	}

	int32 hasUnits;
	wc3lib::read(istream, hasUnits, size);

	if (hasUnits)
	{
		this->m_units.reset(new CustomObjects(CustomObjects::Type::Units));
		size += this->m_units->read(istream);
	}

	int32 hasItems;
	wc3lib::read(istream, hasItems, size);

	if (hasItems)
	{
		this->m_items.reset(new CustomObjects(CustomObjects::Type::Items));
		size += this->m_items->read(istream);
	}

	int32 hasDestructibles;
	wc3lib::read(istream, hasDestructibles, size);

	if (hasDestructibles)
	{
		this->m_destructibles.reset(new CustomObjects(CustomObjects::Type::Destructibles));
		size += this->m_destructibles->read(istream);
	}

	int32 hasDoodads;
	wc3lib::read(istream, hasDoodads, size);

	if (hasDoodads)
	{
		this->m_doodads.reset(new CustomObjects(CustomObjects::Type::Doodads));
		size += this->m_doodads->read(istream);
	}

	int32 hasAbilities;
	wc3lib::read(istream, hasAbilities, size);

	if (hasAbilities)
	{
		this->m_abilities.reset(new CustomObjects(CustomObjects::Type::Abilities));
		size += this->m_abilities->read(istream);
	}

	int32 hasBuffs;
	wc3lib::read(istream, hasBuffs, size);

	if (hasBuffs)
	{
		this->m_buffs.reset(new CustomObjects(CustomObjects::Type::Buffs));
		size += this->m_buffs->read(istream);
	}

	int32 hasUpgrades;
	wc3lib::read(istream, hasUpgrades, size);

	if (hasUpgrades)
	{
		this->m_upgrades.reset(new CustomObjects(CustomObjects::Type::Upgrades));
		size += this->m_upgrades->read(istream);
	}

	return size;
}

std::streamsize CustomObjectsCollection::write(OutputStream &ostream) const
{
	if (this->version() != latestFileVersion())
		std::cerr << boost::format(_("W3O: Unknown version %1%, expected %2%.")) % this->version() % latestFileVersion() << std::endl;

	std::streamsize size = 0;
	wc3lib::write(ostream, this->m_version, size);

	if (hasUnits())
		size += this->units()->write(ostream);

	if (hasItems())
		size += this->items()->write(ostream);

	if (hasDestructibles())
		size += this->destructibles()->write(ostream);

	if (hasDoodads())
		size += this->doodads()->write(ostream);

	if (hasAbilities())
		size += this->abilities()->write(ostream);

	if (hasBuffs())
		size += this->buffs()->write(ostream);

	if (hasUpgrades())
		size += this->upgrades()->write(ostream);

	return size;
}

void CustomObjectsCollection::clear()
{
	if (hasUnits())
	{
		this->units()->clear();
	}
	
	if (hasItems())
	{
		this->items()->clear();
	}
	
	if (hasDestructibles())
	{
		this->destructibles()->clear();
	}
	
	if (hasDoodads())
	{
		this->doodads()->clear();
	}
	
	if (hasAbilities())
	{
		this->abilities()->clear();
	}
	
	if (hasBuffs())
	{
		this->buffs()->clear();
	}
	
	if (hasUpgrades())
	{
		this->upgrades()->clear();
	}
}

}

}
