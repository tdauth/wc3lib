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

#include "triggerfunctionparameter.hpp"
#include "utilities.hpp"

namespace wc3lib
{

namespace map
{

TriggerFunctionParameter::TriggerFunctionParameter(class TriggerFunction *function) : m_function(function), m_type(TriggerFunctionParameter::Type::Preset), m_value(), m_unknown0(0), m_unknown1(0)
{
}


std::streamsize TriggerFunctionParameter::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	int32 type;
	wc3lib::read(istream, type, size);
	this->m_type = BOOST_SCOPED_ENUM(Type)(type);
	readString(istream, this->m_value, size);
	wc3lib::read(istream, this->m_unknown0, size);
	wc3lib::read(istream, this->m_unknown1, size);

	return size;
}


std::streamsize TriggerFunctionParameter::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	int32 type = (int32)(this->type());
	wc3lib::write(ostream, type, size);
	writeString(ostream, this->value(), size);
	wc3lib::write(ostream, this->unknown0(), size);
	wc3lib::write(ostream, this->unknown1(), size);

	return size;
}

}

}
