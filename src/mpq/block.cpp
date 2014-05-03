/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#include "algorithm.hpp"
#include "block.hpp"
#include "mpq.hpp"
#include "attributes.hpp"
#include "../i18n.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mpq
{

uint32 Block::fileKey(const string &fileName, const BlockTableEntry &blockTableEntry)
{
	if (fileName.empty())
		throw std::logic_error(_("Never try to get file keys of empty filenames."));

	// Hash the name to get the base key
	uint32 nFileKey = HashString(Mpq::cryptTable(), fileName.c_str(), HashType::FileKey);

	// Offset-adjust the key if necessary
	if (blockTableEntry.flags & Flags::UsesEncryptionKey)
		nFileKey = (nFileKey + blockTableEntry.blockOffset) ^ blockTableEntry.fileSize;

	return nFileKey;
}

Block::Block(class Mpq *mpq, uint32 index) : m_mpq(mpq), m_index(index), m_blockOffset(0), m_extendedBlockOffset(0), m_blockSize(0), m_fileSize(0), m_flags(Block::Flags::None), m_file(0)
{
}

Block::~Block()
{
}

std::streamsize Block::read(istream &istream) throw (class Exception)
{
	struct BlockTableEntry entry;
	std::streamsize size = 0;
	wc3lib::read(istream, entry, size);

	if (size != sizeof(entry))
		throw Exception(_("Error while reading block table entry."));

	this->m_blockOffset = entry.blockOffset;
	this->m_blockSize = entry.blockSize;
	this->m_fileSize = entry.fileSize;
	this->m_flags = (BOOST_SCOPED_ENUM(Block::Flags))(entry.flags);

	return size;
}

std::streamsize Block::write(ostream &ostream) const throw (class Exception)
{
	struct BlockTableEntry entry;
	entry.blockOffset = this->m_blockOffset;
	entry.blockSize = this->m_blockSize;
	entry.fileSize = this->m_fileSize;
	entry.flags = static_cast<int32>(this->m_flags);
	std::streamsize size = 0;
	wc3lib::write(ostream, entry, size);

	return size;
}

uint32 Block::fileKey(const string &fileName) const
{
	struct BlockTableEntry entry;
	entry.blockOffset = this->m_blockOffset;
	entry.blockSize = this->m_blockSize;
	entry.fileSize = this->m_fileSize;
	entry.flags = this->m_flags;

	return Block::fileKey(fileName, entry);
}

CRC32 Block::crc32() const
{
	return this->mpq()->attributesFile()->crc32(this);
}


const FILETIME& Block::fileTime() const
{
	return this->mpq()->attributesFile()->fileTime(this);
}

bool Block::fileTime(time_t& time)
{
	return this->mpq()->attributesFile()->fileTime(this).toTime(time);
}


MD5 Block::md5() const
{
	return this->mpq()->attributesFile()->md5(this);
}

}

}
