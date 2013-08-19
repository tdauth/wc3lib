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
#include "triggers.hpp"

namespace wc3lib
{

namespace map
{

Variable::Variable() : m_number(0), m_isArray(false), m_isInitialized(false)
{
}

std::streamsize Variable::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	readString(istream, this->m_name, size);
	readString(istream, this->m_type, size);
	wc3lib::read(istream, this->m_number, size);
	wc3lib::read<int32>(istream, (int32&)this->m_isArray, size);
	wc3lib::read<int32>(istream, (int32&)this->m_isInitialized, size);
	readString(istream, this->m_initialValue, size);

	return size;
}

std::streamsize Variable::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeString(ostream, name(), size);
	writeString(ostream, type(), size);
	wc3lib::write(ostream, number(), size);
	wc3lib::write<int32>(ostream, (const int32&)isArray(), size);
	wc3lib::write<int32>(ostream, (const int32&)isInitialized(), size);
	writeString(ostream, initialValue(), size);

	return size;
}

}

}
