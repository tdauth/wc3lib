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

#include "variable.hpp"

namespace wc3lib
{

namespace map
{

Variable::Variable(class Triggers *triggers) : m_name(), m_type(), m_number(0), m_isArray(false), m_isInitialized(false), m_initialValue()
{
}

std::streamsize Variable::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	readString(istream, this->m_name, size);
	readString(istream, this->m_type, size);
	wc3lib::read(istream, this->m_number, size);
	int32 isArray;
	wc3lib::read(istream, isArray, size);
	this->m_isArray = static_cast<bool>(isArray);
	int32 isInitialized;
	wc3lib::read(istream, isInitialized, size);
	this->m_isInitialized = static_cast<bool>(isInitialized);
	readString(istream, this->m_initialValue, size);

	return size;
}

std::streamsize Variable::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeString(ostream, this->m_name, size);
	writeString(ostream, this->m_type, size);
	wc3lib::write(ostream, this->m_number, size);
	wc3lib::write(ostream, *reinterpret_cast<const int32*>(&this->m_isArray), size);
	wc3lib::write(ostream, *reinterpret_cast<const int32*>(&this->m_isInitialized), size);
	writeString(ostream, this->m_initialValue, size);

	return size;
}

}

}
