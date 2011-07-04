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


CustomObjectsCollection::~CustomObjectsCollection()
{
	if (hasUnits())
		delete m_units;
	
	if (hasItems())
		delete m_items;
	
	if (hasDestructibles())
		delete m_destructibles;
	
	if (hasDoodads())
		delete m_doodads;
	
	if (hasAbilities())
		delete m_abilities;
	
	if (hasBuffs())
		delete m_buffs;
	
	if (hasUpgrades())
		delete m_upgrades;
}

std::streamsize CustomObjectsCollection::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_version, size);

	if (this->version() != latestFileVersion())
		throw Exception(boost::format(_("W3O: Unknown version %1%, expected %2%.")) % this->version() % latestFileVersion());

	int32 hasUnits;
	wc3lib::read(istream, hasUnits, size);

	if (hasUnits)
	{
		this->m_units = new CustomObjects(CustomObjects::Units);
		size += this->m_units->read(istream);
	}

	int32 hasItems;
	wc3lib::read(istream, hasItems, size);

	if (hasItems)
	{
		this->m_items = new CustomObjects(CustomObjects::Items);
		size += this->m_items->read(istream);
	}

	int32 hasDestructibles;
	wc3lib::read(istream, hasDestructibles, size);

	if (hasDestructibles)
	{
		this->m_destructibles = new CustomObjects(CustomObjects::Destructibles);
		size += this->m_destructibles->read(istream);
	}

	int32 hasDoodads;
	wc3lib::read(istream, hasDoodads, size);

	if (hasDoodads)
	{
		this->m_doodads = new CustomObjects(CustomObjects::Doodads);
		size += this->m_doodads->read(istream);
	}

	int32 hasAbilities;
	wc3lib::read(istream, hasAbilities, size);

	if (hasAbilities)
	{
		this->m_abilities = new CustomObjects(CustomObjects::Abilities);
		size += this->m_abilities->read(istream);
	}

	int32 hasBuffs;
	wc3lib::read(istream, hasBuffs, size);

	if (hasBuffs)
	{
		this->m_buffs = new CustomObjects(CustomObjects::Buffs);
		size += this->m_buffs->read(istream);
	}

	int32 hasUpgrades;
	wc3lib::read(istream, hasUpgrades, size);

	if (hasUpgrades)
	{
		this->m_upgrades = new CustomObjects(CustomObjects::Upgrades);
		size += this->m_upgrades->read(istream);
	}

	return size;
}

std::streamsize CustomObjectsCollection::write(OutputStream &ostream) const throw (class Exception)
{
	if (this->version() != latestFileVersion())
		throw Exception(boost::format(_("W3O: Unknown version %1%, expected %2%.")) % this->version() % latestFileVersion());

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

}

}
