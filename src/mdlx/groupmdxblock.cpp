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

#include "groupmdxblock.hpp"
#include "groupmdxblockmember.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

GroupMdxBlock::GroupMdxBlock(byte blockName[4], const string &mdlKeyword, bool usesCounter, bool optional) : MdxBlock(blockName, mdlKeyword, optional), m_usesCounter(usesCounter)//, m_members()
{
}

GroupMdxBlock::~GroupMdxBlock()
{
	BOOST_FOREACH(class GroupMdxBlockMember *groupMdxBlockMember, this->m_members)
		delete groupMdxBlockMember;
}

std::streamsize GroupMdxBlock::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize GroupMdxBlock::writeMdl(ostream &ostream) const throw (class Exception)
{
	// if not empty write keyword with number of members (if counted, e. g. "Materials"), otherwise only write members (e. g. "Light")
	if (!mdlKeyword().empty())
		;
	
	return 0;
}

std::streamsize GroupMdxBlock::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = MdxBlock::readMdx(istream);

	if (size == 0)
		return 0;
	
	std::cout << "Test BLOCK member size: " << members().size() << std::endl;

	if (usesCounter())
	{
		long32 groupCount = 0;
		wc3lib::read(istream, groupCount, size);

		for ( ; groupCount > 0; --groupCount)
		{
			class GroupMdxBlockMember *member = this->createNewMember();
			size += member->readMdx(istream);
			this->m_members.push_back(member);
		}
	}
	else
	{
		long32 nbytes = 0;
		wc3lib::read(istream, nbytes, size);
		std::cout << "NBytes of group: " << nbytes << std::endl;
	
		while (nbytes > 0)
		{
			class GroupMdxBlockMember *member = this->createNewMember();
			const std::streamsize readSize = member->readMdx(istream);
			this->m_members.push_back(member);
			nbytes -= boost::numeric_cast<long32>(readSize);
			size += readSize;
			std::cout << "Read one member and remaining bytes are " << nbytes << std::endl;
		}
	}
	
	std::cout << "Group MDX block. Read " << m_members.size() << " members." << std::endl;
	std::cout << "TEST 2" << members().size() << std::endl;
	
	
	return size;
}

std::streamsize GroupMdxBlock::writeMdx(ostream &ostream) const throw (class Exception)
{
	if (!this->exists())
		return 0;

	std::streamsize size = MdxBlock::writeMdx(ostream);

	if (usesCounter())
	{
		long32 groupCount = this->m_members.size();
		wc3lib::write(ostream, groupCount, size);

		BOOST_FOREACH(const class GroupMdxBlockMember *groupMdxBlockMember, this->m_members)
			size += groupMdxBlockMember->writeMdx(ostream);
	}
	else
	{
		std::streampos position;
		skipByteCount<long32>(ostream, position);
		std::streamsize writtenSize = 0;
		
		BOOST_FOREACH(const class GroupMdxBlockMember *groupMdxBlockMember, this->m_members)
		{
			writtenSize += groupMdxBlockMember->writeMdx(ostream);
			size += writtenSize;
		}
			
		writeByteCount(ostream, static_cast<long32>(writtenSize), position, size);
	}
	
	return size;
}

}

}
