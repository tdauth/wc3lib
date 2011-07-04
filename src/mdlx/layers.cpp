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

#include "layers.hpp"
#include "layer.hpp"

namespace wc3lib
{

namespace mdlx
{

Layers::Layers(class Material *material) : GroupMdxBlock("LAYS", ""), m_material(material)
{
}

std::streamsize Layers::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Layers::writeMdl(ostream &ostream) const throw (class Exception)
{
	return 0;
}

std::streamsize Layers::readMdx(istream &istream) throw (class Exception)
{
	return GroupMdxBlock::readMdx(istream);
}

std::streamsize Layers::writeMdx(ostream &ostream) const throw (class Exception)
{
	return GroupMdxBlock::writeMdx(ostream);
}

class GroupMdxBlockMember* Layers::createNewMember()
{
	return new Layer(this);
}

}

}
