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

#include <boost/array.hpp>

#include "platform.hpp"

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
 * The time frame of the transformation is returned by \ref frame().
 *
 * \sa MdlxAnimatedProperties
 * \ingroup animations
 */
template<typename std::size_t N = 3, typename _ValueType = float32>
class MdlxAnimatedProperty
{
	public:
		static const std::size_t dimension = N;

		typedef _ValueType ValueType;
		typedef BasicVertex<ValueType, N> Values;

		MdlxAnimatedProperty();
		virtual ~MdlxAnimatedProperty();

		void setFrame(long32 frame);
		long32 frame() const;
		void setValues(const Values &values);
		const Values& values() const;
		void setInTan(const Values &inTan);
		const Values& inTan() const;
		void setOutTan(const Values &outTan);
		const Values& outTan() const;

	protected:
		long32 m_frame;

		Values m_values;
		//if (LineType > 1) {
		Values m_inTan;
		Values m_outTan;
};

template<typename std::size_t N, typename _ValueType>
MdlxAnimatedProperty<N, _ValueType>::MdlxAnimatedProperty()
{
}

template<typename std::size_t N, typename _ValueType>
MdlxAnimatedProperty<N, _ValueType>::~MdlxAnimatedProperty()
{
}

template<typename std::size_t N, typename _ValueType>
inline void MdlxAnimatedProperty<N, _ValueType>::setFrame(long32 frame)
{
	this->m_frame = frame;
}

template<typename std::size_t N, typename _ValueType>
inline long32 MdlxAnimatedProperty<N, _ValueType>::frame() const
{
	return this->m_frame;
}

template<typename std::size_t N, typename _ValueType>
inline void MdlxAnimatedProperty<N, _ValueType>::setValues(const typename MdlxAnimatedProperty<N, _ValueType>::Values &values)
{
	this->m_values = values;
}

template<typename std::size_t N, typename _ValueType>
inline const typename MdlxAnimatedProperty<N, _ValueType>::Values& MdlxAnimatedProperty<N, _ValueType>::values() const
{
	return m_values;
}

template<typename std::size_t N, typename _ValueType>
inline void MdlxAnimatedProperty<N, _ValueType>::setInTan(const typename MdlxAnimatedProperty<N, _ValueType>::Values &inTan)
{
	this->m_inTan = inTan;
}

template<typename std::size_t N, typename _ValueType>
inline const typename MdlxAnimatedProperty<N, _ValueType>::Values& MdlxAnimatedProperty<N, _ValueType>::inTan() const
{
	return m_inTan;
}

template<typename std::size_t N, typename _ValueType>
inline void MdlxAnimatedProperty<N, _ValueType>::setOutTan(const typename MdlxAnimatedProperty<N, _ValueType>::Values &outTan)
{
	this->m_outTan = outTan;
}

template<typename std::size_t N, typename _ValueType>
inline const typename MdlxAnimatedProperty<N, _ValueType>::Values& MdlxAnimatedProperty<N, _ValueType>::outTan() const
{
	return m_outTan;
}

}

}

#endif
