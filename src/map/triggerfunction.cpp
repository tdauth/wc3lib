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

#include "triggerfunction.hpp"

namespace wc3lib
{

namespace map
{

TriggerFunction::TriggerFunction(class Trigger *trigger) : m_trigger(trigger), m_type(TriggerFunction::Type::Event), m_name(), m_isEnabled(false), m_parameters()
{
}

std::streamsize TriggerFunction::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	int32 type;
	wc3lib::read(istream, type, size);
	this->m_type = BOOST_SCOPED_ENUM(Type)(type);
	readString(istream, this->m_name, size);
	int32 isEnabled;
	wc3lib::read(istream, isEnabled, size);
	this->m_isEnabled = bool(isEnabled);

	/// @todo get trigger parameters, hardcoded or from TriggerData.txt?

	return size;
}

std::streamsize TriggerFunction::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, this->m_type, size);
	writeString(ostream, this->m_name, size);
	wc3lib::write<int32>(ostream, this->m_isEnabled, size);

	BOOST_FOREACH(const ParameterPtr parameter, this->m_parameters)
		size += parameter->write(ostream);

	return size;
}

}

}
