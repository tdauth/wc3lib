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

#include "collisionshapes.hpp"
#include "collisionshape.hpp"

namespace wc3lib
{

namespace mdlx
{

CollisionShapes::CollisionShapes(class Mdlx *mdlx) : GroupMdxBlock("CLID", "", false), m_mdlx(mdlx)
{
}

std::streamsize CollisionShapes::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize CollisionShapes::writeMdl(ostream &ostream) const throw (class Exception)
{
	return 0;
}

class GroupMdxBlockMember* CollisionShapes::createNewMember()
{
	return new CollisionShape(this);
}

}

}
