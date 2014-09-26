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

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include "mpq.hpp"
#include "sector.hpp"

namespace wc3lib
{

namespace mpq
{

const byte Archive::identifier[4] = { 'M', 'P', 'Q',  0x1A };
const int16 Archive::formatVersion1Identifier = 0x0000;
const int16 Archive::formatVersion2Identifier = 0x0001;
const uint32 Archive::maxBlockId = 0xFFFFFFFD;
const uint32 Archive::maxHashId = 0xFFFFFFFF;
const std::size_t Archive::headerSize = sizeof(struct Header);

const uint32* Archive::cryptTable()
{
	static uint32 cryptTable[cryptTableSize];
	static bool isInitialized = false;

	if (!isInitialized)
	{
		InitializeCryptTable(cryptTable);
		isInitialized = true;
	}

	return const_cast<const uint32*>(cryptTable);
}

Archive::Archive()
: m_size(0)
, m_path()
, m_format(Archive::Format::Mpq1)
, m_sectorSize(0)
, m_strongDigitalSignature(0)
, m_isOpen(false)
{
}

Archive::~Archive()
{
	this->close();
}

std::streamsize Archive::create(const boost::filesystem::path &path, bool overwriteExisting, std::streampos startPosition, Format format, uint32 sectorSize)
{
	this->close();

	if (boost::filesystem::exists(path) && !overwriteExisting)
	{
		throw Exception(boost::format(_("Unable to create file \"%1%\". File does already exist.")) % path.string());
	}

	ofstream ofstream(path, std::ios::out | std::ios::binary);

	if (!ofstream)
	{
		throw Exception(boost::format(_("Unable to create file \"%1%\".")) % path.string());
	}

	ofstream.seekp(startPosition);

	if (ofstream.tellp() != startPosition)
	{
		throw Exception(boost::str(boost::format(_("Unable to start in file \"%1%\" at position %2%.")) % path.string() % startPosition));
	}

	this->m_path = path;
	this->m_format = format;
	this->m_sectorSize = sectorSize;
	std::streamsize streamSize = 0;

	try
	{
		streamSize = this->write(ofstream);
	}
	catch (class Exception &exception)
	{
		this->clear();

		throw exception;
	}

	this->m_size = boost::filesystem::file_size(path);
	this->m_isOpen = true;

	return streamSize;
}

std::streamsize Archive::open(const boost::filesystem::path &path)
{
	this->close();

	ifstream stream(boost::filesystem::system_complete(path), std::ios_base::binary | std::ios_base::in);

	if (!stream)
	{
		throw Exception(boost::format(_("Unable to open file \"%1%\".")) % boost::filesystem::system_complete(path));
	}

	this->m_size = boost::filesystem::file_size(boost::filesystem::system_complete(path));
	this->m_path = boost::filesystem::system_complete(path);
	std::streamsize streamSize = 0;

	try
	{
		streamSize = this->read(stream);
	}
	catch (class Exception &exception)
	{
		this->clear();

		throw exception;
	}

	this->m_isOpen = true;

	return streamSize;
}

void Archive::close()
{
	if (!this->m_isOpen)
	{
		return;
	}

	this->clear();
}

std::streamsize Archive::read(InputStream &stream)
{
	/*
	 * Find header structure by using file key.
	 * The header must not start at the beginning of a file so it must be seeked for it.
	 */
	uint32 ident;
	std::streamsize size = 0;

	do
	{
		wc3lib::read(stream, ident, size);

		if (size < 4)
		{
			throw Exception(boost::format(_("Missing identifier \"%1%\" (read count %2%).")) % Archive::identifier % size);
		}
	}
	while (memcmp(&ident, Archive::identifier, 4) != 0);

	stream.seekg(-4, std::ios::cur);
	this->m_startPosition = stream.tellg();
	Header header;
	wc3lib::read(stream, header, size);

	if (header.formatVersion == Archive::formatVersion1Identifier)
	{
		this->m_format = Archive::Format::Mpq1;
	}
	else if (header.formatVersion == Archive::formatVersion2Identifier)
	{
		this->m_format = Archive::Format::Mpq2;
	}
	else
	{
		throw Exception(boost::format(_("Unknown MPQ format \"%1%\".")) % header.formatVersion);
	}

	if (header.blockTableEntries > maxBlockId + 1)
	{
		throw Exception(boost::format(_("Too many block table entries (%1%). Maximum allowed value is %2% since %3% and %4% are reserved for deleted and empty block indices.")) % header.blockTableEntries % maxBlockId % Hash::blockIndexDeleted % Hash::blockIndexEmpty);
	}

	// Number of entries in the hash table. Must be a power of two, and must be less than 2^16 for the original MoPaQ format, or less than 2^20 for the Burning Crusade format.
	if (header.hashTableEntries % 2 != 0)
	{
		std::cerr << boost::format(_("Warning: Hash table entries should be a power of two for MPQ file %1%.\nEntries: %2%.")) % this->path() % header.hashTableEntries << std::endl;
	}

	if ((this->format() == Archive::Format::Mpq1 && header.hashTableEntries >= pow(2, 16)) || (this->format() == Archive::Format::Mpq2 && header.hashTableEntries >= pow(2, 20)))
	{
		std::cerr << boost::format(_("Warning: There are too many MPQ hash table entries in MPQ file %1%.\nEntries: %2%.\nFor MPQ1 it must be less than 2^16 and for MPQ2 less than 2^20.")) % this->path() % header.hashTableEntries << std::endl;
	}

	// According to the StormLib this value is sometimes changed by map creators to protect their maps. As in the StormLib this value is ignored.
	if (header.headerSize != sizeof(header))
	{
		std::cerr << boost::format(_("Warning: MPQ header size is not equal to real header size.\nContained header size: %1%.\nReal header size: %2%.")) % header.headerSize % sizeof(header) << std::endl;
	}

	if (header.archiveSize != this->size())
	{
		std::cerr << boost::format(_("Warning: MPQ file size of MPQ file %1% is not equal to its internal header file size.\nFile size: %2%.\nInternal header file size: %3%.")) % this->path() % this->size()
		% header.archiveSize << std::endl;
	}

	this->m_sectorSize = pow(2, header.sectorSizeShift) * 512;
	boost::scoped_ptr<ExtendedHeader> extendedHeader;

	if (this->format() == Archive::Format::Mpq2)
	{
		extendedHeader.reset(new ExtendedHeader);
		wc3lib::read(stream, *extendedHeader, size);
	}

	uint64 offset = header.blockTableOffset;

	if (this->format() == Archive::Format::Mpq2 && extendedHeader->blockTableOffsetHigh > 0)
	{
		offset += uint64(extendedHeader->blockTableOffsetHigh) << 32;
	}

	stream.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(offset));
	std::size_t encryptedBytesSize = header.blockTableEntries * sizeof(struct BlockTableEntry);
	boost::scoped_array<byte> encryptedBytes(new byte[encryptedBytesSize]);
	stream.read(encryptedBytes.get(), encryptedBytesSize);
	uint32 hashValue = HashString(Archive::cryptTable(), "(block table)", HashType::FileKey);
	DecryptData(Archive::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);
	arraystream sstream(encryptedBytes.get(), encryptedBytesSize);

	for (uint32 i = 0; i < header.blockTableEntries; ++i)
	{
		std::auto_ptr<Block> ptr(new Block(i));
		size += ptr->read(sstream);
		this->m_blocks.push_back(ptr);
	}

	/*
	 * Read extended block table
	 * As of the Burning Crusade Friends and Family beta, this table is not encrypted.
	 */
	if (this->format() == Archive::Format::Mpq2)
	{
		stream.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(extendedHeader->extendedBlockTableOffset));

		BOOST_FOREACH(Blocks::reference block, this->m_blocks)
		{
			struct ExtendedBlockTableEntry extendedBlockTableEntry;
			wc3lib::read(stream, extendedBlockTableEntry, size);
			block.setExtendedBlockOffset(extendedBlockTableEntry.extendedBlockOffset);
		}
	}

	// read encrypted hash table
	offset = header.hashTableOffset;

	if (this->format() == Archive::Format::Mpq2 && extendedHeader->hashTableOffsetHigh > 0)
	{
		offset += uint64(extendedHeader->hashTableOffsetHigh) << 32;
	}

	stream.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(offset));
	encryptedBytesSize = header.hashTableEntries * sizeof(struct HashTableEntry);
	encryptedBytes.reset(new byte[encryptedBytesSize]);
	stream.read(encryptedBytes.get(), encryptedBytesSize);
	hashValue = HashString(Archive::cryptTable(), "(hash table)", HashType::FileKey);
	DecryptData(Archive::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);
	sstream.close();
	sstream.open(encryptedBytes.get(), encryptedBytesSize);

	for (uint32 i = 0; i < header.hashTableEntries; ++i)
	{
		std::auto_ptr<Hash> hash(new Hash(this, i));
		size += hash->read(sstream);
		const HashData hashData = hash->cHashData();
		this->m_hashes.insert(hashData, hash);
	}

	// The strong digital signature is stored immediately after the archive, in the containing file
	const std::streampos position = startPosition() + boost::numeric_cast<std::streampos>(header.archiveSize);

	// jumps to the end of the archive
	if (endPosition(stream) > position && Archive::hasStrongDigitalSignature(stream))
	{
		m_strongDigitalSignaturePosition = stream.tellg();
		size += strongDigitalSignature(stream, this->m_strongDigitalSignature);
	}

	return size;
}

/// @todo Write with format this->m_format!
std::streamsize Archive::write(OutputStream &ostream) const
{
	return 0;
}

uint32_t Archive::version() const
{
	switch (format())
	{
		case Format::Mpq1:
		{
			return formatVersion1Identifier;
		}

		case Format::Mpq2:
		{
			return formatVersion2Identifier;
		}
	}

	return 0;
}

bool Archive::operator!() const
{
	return !this->isOpen();
}

void Archive::clear()
{
	this->m_strongDigitalSignature.reset();

	this->m_hashes.clear();
	this->m_blocks.clear();

	this->m_size = 0;
	this->m_path.clear();
	this->m_format = Archive::Format::Mpq1;
	this->m_sectorSize = 0;
	this->m_isOpen = false;
}

Hash* Archive::findHash(const HashData &hashData)
{
	Hashes::iterator iterator = this->hashes().find(hashData);

	if (iterator == this->hashes().end())
	{
		return 0;
	}

	return iterator->second;
}

const Hash* Archive::findHash(const HashData &hashData) const
{
	Hashes::const_iterator iterator = this->hashes().find(hashData);

	if (iterator == this->hashes().end())
	{
		return 0;
	}

	return iterator->second;
}


Hash* Archive::findHash(const boost::filesystem::path &path, File::Locale locale, File::Platform platform)
{
	if (this->hashes().empty())
	{
		return 0;
	}

	return findHash(HashData(path, locale, platform));
}

const Hash* Archive::findHash(const boost::filesystem::path &path, File::Locale locale, File::Platform platform) const
{
	if (this->hashes().empty())
	{
		return 0;
	}

	return findHash(HashData(path, locale, platform));
}

File Archive::findFile(const HashData &hashData)
{
	Hash *hash = this->findHash(hashData);

	if (hash == 0 || hash->deleted() || hash->empty())
	{
		return File();
	}

	return File(this, hash, "");
}

File Archive::findFile(const boost::filesystem::path &path, File::Locale locale, File::Platform platform)
{
	Hash *hash = this->findHash(path, locale, platform);

	if (hash == 0 || hash->deleted() || hash->empty())
	{
		return File();
	}

	return File(this, hash, path);
}

Listfile Archive::listfileFile()
{
	Hash *hash = this->findHash("(listfile)");

	if (hash == 0)
	{
		return Listfile();
	}

	return Listfile(this, hash);
}

Attributes Archive::attributesFile()
{
	Hash *hash = this->findHash("(attributes)");

	if (hash == 0)
	{
		return Attributes();
	}

	return Attributes(this, hash);
}

Signature Archive::signatureFile()
{
	Hash *hash = this->findHash("(signature)");

	if (hash == 0)
	{
		return Signature();
	}

	return Signature(this, hash);
}

}

}
