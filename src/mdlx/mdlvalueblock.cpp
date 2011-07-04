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

/*
#ifndef WC3LIB_MDLX_MDLVALUEBLOCK_CPP
#define WC3LIB_MDLX_MDLVALUEBLOCK_CPP

#include <boost/lexical_cast.hpp>

#include "mdlvalueblock.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

template<typename T>
MdlValueBlock<T>::MdlValueBlock(const string &mdlIdentifier, T value, bool optional) : MdlBlock(mdlIdentifier, optional), m_value(value)
{
}

template<typename T>
MdlValueBlock<T>::~MdlValueBlock()
{
}

template<typename T>
std::streamsize MdlValueBlock<T>::readMdl(istream &istream) throw (class Exception)
{
	std::streamsize size = MdlBlock::readMdl(istream);
	
	if (size == 0)
		return 0;
	
	string identifier;
	parse(istream, identifier, size);
	
	this->m_value = boost::lexical_cast<ValueType>(identifier);

	return size;
}

template<typename T>
std::streamsize MdlValueBlock<T>::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = MdlBlock::writeMdl(ostream);
	
	if (size == 0)
		return 0;
	
	string identifier(boost::str(boost::format(" %1% ") % value()));
	wc3lib::write(ostream, identifier.c_str()[0], size, identifier.length());
	
	return size;
}

}

}

#endif
*/
