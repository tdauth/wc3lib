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
#include <vector>

#include "platform.hpp"
#include "mdlxanimatedproperty.hpp"

namespace wc3lib
{

namespace mdlx
{

//template<typename std::size_t N, typename _ValueType>
//class MdlxAnimatedProperty;

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
class MdlxAnimatedProperties : public Format
{
	public:
		static const std::size_t dimension = N;
		typedef _ValueType ValueType;
		typedef MdlxAnimatedProperty<N, _ValueType> Property;
		typedef std::vector<Property> Properties;

		MdlxAnimatedProperties();

		void setLineType(LineType lineType);
		LineType lineType() const;
		void setGlobalSequenceId(long32 globalSequenceId);
		long32 globalSequenceId() const;
		bool hasGlobalSequence() const;
		void setProperties(const Properties &properties);
		const Properties& properties() const;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

	protected:
		LineType m_lineType; //(0:don't interp;1:linear;2:hermite;3:bezier)
		long32 m_globalSequenceId; // 0xFFFFFFFF if none
		Properties m_properties;
};

template<typename std::size_t N, typename _ValueType>
inline MdlxAnimatedProperties<N, _ValueType>::MdlxAnimatedProperties()
{
}

template<typename std::size_t N, typename _ValueType>
inline void MdlxAnimatedProperties<N, _ValueType>::setLineType(LineType lineType)
{
	this->m_lineType = lineType;
}

template<typename std::size_t N, typename _ValueType>
inline LineType MdlxAnimatedProperties<N, _ValueType>::lineType() const
{
	return this->m_lineType;
}

template<typename std::size_t N, typename _ValueType>
inline void MdlxAnimatedProperties<N, _ValueType>::setGlobalSequenceId(long32 globalSequenceId)
{
	this->m_globalSequenceId = globalSequenceId;
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
inline void MdlxAnimatedProperties<N, _ValueType>::setProperties(const typename MdlxAnimatedProperties<N, _ValueType>::Properties &properties)
{
	this->m_properties = properties;
}

template<typename std::size_t N, typename _ValueType>
inline const typename MdlxAnimatedProperties<N, _ValueType>::Properties& MdlxAnimatedProperties<N, _ValueType>::properties() const
{
	return this->m_properties;
}

template<typename std::size_t N, typename _ValueType>
std::streamsize MdlxAnimatedProperties<N, _ValueType>::read(InputStream &istream)
{
	std::streamsize size = 0;
	long32 count = 0;
	wc3lib::read(istream, count, size);
	std::cerr << "Count " << count << std::endl;
	wc3lib::read(istream, m_lineType, size);
	wc3lib::read(istream, m_globalSequenceId, size);
	m_properties.resize(count);
	std::cerr << "Reading N " << count << std::endl;

	for (long32 i = 0; i < count; i++)
	{
		size += m_properties[i].read(istream, m_lineType);
	}

	return size;
}

template<typename std::size_t N, typename _ValueType>
std::streamsize MdlxAnimatedProperties<N, _ValueType>::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	const long32 count = m_properties.size();
	wc3lib::write(ostream, count, size);
	wc3lib::write(ostream, m_lineType, size);
	wc3lib::write(ostream, m_globalSequenceId, size);

	for (std::size_t i = 0; i < m_properties.size(); i++)
	{
		size += m_properties[i].write(ostream, m_lineType);
	}

	return size;
}

typedef MdlxAnimatedProperties<1, float32> Alphas;
typedef Alphas::Property Alpha;
typedef MdlxAnimatedProperties<1, long32> TextureIds;
typedef TextureIds::Property TextureId;
typedef MdlxAnimatedProperties<3> Scalings;
typedef Scalings::Property Scaling;
typedef MdlxAnimatedProperties<3> Translations;
typedef Translations::Property Translation;
typedef MdlxAnimatedProperties<4> Rotations;
typedef Rotations::Property Rotation;

}

}

#endif

