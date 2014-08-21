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

HashData::HashData(const boost::filesystem::path& path, File::Locale locale, File::Platform platform) : m_filePathHashA(HashString(Archive::cryptTable(), path.string().c_str(), HashType::NameA)), m_filePathHashB(HashString(Archive::cryptTable(), path.string().c_str(), HashType::NameB)), m_locale(File::localeToInt(locale)), m_platform(File::platformToInt(platform))
{
}

HashData::HashData(const HashData &other) : m_filePathHashA(other.m_filePathHashA), m_filePathHashB(other.m_filePathHashB), m_locale(other.m_locale), m_platform(other.m_platform)
{
}

HashData::HashData(const HashTableEntry& entry)
{
	this->setFilePathHashA(entry.filePathHashA);
	this->setFilePathHashB(entry.filePathHashB);
	this->setLocale(entry.locale);
	this->setPlatform(entry.platform);
}


HashData& HashData::operator=(const HashData &other)
{
	m_filePathHashA = other.m_filePathHashA;
	m_filePathHashB = other.m_filePathHashB;
	m_locale = other.m_locale;
	m_platform = other.m_platform;

	return *this;
}

HashData& HashData::operator=(const HashTableEntry& entry)
{
	this->setFilePathHashA(entry.filePathHashA);
	this->setFilePathHashB(entry.filePathHashB);
	this->setLocale(entry.locale);
	this->setPlatform(entry.platform);

	return *this;
}

HashTableEntry HashData::toEntry() const
{
	HashTableEntry entry;
	entry.filePathHashA = this->filePathHashA();
	entry.filePathHashB = this->filePathHashB();
	entry.locale = this->locale();
	entry.platform = this->platform();

	return entry;
}

bool HashData::isHash(int32 nameHashA, int32 nameHashB, uint16 locale, uint16 platform) const
{
	return this->m_filePathHashA == nameHashA && this->m_filePathHashB == nameHashB && this->m_locale == locale && this->m_platform == platform;
}

bool HashData::isHash(const boost::filesystem::path &path, File::Locale locale, File::Platform platform) const
{
	return isHash(HashString(Archive::cryptTable(), path.string().c_str(), HashType::NameA), HashString(Archive::cryptTable(), path.string().c_str(), HashType::NameB), File::localeToInt(locale), File::platformToInt(platform));
}

const uint32 Hash::blockIndexDeleted = 0xFFFFFFFE;
const uint32 Hash::blockIndexEmpty = 0xFFFFFFFF;

Hash::Hash(Archive *mpq, uint32 index)
: m_mpq(mpq)
, m_index(index)
, m_hashData(0, 0, 0, 0)
, m_block(0)
, m_deleted(false)
{
}

Hash::~Hash()
{
}

std::streamsize Hash::read(istream &istream) throw (class Exception)
{
	struct HashTableEntry entry;
	std::streamsize size = 0;
	wc3lib::read(istream, entry, size);

	if (size != sizeof(entry))
	{
		throw Exception(_("Error while reading hash table entry."));
	}

	this->m_hashData = entry;

	if (entry.fileBlockIndex == Hash::blockIndexDeleted)
	{
		this->m_deleted = true;
	}
	else if (entry.fileBlockIndex != Hash::blockIndexEmpty)
	{
		this->m_block = &this->mpq()->blocks()[entry.fileBlockIndex];
		this->m_deleted = false;

		if (this->m_block == 0)
		{
			throw Exception(_("Error while searching for corresponding block of hash table entry."));
		}
	}

	return size;
}

std::streamsize Hash::write(ostream& ostream) const throw (Exception)
{
	HashTableEntry entry = this->cHashData().toEntry();
	std::streamsize size = 0;
	wc3lib::write(ostream, entry, size);

	return size;
}

void Hash::removeData()
{
	// If the next entry is empty, mark this one as empty; otherwise, mark this as deleted.
	   Archive::Hashes::const_iterator iterator = this->mpq()->hashes().find(this->hashData());

	if (iterator == this->mpq()->hashes().end())
	{
		throw Exception(_("Hash is not in archive."));
	}

	++iterator;

	// If the next entry is empty, mark this one as empty; otherwise, mark this as deleted.
	if (iterator != this->mpq()->hashes().end() && !iterator->second->empty())
	{
		this->m_deleted = true;
	}
	else
	{
		this->m_deleted = false;
	}

	// If the block occupies space, mark the block as free space; otherwise, clear the block table entry.
	/*
	if (this->block()->blockSize() > 0)
	{
		this->block()->m_fileSize = 0;
		this->block()->m_flags = Block::Flags::None;
	}
	else
	{
		/// @todo Change file size?
		/*
		Archive:Blocks::iterator iterator = this->mpq()->blocks().find(this->block());
		this->mpq()->m_blocks.erase(iterator);
		*/
	//}

	//this->m_block = 0;

	/// @todo Clear or write file hash and block data (file sync)!
}

/*
bool Hash::isHash(const boost::filesystem::path &path, enum File::Locale locale, enum File::Platform platform) const
{
	uint32 nameHashA = HashString(Archive:cryptTable(), path.string().c_str(), NameA);
	uint32 nameHashB = HashString(Archive:cryptTable(), path.string().c_str(), NameB);
	int16 realLocale = File::localeToInt(locale);
	int16 realPlatform = File::platformToInt(platform);

	return this->m_filePathHashA == nameHashA && this->m_filePathHashB == nameHashB && this->m_locale == realLocale && this->m_platform == realPlatform;
}

bool Hash::isHash(int32 nameHashA, int32 nameHashB, enum File::Locale locale, enum File::Platform platform) const
{
	int16 realLocale = File::localeToInt(locale);
	int16 realPlatform = File::platformToInt(platform);

	return this->isHash(nameHashA, nameHashB, locale, platform);
}
*/

/// @todo Write data into hash table.
void Hash::changePath(const boost::filesystem::path &path)
{
	this->hashData().setFilePathHashA(HashString(Archive::cryptTable(), path.string().c_str(), HashType::NameA));
	this->hashData().setFilePathHashB(HashString(Archive::cryptTable(), path.string().c_str(), HashType::NameB));
}

}

}
