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

#include "groupmdxblockmember.hpp"
#include "groupmdxblock.hpp"
#include "../exception.hpp"

namespace wc3lib
{

namespace mdlx
{

GroupMdxBlockMember::GroupMdxBlockMember(GroupMdxBlock *parent, const string &mdlKeyword) : m_parent(parent), m_mdlKeyword(mdlKeyword)
{
	//std::cout << "PARENT address " << parent << std::endl;
}

GroupMdxBlockMember::~GroupMdxBlockMember()
{
}

void GroupMdxBlockMember::checkBytesIncluding(std::streamsize size, long32 nbytesi)
{
	if (size != nbytesi)
		throw Exception(boost::format(_("%1%: File byte count is not equal to real byte count.\nFile byte count: %2%.\nReal byte count: %3%.\n")) % this->parent()->mdxIdentifier() % nbytesi % size);
}

void GroupMdxBlockMember::checkBytes(std::streamsize size, long32 nbytes)
{
	if (size != nbytes - GroupMdxBlock::mdxIdentifierSize)
		throw Exception(boost::format(_("%1%: File byte count is not equal to real byte count.\nFile byte count: %2%.\nReal byte count: %3%.\n")) % this->parent()->mdxIdentifier() % nbytes % size);
}

}

}
