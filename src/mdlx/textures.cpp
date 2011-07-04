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

#include "textures.hpp"
#include "texture.hpp"

namespace wc3lib
{

namespace mdlx
{

Textures::Textures(class Mdlx *mdlx) : GroupMdxBlock("TEXS", "Textures", false), m_mdlx(mdlx)
{
}

std::streamsize Textures::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Textures::writeMdl(ostream &ostream) const throw (class Exception)
{
	return 0;
}

std::streamsize Textures::readMdx(istream &istream) throw (class Exception)
{
	return GroupMdxBlock::readMdx(istream);
}

std::streamsize Textures::writeMdx(ostream &ostream) const throw (class Exception)
{
	return GroupMdxBlock::writeMdx(ostream);
}

class GroupMdxBlockMember* Textures::createNewMember()
{
	return new Texture(this);
}

}

}
