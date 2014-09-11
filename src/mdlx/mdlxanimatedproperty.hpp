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

#ifndef WC3LIB_MDLX_MDLXANIMATEDPROPERTY_HPP
#define WC3LIB_MDLX_MDLXANIMATEDPROPERTY_HPP

#include <boost/cast.hpp> // for polymorphic down casts of properties()
#include <boost/array.hpp>

#include "mdlxproperty.hpp"
#include "mdlxanimatedproperties.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * \brief One single tuple of values for a specific time frame which is used for animating a node.
 * 
 * The amount of values for one single object can be specified via \p N. The value is of type \ref BasicVertex using
 * \p N as its dimension. The value type of the vertex can be specified via \p ValueType.
 * 
 * The corresponding list of properties which contains the type of mathematical transformation and the corresponding global
 * sequence can be accessed using \ref properties().
 * 
 * The time frame of the transformation is returned by \ref frame().
 * 
 * \sa MdlxAnimatedProperties
 * \ingroup animations
 */
template<typename std::size_t N = 3, typename _ValueType = float32>
class MdlxAnimatedProperty : public MdlxProperty
{
	public:
		static const std::size_t dimension = N;

		typedef _ValueType ValueType;
		typedef BasicVertex<ValueType, N> Values;
		typedef MdlxAnimatedProperties<N, _ValueType> Properties;

		MdlxAnimatedProperty(Properties *properties);
		virtual ~MdlxAnimatedProperty();

		Properties* properties() const;
		long32 frame() const;
		Values& values();
		const Values& values() const;
		Values& inTan();
		const Values& inTan() const;
		Values& outTan();
		const Values& outTan() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		Properties *m_properties;
		long32 m_frame;

		Values m_values;
		//if (LineType > 1) {
		Values m_inTan;
		Values m_outTan;
};

template<typename std::size_t N, typename _ValueType>
MdlxAnimatedProperty<N, _ValueType>::MdlxAnimatedProperty(MdlxAnimatedProperty<N, _ValueType>::Properties *properties) : m_properties(properties)
{
}

template<typename std::size_t N, typename _ValueType>
MdlxAnimatedProperty<N, _ValueType>::~MdlxAnimatedProperty()
{
}

template<typename std::size_t N, typename _ValueType>
std::streamsize MdlxAnimatedProperty<N, _ValueType>::readMdl(istream &istream)
{
	/// \todo FIXME
	return 0;
}

template<typename std::size_t N, typename _ValueType>
std::streamsize MdlxAnimatedProperty<N, _ValueType>::writeMdl(ostream &ostream) const
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

template<typename std::size_t N, typename _ValueType>
std::streamsize MdlxAnimatedProperty<N, _ValueType>::readMdx(istream &istream)
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

template<typename std::size_t N, typename _ValueType>
std::streamsize MdlxAnimatedProperty<N, _ValueType>::writeMdx(ostream &ostream) const
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

template<typename std::size_t N, typename _ValueType>
inline typename MdlxAnimatedProperty<N, _ValueType>::Properties* MdlxAnimatedProperty<N, _ValueType>::properties() const
{
	return this->m_properties;
}

template<typename std::size_t N, typename _ValueType>
inline long32 MdlxAnimatedProperty<N, _ValueType>::frame() const
{
	return this->m_frame;
}

template<typename std::size_t N, typename _ValueType>
inline typename MdlxAnimatedProperty<N, _ValueType>::Values& MdlxAnimatedProperty<N, _ValueType>::values()
{
	return m_values;
}

template<typename std::size_t N, typename _ValueType>
inline const typename MdlxAnimatedProperty<N, _ValueType>::Values& MdlxAnimatedProperty<N, _ValueType>::values() const
{
	return m_values;
}

template<typename std::size_t N, typename _ValueType>
inline typename MdlxAnimatedProperty<N, _ValueType>::Values& MdlxAnimatedProperty<N, _ValueType>::inTan()
{
	return m_inTan;
}

template<typename std::size_t N, typename _ValueType>
inline const typename MdlxAnimatedProperty<N, _ValueType>::Values& MdlxAnimatedProperty<N, _ValueType>::inTan() const
{
	return m_inTan;
}

template<typename std::size_t N, typename _ValueType>
inline typename MdlxAnimatedProperty<N, _ValueType>::Values& MdlxAnimatedProperty<N, _ValueType>::outTan()
{
	return m_outTan;
}

template<typename std::size_t N, typename _ValueType>
inline const typename MdlxAnimatedProperty<N, _ValueType>::Values& MdlxAnimatedProperty<N, _ValueType>::outTan() const
{
	return m_outTan;
}

}

}

#endif
