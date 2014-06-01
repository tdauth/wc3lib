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

#ifndef WC3LIB_MDLX_MDLXANIMATEDPROPERTIES_HPP
#define WC3LIB_MDLX_MDLXANIMATEDPROPERTIES_HPP

#include <cstddef>

#include <boost/ptr_container/ptr_vector.hpp>

#include "mdlxproperty.hpp"
#include "mdxblock.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace mdlx
{

template<typename std::size_t N, typename _ValueType>
class MdlxAnimatedProperty;

/**
 * \brief Abstract template class for all kinds of animated properties.
 *
 * Animated properties may belong to a specific global sequence.
 * There exist different types of transformations. Usually:
 * <ul>
 * <li> translations </li>
 * <li> scalings </li>
 * <li> rotations </li>
 * <li> transparency adjustments </li>
 * </ul>
 *
 * Each transformation type needs specific values for applying the transformation. For example a translation
 * needs three coordinates where the corresponding node should be moved to (X, Y, Z).
 *
 * Rotations usually on the other hand usually use quaternions and therefore need four values.
 *
 * Therefore this class is defined as template for which the count of values can be specified with \p N
 * and the data type with \p _ValueType.
 *
 * All animated properties can be accessed using \ref properties(). They share the global sequence (\ref globalSequenceId() ) and the mathemetical method of transformation which
 * can be accessed via \ref lineType(). See \ref LineType for all supported mathematical methods.
 *
 * Check if the properties belong to a global sequence using \ref hasGlobalSequence().
 *
 * \sa MdlxAnimatedProperty
 * \ingroup animations
 */
template<typename std::size_t N = 3, typename _ValueType = float32>
class MdlxAnimatedProperties : public MdxBlock
{
	public:
		static const std::size_t dimension = N;
		typedef _ValueType ValueType;
		typedef MdlxAnimatedProperty<N, _ValueType> Property;
		typedef boost::ptr_vector<Property> Properties;
		MdlxAnimatedProperties(class Mdlx *mdlx, const byte mdxIdentifier[MdxBlock::mdxIdentifierSize], const string &mdlIdentifier, bool optional = true);
		virtual ~MdlxAnimatedProperties();

		class Mdlx* mdlx() const;
		LineType lineType() const;
		long32 globalSequenceId() const;
		bool hasGlobalSequence() const;
		const Properties& properties() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		virtual MdlxAnimatedProperty<N, _ValueType>* createAnimatedProperty() = 0;

		class Mdlx *m_mdlx;
		LineType m_lineType; //(0:don't interp;1:linear;2:hermite;3:bezier)
		long32 m_globalSequenceId; // 0xFFFFFFFF if none
		Properties m_properties;
};

template<typename std::size_t N, typename _ValueType>
MdlxAnimatedProperties<N, _ValueType>::MdlxAnimatedProperties(class Mdlx *mdlx, const byte mdxIdentifier[MdxBlock::mdxIdentifierSize], const string &mdlIdentifier, bool optional) : MdxBlock(mdxIdentifier, mdlIdentifier, optional), m_mdlx(mdlx)
{
}

template<typename std::size_t N, typename _ValueType>
MdlxAnimatedProperties<N, _ValueType>::~MdlxAnimatedProperties()
{
}

template<typename std::size_t N, typename _ValueType>
std::streamsize MdlxAnimatedProperties<N, _ValueType>::readMdl(istream &istream) throw (class Exception)
{
	/// \todo FIXME
	return 0;
}

template<typename std::size_t N, typename _ValueType>
std::streamsize MdlxAnimatedProperties<N, _ValueType>::writeMdl(ostream &ostream) const throw (class Exception)
{
	if (this->properties().empty())
		return 0;

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

template<typename std::size_t N, typename _ValueType>
std::streamsize MdlxAnimatedProperties<N, _ValueType>::readMdx(istream &istream) throw (class Exception)
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
		MdlxAnimatedProperty<N, _ValueType> *property = createAnimatedProperty();
		size += property->readMdx(istream);
		this->m_properties.push_back(property);
	}

	return size;
}

template<typename std::size_t N, typename _ValueType>
std::streamsize MdlxAnimatedProperties<N, _ValueType>::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = MdxBlock::writeMdx(ostream);

	// is optional!
	if (size == 0)
		return 0;

	wc3lib::write<long32>(ostream, boost::numeric_cast<long32>(properties().size()), size);
	wc3lib::write<long32>(ostream, static_cast<long32>(lineType()), size);
	wc3lib::write(ostream, globalSequenceId(), size);

	BOOST_FOREACH(typename Properties::const_reference property, this->properties())
		size += property.writeMdx(ostream);

	return size;
}

template<typename std::size_t N, typename _ValueType>
inline class Mdlx* MdlxAnimatedProperties<N, _ValueType>::mdlx() const
{
	return this->m_mdlx;
}

template<typename std::size_t N, typename _ValueType>
inline LineType MdlxAnimatedProperties<N, _ValueType>::lineType() const
{
	return this->m_lineType;
}

template<typename std::size_t N, typename _ValueType>
inline long32 MdlxAnimatedProperties<N, _ValueType>::globalSequenceId() const
{
	return this->m_globalSequenceId;
}

template<typename std::size_t N, typename _ValueType>
inline bool MdlxAnimatedProperties<N, _ValueType>::hasGlobalSequence() const
{
	return this->m_globalSequenceId != mdlx::noneId;
}

template<typename std::size_t N, typename _ValueType>
inline const typename MdlxAnimatedProperties<N, _ValueType>::Properties& MdlxAnimatedProperties<N, _ValueType>::properties() const
{
	return this->m_properties;
}

}

}

#endif

