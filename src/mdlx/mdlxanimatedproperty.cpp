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

#include "mdlxanimatedproperty.hpp"
#include "mdlxanimatedproperties.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

MdlxAnimatedProperty::MdlxAnimatedProperty(class MdlxAnimatedProperties *properties, std::size_t valuesCount) : m_properties(properties), m_valuesCount(valuesCount), m_values(valuesCount, 0), m_inTan(valuesCount, 0), m_outTan(valuesCount, 0)
{
}

MdlxAnimatedProperty::~MdlxAnimatedProperty()
{
}

std::streamsize MdlxAnimatedProperty::readMdl(istream &istream) throw (class Exception)
{
	/// \todo FIXME
	return 0;
}

std::streamsize MdlxAnimatedProperty::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeMdlVectorProperty(ostream, boost::str(boost::format("%1%:") % frame()), values(), size);
	
	if (properties()->lineType() > LineType::Linear)
	{
		writeMdlVectorProperty(ostream, "InTan", inTan(), size);
		writeMdlVectorProperty(ostream, "OutTan", outTan(), size);
	}
	
	return size;
}

std::streamsize MdlxAnimatedProperty::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_frame, size);
	
	for (std::size_t i = 0; i < valuesCount(); ++i)
		wc3lib::read(istream, m_values[i], size);

	std::cout << "Line type for property " << properties()->lineType() << std::endl;
	
	if (properties()->lineType() > LineType::Linear)
	{
		for (std::size_t i = 0; i < valuesCount(); ++i)
			wc3lib::read(istream, m_inTan[i], size);
		
		for (std::size_t i = 0; i < valuesCount(); ++i)
			wc3lib::read(istream, m_outTan[i], size);
	}

	return size;
}

std::streamsize MdlxAnimatedProperty::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->frame(), size);
	
	for (std::size_t i = 0; i < valuesCount(); ++i)
		wc3lib::write(ostream, values()[i], size);

	if (properties()->lineType() > LineType::Linear)
	{
		for (std::size_t i = 0; i < valuesCount(); ++i)
			wc3lib::write(ostream, inTan()[i], size);

		for (std::size_t i = 0; i < valuesCount(); ++i)
			wc3lib::write(ostream, outTan()[i], size);
	}
	
	return size;
}

}

}
