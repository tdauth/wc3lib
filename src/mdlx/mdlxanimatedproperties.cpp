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

#ifndef WC3LIB_MDLX_MDLXANIMATEDPROPERTIES_CPP
#define WC3LIB_MDLX_MDLXANIMATEDPROPERTIES_CPP

#include "mdlxanimatedproperties.hpp"

namespace wc3lib
{

namespace mdlx
{

template<typename std::size_t N>
MdlxAnimatedProperties<N>::MdlxAnimatedProperties(class Mdlx *mdlx, const byte mdxIdentifier[MdxBlock::mdxIdentifierSize], const string &mdlIdentifier, bool optional) : MdxBlock(mdxIdentifier, mdlIdentifier, optional), m_mdlx(mdlx)
{
}

template<typename std::size_t N>
MdlxAnimatedProperties<N>::~MdlxAnimatedProperties()
{
}

template<typename std::size_t N>
std::streamsize MdlxAnimatedProperties<N>::readMdl(istream &istream) throw (class Exception)
{
	/// \todo FIXME
	return 0;
}

template<typename std::size_t N>
std::streamsize MdlxAnimatedProperties<N>::writeMdl(ostream &ostream) const throw (class Exception)
{
	// Tag <long_count> {
	std::streamsize size = 0;
	writeMdlCountedBlock(ostream, size, mdlKeyword(), this->m_properties.size());//MdlValueBlock<ValueType>::writeMdl(ostream);

	switch (lineType())
	{
		case LineType::DontInterpolate:
			writeMdlProperty(ostream, size, "DontInterp");

			break;

		case LineType::Linear:
			writeMdlProperty(ostream, size, "Linear");

			break;

		case LineType::Hermite:
			writeMdlProperty(ostream, size, "Hermite");

			break;

		case LineType::Bezier:
			writeMdlProperty(ostream, size, "Bezier");

			break;
	}

	if (hasGlobalSequence())
		writeMdlValueProperty(ostream, size, "GlobalSeqId", globalSequenceId());

	BOOST_FOREACH(typename Properties::const_reference property, properties())
		size += property.writeMdl(ostream);

	writeMdlBlockConclusion(ostream, size);

	return size;
}

template<typename std::size_t N>
std::streamsize MdlxAnimatedProperties<N>::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = MdxBlock::readMdx(istream);

	// is optional!
	if (size == 0)
		return 0;

	long32 number;
	wc3lib::read(istream, number, size);
	this->m_properties.reserve(number);
	wc3lib::read(istream, *reinterpret_cast<long32*>(&this->m_lineType), size);
	wc3lib::read(istream, this->m_globalSequenceId, size);

	for (long32 i = 0; i < number; ++i)
	{
		MdlxAnimatedProperty<N> *property = createAnimatedProperty();
		size += property->readMdx(istream);
		this->m_properties.push_back(property);
	}

	return size;
}

template<typename std::size_t N>
std::streamsize MdlxAnimatedProperties<N>::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = MdxBlock::writeMdx(ostream);

	// is optional!
	if (size == 0)
		return 0;

	wc3lib::write<long32>(ostream, properties().size(), size);
	wc3lib::write<long32>(ostream, lineType(), size);
	wc3lib::write(ostream, globalSequenceId(), size);

	BOOST_FOREACH(typename Properties::const_reference property, this->properties())
		size += property.writeMdx(ostream);

	return size;
}

}

}

#endif
