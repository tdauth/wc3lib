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

CustomObjects::Modification::Modification(BOOST_SCOPED_ENUM(CustomObjects::Type) type) : m_type(type)
{
}

std::streamsize CustomObjects::Modification::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = readData(istream);

	if (this->type() == CustomObjects::Type::Doodads || this->type() == CustomObjects::Type::Abilities || this->type() == CustomObjects::Type::Upgrades)
	{
		wc3lib::read(istream, this->m_level, size);
		wc3lib::read(istream, this->m_data, size);
	}

	int32 end;
	wc3lib::read(istream, end, size);

	return size;
}

std::streamsize CustomObjects::Modification::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = writeData(ostream);

	if (this->type() == CustomObjects::Type::Doodads || this->type() == CustomObjects::Type::Abilities || this->type() == CustomObjects::Type::Upgrades)
	{
		wc3lib::write(ostream, this->m_level, size);
		wc3lib::write(ostream, this->m_data, size);
	}

	int32 end = 0;
	wc3lib::write(ostream, end, size);

	return size;
}

CustomObjects::CustomObjects(BOOST_SCOPED_ENUM(CustomObjects::Type) type) : m_type(type)
{
}

const char8* CustomObjects::fileName() const
{
	string name = "war3map.";

	switch (this->type())
	{
		case Type::Units:
			return name.append("w3u").c_str();

		case Type::Items:
			return name.append("w3t").c_str();

		case Type::Destructibles:
			return name.append("w3b").c_str();

		case Type::Doodads:
			return name.append("w3d").c_str();

		case Type::Abilities:
			return name.append("w3a").c_str();

		case Type::Buffs:
			return name.append("w3h").c_str();

		case Type::Upgrades:
			return name.append("w3q").c_str();
	}

	return name.c_str();
}

CustomUnits::Unit* CustomObjects::createUnit() const
{
	return new CustomObjects::Object(this->type());
}

}

}
