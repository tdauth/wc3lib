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

CustomUnits::Modification::Modification() : m_id(0)
{
}

CustomUnits::Modification::Modification(const CustomUnits::Modification &other) : m_id(other.m_id), m_value(other.m_value)
{
}

CustomUnits::Modification::~Modification()
{
}

std::streamsize CustomUnits::Modification::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_id, size);
	int32 type;
	wc3lib::read<int32>(istream, type, size);
	size += readData(istream, type, static_cast<Value::Type>(type));

	// strings and lists (are strings as well) do already have to end with a zero terminating byte
	//if (!this->value().isString() && !this->value().isList())
	//{
		int32 end = 0;
		wc3lib::read(istream, end, size); // usually 0

		if (end != 0)
		{
			std::cerr << boost::format(_("Modification \"%1%\" with type %2% end byte is not 0: %3%")) % this->valueId() % static_cast<int32>(this->value().type()) % end << std::endl;
		}
	//}

	return size;
}

std::streamsize CustomUnits::Modification::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->valueId(), size);
	wc3lib::write<int32>(ostream, static_cast<int32>(this->value().type()), size);

	size += writeData(ostream, static_cast<int32>(this->value().type()), this->value().type());

	// strings and lists (are strings as well) do already have to end with a zero terminating byte
	//if (!this->value().isString() && !this->value().isList())
		wc3lib::write<int32>(ostream, 0, size);

	return size;
}

CustomUnits::Modification* CustomUnits::Modification::clone() const
{
	return new CustomUnits::Modification(*this);
}

std::streamsize CustomUnits::Modification::readList(InputStream &istream, Value::Type type)
{
	std::streamsize size = 0;
	string str;
	wc3lib::readString(istream, str, size);
	List list;
	boost::algorithm::split(list, str, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
	this->m_value = Value(list, type);

	return size;
}

std::streamsize CustomUnits::Modification::writeList(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::writeString(ostream, boost::algorithm::join(value().toList(), ":"), size);

	return size;
}

std::streamsize CustomUnits::Modification::readData(InputStream &istream, int32 t, Value::Type type)
{
	std::streamsize size = 0;

	switch (type)
	{
		case Value::Type::Integer:
		{
			int32 v;
			wc3lib::read(istream, v, size);
			this->m_value = Value(v);

			//std::cerr << "Reading integer modification with value" << v << std::endl;

			break;
		}

		case Value::Type::Real:
		case Value::Type::Unreal:
		{
			//std::cerr << "Reading real modification" << std::endl;

			float32 v;
			wc3lib::read(istream, v, size);
			this->m_value = Value(v, type);

			break;
		}

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
		{
			//std::cerr << "Reading string modification" << std::endl;

			string v;
			wc3lib::readString(istream, v, size);
			this->m_value = Value(v, type);

			break;
		}

		case Value::Type::Boolean:
		{
			//std::cerr << "Reading boolean modification" << std::endl;

			int32 v;
			wc3lib::read(istream, v, size);
			this->m_value = Value((bool)v);

			break;
		}

		case Value::Type::Character:
		{
			//std::cerr << "Reading character modification" << std::endl;

			byte v;
			wc3lib::read(istream, v, size);
			this->m_value = Value(v);

			break;
		}

		case Value::Type::UnitList:
		case Value::Type::ItemList:
		case Value::Type::UpgradeList:
		case Value::Type::StringList:
		case Value::Type::AbilityList:
		case Value::Type::HeroAbilityList:
		{
			//std::cerr << "Reading list modification" << std::endl;

			size += this->readList(istream, type);
			this->writeList(std::cout);
			std::cout << std::endl;

			break;
		}

		default:
		{
			std::cerr << boost::format(_("Unknown value type %1% for modification %2%.")) % t % idToString(this->m_id) << std::endl;

			int32 v;
			wc3lib::read(istream, v, size);
			this->m_value = Value(v);

			break;
		}
	}

	// TEST
	//std::cout << "Id: " << this->valueId() << "\nType: " << type << "\nValue: " << this->value() << std::endl;
	//std::streamsize tmpSize = 0;
	//boost::apply_visitor(ValueWriter(std::cout, tmpSize), this->value());

	return size;
}

std::streamsize CustomUnits::Modification::writeData(OutputStream &ostream, int32 t, Value::Type type) const
{
	std::streamsize size = 0;

	switch (this->value().type())
	{
		case Value::Type::Integer:
		{
			wc3lib::write(ostream, this->value().toInteger(), size);

			break;
		}

		case Value::Type::Real:
		case Value::Type::Unreal:
		{
			wc3lib::write(ostream, this->value().toReal(), size);

			break;
		}

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
		{
			wc3lib::writeString(ostream, this->value().toString(), size);

			break;
		}

		case Value::Type::Boolean:
		{
			wc3lib::write<int32>(ostream, this->value().toBoolean(), size);

			break;
		}

		case Value::Type::Character:
		{
			wc3lib::write(ostream, this->value().toCharacter(), size);

			break;
		}

		case Value::Type::UnitList:
		case Value::Type::ItemList:
		case Value::Type::UpgradeList:
		case Value::Type::StringList:
		case Value::Type::AbilityList:
		case Value::Type::HeroAbilityList:
		{
			size += this->writeList(ostream);

			break;
		}

		default:
		{
			std::cerr << boost::format(_("Unknown value type %1% for modification %2%.")) % t % idToString(this->m_id) << std::endl;

			wc3lib::write(ostream, this->value().toInteger(), size);

			break;
		}
	}

	return size;
}

CustomUnits::Set::Set(uint32 version) : m_version(version), m_flag(0)
{
}

CustomUnits::Set::Set(const CustomUnits::Set &other) : m_version(other.m_version), m_flag(other.m_flag)
{
	BOOST_FOREACH(Modifications::const_reference ref, other.m_modifications)
	{
		this->modifications().push_back(ref.clone()); // polymorph call to the virtual method clone which should return a value of the proper type
	}
}

CustomUnits::Set::~Set()
{
}

CustomUnits::Set* CustomUnits::Set::clone() const
{
	return new Set(*this);
}

std::streamsize CustomUnits::Set::read(InputStream &istream)
{
	std::streamsize size = 0;

	//std::cerr << "Reading set with version " << version() << std::endl;

	if (version() >= 3)
	{
		wc3lib::read(istream, m_flag, size);
	}

	int32 modifications;
	wc3lib::read(istream, modifications, size);

	//std::cerr << "Modifications " << modifications << std::endl;

	this->modifications().reserve(modifications);

	for (int32 i = 0; i < modifications; ++i)
	{
		std::auto_ptr<Modification> ptr(createModification());
		size += ptr->read(istream);
		this->modifications().push_back(ptr);
	}

	return size;
}

std::streamsize CustomUnits::Set::write(OutputStream &ostream) const
{
	std::streamsize size = 0;

	if (version() >= 3)
	{
		wc3lib::write(ostream, m_flag, size);
	}

	wc3lib::write<int32>(ostream, modifications().size(), size);

	BOOST_FOREACH(Modifications::const_reference modification, this->modifications())
	{
		size += modification.write(ostream);
	}

	return size;
}

CustomUnits::Modification* CustomUnits::Set::createModification() const
{
	return new Modification();
}

CustomUnits::Unit::Unit(uint32 version) : m_version(version), m_originalId(0), m_customId(0)
{
}

CustomUnits::Unit::Unit(const CustomUnits::Unit &other) : m_version(other.m_version), m_originalId(other.m_originalId), m_customId(other.m_customId)// TODO cloning crashes , m_sets(other.m_sets.clone())
{
	BOOST_FOREACH(Sets::const_reference ref, other.m_sets)
	{
		this->sets().push_back(ref.clone()); // polymorph call to the virtual method clone which should return a value of the proper type
	}
}

CustomUnits::Unit::~Unit()
{
}

CustomUnits::Unit* CustomUnits::Unit::clone() const
{
	return new Unit(*this);
}

std::streamsize CustomUnits::Unit::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_originalId, size);
	wc3lib::read(istream, this->m_customId, size);

	// https://github.com/stijnherfst/HiveWE/wiki/war3map(skin).w3*-Modifications
	int32 sets_count = 1;

	if (version() >= 3)
	{
		wc3lib::read(istream, sets_count, size);
	}

	//std::cerr << "Sets count " << sets_count << " and version " << version() << std::endl;

	this->sets().reserve(sets_count);

	for (int32 i = 0; i < sets_count; ++i)
	{
		std::auto_ptr<Set> ptr(createSet());
		size += ptr->read(istream);
		this->sets().push_back(ptr);

		//exit(1);
	}

	return size;
}

std::streamsize CustomUnits::Unit::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, originalId(), size);
	wc3lib::write(ostream, customId(), size);

	if (version() >= 3)
	{
		wc3lib::write<int32>(ostream, sets().size(), size);
	}

	BOOST_FOREACH(Sets::const_reference set, this->sets())
	{
		size += set.write(ostream);
	}

	return size;
}

CustomUnits::Set* CustomUnits::Unit::createSet() const
{
	//std::cerr << "Creating set with version " << this->version() << std::endl;

	return new Set(this->version());
}

CustomUnits::CustomUnits()
{
	// set latest file version by default
	this->m_version = latestFileVersion();
}

CustomUnits::CustomUnits(const CustomUnits &other)
{
	this->m_version = other.version(); // TODO cannot be initialized as field with the : operator?

	BOOST_FOREACH(Table::const_reference ref, other.originalTable())
	{
		// call polymorphic clone method which might also create a new custom object for custom objects
		this->m_originalTable.push_back(ref.clone());
	}

	BOOST_FOREACH(Table::const_reference ref, other.customTable())
	{
		// call polymorphic clone method which might also create a new custom object for custom objects
		this->m_customTable.push_back(ref.clone());
	}
}

CustomUnits::~CustomUnits()
{
}

std::streamsize CustomUnits::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_version, size);

	//std::cerr << "Reading custom units/objects version " << m_version << std::endl;

	int32 originalUnits = 0;
	wc3lib::read(istream, originalUnits, size);
	this->originalTable().reserve(originalUnits);

	for (int32 i = 0; i < originalUnits; ++i)
	{
		std::auto_ptr<Unit> ptr(createUnit());
		//std::cerr << "Version of created unit " << ptr->version() << std::endl;
		size += ptr->read(istream);
		originalTable().push_back(ptr);
	}

	int32 customUnits = 0;
	wc3lib::read(istream, customUnits, size);
	this->customTable().reserve(customUnits);

	for (int32 i = 0; i < customUnits; ++i)
	{
		std::auto_ptr<Unit> ptr(createUnit());
		size += ptr->read(istream);
		customTable().push_back(ptr);
	}

	return size;
}

std::streamsize CustomUnits::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, version(), size);
	wc3lib::write<int32>(ostream, originalTable().size(), size);

	BOOST_FOREACH(Table::const_reference unit, originalTable())
	{
		size += unit.write(ostream);
	}

	wc3lib::write<int32>(ostream, customTable().size(), size);

	BOOST_FOREACH(Table::const_reference unit, customTable())
	{
		size += unit.write(ostream);
	}

	return size;
}

void CustomUnits::clear()
{
	this->originalTable().clear();
	this->customTable().clear();
}

CustomUnits::Unit* CustomUnits::createUnit() const
{
	return new Unit(this->version());
}

}

}
