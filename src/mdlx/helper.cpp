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

#include "helper.hpp"
#include "helpers.hpp"

namespace wc3lib
{

namespace mdlx
{

Helper::Helper(class Helpers *helpers) : Object(helpers->mdlx()), GroupMdxBlockMember(helpers, "Helper")
{
}

std::streamsize Helper::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Helper::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "Helper", this->name(), 0, true);

	size += Object::writeMdl(ostream);

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize Helper::readMdx(istream &istream) throw (class Exception)
{
	return Object::readMdx(istream);
}

std::streamsize Helper::writeMdx(ostream &ostream) const throw (class Exception)
{
	return Object::writeMdx(ostream);
}

}

}
