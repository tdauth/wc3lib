/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_VARIABLE_HPP
#define WC3LIB_MAP_VARIABLE_HPP

#include "platform.hpp"
#include "triggerdata.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \todo Add derived class VariableX.
 * \sa VariableX
 */
class Variable : public Format
{
	public:
		Variable();
		virtual ~Variable();

		std::streamsize read(InputStream &istream);
		std::streamsize write(OutputStream &ostream) const;

		void setName(const string &name);
		const string& name() const;
		void setType(const string &type);
		const string& type() const;
		void setNumber(int32 number);
		int32 number() const;
		void setArray(bool isArray);
		bool isArray() const;
		void setInitialized(bool isInitialized);
		bool isInitialized() const;
		void setInitialValue(const string &initialValue);
		const string& initialValue() const;

	protected:
		string m_name;
		string m_type;
		int32 m_number;
		bool m_isArray;
		bool m_isInitialized;
		string m_initialValue;
};

inline void Variable::setName(const string& name)
{
	this->m_name = name;
}

inline const string& Variable::name() const
{
	return m_name;
}

inline void Variable::setType(const string& type)
{
	this->m_type = type;
}

inline const string& Variable::type() const
{
	return m_type;
}

inline void Variable::setNumber(int32 number)
{
	this->m_number = number;
}

inline int32 Variable::number() const
{
	return m_number;
}

inline void Variable::setArray(bool isArray)
{
	this->m_isArray = isArray;
}

inline bool Variable::isArray() const
{
	return m_isArray;
}

inline void Variable::setInitialized(bool isInitialized)
{
	this->m_isInitialized = isInitialized;
}

inline bool Variable::isInitialized() const
{
	return m_isInitialized;
}

inline void Variable::setInitialValue(const string &initialValue)
{
	this->m_initialValue = initialValue;
}

inline const string& Variable::initialValue() const
{
	return m_initialValue;
}

}

}

#endif
