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

#include "value.hpp"

namespace wc3lib
{

namespace map
{

class ValueComparator : public boost::static_visitor<bool>
{
	public:
                template <typename T, typename U>
                bool operator()(const T&, const U &) const
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

Value::Value() : ValueBase(int32(0)), m_type(Type::Integer)
{
}

Value::Value(int32 value) : ValueBase(value), m_type(Type::Integer)
{
}

Value::Value(float32 value, BOOST_SCOPED_ENUM(Type) type) : ValueBase(value), m_type(type)
{
	if (!isReal())
		throw Exception(_(""));
}

Value::Value(const string &value, BOOST_SCOPED_ENUM(Type) type) : ValueBase(value), m_type(type)
{
	if (!isString())
		throw Exception(_(""));
}

Value::Value(bool value) : ValueBase(value), m_type(Type::Boolean)
{
}

Value::Value(byte value) : ValueBase(value), m_type(Type::Character)
{
}

Value::Value(List value, BOOST_SCOPED_ENUM(Type) type) : ValueBase(value), m_type(type)
{
	if (!isList())
		throw Exception(_(""));
}

BOOST_SCOPED_ENUM(Value::Type) Value::type() const
{
	return m_type;
}

int32& Value::toInteger()
{
	if (type() != Type::Integer)
	{
		throw boost::bad_get();
	}

	return boost::get<int32&>(*this);
}

const int32& Value::toInteger() const
{
	if (type() != Type::Integer)
	{
		throw boost::bad_get();
	}

	return boost::get<const int32&>(*this);
}

bool Value::isReal() const
{
	return (type() == Type::Real || type() == Type::Unreal);
}

float32& Value::toReal()
{
	if (!isReal())
	{
		throw boost::bad_get();
	}

	return boost::get<float32&>(*this);
}

const float32& Value::toReal() const
{
	if (!isReal())
	{
		throw boost::bad_get();
	}

	return boost::get<const float32&>(*this);
}

bool Value::isString() const
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

string& Value::toString()
{
	if (!isString())
	{
		throw boost::bad_get();
	}

	return boost::get<string&>(*this);
}

const string& Value::toString() const
{
	if (!isString())
	{
		throw boost::bad_get();
	}

	return boost::get<const string&>(*this);
}

bool& Value::toBoolean()
{
	if (type() != Type::Boolean)
	{
		throw boost::bad_get();
	}

	return boost::get<bool&>(*this);
}

const bool& Value::toBoolean() const
{
	if (type() != Type::Boolean)
	{
		throw boost::bad_get();
	}

	return boost::get<const bool&>(*this);
}

byte& Value::toCharacter()
{
	if (type() != Type::Character)
	{
		throw boost::bad_get();
	}

	return boost::get<byte&>(*this);
}

const byte& Value::toCharacter() const
{
	if (type() != Type::Character)
	{
		throw boost::bad_get();
	}

	return boost::get<const byte&>(*this);
}

bool Value::isList() const
{
	return (type() == Type::AbilityList
|| type() == Type::HeroAbilityList
|| type() == Type::ItemList
|| type() == Type::StringList
|| type() == Type::UnitList
|| type() == Type::UpgradeList);
}

List& Value::toList()
{
	if (!isList())
	{
		throw boost::bad_get();
	}

	return boost::get<List&>(*this);
}

const List& Value::toList() const
{
	if (!isList())
	{
		throw boost::bad_get();
	}

	return boost::get<const List&>(*this);
}

bool Value::operator<(const Value &value) const
{
	if (type() == value.type())
	{
		return boost::apply_visitor(ValueLessComparator(), *this, value);
	}

	return false;
}

bool Value::operator==(const Value &value) const
{
	if (type() == value.type())
	{
		return boost::apply_visitor(ValueComparator(), *this, value);
	}

	return false;
}

}

}

