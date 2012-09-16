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

#ifndef WC3LIB_MDLX_MDLXALPHAS_CPP
#define WC3LIB_MDLX_MDLXALPHAS_CPP

#include "mdlxalpha.hpp"

namespace wc3lib
{

namespace mdlx
{

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
