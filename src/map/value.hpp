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

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

class ValueComparator : public boost::static_visitor<bool>
{
	public:
                template <typename T, typename U>
                bool operator()( const T &, const U & ) const
                {
                        return false; // cannot compare different types
                }

		template <typename T>
		bool operator()(T operand1, T operand2) const
		{
			return operand1 == operand2;
		}

		bool operator()(const string &operand1, const string &operand2) const
		{
			return operand1 == operand2;
		}

		bool operator()(const List &operand1, const List &operand2) const
		{
			if (operand1.size() != operand2.size())
				return false;

			List::const_iterator it1 = operand1.begin();
			List::const_iterator it2 = operand2.begin();

			while (it1 != operand1.end() && it2 != operand2.end())
			{
				if (*it1 != *it2)
					return false;

				++it1;
				++it2;
			}

			return true;
		}

};

class ValueLessComparator : public boost::static_visitor<bool>
{
	public:
                template <typename T, typename U>
                bool operator()( const T &, const U & ) const
                {
                        return false; // cannot compare different types
                }

		template <typename T>
		bool operator()(T operand1, T operand2) const
		{
			return operand1 < operand2;
		}

		bool operator()(bool operand1, bool operand2) const
		{
			if (operand1 != operand2 && !operand1) // false = 0 means smaller than 1
				return true;

			return false;
		}

		bool operator()(const string &operand1, const string &operand2) const
		{
			return (operand1 < operand2);
		}

		bool operator()(const List &operand1, const List &operand2) const
		{
			return operand1.size() < operand2.size();
		}
};

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

class ValueWriter : public boost::static_visitor<>
{
	public:
		template <typename T>
		void operator()(const T &operand, ostream &ostream, std::streamsize &size) const
		{
			wc3lib::write(ostream, operand, size);
		}

		void operator()(const List &operand, ostream &ostream, std::streamsize &size) const
		{
			const string str = boost::algorithm::join(operand, ":");
			wc3lib::writeString(ostream, str, size);
		}

};
*/

typedef boost::variant<
int32,
float32,
string,
bool,
char8,
List
> ValueBase;

/**
 * Template type for variant types for the same value.
 * Required for custom objects.
 * Each modification of a custom object requires a speific type which indicates value's size as well.
 * \sa CustomUnits::Modification, CustomObjects::Modification
 */
class Value : public ValueBase
{
	public:
		BOOST_SCOPED_ENUM_START(Type) /// \todo C++11 : int32
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
		BOOST_SCOPED_ENUM_END

		Value();
		Value(int32 value);
		Value(float32 value, BOOST_SCOPED_ENUM(Type) type = Type::Real);
		Value(const string &value, BOOST_SCOPED_ENUM(Type) type = Type::String);
		Value(bool value);
		Value(char8 value);
		Value(List value, BOOST_SCOPED_ENUM(Type) type = Type::StringList) ;

		BOOST_SCOPED_ENUM(Type) type() const
		{
			return m_type;
		}

		int32& toInteger()
		{
			if (type() != Type::Integer)
				throw boost::bad_get();

			return boost::get<int32>(*this);
		}

		const int32& toInteger() const
		{
			return const_cast<const Value*>(this)->toInteger();
		}

		bool isReal() const
		{
			return (type() == Type::Real || type() == Type::Unreal);
		}

		float32& toReal()
		{
			if (!isReal())
				throw boost::bad_get();

			return boost::get<float32>(*this);
		}

		const float32& toReal() const
		{
			return const_cast<const Value*>(this)->toReal();
		}

		bool isString() const
		{
			return (type() == Type::String
		|| type() == Type::AttackBits
		|| type() == Type::AttackType
		|| type() == Type::AttributeType
		|| type() == Type::DefenseType
		|| type() == Type::MissileArt
		|| type() == Type::MoveType
		|| type() == Type::PathingTexture
		|| type() == Type::RegenerationType
		|| type() == Type::TargetType
		|| type() == Type::WeaponType
			);
		}

		string& toString()
		{
			if (!isString())
				throw boost::bad_get();

			return boost::get<string>(*this);
		}

		const string& toString() const
		{
			return const_cast<const Value*>(this)->toString();
		}

		bool& toBoolean()
		{
			if (type() != Type::Boolean)
				throw boost::bad_get();

			return boost::get<bool>(*this);
		}

		const bool& toBoolean() const
		{
			return const_cast<const Value*>(this)->toBoolean();
		}

		char8& toCharacter()
		{
			if (type() != Type::Character)
				throw boost::bad_get();

			return boost::get<char8>(*this);
		}

		const char8& toCharacter() const
		{
			return const_cast<const Value*>(this)->toCharacter();
		}

		bool isList() const
		{
			return (type() == Type::AbilityList
		|| type() == Type::HeroAbilityList
		|| type() == Type::ItemList
		|| type() == Type::StringList
		|| type() == Type::UnitList
		|| type() == Type::UpgradeList);
		}

		List& toList()
		{
			if (!isList())
				throw boost::bad_get();

			return boost::get<List>(*this);
		}

		const List& toList() const
		{
			return const_cast<const Value*>(this)->toList();
		}

		bool operator<(const Value &value) const
		{
			if (type() == value.type())
				return boost::apply_visitor(ValueLessComparator(), *this, value);

			return false;
		}

		bool operator==(const Value &value) const
		{
			if (type() == value.type())
				return boost::apply_visitor(ValueComparator(), *this, value);

			return false;
		}

	protected:
		BOOST_SCOPED_ENUM(Type) m_type;
};

}

}

#endif // VALUE_HPP
