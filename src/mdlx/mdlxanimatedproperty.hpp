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

namespace wc3lib
{

namespace mdlx
{

template<typename std::size_t N = 3>
class MdlxAnimatedProperty : public MdlxProperty
{
	public:
		static const std::size_t dimension = N;

		typedef BasicVertex<float32, N> Values;

		MdlxAnimatedProperty(MdlxAnimatedProperties<N> *properties);
		virtual ~MdlxAnimatedProperty();

		MdlxAnimatedProperties<N>* properties() const;
		float32 frame() const;
		Values& values();
		const Values& values() const;
		Values& inTan();
		const Values& inTan() const;
		Values& outTan();
		const Values& outTan() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		MdlxAnimatedProperties<N> *m_properties;
		long32 m_frame;

		Values m_values;
		//if (LineType > 1) {
		Values m_inTan;
		Values m_outTan;
};

template<typename std::size_t N>
inline MdlxAnimatedProperties<N>* MdlxAnimatedProperty<N>::properties() const
{
	return this->m_properties;
}

template<typename std::size_t N>
inline float32 MdlxAnimatedProperty<N>::frame() const
{
	return this->m_frame;
}

template<typename std::size_t N>
inline typename MdlxAnimatedProperty<N>::Values& MdlxAnimatedProperty<N>::values()
{
	return m_values;
}

template<typename std::size_t N>
inline const typename MdlxAnimatedProperty<N>::Values& MdlxAnimatedProperty<N>::values() const
{
	return m_values;
}

template<typename std::size_t N>
inline typename MdlxAnimatedProperty<N>::Values& MdlxAnimatedProperty<N>::inTan()
{
	return m_inTan;
}

template<typename std::size_t N>
inline const typename MdlxAnimatedProperty<N>::Values& MdlxAnimatedProperty<N>::inTan() const
{
	return m_inTan;
}

template<typename std::size_t N>
inline typename MdlxAnimatedProperty<N>::Values& MdlxAnimatedProperty<N>::outTan()
{
	return m_outTan;
}

template<typename std::size_t N>
inline const typename MdlxAnimatedProperty<N>::Values& MdlxAnimatedProperty<N>::outTan() const
{
	return m_outTan;
}

}

}

#include "mdlxanimatedproperty.cpp"

#endif
