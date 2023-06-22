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

CustomObjects::Set::Set(uint32 version, CustomObjects::Type type) : CustomUnits::Set(version), m_type(type)
{
}

CustomObjects::Set::Set(const CustomObjects::Set &other) : CustomUnits::Set(other), m_type(other.type())
{
}

CustomObjects::Set::~Set()
{
}

CustomUnits::Set* CustomObjects::Set::clone() const
{
	return new Set(*this);
}

CustomUnits::Modification* CustomObjects::Set::createModification() const
{
	return new CustomObjects::Modification(this->type());
}

CustomObjects::Object::Object(uint32 version, CustomObjects::Type type) : CustomUnits::Unit(version), m_type(type)
{
}

CustomObjects::Object::Object(const CustomObjects::Object &other) : CustomUnits::Unit(other), m_type(other.type())
{
}

CustomObjects::Object::~Object()
{
}

CustomUnits::Unit* CustomObjects::Object::clone() const
{
	return new Object(*this);
}

CustomUnits::Set* CustomObjects::Object::createSet() const
{
	return new CustomObjects::Set(this->version(), this->type());
}

CustomObjects::Modification::Modification(CustomObjects::Type type) : m_type(type), m_level(0), m_data(0)
{
}

CustomObjects::Modification::Modification(const CustomObjects::Modification &other) : CustomUnits::Modification(other), m_type(other.type()), m_level(other.level()), m_data(other.data())
{
}

CustomObjects::Modification::~Modification()
{
}

std::streamsize CustomObjects::Modification::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_id, size);
	Value::Type type;
	wc3lib::read<int32>(istream, (int32&)type, size);

	/*
	 * As the specification states only these three types use optional integer values to specify a level
	 * and a specific data field for a modification.
	 * This data is placed before the actual modification data.
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

std::streamsize CustomObjects::Modification::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->valueId(), size);
	wc3lib::write<int32>(ostream, static_cast<int32>(this->value().type()), size);

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

CustomUnits::Modification* CustomObjects::Modification::clone() const
{
	return new CustomObjects::Modification(*this);
}

CustomObjects::CustomObjects(CustomObjects::Type type) : m_type(type)
{
	// set latest file version by default
	this->m_version = latestFileVersion();
}

CustomObjects::CustomObjects(const CustomObjects &other) : CustomUnits(other), m_type(other.type())
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
	return 3; // 2 Frozen Throne (level data), 3 Reforged (sets)
}

CustomUnits::Unit* CustomObjects::createUnit() const
{
	return new CustomObjects::Object(this->version(), this->type());
}

}

}
