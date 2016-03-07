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
, m_blockTableOffset(0)
, m_extendedBlockTableOffset(0)
, m_hashTableOffset(0)
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
	catch (Exception &exception)
	{
		this->clear();

		throw;
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
	catch (Exception &exception)
	{
		this->clear();

		throw;
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
	std::streamsize size = 0;
	Header header;
	readHeader(stream, header, this->m_startPosition, size);

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

	this->m_blockTableOffset = offset;

	if (!readBlockTable(stream, header.blockTableEntries, size))
	{
		throw Exception();
	}

	/*
	 * Read extended block table
	 * As of the Burning Crusade Friends and Family beta, this table is not encrypted.
	 */
	if (this->format() == Archive::Format::Mpq2)
	{
		this->m_extendedBlockTableOffset = extendedHeader->extendedBlockTableOffset;

		if (!readExtendedBlockTable(stream, size))
		{
			throw Exception();
		}
	}

	// read encrypted hash table
	offset = header.hashTableOffset;

	if (this->format() == Archive::Format::Mpq2 && extendedHeader->hashTableOffsetHigh > 0)
	{
		offset += uint64(extendedHeader->hashTableOffsetHigh) << 32;
	}

	this->m_hashTableOffset = offset;

	if (!readHashTable(stream, header.hashTableEntries, size))
	{
		return false;
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

bool Archive::readHeader(istream &in, Header &header, uint64 &startPosition, std::streamsize &size)
{
	/*
	 * Find header structure by using file key.
	 * The header must not start at the beginning of a file so it must be seeked for it.
	 */
	uint32 ident = 0;
	std::streamsize currentSize = 0;
	bool found = false;

	do
	{
		// TODO only read one byte after another
		wc3lib::read(in, ident, currentSize);

		if (currentSize < 4)
		{
			throw Exception(boost::format(_("Missing identifier \"%1%\" (read count %2%).")) % Archive::identifier % size);
		}

		if (memcmp(&ident, Archive::identifier, 4) == 0)
		{
			found = true;
		}
		else
		{
			// start with the next byte
			in.seekg(-3, std::ios::cur);
		}
	}
	while (!found && in);

	if (!found)
	{
		return false;
	}

	in.seekg(-4, std::ios::cur);
	startPosition = in.tellg();
	wc3lib::read(in, header, size);

	return true;
}

bool Archive::readBlockTable(istream &in, uint32 entries, std::streamsize &size)
{
	in.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(this->m_blockTableOffset));
	const std::size_t encryptedBytesSize = entries * sizeof(struct BlockTableEntry);
	boost::scoped_array<byte> encryptedBytes(new byte[encryptedBytesSize]);
	in.read(encryptedBytes.get(), encryptedBytesSize);
	const uint32 hashValue = HashString(Archive::cryptTable(), "(block table)", HashType::FileKey);
	DecryptData(Archive::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);
	arraystream sstream(encryptedBytes.get(), encryptedBytesSize);

	for (uint32 i = 0; i < entries; ++i)
	{
		std::auto_ptr<Block> ptr(new Block(i));
		size += ptr->read(sstream);
		this->m_blocks.push_back(ptr);
	}

	return true;
}

bool Archive::readExtendedBlockTable(istream &in, std::streamsize &size)
{
	in.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(this->m_extendedBlockTableOffset));

	BOOST_FOREACH(Blocks::reference block, this->m_blocks)
	{
		struct ExtendedBlockTableEntry extendedBlockTableEntry;
		wc3lib::read(in, extendedBlockTableEntry, size);
		block.setExtendedBlockOffset(extendedBlockTableEntry.extendedBlockOffset);
	}

	return true;
}

bool Archive::readHashTable(istream &in, uint32 entries, std::streamsize &size)
{
	in.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(this->m_hashTableOffset));
	const std::size_t encryptedBytesSize = entries * sizeof(struct HashTableEntry);
	boost::scoped_array<byte> encryptedBytes(new byte[encryptedBytesSize]);
	in.read(encryptedBytes.get(), encryptedBytesSize);
	const uint32 hashValue = HashString(Archive::cryptTable(), "(hash table)", HashType::FileKey);
	DecryptData(Archive::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);
	arraystream sstream(encryptedBytes.get(), encryptedBytesSize);

	for (uint32 i = 0; i < entries; ++i)
	{
		std::auto_ptr<Hash> hash(new Hash(this, i));
		size += hash->read(sstream);
		const HashData hashData = hash->cHashData();
		this->m_hashes.insert(hashData, hash);
	}

	return true;
}

bool Archive::writeHeader(ostream &out, std::streamsize &size) const
{
	std::streamsize currentSize = 0;

	Header header;
	memcpy(header.magic, Archive::identifier, 4);
	header.headerSize = sizeof(header);
	header.archiveSize = this->size();
	header.formatVersion = static_cast<uint16>(this->format());
	header.sectorSizeShift = this->sectorSizeShift();
	header.hashTableOffset = this->m_hashTableOffset;
	header.blockTableOffset = this->m_blockTableOffset;
	header.hashTableEntries = this->m_hashes.size();
	header.blockTableEntries = this->m_blocks.size();

	out.seekp(this->m_startPosition, std::ios::cur);
	wc3lib::write(out, header, currentSize);

	size += currentSize;

	return true;
}

bool Archive::writeBlockTable(ostream &out, std::streamsize &size) const
{
	const std::size_t encryptedBytesSize = this->m_blocks.size() * sizeof(struct BlockTableEntry);
	boost::scoped_array<byte> encryptedBytes(new byte[encryptedBytesSize]);
	std::streamsize tmpSize = 0;

	BOOST_FOREACH(Blocks::const_reference ref, this->blocks())
	{
		const BlockTableEntry blockTableEntry = ref.toBlockTableEntry();
		memcpy(encryptedBytes.get(), &blockTableEntry, sizeof(BlockTableEntry));
	}

	const uint32 hashValue = HashString(Archive::cryptTable(), "(block table)", HashType::FileKey);
	EncryptData(Archive::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);

	out.seekp(this->startPosition() + boost::numeric_cast<std::streamoff>(this->m_blockTableOffset));
	wc3lib::write(out, encryptedBytes, size, encryptedBytesSize);

	return true;
}

bool Archive::writeExtendedBlockTable(ostream &out, std::streamsize &size) const
{
	return false;
}

bool Archive::writeHashTable(ostream &out, std::streamsize &size) const
{
	return false;
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

bool Archive::removeFile(const File &mpqFile)
{
	Hashes::iterator hashIterator = this->hashes().find(mpqFile.hash()->hashData());

	// return false if the hash or block entry does not belong to the archive
	if (hashIterator == this->hashes().end() || this->blocks().size() >= mpqFile.block()->index() || &this->blocks()[mpqFile.block()->index()] != mpqFile.block())
	{
		return false;
	}

	// TODO jump to hash table and delete entry
	// TODO jump to block table

	/*
	 * Change both entries.
	 */
	mpqFile.block()->remove();
	mpqFile.hash()->remove();


	// rewrite both tables, since they are encrypted the whole tables have to be rewritten

	ofstream stream(this->path());
	stream.seekp(this->m_startPosition + this->m_blockTableOffset);
	std::streamsize size = 0;

	// write the whole block table since it is encrypted
	if (!writeBlockTable(stream, size))
	{
		return false;
	}

	stream.seekp(this->m_startPosition + this->m_hashTableOffset);

	if (!writeHashTable(stream, size))
	{
		return false;
	}

	// don't free any space, otherwise it takes longer

	return true;
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
