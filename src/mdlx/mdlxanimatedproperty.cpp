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

#ifndef WC3LIB_MDLX_MDLXANIMATEDPROPERTY_CPP
#define WC3LIB_MDLX_MDLXANIMATEDPROPERTY_CPP

#include "mdlxanimatedproperty.hpp"
#include "mdlxanimatedproperties.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

template<typename std::size_t N>
MdlxAnimatedProperty<N>::MdlxAnimatedProperty(MdlxAnimatedProperties<N> *properties) : m_properties(properties)
{
}

template<typename std::size_t N>
MdlxAnimatedProperty<N>::~MdlxAnimatedProperty()
{
}

template<typename std::size_t N>
std::streamsize MdlxAnimatedProperty<N>::readMdl(istream &istream) throw (class Exception)
{
	/// \todo FIXME
	return 0;
}

template<typename std::size_t N>
std::streamsize MdlxAnimatedProperty<N>::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeMdlVectorProperty(ostream, size, boost::str(boost::format("%1%:") % frame()), values());

	if (properties()->lineType() > LineType::Linear)
	{
		writeMdlVectorProperty(ostream, size, "InTan", inTan());
		writeMdlVectorProperty(ostream, size, "OutTan", outTan());
	}

	return size;
}

template<typename std::size_t N>
std::streamsize MdlxAnimatedProperty<N>::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_frame, size);
	size += values().read(istream);

	if (properties()->lineType() > LineType::Linear)
	{
		size += inTan().read(istream);
		size += outTan().read(istream);
	}

	return size;
}

template<typename std::size_t N>
std::streamsize MdlxAnimatedProperty<N>::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->frame(), size);
	size += values().write(ostream);

	if (properties()->lineType() > LineType::Linear)
	{
		size += inTan().write(ostream);
		size += outTan().write(ostream);
	}

	return size;
}

}

}

#endif
