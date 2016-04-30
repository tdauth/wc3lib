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
#include "../i18n.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mpq
{

uint32 Block::fileKey(const string &fileName, Block::Flags flags, uint32 blockOffset, uint32 fileSize)
{
	if (fileName.empty())
	{
		throw std::logic_error(_("Never try to get file keys of empty filenames."));
	}

	// Hash the name to get the base key
	uint32 nFileKey = HashString(Archive::cryptTable(), fileName.c_str(), HashType::FileKey);

	// Offset-adjust the key if necessary
	if (flags & Flags::UsesEncryptionKey)
	{
		nFileKey = (nFileKey + blockOffset) ^ fileSize;
	}

	return nFileKey;
}

uint32 Block::fileKey(const string &fileName, const BlockTableEntry &blockTableEntry)
{
	return fileKey(fileName, static_cast<Flags>(blockTableEntry.flags), blockTableEntry.blockOffset, blockTableEntry.fileSize);
}

Block::Block(uint32 index)
: m_index(index)
, m_blockOffset(0)
, m_extendedBlockOffset(0)
, m_blockSize(0)
, m_fileSize(0)
, m_flags(Block::Flags::None)
{
}

Block::~Block()
{
}

std::streamsize Block::read(istream &istream)
{
	struct BlockTableEntry entry;
	std::streamsize size = 0;
	wc3lib::read(istream, entry, size);

	if (size != sizeof(entry))
	{
		throw Exception(_("Error while reading block table entry."));
	}

	this->m_blockOffset = entry.blockOffset;
	this->m_blockSize = entry.blockSize;
	this->m_fileSize = entry.fileSize;
	this->m_flags = static_cast<Block::Flags>(entry.flags);

	return size;
}

Block::Block(const Block& other): m_index(other.index())
, m_blockOffset(other.blockOffset())
, m_extendedBlockOffset(other.extendedBlockOffset())
, m_blockSize(other.blockSize())
, m_fileSize(other.fileSize())
, m_flags(other.flags())
{

}

std::streamsize Block::write(ostream &ostream) const
{
	const BlockTableEntry entry = toBlockTableEntry();
	std::streamsize size = 0;
	wc3lib::write(ostream, entry, size);

	return size;
}

void Block::remove()
{
	this->m_fileSize = 0;
	this->m_flags = Block::Flags::None;
}

uint32 Block::fileKey(const string &fileName) const
{
	BlockTableEntry entry = toBlockTableEntry();

	return Block::fileKey(fileName, entry);
}

}

}
