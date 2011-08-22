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

#include "mpq.hpp"
#include "sector.hpp"

namespace wc3lib
{

namespace mpq
{

const byte Mpq::identifier[4] = { 'M', 'P', 'Q',  0x1A };
const int16 Mpq::formatVersion1Identifier = 0x0000;
const int16 Mpq::formatVersion2Identifier = 0x0001;
const int32 Mpq::extendedAttributesVersion = 100;
const uint32 Mpq::maxBlockId = 0xFFFFFFFD;
const uint32 Mpq::maxHashId = 0xFFFFFFFF;
const std::size_t Mpq::headerSize = sizeof(struct Header);

const uint32* Mpq::cryptTable()
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

Mpq::Mpq() : m_size(0), m_path(), m_format(Mpq::Format::Mpq1), m_extendedAttributes(Mpq::ExtendedAttributes::None), m_sectorSize(0), m_strongDigitalSignature(0), m_isOpen(false), m_fileLock()
{
}

Mpq::~Mpq()
{
	this->close();
}

std::streamsize Mpq::create(const boost::filesystem::path &path, bool overwriteExisting, std::streampos startPosition, BOOST_SCOPED_ENUM(Format) format, BOOST_SCOPED_ENUM(ExtendedAttributes) extendedAttributes, int32 sectorSize, bool hasStrongDigitalSignature, bool containsListfileFile, bool containsSignatureFile) throw (class Exception)
{
	this->close();

	if (boost::filesystem::exists(path) && !overwriteExisting)
		throw Exception(boost::str(boost::format(_("Unable to create file \"%1%\". File does already exist.")) % path.string()));

	ofstream ofstream(path, std::ios_base::binary);

	if (!ofstream)
		throw Exception(boost::format(_("Unable to create file \"%1%\".")) % path.string());

	boost::interprocess::file_lock fileLock(path.string().c_str());
	
	if (!fileLock.try_lock())
		throw Exception(boost::format(_("Unable to lock file \"%1%\".")) % path);

	ofstream.seekp(startPosition);

	if (ofstream.tellp() != startPosition)
	{
		fileLock.unlock();
		
		throw Exception(boost::str(boost::format(_("Unable to start in file \"%1%\" at position %2%.")) % path.string() % startPosition));
	}

	this->m_fileLock.swap(fileLock);
	this->m_size = boost::filesystem::file_size(path);
	this->m_path = path;
	this->m_format = format;
	this->m_extendedAttributes = extendedAttributes;
	this->m_sectorSize = sectorSize;
	std::streamsize streamSize = 0;

	try
	{
		streamSize = this->write(ofstream);

		/// @todo FIXME, set stream size!!!
		if (hasStrongDigitalSignature)
			;
			//this->createStrongDigitalSignature();

		if (containsListfileFile)
			this->createListfileFile();

		if (containsSignatureFile)
			;
			//this->createSignatureFile();
	}
	catch (class Exception &exception)
	{
		this->clear();

		throw exception;
	}

	this->m_isOpen = true;

	return streamSize;
}

std::streamsize Mpq::open(const boost::filesystem::path &path, const MpqFile::ListfileEntries &listfileEntries) throw (class Exception)
{
	this->close();
	
	boost::interprocess::file_lock fileLock(path.string().c_str());
	
	if (!fileLock.try_lock())
		throw Exception(boost::format(_("Unable to lock file \"%1%\".")) % path);
	
	this->m_fileLock.swap(fileLock);
	ifstream stream(boost::filesystem::system_complete(path), std::ios_base::binary | std::ios_base::in);

	if (!stream)
		throw Exception(boost::format(_("Unable to open file \"%1%\".")) % boost::filesystem::system_complete(path));

	this->m_size = boost::filesystem::file_size(boost::filesystem::system_complete(path));
	this->m_path = boost::filesystem::system_complete(path);
	std::streamsize streamSize = 0;

	try
	{
		streamSize = this->read(stream, listfileEntries);
	}
	catch (class Exception &exception)
	{
		this->clear();

		throw exception;
	}

	this->m_isOpen = true;

	return streamSize;
}

void Mpq::close()
{
	if (!this->m_isOpen)
		return;

	std::cout << "Closing archive." << std::endl;
	this->clear();
}

std::streamsize Mpq::read(InputStream &stream, const MpqFile::ListfileEntries &listfileEntries) throw (class Exception)
{
	// find header structure by using file key
	uint32 ident;
	std::streamsize size = 0;

	do
	{
		wc3lib::read(stream, ident, size);

		if (size < 4)
			throw Exception(boost::format(_("Missing identifier \"%1%\" (read count %2%).")) % Mpq::identifier % size);
	}
	while (memcmp(&ident, Mpq::identifier, 4) != 0);

	stream.seekg(-4, std::ios::cur);
	this->m_startPosition = stream.tellg();
	struct Header header;
	wc3lib::read(stream, header, size);

	if (memcmp(header.magic, Mpq::identifier, sizeof(Mpq::identifier)))
		throw Exception(boost::str(boost::format(_("Missing MPQ identifier \"%1%\".")) % Mpq::identifier));

	if (header.formatVersion == Mpq::formatVersion1Identifier)
		this->m_format = Mpq::Format::Mpq1;
	else if (header.formatVersion == Mpq::formatVersion2Identifier)
		this->m_format = Mpq::Format::Mpq2;
	else
		throw Exception(boost::str(boost::format(_("Unknown MPQ format \"%1%\".")) % header.formatVersion));
	
	if (header.blockTableEntries > maxBlockId + 1)
		throw Exception(boost::format(_("Too many block table entries (%1%). Maximum allowed value is %2% since %3% and %4% are reserved for deleted and empty block indices.")) % header.blockTableEntries % maxBlockId % Hash::blockIndexDeleted % Hash::blockIndexEmpty);

	// Number of entries in the hash table. Must be a power of two, and must be less than 2^16 for the original MoPaQ format, or less than 2^20 for the Burning Crusade format.
	if (header.hashTableEntries % 2 != 0)
		std::cerr << boost::format(_("Warning: Hash table entries should be a power of two for MPQ file \"%1%\".\nEntries: %2%.")) % this->m_path.string() % header.hashTableEntries << std::endl;
		
	if ((this->format() == Mpq::Format::Mpq1 && header.hashTableEntries >= pow(2, 16)) || (this->format() == Mpq::Format::Mpq2 && header.hashTableEntries >= pow(2, 20)))
		std::cerr << boost::format(_("Warning: There are too many MPQ hash table entries in MPQ file \"%1%\".\nEntries: %2%.\nFor MPQ1 it must be less than 2^16 and for MPQ2 less than 2^20.")) % this->m_path.string() % header.hashTableEntries << std::endl;

	// According to the StormLib this value is sometimes changed by map creators to protect their maps. As in the StormLib this value is ignored.
	if (header.headerSize != sizeof(header))
		std::cerr << boost::format(_("Warning: MPQ header size is not equal to real header size.\nContained header size: %1%.\nReal header size: %2%.")) % header.headerSize % sizeof(header) << std::endl;

	if (header.archiveSize != this->m_size)
		std::cerr << boost::format(_("Warning: MPQ file size of MPQ file \"%1%\" is not equal to its internal header file size.\nFile size: %2%.\nInternal header file size: %3%.")) % this->m_path.string() % this->m_size % header.archiveSize << std::endl;

	this->m_sectorSize = pow(2, header.sectorSizeShift) * 512;
	struct ExtendedHeader extendedHeader;

	if (this->format() == Mpq::Format::Mpq2)
		wc3lib::read(stream, extendedHeader, size);

	uint64 offset = header.blockTableOffset;

	if (this->format() == Mpq::Format::Mpq2 && extendedHeader.blockTableOffsetHigh > 0)
		offset += int64(extendedHeader.blockTableOffsetHigh) << 32;

	stream.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(offset));
	std::size_t encryptedBytesSize = header.blockTableEntries * sizeof(struct BlockTableEntry);
	boost::scoped_array<byte> encryptedBytes(new byte[encryptedBytesSize]);
	stream.read(encryptedBytes.get(), encryptedBytesSize);
	uint32 hashValue = HashString(Mpq::cryptTable(), "(block table)", HashType::FileKey);
	DecryptData(Mpq::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);
	std::basic_istringstream<byte> sstream;
	sstream.rdbuf()->pubsetbuf(encryptedBytes.get(), encryptedBytesSize);
	//sstream.write(encryptedBytes.get(), encryptedBytesSize);
	//encryptedBytes.reset();;

	for (std::size_t i = 0; i < header.blockTableEntries; ++i)
	{
		this->m_blocks.left.insert(std::make_pair(i, BlockPtr(new Block(this))));
		size += this->m_blocks.left.find(i)->second->read(sstream);
	}

	/*
	read extended block table
	As of the Burning Crusade Friends and Family beta, this table is not encrypted.
	*/
	if (this->format() == Mpq::Format::Mpq2)
	{
		stream.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(extendedHeader.extendedBlockTableOffset));

		BOOST_FOREACH(Blocks::left_reference block, this->m_blocks.left)
		{
			struct ExtendedBlockTableEntry extendedBlockTableEntry;
			wc3lib::read(stream, extendedBlockTableEntry, size);
			block.second->m_extendedBlockOffset = extendedBlockTableEntry.extendedBlockOffset;
		}
	}

	// read encrypted hash table
	offset = header.hashTableOffset;

	if (this->format() == Mpq::Format::Mpq2 && extendedHeader.hashTableOffsetHigh > 0)
		offset += boost::numeric_cast<uint64>(extendedHeader.hashTableOffsetHigh) << 32;

	stream.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(offset));
	encryptedBytesSize = header.hashTableEntries * sizeof(struct HashTableEntry);
	encryptedBytes.reset(new byte[encryptedBytesSize]);
	stream.read(encryptedBytes.get(), encryptedBytesSize);
	hashValue = HashString(Mpq::cryptTable(), "(hash table)", HashType::FileKey);
	DecryptData(Mpq::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);
	sstream.rdbuf()->pubsetbuf(encryptedBytes.get(), encryptedBytesSize);

	for (std::size_t i = 0; i < header.hashTableEntries; ++i)
	{
		HashPtr hash(new Hash(this));
		hash->m_index = i;
		size += hash->read(sstream);
		this->m_hashes.get<uint32>().insert(hash);
	}

	// file data / add file instances
	BOOST_FOREACH(HashPtr hash, this->m_hashes.get<uint32>())
	{
		if (!hash->empty() && !hash->deleted())
		{
			FilePtr mpqFile(new MpqFile(this, hash.get()));
			//mpqFile->m_path = // path can only be set if there is a listfile file or if we're able to convert its hash into file path
			this->m_files.get<0>().push_back(mpqFile);

			// seek to file data beginning
			stream.seekg(startPosition() + boost::numeric_cast<std::streampos>(mpqFile->hash()->block()->blockOffset()));
			
			if (format() == Mpq::Format::Mpq2 && mpqFile->hash()->block()->extendedBlockOffset() > 0)
				stream.seekg(boost::numeric_cast<std::streamoff>(mpqFile->hash()->block()->extendedBlockOffset()), std::ios_base::cur);
			
			/// \todo Decrypt and unimplode data? boost::numeric_cast<uint32>(this->m_startPosition)

			size += mpqFile->read(stream);
		}
	}
	
	MpqFile::ListfileEntries entries;

	if (listfileEntries.empty())
	{
		MpqFile *listfileFile = this->listfileFile();
		
		if (listfileFile != 0)
		{
			// read listfile file and create path entries
			entries = listfileFile->listfileEntries();
			
			BOOST_FOREACH(MpqFile::ListfileEntries::const_reference path, entries)
				this->findFile(path);
			
			// TEST
			//std::cout << "Data:\n" << astream << std::endl;
			//std::cout << "Second Data:\n" << *this->listfileFile() << std::endl;
		}
	}
	
	// read listfile file and create path entries
	BOOST_FOREACH(MpqFile::ListfileEntries::const_reference path, listfileEntries.empty() ? entries : listfileEntries)
		this->findFile(path);

	/// @todo Single "(attributes)" file?
	/*
	// extended block attributes
	struct ExtendedAttributes extendedAttributes;
	istream.read(reinterpret_cast<char*>(&extendedAttributes), sizeof(extendedAttributes));
	bytes += istream.gcount();

	if (extendedAttributes.version != Mpq::extendedAttributesVersion)
		std::cout << boost::format(_("Warning: Unknown extended attributes version \"%1%\".\nCurrent version is \"%2%\".")) % extendedAttributes.version % Mpq::extendedAttributesVersion << std::endl;

	this->m_extendedAttributes = extendedAttributes.attributesPresent; /// @todo Convert to enum

	if (this->m_extendedAttributes & Mpq::FileCrc32s)
	{
		BOOST_FOREACH(class Block *block, this->m_blocks)
			size += istream.read(reinterpret_cast<char*>(&block->m_crc32), sizeof(block->m_crc32));
	}

	if (this->m_extendedAttributes & Mpq::FileTimeStamps)
	{
		BOOST_FOREACH(class Block *block, this->m_blocks)
			size += istream.read(reinterpret_cast<char*>(&block->m_fileTime), sizeof(block->m_fileTime));
	}

	if (this->m_extendedAttributes & Mpq::FileMd5s)
	{
		BOOST_FOREACH(class Block *block, this->m_blocks)
			size += istream.read(reinterpret_cast<char*>(&block->m_md5), sizeof(block->m_md5));
	}
	*/

	/// \todo Read "(signature)" file.
	// The strong digital signature is stored immediately after the archive, in the containing file
	const std::streampos position = startPosition() + boost::numeric_cast<std::streampos>(header.archiveSize);
	// jumps to the end of the archive
	if (endPosition(stream) > position && Mpq::hasStrongDigitalSignature(stream))
	{
		this->m_strongDigitalSignature.reset(new char[256]);
		size += strongDigitalSignature(stream, this->m_strongDigitalSignature.get());
	}

	return size;

}

/// @todo Write with format this->m_format!
std::streamsize Mpq::write(OutputStream &ostream) const throw (class Exception)
{
	return 0;
}

uint32_t Mpq::version() const
{
	switch (format())
	{
		case Format::Mpq1:
			return formatVersion1Identifier;
			
		case Format::Mpq2:
			return formatVersion2Identifier;
	}
	
	return 0;
}

const class MpqFile* Mpq::findFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale, BOOST_SCOPED_ENUM(MpqFile::Platform) platform) const throw (Exception)
{
	class Hash *hash = const_cast<class Mpq*>(this)->findHash(path, locale, platform);

	if (hash == 0 || hash->deleted() || hash->empty())
		return 0;

	if (hash->m_mpqFile->m_path != path) // path has not been set yet
	{
		hash->m_mpqFile->m_path = path;
		
		// if we have a sector offset table and file is encrypted we first need to know its path for proper decryption!
		// TODO READ SECTOR OFFSET TABLE AND DECRYPT IT NOW THAT WE HAVE ITS PATH
		if (hash->mpqFile()->hasSectorOffsetTable() && hash->mpqFile()->isEncrypted())
		{
			std::cerr << "File sector offset table is encrypted and we have no file path. Refreshing sector data." << std::endl;
			
			boost::interprocess::file_lock fileLock(hash->mpqFile()->mpq()->path().string().c_str());
			
			if (!fileLock.try_lock())
				throw Exception(boost::format(_("Warning: Couldn't lock MPQ file for refreshing sector data of file %1%.")) % path);
			
			ifstream istream(this->path(), std::ios::in | std::ios::binary);
				
			if (!istream)
				throw Exception();
			
			istream.seekg(startPosition());
			istream.seekg(hash->block()->blockOffset(), std::ios::cur);
			
			if (format() == Mpq::Format::Mpq2 && hash->block()->extendedBlockOffset() > 0)
				istream.seekg(hash->block()->extendedBlockOffset(), std::ios::cur);
				
			
			hash->mpqFile()->read(istream);
			fileLock.unlock();
		}
		
	}

	return const_cast<const class MpqFile*>(hash->m_mpqFile);
}

const class MpqFile* Mpq::findFile(const class MpqFile &mpqFile) const throw (Exception)
{
	return this->findFile(mpqFile.path(), mpqFile.locale(), mpqFile.platform());
}

class MpqFile* Mpq::addFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale, BOOST_SCOPED_ENUM(MpqFile::Platform) platform, const istream *istream, bool overwriteExisting, int32 reservedSpace) throw (class Exception)
{
	class MpqFile *mpqFile = const_cast<class MpqFile*>(this->findFile(path, locale, platform));

	// In the event of a collision (the home entry is occupied by another file), progressive overflow is used, and the file is placed in the next available hash table entry
	if (mpqFile != 0)
	{
		if (overwriteExisting)
			mpqFile->remove();
	}

	BlockPtr block;

	if (reservedSpace == 0)
	{
		block.reset(this->firstEmptyBlock());

		if (block == 0)
			block.reset(this->firstUnusedBlock());
	}
	else
	{
		// get block with the best size
		BOOST_FOREACH(Blocks::left_reference reference, this->m_blocks.left)
		{
			BlockPtr iteratedBlock = reference.second;
			
			if ((iteratedBlock->empty() || iteratedBlock->unused()) && iteratedBlock->m_blockSize >= reservedSpace && (block == 0 || iteratedBlock->m_blockSize - reservedSpace < block->m_blockSize - reservedSpace))
			{
				block = iteratedBlock;
				block->m_fileSize = reservedSpace;
			}
		}
	}

	bool newBlock = false;
	uint32 blockIndex = 0;

	// no block was found, create new one
	if (block == 0)
	{
		newBlock = true;
		block.reset(new Block(this));
		this->nextBlockOffsets(block->m_blockOffset, block->m_extendedBlockOffset);

		if (this->format() != Mpq::Format::Mpq2 && block->m_extendedBlockOffset > 0)
			throw Exception(_("Extended block offset can not be used in MPQ format 1."));

		block->m_blockSize = reservedSpace;
		block->m_fileSize = reservedSpace;
		block->m_flags = Block::Flags::IsFile; /// @todo Set right flags (user-defined).
		blockIndex = this->m_blocks.size() - 1;
		this->m_blocks.left.insert(std::make_pair(blockIndex, block));

	}

	class Hash *hash = this->firstDeletedHash();

	if (hash == 0)
		hash = this->firstEmptyHash();
	else
		hash->m_deleted = false;


	if (hash == 0)
	{
		if (newBlock)
		{
			this->m_blocks.left.erase(blockIndex);
			block.reset();
		}

		throw Exception(_("There isn't any hash table entry left."));
	}

	hash->m_block = block.get();
	hash->hashData().setFilePathHashA(HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameA));
	hash->hashData().setFilePathHashB(HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameB));
	hash->hashData().setLocale(MpqFile::localeToInt(locale));
	hash->hashData().setPlatform(MpqFile::platformToInt(platform));
	FilePtr file(new MpqFile(this, hash));
	hash->m_mpqFile = file.get();
	this->m_files.get<0>().push_back(file);

	/// @todo Write block table entry into file. This can take a lot of time since the whole MPQ data after the entry has to be written newly.
	if (newBlock)
		;

	/// @todo Write file data/free reserved space in MPQ.
	if (istream != 0)
		;

	// Add "(listfile)" file entry.
	if (this->containsListfileFile())
	{
		stringstream sstream(reinterpret_cast<const byte*>(path.string().c_str()));
		this->listfileFile()->appendData(sstream);
	}

	// Add "(attributes)" file entries.
	if (this->extendedAttributes() != Mpq::ExtendedAttributes::None && this->containsAttributesFile())
	{
		/// @todo Write meta data/extended attributes
		if (this->extendedAttributes() & Mpq::ExtendedAttributes::FileCrc32s)
			//block->m_crc32;
			;

		if (this->extendedAttributes() & Mpq::ExtendedAttributes::FileTimeStamps)
			block->setFileTime(time(0));

		if (this->extendedAttributes() & Mpq::ExtendedAttributes::FileMd5s)
			//block->m_md5 = ;
			;

		this->refreshAttributesFile();
	}

	// Refresh "(signature)" file.
	if (this->containsSignatureFile())
		this->refreshSignatureFile();

	return hash->m_mpqFile;
}

class MpqFile* Mpq::addFile(const MpqFile &mpqFile, bool addData, bool overwriteExisting) throw (class Exception)
{
	stringstream sstream;

	if (addData)
		mpqFile.writeData(sstream);

	return this->addFile(mpqFile.path(), mpqFile.locale(), mpqFile.platform(), &sstream, overwriteExisting, mpqFile.size());
}

bool Mpq::removeFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale, BOOST_SCOPED_ENUM(MpqFile::Platform) platform)
{
	class MpqFile *mpqFile = this->findFile(path, locale, platform);

	if (mpqFile == 0)
		return false;

	mpqFile->remove();

	return true;
}

bool Mpq::removeFile(const MpqFile &mpqFile)
{
	class MpqFile *searchedMpqFile = this->findFile(mpqFile);

	if (searchedMpqFile == 0)
		return false;

	searchedMpqFile->remove();

	return true;
}

bool Mpq::operator!() const
{
	return !this->isOpen();
}

class Mpq& Mpq::operator<<(const class MpqFile &mpqFile) throw (class Exception)
{
	// Copy all file data, existing files won't be overwritten.
	this->addFile(mpqFile, false);

	return *this;
}

class Mpq& Mpq::operator<<(const class Mpq &mpq) throw (class Exception)
{
	// Copy all file data of the other MPQ, existing files won't be overwritten.
	BOOST_FOREACH(const FilePtr mpqFile, mpq.files())
		this->addFile(*mpqFile.get(), false);

	return *this;
}

class Mpq& Mpq::operator>>(class Mpq &mpq) throw (class Exception)
{
	// Copy all file data into the other MPQ, existing files won't be overwritten.
	BOOST_FOREACH(const FilePtr mpqFile, this->files())
		mpq.addFile(*mpqFile.get(), false);

	return *this;
}

void Mpq::clear()
{
	this->m_strongDigitalSignature.reset();

	//BOOST_FOREACH(class Block *block, this->m_blocks)
	//	delete block;

	//BOOST_FOREACH(class Hash *hash, this->m_hashes)
	//	delete hash;

	//BOOST_FOREACH(class MpqFile *file, this->m_files)
	//	delete file;

	this->m_size = 0;
	this->m_path.clear();
	this->m_format = Mpq::Format::Mpq1;
	this->m_extendedAttributes = Mpq::ExtendedAttributes::None;
	this->m_sectorSize = 0;
	this->m_fileLock.unlock();
	this->m_isOpen = false;
}

/// @todo There seems to be some MPQ archives which do contain hash tables which do start with an empty entry. Therefore I commented checks for such tables.
class Hash* Mpq::findHash(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale, BOOST_SCOPED_ENUM(MpqFile::Platform) platform)
{
	if (this->m_hashes.get<uint32>().empty()) //|| this->m_hashes.front()->empty())
	{
		if (this->m_hashes.get<uint32>().empty())
			std::cout << "Hashes are empty." << std::endl;
		else if ((*this->m_hashes.get<uint32>().find(0))->empty())
			std::cout << "Front hash is empty (Block address " << (*this->m_hashes.get<uint32>().find(0))->block() << ")." << std::endl;

		return 0;
	}

	// Compute the hashes to compare the hash table entry against
	const uint32 nameHashA = HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameA);
	const uint32 nameHashB = HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameB);
	const int16 realLocale = MpqFile::localeToInt(locale);
	const int16 realPlatform = MpqFile::platformToInt(platform);
	
	HashData hashData(nameHashA, nameHashB, realLocale, realPlatform);
	std::cout << "Searching for hash " << path << " with value a " << nameHashA << " and value b " << nameHashB << " and locale " << locale << " and platform " << realPlatform << std::endl; // TEST
	//index<HashData>:
	Hashes::index_iterator<HashData>::type iterator = this->m_hashes.get<HashData>().find(hashData);
	
	if (iterator == this->m_hashes.get<HashData>().end() || (*iterator)->deleted())
		return 0;
	
	return iterator->get();
}

class Hash* Mpq::findHash(const Hash &hash)
{
	return this->findHash(hash.mpqFile()->path(), hash.mpqFile()->locale(), hash.mpqFile()->platform());
}

class MpqFile* Mpq::findFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale, BOOST_SCOPED_ENUM(MpqFile::Platform) platform) throw (Exception)
{
	return const_cast<class MpqFile*>(const_cast<const Mpq*>(this)->findFile(path, locale, platform));
}

class MpqFile* Mpq::findFile(const class MpqFile &mpqFile) throw (Exception)
{
	return const_cast<class MpqFile*>(const_cast<const Mpq*>(this)->findFile(mpqFile));
}

bool Mpq::removeFile(class MpqFile *&mpqFile)
{
	if (mpqFile->mpq() != this)
		return false;

	mpqFile->remove();
	delete mpqFile;
	mpqFile = 0;

	return true;
}

class MpqFile* Mpq::refreshAttributesFile()
{
	throw Exception(_("Mpq: refreshAttributesFile is not implemented yet!"));

	return 0;
}

class MpqFile* Mpq::refreshSignatureFile()
{
	throw Exception(_("Mpq: refreshSignatureFile is not implemented yet!"));

	return 0;
}

}

}
