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
#include "hash.hpp"
#include "mpq.hpp"
#include "block.hpp"

namespace wc3lib
{

namespace mpq
{

HashData::HashData(int32 filePathHashA, int32 filePathHashB, uint16 locale, uint16 platform) : m_filePathHashA(filePathHashA), m_filePathHashB(filePathHashB), m_locale(locale), m_platform(platform)
{
}

HashData::HashData(const boost::filesystem::path& path, MpqFile::Locale locale, MpqFile::Platform platform) : m_filePathHashA(HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameA)), m_filePathHashB(HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameB)), m_locale(MpqFile::localeToInt(locale)), m_platform(MpqFile::platformToInt(platform))
{
}

HashData::HashData(const HashData &other) : m_filePathHashA(other.m_filePathHashA), m_filePathHashB(other.m_filePathHashB), m_locale(other.m_locale), m_platform(other.m_platform)
{
}

HashData& HashData::operator=(const HashData &other)
{
	m_filePathHashA = other.m_filePathHashA;
	m_filePathHashB = other.m_filePathHashB;
	m_locale = other.m_locale;
	m_platform = other.m_platform;

	return *this;
}

bool HashData::isHash(int32 nameHashA, int32 nameHashB, uint16 locale, uint16 platform) const
{
	return this->m_filePathHashA == nameHashA && this->m_filePathHashB == nameHashB && this->m_locale == locale && this->m_platform == platform;
}

bool HashData::isHash(const boost::filesystem::path &path, MpqFile::Locale locale, MpqFile::Platform platform) const
{
	return isHash(HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameA), HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameB), MpqFile::localeToInt(locale), MpqFile::platformToInt(platform));
}

const uint32 Hash::blockIndexDeleted = 0xFFFFFFFE;
const uint32 Hash::blockIndexEmpty = 0xFFFFFFFF;

Hash::Hash(class Mpq *mpq, uint32 index) : m_mpq(mpq), m_index(index), m_hashData(0, 0, 0, 0), m_block(0), m_deleted(false)
{
}

std::streamsize Hash::read(istream &istream) throw (class Exception)
{
	struct HashTableEntry entry;
	std::streamsize size = 0;
	wc3lib::read(istream, entry, size);

	if (size != sizeof(entry))
		throw Exception(_("Error while reading hash table entry."));

	this->hashData().setFilePathHashA(entry.filePathHashA);
	this->hashData().setFilePathHashB(entry.filePathHashB);
	this->hashData().setLocale(entry.locale);
	this->hashData().setPlatform(entry.platform);

	if (entry.fileBlockIndex == Hash::blockIndexDeleted)
	{
		this->m_deleted = true;
		//std::cout << "Hash entry is deleted." << std::endl;
		//exit(0);
	}
	else if (entry.fileBlockIndex != Hash::blockIndexEmpty)
	{
		this->m_block = this->mpq()->m_blocks.find(entry.fileBlockIndex)->get();
		this->m_deleted = false;

		//std::cout << "BLOCK INDEX: " << entry.fileBlockIndex << " and block address " << this->m_block << std::endl;
		//exit(0);

		if (this->m_block == 0)
			throw Exception(_("Error while searching for corresponding block of hash table entry."));
	}
	// otherwise it's empty (block == 0)
	else
	{
		//std::cout << "Entry is EMPTY WAAAAAAAAAHHHHHHH" << std::hex << " with block index " << entry.fileBlockIndex << std::dec << std::endl;
		//exit(0);
	}

	return size;
}

void Hash::clear()
{
	//exit(0);
	// If the next entry is empty, mark this one as empty; otherwise, mark this as deleted.
	Mpq::Hashes::index_const_iterator<HashData>::type iterator = this->m_mpq->m_hashes.get<HashData>().find(this->hashData());

	if (iterator == this->m_mpq->m_hashes.get<HashData>().end())
		return;

	++iterator;

	// If the next entry is empty, mark this one as empty; otherwise, mark this as deleted.
	if (iterator != this->m_mpq->m_hashes.get<HashData>().end() && !(*iterator)->empty())
		this->m_deleted = true;

	// If the block occupies space, mark the block as free space; otherwise, clear the block table entry.
	if (this->m_block->m_blockSize > 0)
	{
		this->m_block->m_fileSize = 0;
		this->m_block->m_flags = Block::Flags::None;
	}
	else
	{
		/// @todo Change file size?
		/*
		Mpq::Blocks::iterator iterator = this->mpq()->blocks().find(this->block());
		this->mpq()->m_blocks.erase(iterator);
		*/
	}

	this->m_block = 0;

	/// @todo Clear or write file hash and block data (file sync)!
}

/*
bool Hash::isHash(const boost::filesystem::path &path, enum MpqFile::Locale locale, enum MpqFile::Platform platform) const
{
	uint32 nameHashA = HashString(Mpq::cryptTable(), path.string().c_str(), NameA);
	uint32 nameHashB = HashString(Mpq::cryptTable(), path.string().c_str(), NameB);
	int16 realLocale = MpqFile::localeToInt(locale);
	int16 realPlatform = MpqFile::platformToInt(platform);

	return this->m_filePathHashA == nameHashA && this->m_filePathHashB == nameHashB && this->m_locale == realLocale && this->m_platform == realPlatform;
}

bool Hash::isHash(int32 nameHashA, int32 nameHashB, enum MpqFile::Locale locale, enum MpqFile::Platform platform) const
{
	int16 realLocale = MpqFile::localeToInt(locale);
	int16 realPlatform = MpqFile::platformToInt(platform);

	return this->isHash(nameHashA, nameHashB, locale, platform);
}
*/

/// @todo Write data into hash table.
void Hash::changePath(const boost::filesystem::path &path)
{
	this->hashData().setFilePathHashA(HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameA));
	this->hashData().setFilePathHashB(HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameB));
}

}

}
