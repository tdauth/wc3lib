/***************************************************************************
 *   Copyright (C) 2012 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_VALUE_HPP
#define WC3LIB_MAP_VALUE_HPP

#include <boost/variant.hpp>
#include <boost/algorithm/string.hpp>

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/*
class ValueReader : public boost::static_visitor<>
{
	public:
		ValueReader(istream &istream, std::streamsize &size) : m_istream(istream), m_size(size)
		{
		}

		template <typename T>
		void operator()(T &operand) const
		{
			wc3lib::read(m_istream, operand, m_size);
		}

		void operator()(List &operand) const
		{
			string str;
			wc3lib::readString(m_istream, str, m_size);
			boost::algorithm::split(operand, str, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
		}

	private:
		std::istream &m_istream;
		std::streamsize &m_size;

};
*/

class ValueWriter : public boost::static_visitor<>
{
	public:
		ValueWriter(ostream &ostream, std::streamsize &size) : m_ostream(ostream), m_size(size)
		{
		}

		template <typename T>
		void operator()(const T &operand) const
		{
			wc3lib::write(m_ostream, operand, m_size);
		}

		void operator()(const string &operand) const
		{
			wc3lib::writeString(m_ostream, operand, m_size);
		}

		void operator()(const List &operand) const
		{
			const string str = boost::algorithm::join(operand, ":");
			wc3lib::writeString(m_ostream, str, m_size);
		}

	private:
		std::ostream &m_ostream;
		std::streamsize &m_size;

};

/**
 * This operator is required for displaying objects of type \ref Value on std::cout for instance.
 */
inline std::ostream& operator<<(std::ostream &ostream, const List &list)
{
	return (ostream << boost::algorithm::join<List, string>(list, ":"));
}

typedef boost::variant<
int32,
float32,
string,
bool,
byte,
List
> ValueBase;

/**
 * Template type for variant types for the same value.
 * Required for custom objects.
 * Each modification of a custom object requires a speific type which indicates value's size as well.
 * \sa ValueComparator
 * \sa ValueLessComparator
 * \sa CustomUnits::Modification
 * \sa CustomObjects::Modification
 * \todo type() is already used in class boost::variant.
 * \todo Implement custom type check using stored type for calls of boost::apply_visitor() and boost::get().
 *
 * \ingroup objectdata
 */
class Value : public ValueBase
{
	public:
		enum class Type : int32
		{
			Integer = 0,
			Real = 1,
			Unreal = 2,
			String = 3,
			Boolean = 4,
			Character = 5,
			UnitList = 6,
			ItemList = 7,
			RegenerationType = 8,
			AttackType = 9,
			WeaponType = 10,
			TargetType = 11,
			MoveType = 12,
			DefenseType = 13,
			PathingTexture = 14,
			UpgradeList = 15,
			StringList = 16,
			AbilityList = 17,
			HeroAbilityList = 18,
			MissileArt = 19,
			AttributeType = 20,
			AttackBits = 21
		};

		Value();
		Value(int32 value);
		Value(float32 value, Type type = Type::Real);
		Value(const string &value, Type type = Type::String);
		Value(bool value);
		Value(byte value);
		Value(List value, Type type = Type::StringList);
		Value(const Value &other);

		Type type() const;
		bool isInteger() const;
		int32& toInteger();
		const int32& toInteger() const;
		bool isReal() const;
		float32& toReal();
		const float32& toReal() const;
		bool isString() const;
		string& toString();
		const string& toString() const;
		bool isBoolean() const;
		bool& toBoolean();
		const bool& toBoolean() const;
		bool isCharacter() const;
		byte& toCharacter();
		const byte& toCharacter() const;
		bool isList() const;
		List& toList();
		const List& toList() const;
		bool operator<(const Value &value) const;
		bool operator==(const Value &value) const;

	protected:
		Type m_type;
};

/**
 * This operator is required for displaying objects of type \ref wc3lib::map::Value::Type on std::cout for instance.
 */
inline std::ostream& operator<<(std::ostream &ostream, const wc3lib::map::Value::Type &t)
{
	switch (t) {
		case wc3lib::map::Value::Type::Integer: {
			return (ostream << "AttributeType");
		}
		case wc3lib::map::Value::Type::Real: {
			return (ostream << "Real");
		}
		case wc3lib::map::Value::Type::Unreal: {
			return (ostream << "Unreal");
		}
		case wc3lib::map::Value::Type::String: {
			return (ostream << "String");
		}
		case wc3lib::map::Value::Type::Boolean: {
			return (ostream << "Boolean");
		}
		case wc3lib::map::Value::Type::Character: {
			return (ostream << "Character");
		}
		case wc3lib::map::Value::Type::UnitList: {
			return (ostream << "UnitList");
		}
		case wc3lib::map::Value::Type::ItemList: {
			return (ostream << "ItemList");
		}
		case wc3lib::map::Value::Type::RegenerationType: {
			return (ostream << "RegenerationType");
		}
		case wc3lib::map::Value::Type::AttackType: {
			return (ostream << "AttackType");
		}
		case wc3lib::map::Value::Type::WeaponType: {
			return (ostream << "WeaponType");
		}
		case wc3lib::map::Value::Type::TargetType: {
			return (ostream << "TargetType");
		}
		case wc3lib::map::Value::Type::MoveType: {
			return (ostream << "MoveType");
		}
		case wc3lib::map::Value::Type::DefenseType: {
			return (ostream << "DefenseType");
		}
		case wc3lib::map::Value::Type::PathingTexture: {
			return (ostream << "PathingTexture");
		}
		case wc3lib::map::Value::Type::UpgradeList: {
			return (ostream << "UpgradeList");
		}
		case wc3lib::map::Value::Type::StringList: {
			return (ostream << "StringList");
		}
		case wc3lib::map::Value::Type::AbilityList: {
			return (ostream << "AbilityList");
		}
		case wc3lib::map::Value::Type::HeroAbilityList: {
			return (ostream << "HeroAbilityList");
		}
		case wc3lib::map::Value::Type::MissileArt: {
			return (ostream << "MissileArt");
		}
		case wc3lib::map::Value::Type::AttributeType: {
			return (ostream << "AttributeType");
		}
		case wc3lib::map::Value::Type::AttackBits: {
			return (ostream << "unknown");
		}
	}
	
	return (ostream << "unknown");
}

}

}

#endif // VALUE_HPP
