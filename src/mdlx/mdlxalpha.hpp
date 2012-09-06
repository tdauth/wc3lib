/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MDLX_MDLXALPHA_HPP
#define WC3LIB_MDLX_MDLXALPHA_HPP

#include "mdlxanimatedproperty.hpp"
#include "mdlxalphas.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * \param _ValueType can be specified for chunks like "KMTF" which uses \ref long32 values for texture ids instead of \ref float32 values.
 */
template<typename _ValueType = float32>
class BasicMdlxAlpha : public MdlxAnimatedProperty<1>
{
	public:
		typedef _ValueType ValueType;

		BasicMdlxAlpha(class MdlxAlphas *alphas);
		virtual ~BasicMdlxAlpha();

		class MdlxAlphas* mdlxAlphas() const;
		_ValueType alpha() const;
		_ValueType inTanAlpha() const;
		_ValueType outTanAlpha() const;
};

template<typename _ValueType>
BasicMdlxAlpha<_ValueType>::BasicMdlxAlpha(class MdlxAlphas *alphas) : MdlxAnimatedProperty(alphas)
{
}

template<typename _ValueType>
BasicMdlxAlpha<_ValueType>::~BasicMdlxAlpha()
{
}

template<typename _ValueType>
inline class MdlxAlphas* BasicMdlxAlpha<_ValueType>::mdlxAlphas() const
{
	return boost::polymorphic_cast<class MdlxAlphas*>(properties());
}

template<typename _ValueType>
inline _ValueType BasicMdlxAlpha<_ValueType>::alpha() const
{
	return values()[0];
}

template<typename _ValueType>
inline _ValueType BasicMdlxAlpha<_ValueType>::inTanAlpha() const
{
	return inTan()[0];
}

template<typename _ValueType>
inline _ValueType BasicMdlxAlpha<_ValueType>::outTanAlpha() const
{
	return outTan()[0];
}

typedef BasicMdlxAlpha<float32> MdlxAlpha;

}

}

#endif
