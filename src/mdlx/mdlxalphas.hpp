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

#ifndef WC3LIB_MDLX_MDLXALPHAS_HPP
#define WC3LIB_MDLX_MDLXALPHAS_HPP

#include "mdlxanimatedproperties.hpp"
#include "mdlxanimatedproperty.hpp"
#include "mdlxalpha.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * \param _ValueType can be specified for chunks like "KMTF" which uses \ref long32 values for texture ids instead of \ref float32 values.
 */
template<typename _ValueType = float32>
class BasicMdlxAlphas : public MdlxAnimatedProperties<1, _ValueType>
{
	public:
		typedef MdlxAnimatedProperties<1, _ValueType> Base;
		typedef _ValueType ValueType;
		typedef BasicMdlxAlpha<_ValueType> Alpha;

		BasicMdlxAlphas(class Mdlx *mdlx);
		virtual ~BasicMdlxAlphas();

	protected:
		BasicMdlxAlphas(class Mdlx *mdlx, const byte mdxIdentifier[MdxBlock::mdxIdentifierSize], const string &mdlKeyword);

		/// \todo C++11 override
		virtual typename Base::Property* createAnimatedProperty();
};

// TODO C++11
//extern template class BasicMdlxAlphas<float32>;
typedef BasicMdlxAlphas<float32> MdlxAlphas;

template<typename _ValueType>
BasicMdlxAlphas<_ValueType>::BasicMdlxAlphas(class Mdlx *mdlx) : Base(mdlx, "KMTA", "Alpha")
{
}

template<typename _ValueType>
BasicMdlxAlphas<_ValueType>::~BasicMdlxAlphas()
{
}

template<typename _ValueType>
BasicMdlxAlphas<_ValueType>::BasicMdlxAlphas(class Mdlx *mdlx, const byte mdxIdentifier[MdxBlock::mdxIdentifierSize], const string &mdlKeyword) : Base(mdlx, mdxIdentifier, mdlKeyword)
{
}

template<typename _ValueType>
typename BasicMdlxAlphas<_ValueType>::Base::Property* BasicMdlxAlphas<_ValueType>::createAnimatedProperty()
{
	return new Alpha(this);
}

}

}

#endif
