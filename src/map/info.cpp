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

#include <boost/foreach.hpp>

#include "info.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize Info::Player::read(InputStream &istream) throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_number, size);
	wc3lib::read<int32>(istream, (int32&)m_type, size);
	wc3lib::read<int32>(istream, (int32&)m_race, size);
	wc3lib::read<int32>(istream, (int32&)m_hasFixedStartPosition, size);
	wc3lib::readString(istream, m_name, size);
	size += m_position.read(istream);
	wc3lib::read(istream, m_allowLowPriorityFlags, size);
	wc3lib::read(istream, m_allowHighPriorityFlags, size);

	return size;
}

std::streamsize Info::Player::write(OutputStream &ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, number(), size);
	wc3lib::write<int32>(ostream, static_cast<int32>(type()), size);
	wc3lib::write<int32>(ostream, static_cast<int32>(race()), size);
	wc3lib::write<int32>(ostream, hasFixedStartPosition(), size);
	wc3lib::writeString(ostream, name(), size);
	size += position().write(ostream);
	wc3lib::write(ostream, allowLowPriorityFlags(), size);
	wc3lib::write(ostream, allowHighPriorityFlags(), size);

	return size;
}

std::streamsize Info::Force::read(InputStream &istream) throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::read<int32>(istream, (int32&)m_flags, size);
	wc3lib::read(istream, m_playerMask, size);
	wc3lib::readString(istream, m_name, size);

	return size;
}

std::streamsize Info::Force::write(OutputStream &ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, static_cast<int32>(flags()), size);
	wc3lib::write(ostream, playerMask(), size);
	wc3lib::writeString(ostream, name(), size);

	return size;
}

Info::UpgradeAvailability::UpgradeAvailability()
: m_playerMask(0)
, m_upgrade(0)
, m_level(0)
, m_availability(Availability::Unavailable)
{
}

std::streamsize Info::UpgradeAvailability::read(InputStream &istream) throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_playerMask, size);
	wc3lib::read(istream, m_upgrade, size);
	wc3lib::read(istream, m_level, size);
	wc3lib::read(istream, m_availability, size);

	return size;
}

std::streamsize Info::UpgradeAvailability::write(OutputStream &ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, playerMask(), size);
	wc3lib::write(ostream, upgrade(), size);
	wc3lib::write(ostream, level(), size);
	wc3lib::write<int32>(ostream, static_cast<int32>(availability()), size);

	return size;
}

std::streamsize Info::TechAvailability::read(InputStream &istream) throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_playerMask, size);
	wc3lib::read(istream, m_tech, size);

	return size;
}

std::streamsize Info::TechAvailability::write(OutputStream &ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, playerMask(), size);
	wc3lib::write(ostream, tech(), size);

	return size;
}


Info::RandomUnitTable::Group::Column::Column() : m_type(Column::Type::UnitTable)
{

}

std::streamsize Info::RandomUnitTable::Group::Column::read(InputStream& istream) throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::read<int32>(istream, (int32&)m_type, size);

	return size;
}

std::streamsize Info::RandomUnitTable::Group::Column::write(OutputStream& ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, static_cast<int32>(type()), size);

	return size;
}

std::streamsize Info::RandomUnitTable::Group::read(InputStream &istream) throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_number, size);
	wc3lib::readString(istream, m_name, size);
	int32 number = 0;
	wc3lib::read(istream, number, size);
	m_columns.reserve(number);

	// columns
	for (int32 i = 0; i < number; ++i)
	{
		std::auto_ptr<Column> ptr(new Column());
		size += ptr->read(istream);
		m_columns.push_back(ptr);
	}

	// number of rows
	wc3lib::read(istream, number, size);
	m_chances.resize(number);

	for (int32 i = 0; i < columns().size(); ++i)
	{
		columns()[i].rows().resize(number);
	}

	for (int32 i = 0; i < number; ++i)
	{
		wc3lib::read(istream, m_chances[i], size);

		// read the id per column of the row
		for (int32 j = 0; j < columns().size(); ++j)
		{
			wc3lib::read(istream, columns()[j].rows()[i], size);
		}
	}

	return size;
}

std::streamsize Info::RandomUnitTable::Group::write(OutputStream &ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, number(), size);
	wc3lib::writeString(ostream, name(), size);
	wc3lib::write<int32>(ostream, columns().size(), size);

	BOOST_FOREACH(Columns::const_reference column, columns())
	{
		size += column.write(ostream);
	}

	// rows
	wc3lib::write<int32>(ostream, chances().size(), size);
	int32 i = 0;

	BOOST_FOREACH(Chances::const_reference chance, chances())
	{
		wc3lib::write(ostream, chance, size);

		BOOST_FOREACH(Columns::const_reference column, columns())
		{
			wc3lib::write(ostream, column.rows()[i], size);
		}

		++i;
	}

	return size;
}

std::streamsize Info::RandomUnitTable::read(InputStream &istream) throw (Exception)
{
	std::streamsize size = 0;
	int32 number = 0;
	wc3lib::read(istream, number, size);
	this->m_groups.reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::auto_ptr<Group> group(new Group());
		size += group->read(istream);
		this->m_groups.push_back(group);
	}

	return size;
}

std::streamsize Info::RandomUnitTable::write(OutputStream &ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, groups().size(), size);

	BOOST_FOREACH(Groups::const_reference group, groups())
	{
		size += group.write(ostream);
	}

	return size;
}

std::streamsize Info::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_version, size);

	if (version() != latestFileVersion())
	{
		std::cerr << boost::format(_("Info: Unsupported version %1%. Latest version is %2%.")) % version() % latestFileVersion() << std::endl;
	}

	wc3lib::read(istream, m_mapVersion, size);
	wc3lib::read(istream, m_editorVersion, size);
	wc3lib::readString(istream, m_name, size);
	wc3lib::readString(istream, m_author, size);
	wc3lib::readString(istream, m_description, size);
	wc3lib::readString(istream, m_playersRecommended, size);
	wc3lib::read(istream, m_cameraBoundsJass, size);
	wc3lib::read(istream, m_cameraBounds, size);
	wc3lib::read(istream, m_playableWidth, size);
	wc3lib::read(istream, m_playableHeight, size);
	wc3lib::read<int32>(istream, (int32&)m_flags, size);
	wc3lib::read(istream, m_mainGroundType, size);
	wc3lib::read(istream, m_campaignBackgroundIndex, size);
	wc3lib::readString(istream, m_loadingScreenText, size);
	wc3lib::readString(istream, m_loadingScreenTitle, size);
	wc3lib::readString(istream, m_loadingScreenSubtitle, size);
	wc3lib::read(istream, m_loadingScreenIndex, size);
	wc3lib::readString(istream, m_prologueScreenText, size);
	wc3lib::readString(istream, m_prologueScreenTitle, size);
	wc3lib::readString(istream, m_prologueScreenSubtitle, size);

	int32 number = 0;
	wc3lib::read(istream, number, size);
	m_players.reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::auto_ptr<Player> player(new Player());
		size += player->read(istream);
		m_players.push_back(player);
	}

	number = 0;
	wc3lib::read(istream, number, size);
	m_forces.reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::auto_ptr<Force> force(new Force());
		size += force->read(istream);
		m_forces.push_back(force);
	}

	number = 0;
	wc3lib::read(istream, number, size);
	m_upgradeAvailabilities.reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::auto_ptr<UpgradeAvailability> upgradeAvailability(new UpgradeAvailability());
		size += upgradeAvailability->read(istream);
		m_upgradeAvailabilities.push_back(upgradeAvailability);
	}

	wc3lib::read(istream, number, size);
	m_techAvailabilities.reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::auto_ptr<TechAvailability> techAvailability(new TechAvailability());
		size += techAvailability->read(istream);
		m_techAvailabilities.push_back(techAvailability);
	}

	wc3lib::read(istream, number, size);
	m_randomUnitTables.reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::auto_ptr<RandomUnitTable> randomUnitTable(new RandomUnitTable());
		size += randomUnitTable->read(istream);
		m_randomUnitTables.push_back(randomUnitTable);
	}

	return size;
}

std::streamsize Info::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;

	if (version() != latestFileVersion())
	{
		std::cerr << boost::format(_("Info: Unsupported version %1%. Latest version is %2%.")) % version() % latestFileVersion() << std::endl;
	}

	wc3lib::write(ostream, version(), size);
	wc3lib::write(ostream, mapVersion(), size);
	wc3lib::write(ostream, editorVersion(), size);
	wc3lib::writeString(ostream, name(), size);
	wc3lib::writeString(ostream, author(), size);
	wc3lib::writeString(ostream, description(), size);
	wc3lib::writeString(ostream, playersRecommended(), size);
	// TODO write arrays by using member functions and sizes
	wc3lib::write(ostream, m_cameraBoundsJass, size);
	wc3lib::write(ostream, m_cameraBounds, size);
	wc3lib::write(ostream, playableWidth(), size);
	wc3lib::write(ostream, playableHeight(), size);
	wc3lib::write<int32>(ostream, static_cast<int32>(flags()), size);
	wc3lib::write(ostream, mainGroundType(), size);
	wc3lib::write(ostream, campaignBackgroundIndex(), size);
	wc3lib::writeString(ostream, loadingScreenText(), size);
	wc3lib::writeString(ostream, loadingScreenTitle(), size);
	wc3lib::writeString(ostream, loadingScreenSubtitle(), size);
	wc3lib::write(ostream, loadingScreenIndex(), size);
	wc3lib::writeString(ostream, prologueScreenText(), size);
	wc3lib::writeString(ostream, prologueScreenTitle(), size);
	wc3lib::writeString(ostream, prologueScreenSubtitle(), size);

	wc3lib::write<int32>(ostream, players().size(), size);

	BOOST_FOREACH(Players::const_reference player, players())
	{
		size += player.write(ostream);
	}


	wc3lib::write<int32>(ostream, forces().size(), size);

	BOOST_FOREACH(Forces::const_reference force, forces())
	{
		size += force.write(ostream);
	}

	wc3lib::write<int32>(ostream, upgradeAvailabilities().size(), size);

	BOOST_FOREACH(UpgradeAvailabilities::const_reference upgradeAvailability, upgradeAvailabilities())
	{
		size += upgradeAvailability.write(ostream);
	}

	wc3lib::write<int32>(ostream, techAvailabilities().size(), size);

	BOOST_FOREACH(TechAvailabilities::const_reference techAvailability, techAvailabilities())
	{
		size += techAvailability.write(ostream);
	}

	wc3lib::write<int32>(ostream, randomUnitTables().size(), size);

	BOOST_FOREACH(RandomUnitTables::const_reference randomUnitTable, randomUnitTables())
	{
		size += randomUnitTable.write(ostream);
	}

	return size;
}

void Info::clear()
{
	this->players().clear();
	this->forces().clear();
	this->upgradeAvailabilities().clear();
	this->techAvailabilities().clear();
	this->randomUnitTables().clear();
}

}

}
