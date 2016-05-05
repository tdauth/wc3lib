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
, m_startPosition(0)
, m_blockTableOffset(0)
, m_extendedBlockTableOffset(0)
, m_hashTableOffset(0)
, m_strongDigitalSignaturePosition(0)
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

std::streamsize Archive::create(const boost::filesystem::path &path, uint32 hashTableEntries, uint32 blockTableEntries, Format format, uint32 sectorSize, std::streampos startPosition)
{
	this->close();

	ofstream out(path, std::ios::out | std::ios::binary);

	if (!out)
	{
		throw Exception(boost::format(_("Unable to create file \"%1%\".")) % path.string());
	}

	out.seekp(startPosition);

	if (out.tellp() != startPosition)
	{
		throw Exception(boost::str(boost::format(_("Unable to start in file \"%1%\" at position %2%.")) % path.string() % startPosition));
	}

	this->m_path = path;
	this->m_format = format;
	this->m_sectorSize = sectorSize;
	this->m_startPosition = startPosition;
	std::streamsize streamSize = 0;

	// create empty blocks
	for (uint32 i = 0; i < blockTableEntries; ++i)
	{
		this->m_blocks.push_back(new Block(i));
	}

	// create empty hashes
	for (uint32 i = 0; i < blockTableEntries; ++i)
	{
		HashData key;
		this->m_hashes.insert(key, new Hash(this, i));
	}

	// skip header for later writing
	out.seekp(sizeof(Header), std::ios::cur);

	this->m_blockTableOffset = out.tellp() - this->startPosition();

	if (!this->writeBlockTable(out, streamSize))
	{
		throw Exception(_("Error on writing block table."));
	}

	if (this->format() == Archive::Format::Mpq2)
	{
		this->m_extendedBlockTableOffset = out.tellp() - this->startPosition();

		if (!this->writeExtendedBlockTable(out, streamSize))
		{
			throw Exception(_("Error on writing extended block table."));
		}
	}

	this->m_hashTableOffset = out.tellp() - this->startPosition();

	if (!this->writeHashTable(out, streamSize))
	{
		throw Exception();
	}

	const std::streampos current = out.tellp();
	const uint32 archiveSize = boost::numeric_cast<uint32>(streamSize) + sizeof(Header); // includes the header size
	this->m_size = archiveSize;

	// seeks automatically
	if (!this->writeHeader(out, streamSize))
	{
		throw Exception(_("Error on writing header."));
	}

	out.seekp(current);

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

	if (!readHeader(stream, header, this->m_startPosition, size))
	{
		throw Exception(_("Missing MPQ header."));
	}

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

	const uint64 expectedArchiveSize = boost::filesystem::file_size(this->path()) - this->startPosition();

	if (header.archiveSize != expectedArchiveSize)
	{
		std::cerr << boost::format(_("Warning: MPQ file size of MPQ file %1% is not equal to its internal header file size.\nFile size: %2%.\nInternal header file size: %3%.")) % this->path() % expectedArchiveSize
		% header.archiveSize << std::endl;
	}

	this->m_size = header.archiveSize;
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
		throw Exception(_("Error on reading block table."));
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
			throw Exception(_("Error on reading extended block table."));
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
		throw Exception(_("Error on reading hash table."));
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
	Blocks blocks;

	for (uint32 i = 0; i < entries; ++i)
	{
		std::auto_ptr<Block> ptr(new Block(i));
		size += ptr->read(sstream);
		blocks.push_back(ptr);
	}

	// exception safe
	this->m_blocks.swap(blocks);

	return true;
}

bool Archive::readExtendedBlockTable(istream &in, std::streamsize &size)
{
	in.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(this->m_extendedBlockTableOffset));
	Blocks blocks = this->blocks();

	BOOST_FOREACH(Blocks::reference block, blocks)
	{
		ExtendedBlockTableEntry extendedBlockTableEntry;
		wc3lib::read(in, extendedBlockTableEntry, size);
		block.setExtendedBlockOffset(extendedBlockTableEntry.extendedBlockOffset);
	}

	// exception safe
	this->m_blocks.swap(blocks);

	return true;
}

bool Archive::readHashTable(istream &in, uint32 entries, std::streamsize &size)
{
	/*
	 * The hash table offset is relative to the start position of the archive.
	 * Therefore jump to the absolute position to get the hash table.
	 */
	const std::streamoff absoluteOffset = this->startPosition() + boost::numeric_cast<std::streamoff>(this->hashTableOffset());
	in.seekg(absoluteOffset);
	/*
	 * The complete hash table is encrypted by the file key of the string "(hash table)".
	 * Therefore it has to be decrypted.
	 * Use a scope pointer to prevent memory leaks but still use the heap.
	 */
	const std::size_t encryptedBytesSize = entries * sizeof(struct HashTableEntry);
	boost::scoped_array<byte> encryptedBytes(new byte[encryptedBytesSize]);
	in.read(encryptedBytes.get(), encryptedBytesSize);
	const uint32 hashValue = HashString(Archive::cryptTable(), "(hash table)", HashType::FileKey);
	DecryptData(Archive::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);
	/*
	 * The arraystream is used as wrapper for the array to read from it like from a stream.
	 */
	arraystream sstream(encryptedBytes.get(), encryptedBytesSize);
	Hashes hashes;

	for (uint32 i = 0; i < entries; ++i)
	{
		std::auto_ptr<Hash> hash(new Hash(this, i));
		size += hash->read(sstream);
		const HashData hashData = hash->cHashData();
		hashes.insert(hashData, hash);
	}

	// exception safe
	this->m_hashes.swap(hashes);

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

	// absolute position
	out.seekp(this->m_startPosition);
	wc3lib::write(out, header, currentSize);

	size += currentSize;

	return true;
}

bool Archive::writeBlockTable(ostream &out, std::streamsize &size) const
{
	const std::size_t encryptedBytesSize = this->m_blocks.size() * sizeof(struct BlockTableEntry);
	boost::scoped_array<byte> encryptedBytes(new byte[encryptedBytesSize]);

	BOOST_FOREACH(Blocks::const_reference ref, this->blocks())
	{
		const BlockTableEntry blockTableEntry = ref.toBlockTableEntry();
		memcpy(encryptedBytes.get(), &blockTableEntry, sizeof(BlockTableEntry));
	}

	const uint32 hashValue = HashString(Archive::cryptTable(), "(block table)", HashType::FileKey);
	EncryptData(Archive::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);

	out.seekp(this->startPosition() + boost::numeric_cast<std::streamoff>(this->blockTableOffset()));
	wc3lib::write(out, encryptedBytes.get(), size, encryptedBytesSize);

	return true;
}

bool Archive::writeExtendedBlockTable(ostream &out, std::streamsize &size) const
{
	return false;
}

bool Archive::writeHashTable(ostream &out, std::streamsize &size) const
{
	const std::size_t encryptedBytesSize = this->m_blocks.size() * sizeof(struct BlockTableEntry);
	boost::scoped_array<byte> encryptedBytes(new byte[encryptedBytesSize]);

	BOOST_FOREACH(Hashes::const_reference ref, this->hashes())
	{
		const HashTableEntry hashEntry = ref.second->toHashTableEntry();
		memcpy(encryptedBytes.get(), &hashEntry, sizeof(HashTableEntry));
	}

	const uint32 hashValue = HashString(Archive::cryptTable(), "(hash table)", HashType::FileKey);
	EncryptData(Archive::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);

	out.seekp(this->startPosition() + boost::numeric_cast<std::streamoff>(this->hashTableOffset()));
	wc3lib::write(out, encryptedBytes.get(), size, encryptedBytesSize);

	return true;
}

void Archive::clear()
{
	this->m_strongDigitalSignature.reset();

	this->m_hashes.clear();
	this->m_blocks.clear();

	this->m_size = 0;
	this->m_path.clear();
	this->m_startPosition = 0;
	this->m_blockTableOffset = 0;
	this->m_extendedBlockTableOffset = 0;
	this->m_hashTableOffset = 0;
	this->m_format = Archive::Format::Mpq1;
	this->m_sectorSize = 0;
	this->m_strongDigitalSignaturePosition = 0;
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

	// TODO dont rewrite everything look at DeleteFile() from the specification, just NULL the entry and make the previous empty different?
	stream.seekp(this->m_startPosition + this->m_hashTableOffset);

	if (!writeHashTable(stream, size))
	{
		return false;
	}

	// don't free any space, otherwise it takes longer

	return true;
}

File Archive::addFile(const boost::filesystem::path &filePath, const byte *data, uint64 dataSize, Sector::Compression compression, Block::Flags flags, File::Locale locale, File::Platform platform)
{
	/*
	 * Get a free block for the file data and a free hash for the hash data of the file path etc.
	 */
	Hash *hash = firstFreeHash();

	/*
	 * If there is no free hash or block then the table is too small and no further file can be added.
	 */
	if (hash == nullptr)
	{
		throw TooSmallHashTableException();
	}

	Block *block = firstFreeBlock();

	if (block == nullptr)
	{
		throw TooSmallBlockTableException();
	}

	/*
	 * Store the written sectors
	 */
	Sector::Sectors sectors;

	/*
	 * Compress the whole input data at once with the specified flags and compression type.
	 */
	uint32 compressedSize = 0;
	boost::scoped_array<byte> compressedData(Sector::compress(data, dataSize, flags, compression, compressedSize));

	/*
	 * The filename is required to generate sector keys for encrypted files.
	 */
	string fileName = filePath.c_str();
	Listfile::toListfileEntry(fileName);

	// write sector table
	if (Block::hasSectorOffsetTable(flags))
	{
		uint32 sectorIndex = 0;
		uint32 sectorOffset = 0;

		while (sectorOffset < compressedSize)
		{
			const uint32 remainingSize = compressedSize - sectorOffset;
			uint32 sectorSize = std::min<uint32>(this->sectorSize(), remainingSize);

			if (compression != Sector::Compression::Uncompressed)
			{
				++sectorSize;
			}

			// TODO how to calculate uncompressed size?
			sectors.push_back(Sector(this, block, fileName, sectorIndex, sectorOffset, sectorSize, 0, compression));

			sectorOffset += sectorSize;
			++sectorIndex;
		}
	}
	// write single block data without sector table
	else
	{
		// The compression byte is stored as well for compressed sectors BUT is written in writeCompressed() not here.
		const uint32 sectorSize = compression == Sector::Compression::Uncompressed ? compressedSize : compressedSize + 1;
		sectors.push_back(Sector(this, block, fileName, 0, 0, sectorSize, dataSize, compression));
	}

	/*
	 * Calculate the complete offset of the used block for writing the data at the correct position into the file.
	 * The newly calculated offset should be the end of the archive.
	 */
	uint32 blockOffset = 0;
	uint16 extendedBlockOffset = 0;
	nextBlockOffsets(blockOffset,  extendedBlockOffset);
	// At this offset the sector table starts.
	const uint64 completeBlockOffset = blockOffset + (((uint64)extendedBlockOffset) << 32);

	// prepare the block entry
	block->setBlockOffset(blockOffset);
	block->setExtendedBlockOffset(extendedBlockOffset);
	// the flags are required by Sector::writeCompressed()!
	block->setFlags(flags);

	/*
	 * Write data to the end of the archive.
	 */
	ofstream out(this->path());
	// Start at the position of the sector table.
	out.seekp(completeBlockOffset);
	uint32 fileSize = 0; // uncompressed size
	uint32 blockSize = 0; // compressed size

	// Write the sector table before writing the actual file data.
	// TODO does the block size include the size of the sector table?
	blockSize += Sector::writeSectors(out, sectors, flags, compressedSize, dataSize, blockOffset, filePath.string().c_str());

	/*
	 * Write all sectors data into the archive and use all block space.
	 */
	BOOST_FOREACH(Sector::Sectors::reference ref, sectors)
	{
		const uint32 sectorOffset = ref.sectorOffset();
		const uint32 sectorSize = ref.sectorSize(); // compressed size
		// since our data is already compressed it only has to be encrypted (if the file is encrypted) and the compression type has to be stored if the file is compressed
		ref.writeCompressed(&compressedData[sectorOffset], sectorSize, out);

		fileSize += ref.uncompressedSize();
		blockSize += sectorSize;
	}

	/*
	 * Remove old hash entry to update its hash key.
	 */
	HashData oldHashData = hash->hashData();
	Hashes::iterator iterator = this->m_hashes.find(oldHashData);

	if (iterator == this->m_hashes.end())
	{
		throw Exception();
	}

	// store old hash for reinserting it
	// TODO dont copy but rather transfer the pointer into the autopointer!
	std::auto_ptr<Hash> newHash(new Hash(*iterator->second));
	this->m_hashes.erase(iterator);

	/*
	 * Prepare the updated hash entry.
	 */
	HashData hashData(filePath, locale, platform);
	newHash->setHashData(hashData);
	newHash->setBlock(block);
	newHash->setDeleted(false);

	// update the hash key in the hash table
	this->m_hashes.insert(hashData, newHash);

	/*
	 * Prepare the updated block entry.
	 */
	block->setBlockSize(blockSize);
	block->setFileSize(fileSize);

	// write the tables to the output file that the archive file is up to date
	std::streamsize size = 0;

	// write the whole block table since it is encrypted
	// seeks automatically with seekp
	if (!writeBlockTable(out, size))
	{
		return File();
	}

	// seeks automatically with seekp
	if (!writeHashTable(out, size))
	{
		return File();
	}

	// update the header since the whole archive size has changed
	this->m_size += blockSize; // add the compressed size

	// seeks automatically with seekp
	if (!writeHeader(out, size))
	{
		return File();
	}

	// the resulting file should be available now since the hash table entry has been updated
	return findFile(filePath, locale, platform);
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
