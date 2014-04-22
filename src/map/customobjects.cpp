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

#include "customobjects.hpp"
#include "../utilities.hpp"


namespace wc3lib
{

namespace map
{

CustomObjects::Object::Object(BOOST_SCOPED_ENUM(CustomObjects::Type) type) : m_type(type)
{
}

CustomUnits::Modification* CustomObjects::Object::createModification() const
{
	return new CustomObjects::Modification(this->type());
}

CustomObjects::Modification::Modification(BOOST_SCOPED_ENUM(CustomObjects::Type) type) : m_type(type), m_level(0), m_data(0)
{
}

std::streamsize CustomObjects::Modification::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_id, size);
	BOOST_SCOPED_ENUM(Value::Type) type;
	wc3lib::read<int32>(istream, (int32&)type, size);

	/*
	 * As the specification states only these three types use optional integer values to specify a level
	 * and a specific data field for a modification.
	 * This data is placed before the actual modification data.
	 * 
	 *
	 */
	if (this->type() == CustomObjects::Type::Doodads || this->type() == CustomObjects::Type::Abilities || this->type() == CustomObjects::Type::Upgrades)
	{
		wc3lib::read(istream, this->m_level, size);
		wc3lib::read(istream, this->m_data, size);
	}
	
	size += readData(istream, type);

	int32 end;
	wc3lib::read(istream, end, size);

	return size;
}

std::streamsize CustomObjects::Modification::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->valueId(), size);
	wc3lib::write<int32>(ostream, this->value().type(), size);

	if (this->type() == CustomObjects::Type::Doodads || this->type() == CustomObjects::Type::Abilities || this->type() == CustomObjects::Type::Upgrades)
	{
		wc3lib::write(ostream, this->level(), size);
		wc3lib::write(ostream, this->data(), size);
	}

	size += writeData(ostream, this->value().type());
	
	int32 end = 0;
	wc3lib::write(ostream, end, size);

	return size;
}

CustomObjects::CustomObjects(BOOST_SCOPED_ENUM(CustomObjects::Type) type) : m_type(type)
{
}

const byte* CustomObjects::fileName() const
{
	switch (this->type())
	{
		case Type::Units:
			return "war3map.w3u";

		case Type::Items:
			return "war3map.w3t";

		case Type::Destructibles:
			return "war3map.w3b";

		case Type::Doodads:
			return "war3map.w3d";

		case Type::Abilities:
			return "war3map.w3a";

		case Type::Buffs:
			return "war3map.w3h";

		case Type::Upgrades:
			return "war3map.w3q";
	}

	return "";
}

uint32 CustomObjects::latestFileVersion() const
{
	return 2;
}


CustomUnits::Unit* CustomObjects::createUnit() const
{
	return new CustomObjects::Object(this->type());
}

}

}
