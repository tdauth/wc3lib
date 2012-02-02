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

#include "customunits.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace map
{

CustomUnits::Modification::Modification()
{
}

CustomUnits::Modification::~Modification()
{
}

std::streamsize CustomUnits::Modification::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = readData(istream);
	int32 end;
	wc3lib::read(istream, end, size); // usually 0

	if (end != 0)
		std::cerr << boost::format(_("Modification end byte is not 0: ")) % end << std::endl;

	return size;
}

std::streamsize CustomUnits::Modification::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = writeData(ostream);
	int32 end = 0;
	wc3lib::write(ostream, end, size);

	return size;
}

std::streamsize CustomUnits::Modification::readList(InputStream &istream)
{
	std::streamsize size = 0;
	string str;
	wc3lib::readString(istream, str, size);
	boost::algorithm::split(this->value().toList(), str, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);

	return size;
}

std::streamsize CustomUnits::Modification::readData(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_id, size);
	int32 type;
	wc3lib::read(istream, type, size);
	this->m_value = Value(type);

	switch (BOOST_SCOPED_ENUM(Value::Type)(type))
	{
		case Value::Type::Integer:
			wc3lib::read(istream, this->m_value.toInteger(), size);

			break;

		case Value::Type::Real:
			wc3lib::read(istream, this->m_value.toReal(), size);

			break;

		case Value::Type::Unreal:
			wc3lib::read(istream, this->value().toUnreal(), size);

			break;

		case Value::Type::String:
		case Value::Type::RegenerationType:
		case Value::Type::AttackType:
		case Value::Type::WeaponType:
		case Value::Type::TargetType:
		case Value::Type::MoveType:
		case Value::Type::DefenseType:
		case Value::Type::PathingTexture:
		case Value::Type::MissileArt:
		case Value::Type::AttributeType:
		case Value::Type::AttackBits:
			wc3lib::readString(istream, this->value().toString(), size);

			break;

		case Value::Type::Boolean:
			wc3lib::read(istream, this->value().toBoolean(), size);

			break;

		case Value::Type::Character:
			wc3lib::read(istream, this->value().toCharacter(), size);

			break;

		case Value::Type::UnitList:
		case Value::Type::ItemList:
		case Value::Type::UpgradeList:
		case Value::Type::StringList:
		case Value::Type::AbilityList:
		case Value::Type::HeroAbilityList:
			size += this->readList(istream);

			break;
	}

	return size;
}

std::streamsize CustomUnits::Modification::writeData(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->valueId(), size);
	wc3lib::write<int32>(ostream, this->value().type(), size);

	switch (this->value().type())
	{
		case Value::Type::Integer:
			wc3lib::write(ostream, this->value().toInteger(), size);

			break;

		case Value::Type::Real:
			wc3lib::write(ostream, this->value().toReal(), size);

			break;

		// TODO FINISH
	}

	return size;
}

CustomUnits::Unit::Unit()
{
}

CustomUnits::Unit::~Unit()
{
}

std::streamsize CustomUnits::Unit::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_originalId, size);
	wc3lib::read(istream, this->m_customId, size);
	int32 modifications;
	wc3lib::read(istream, modifications, size);
	this->modifications().resize(modifications);

	for (int32 i = 0; i < modifications; ++i)
	{
		ModificationPtr modification(createModification());
		size += modification->read(istream);
		this->modifications()[i].swap(modification);
	}

	return size;
}

std::streamsize CustomUnits::Unit::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, originalId(), size);
	wc3lib::write(ostream, customId(), size);
	wc3lib::write<int32>(ostream, modifications().size(), size);

	BOOST_FOREACH(Modifications::const_reference modification, this->modifications())
		size += modification->write(ostream);

	return size;
}

CustomUnits::Modification* CustomUnits::Unit::createModification() const
{
	return new Modification();
}

CustomUnits::CustomUnits()
{
}

CustomUnits::~CustomUnits()
{
}

std::streamsize CustomUnits::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_version, size);

	if (this->version() != latestFileVersion())
		throw Exception(boost::format(_("Custom Units: Unknown version \"%1%\", expected \"%2%\".")) % this->version() % latestFileVersion());

	int32 originalUnits;
	wc3lib::read(istream, originalUnits, size);
	this->originalTable().resize(originalUnits);

	for (int32 i = 0; i < originalUnits; ++i)
	{
		UnitPtr unit(createUnit());
		size += unit->read(istream);
		originalTable()[i].swap(unit);
	}

	int32 customUnits;
	wc3lib::read(istream, customUnits, size);
	this->customTable().resize(customUnits);

	for (int32 i = 0; i < customUnits; ++i)
	{
		UnitPtr unit(createUnit());
		size += unit->read(istream);
		customTable()[i].swap(unit);
	}

	return size;
}

std::streamsize CustomUnits::write(OutputStream &ostream) const throw (class Exception)
{
	if (version() != latestFileVersion())
		throw Exception(boost::format(_("Custom Units: Unknown version \"%1%\", expected \"%2%\".")) % version() % latestFileVersion());

	std::streamsize size = 0;
	wc3lib::write(ostream, version(), size);
	wc3lib::write<int32>(ostream, originalTable().size(), size);

	BOOST_FOREACH(Table::const_reference unit, originalTable())
		size += unit->write(ostream);

	wc3lib::write<int32>(ostream, customTable().size(), size);

	BOOST_FOREACH(Table::const_reference unit, customTable())
		size += unit->write(ostream);

	return size;
}

CustomUnits::Unit* CustomUnits::createUnit() const
{
	return new Unit();
}

}

}
