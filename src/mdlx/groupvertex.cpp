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

#include "groupvertex.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

GroupVertex::GroupVertex(class GroupVertices *groupVertices) : GroupMdxBlockMember(groupVertices, ""), m_data(0)
{
}

GroupVertex::~GroupVertex()
{
}

std::streamsize GroupVertex::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize GroupVertex::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;

	writeMdlValueProperty(ostream, size, "", boost::numeric_cast<long32>(this->data())); // TODO without casting to long32 it's written as char?

	return size;
}

std::streamsize GroupVertex::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_data, size);

	return size;
}

std::streamsize GroupVertex::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->data(), size);

	return size;
}

}

}