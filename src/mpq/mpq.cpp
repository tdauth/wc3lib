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

const byte Mpq::identifier[4] = { 'M', 'P', 'Q',  0x1A };
const int16 Mpq::formatVersion1Identifier = 0x0000;
const int16 Mpq::formatVersion2Identifier = 0x0001;
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

Mpq::Mpq() : m_size(0), m_path(), m_format(Mpq::Format::Mpq1), m_sectorSize(0), m_strongDigitalSignature(0), m_isOpen(false), m_storeSectors(defaultStoreSectors)
{
}

Mpq::~Mpq()
{
	this->close();
}

std::streamsize Mpq::create(const boost::filesystem::path &path, bool overwriteExisting, std::streampos startPosition, BOOST_SCOPED_ENUM(Format) format, uint32 sectorSize) throw (class Exception)
{
	this->close();

	if (boost::filesystem::exists(path) && !overwriteExisting)
		throw Exception(boost::format(_("Unable to create file \"%1%\". File does already exist.")) % path.string());

	ofstream ofstream(path, std::ios::out | std::ios::binary);

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

std::streamsize Mpq::open(const boost::filesystem::path &path, const Listfile::Entries &listfileEntries) throw (class Exception)
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

	this->clear();
}

std::streamsize Mpq::read(InputStream &stream, const Listfile::Entries &listfileEntries) throw (class Exception)
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

	if (header.formatVersion == Mpq::formatVersion1Identifier)
		this->m_format = Mpq::Format::Mpq1;
	else if (header.formatVersion == Mpq::formatVersion2Identifier)
		this->m_format = Mpq::Format::Mpq2;
	else
		throw Exception(boost::format(_("Unknown MPQ format \"%1%\".")) % header.formatVersion);

	if (header.blockTableEntries > maxBlockId + 1)
		throw Exception(boost::format(_("Too many block table entries (%1%). Maximum allowed value is %2% since %3% and %4% are reserved for deleted and empty block indices.")) % header.blockTableEntries % maxBlockId % Hash::blockIndexDeleted % Hash::blockIndexEmpty);

	// Number of entries in the hash table. Must be a power of two, and must be less than 2^16 for the original MoPaQ format, or less than 2^20 for the Burning Crusade format.
	if (header.hashTableEntries % 2 != 0)
		std::cerr << boost::format(_("Warning: Hash table entries should be a power of two for MPQ file %1%.\nEntries: %2%.")) % this->path() % header.hashTableEntries << std::endl;

	if ((this->format() == Mpq::Format::Mpq1 && header.hashTableEntries >= pow(2, 16)) || (this->format() == Mpq::Format::Mpq2 && header.hashTableEntries >= pow(2, 20)))
		std::cerr << boost::format(_("Warning: There are too many MPQ hash table entries in MPQ file %1%.\nEntries: %2%.\nFor MPQ1 it must be less than 2^16 and for MPQ2 less than 2^20.")) % this->path() % header.hashTableEntries << std::endl;

	// According to the StormLib this value is sometimes changed by map creators to protect their maps. As in the StormLib this value is ignored.
	if (header.headerSize != sizeof(header))
		std::cerr << boost::format(_("Warning: MPQ header size is not equal to real header size.\nContained header size: %1%.\nReal header size: %2%.")) % header.headerSize % sizeof(header) << std::endl;

	if (header.archiveSize != this->size())
		std::cerr << boost::format(_("Warning: MPQ file size of MPQ file %1% is not equal to its internal header file size.\nFile size: %2%.\nInternal header file size: %3%.")) % this->path() % this->size()
		% header.archiveSize << std::endl;

	this->m_sectorSize = pow(2, header.sectorSizeShift) * 512;
	boost::scoped_ptr<ExtendedHeader> extendedHeader;

	if (this->format() == Mpq::Format::Mpq2)
	{
		extendedHeader.reset(new ExtendedHeader);
		wc3lib::read(stream, *extendedHeader, size);
	}

	uint64 offset = header.blockTableOffset;

	if (this->format() == Mpq::Format::Mpq2 && extendedHeader->blockTableOffsetHigh > 0)
		offset += uint64(extendedHeader->blockTableOffsetHigh) << 32;

	stream.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(offset));
	std::size_t encryptedBytesSize = header.blockTableEntries * sizeof(struct BlockTableEntry);
	boost::scoped_array<byte> encryptedBytes(new byte[encryptedBytesSize]);
	stream.read(encryptedBytes.get(), encryptedBytesSize);
	uint32 hashValue = HashString(Mpq::cryptTable(), "(block table)", HashType::FileKey);
	DecryptData(Mpq::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);
	arraystream sstream(encryptedBytes.get(), encryptedBytesSize);

	for (uint32 i = 0; i < header.blockTableEntries; ++i)
	{
		BlockPtr ptr(newBlock(i));
		size += ptr->read(sstream);
		this->m_blocks.insert(ptr);
	}

	/*
	read extended block table
	As of the Burning Crusade Friends and Family beta, this table is not encrypted.
	*/
	if (this->format() == Mpq::Format::Mpq2)
	{
		stream.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(extendedHeader->extendedBlockTableOffset));

		BOOST_FOREACH(BlockPtr block, this->m_blocks)
		{
			struct ExtendedBlockTableEntry extendedBlockTableEntry;
			wc3lib::read(stream, extendedBlockTableEntry, size);
			block->m_extendedBlockOffset = extendedBlockTableEntry.extendedBlockOffset;
		}
	}

	// read encrypted hash table
	offset = header.hashTableOffset;

	if (this->format() == Mpq::Format::Mpq2 && extendedHeader->hashTableOffsetHigh > 0)
		offset += uint64(extendedHeader->hashTableOffsetHigh) << 32;

	stream.seekg(this->startPosition() + boost::numeric_cast<std::streamoff>(offset));
	encryptedBytesSize = header.hashTableEntries * sizeof(struct HashTableEntry);
	encryptedBytes.reset(new byte[encryptedBytesSize]);
	stream.read(encryptedBytes.get(), encryptedBytesSize);
	hashValue = HashString(Mpq::cryptTable(), "(hash table)", HashType::FileKey);
	DecryptData(Mpq::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);
	sstream.close();
	sstream.open(encryptedBytes.get(), encryptedBytesSize);

	for (uint32 i = 0; i < header.hashTableEntries; ++i)
	{
		HashPtr hash(newHash(i));
		size += hash->read(sstream);
		this->m_hashes.insert(hash);
	}

	// file data / add file instances
	BOOST_FOREACH(HashPtr hash, this->m_hashes)
	{
		if (!hash->empty() && !hash->deleted())
		{
			FilePtr mpqFile;

			// usual file
			if (!containsListfileFile() && hash->hashData() == "(listfile)")
				mpqFile.reset(new Listfile(this, hash.get()));
			else if (!containsAttributesFile() && hash->hashData() == "(attributes)")
				mpqFile.reset(new Attributes(this, hash.get(), Attributes::ExtendedAttributes::None));
			else if (!containsSignatureFile() && hash->hashData() == "(signature)")
				mpqFile.reset(new Signature(this, hash.get()));
			else
				mpqFile.reset(newFile(hash.get()));

			//mpqFile->m_path = // path can only be set if there is a listfile file or if we're able to convert its hash into file path
			this->m_files.get<0>().push_back(mpqFile);

			if (storeSectors())
				size += mpqFile->read(stream);

			hash->block()->m_file = mpqFile.get();
		}
	}

	// if custom entries were specified we do NOT use internal "(listfile)" file for path detection!
	Listfile::Entries entries;

	if (listfileEntries.empty())
	{
		Listfile *listfileFile = this->listfileFile();

		if (listfileFile != 0)
			entries = listfileFile->entries();
	}

	// read listfile file and create path entries
	BOOST_FOREACH(Listfile::Entries::const_reference path, listfileEntries.empty() ? entries : listfileEntries)
	{
		if (!path.empty()) // ignore empty entries
			this->findFile(path);
	}

	// refresh extended attributes if found (we should have all file instances before!)
	if (containsAttributesFile())
		attributesFile()->readData();

	// The strong digital signature is stored immediately after the archive, in the containing file
	const std::streampos position = startPosition() + boost::numeric_cast<std::streampos>(header.archiveSize);
	// jumps to the end of the archive
	if (endPosition(stream) > position && Mpq::hasStrongDigitalSignature(stream))
	{
		m_strongDigitalSignaturePosition = stream.tellg();
		size += strongDigitalSignature(stream, this->m_strongDigitalSignature);
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

Mpq::FilePtr Mpq::addFile(const boost::filesystem::path &path, const byte *buffer, std::size_t bufferSize, bool overwriteExisting, BOOST_SCOPED_ENUM(MpqFile::Locale) locale, BOOST_SCOPED_ENUM(MpqFile::Platform) platform) throw (class Exception)
{
	MpqFile *mpqFile = this->findFile(HashData(path, locale, platform));

	// In the event of a collision (the home entry is occupied by another file), progressive overflow is used, and the file is placed in the next available hash table entry
	if (mpqFile != 0)
	{
		if (overwriteExisting)
			mpqFile->remove();
	}

	BlockPtr block;

	/*
	if (reservedSpace == 0)
	{
		block.reset(this->firstEmptyBlock());

		if (block == 0)
			block.reset(this->firstUnusedBlock());
	}
	*/
	/*
	else
	{
		// get block with the best size
		BOOST_FOREACH(BlockPtr reference, this->m_blocks)
		{
			if ((reference->empty() || reference->unused()) && reference->m_blockSize >= reservedSpace && (block == 0 || reference->m_blockSize - reservedSpace < block->m_blockSize - reservedSpace))
			{
				block = reference;
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
		blockIndex = this->m_blocks.size() - 1;
		block.reset(this->newBlock(blockIndex));
		this->nextBlockOffsets(block->m_blockOffset, block->m_extendedBlockOffset);

		if (this->format() != Mpq::Format::Mpq2 && block->m_extendedBlockOffset > 0)
			throw Exception(_("Extended block offset can not be used in MPQ format 1."));

		block->m_blockSize = reservedSpace;
		block->m_fileSize = reservedSpace;
		block->m_flags = Block::Flags::IsFile; /// @todo Set right flags (user-defined).
		this->m_blocks.insert(block);

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
			Blocks::iterator iterator = m_blocks.find(blockIndex);
			this->m_blocks.erase(iterator);
			block.reset();
		}

		throw Exception(_("There isn't any hash table entry left."));
	}

	hash->m_block = block.get();
	hash->hashData().setFilePathHashA(HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameA));
	hash->hashData().setFilePathHashB(HashString(Mpq::cryptTable(), path.string().c_str(), HashType::NameB));
	hash->hashData().setLocale(MpqFile::localeToInt(locale));
	hash->hashData().setPlatform(MpqFile::platformToInt(platform));
	FilePtr file(newFile(hash));
	hash->m_mpqFile = file.get();
	this->m_files.get<0>().push_back(file);

	/// @todo Write block table entry into file. This can take a lot of time since the whole MPQ data after the entry has to be written newly.
	if (newBlock)
		;

	/// @todo Write file data/free reserved space in MPQ.
	if (istream != 0)
		;
	*/

	// TODO refreshing all stuff can take huge amount of performance -> should be optional!
	/*

	// Add "(listfile)" file entry.
	if (this->containsListfileFile())
	{
		this->listfileFile()->files().insert(hash->m_mpqFile);
		this->listfileFile()->writeData();
	}

	// Add "(attributes)" file entries.
	if (this->containsAttributesFile())
	{
		this->attributesFile()->refreshFile(hash->m_mpqFile);
		this->attributesFile()->writeData();
	}

	// Refresh "(signature)" file.
	if (this->containsSignatureFile())
		; //this->refreshSignatureFile();

	return hash->m_mpqFile;
	*/
	throw Exception(_("Not implemented yet!"));
}

Mpq::FilePtr Mpq::addFile(const MpqFile &mpqFile, bool addData, bool overwriteExisting) throw (class Exception)
{
	throw Exception(_("Not implemented yet!"));
	arraystream stream;

	if (addData)
		mpqFile.writeData(stream);

	//return this->addFile(mpqFile.path(), mpqFile.locale(), mpqFile.platform(), &stream, overwriteExisting, mpqFile.size());
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

class MpqFile* Mpq::newFile(class Hash *hash) throw ()
{
	try
	{
		return new MpqFile(this, hash);
	}
	catch (std::bad_alloc &)
	{
		return 0;
	}
}

class Hash* Mpq::newHash(uint32 index) throw ()
{
	try
	{
		return new Hash(this, index);
	}
	catch (std::bad_alloc &)
	{
		return 0;
	}
}

class Block* Mpq::newBlock(uint32 index) throw ()
{
	try
	{
		return new Block(this, index);
	}
	catch (std::bad_alloc &)
	{
		return 0;
	}
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
	this->m_sectorSize = 0;
	this->m_fileLock.unlock();
	this->m_isOpen = false;
}

Mpq::HashPtr Mpq::findHash(const HashData &hashData)
{
	Hashes::index_iterator<HashData>::type iterator = this->hashes().get<HashData>().find(hashData);

	if (iterator == this->hashes().get<HashData>().end())
		return HashPtr();

	return *iterator;
}

Mpq::HashPtr Mpq::findHash(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale, BOOST_SCOPED_ENUM(MpqFile::Platform) platform)
{
	if (this->hashes().get<uint32>().empty())
		return HashPtr();

	HashPtr result = findHash(HashData(path, locale, platform));

	if (result.get() != 0 && !result->empty() && !result->deleted() && result->block() != 0 && result->block()->file() != 0 && result->block()->file()->path().empty())
	{
		result->block()->file()->m_path = path;

		// if we have a sector offset table and file is encrypted we first need to know its path for proper decryption!
		// TODO READ SECTOR OFFSET TABLE AND DECRYPT IT NOW THAT WE HAVE ITS PATH
		if (result->block()->file()->hasSectorOffsetTable() && result->block()->file()->isEncrypted() && storeSectors())
			result->block()->file()->read();
	}

	return result;
}

MpqFile* Mpq::findFile(const HashData &hashData)
{
	HashPtr hash = this->findHash(hashData);

	// hash->m_mpqFile == 0 could happen in early stage when loading all hash entries
	if (hash.get() == 0 || hash->deleted() || hash->empty())
		return 0;

	return hash->block()->file();
}

const MpqFile* Mpq::findFile(const HashData &hashData) const
{
	return const_cast<Mpq*>(this)->findFile(hashData);
}

// NOTE don't ever just call findFile(HashData(path, locale, platform) since it wouldn't update the path as expected
MpqFile* Mpq::findFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale, BOOST_SCOPED_ENUM(MpqFile::Platform) platform)
{
	HashPtr hash = this->findHash(path, locale, platform);

	// hash->m_mpqFile == 0 could happen in early stage when loading all hash entries
	if (hash.get() == 0 || hash->deleted() || hash->empty())
		return 0;

	return hash->block()->file();
}

const MpqFile* Mpq::findFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale, BOOST_SCOPED_ENUM(MpqFile::Platform) platform) const
{
	return const_cast<Mpq*>(this)->findFile(path, locale, platform);
}

#ifdef USE_ENCRYPTION
bool Mpq::check(const CryptoPP::RSA::PrivateKey &strongPrivateKey, const CryptoPP::RSA::PrivateKey &weakPrivateKey) const
{
	if (!checkStrong(strongPrivateKey) || !this->signatureFile()->check(weakPrivateKey))
		return false;

	return true;
}

void Mpq::sign(const CryptoPP::RSA::PublicKey &strongPublicKey, const CryptoPP::RSA::PublicKey &weakPublicKey)
{
	if (hasStrongDigitalSignature())
		signStrong(strongPublicKey);

	if (containsSignatureFile())
		this->signatureFile()->sign(weakPublicKey);
}

void Mpq::digest(Mpq::SHA1Digest &digest) const
{
	// The entire archive (ArchiveSize bytes, starting at ArchiveOffset in the containing file) is hashed as a single block (there is one known exception to that algorithm, see below).
	if (!const_cast<Mpq*>(this)->m_fileLock.try_lock())
		throw Exception();

	boost::filesystem::ifstream ifstream(path(), std::ios::in | std::ios::binary);
	ifstream.seekg(startPosition());
	std::streamsize size = 0;
	struct Header header;
	wc3lib::read(ifstream, header, size);
	/*
	 * NOTE
	 * header.archiveSize
	 * This field is deprecated in the Burning Crusade MoPaQ format, and the size
	 * of the archive is calculated as the size from the beginning of the archive to the end of the
	 * hash table, block table, or extended block table (whichever is largest).
	 * TODO improve archive allocation/size calculation
	 * TODO probably it's more usable to calculate the digest progressively instead of allocating the whole archive on heap. Use sha.OptimalBlockSize() to get optimal block size and allocate single blocks etc. (checkout Crypto++ documentation).
	 */
	boost::scoped_array<byte> data(new byte[header.archiveSize]);
	memcpy(data.get(), &header, sizeof(header));
	wc3lib::read(ifstream, data[sizeof(header)], size, header.archiveSize - sizeof(header));
	ifstream.close();
	const_cast<Mpq*>(this)->m_fileLock.unlock();
	CryptoPP::SHA1 sha;
	digest.reset(new unsigned char[CryptoPP::SHA1::DIGESTSIZE]);
	sha.CalculateDigest(digest.get(), (unsigned char*)data.get(), header.archiveSize);
}

void Mpq::storedDigest(Mpq::SHA1Digest &digest, const CryptoPP::RSA::PrivateKey &privateKey) const
{
	// decrypting digest
	CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(privateKey);
	CryptoPP::RandomNumberGenerator rng;
	digest.reset(new unsigned char[CryptoPP::SHA1::DIGESTSIZE]);
	decryptor.Decrypt(rng, (unsigned char*)strongDigitalSignature().get(), strongDigitalSignatureSize, digest.get());
}

bool Mpq::checkStrong(const CryptoPP::RSA::PrivateKey &privateKey) const
{
	if (hasStrongDigitalSignature())
	{
		Mpq::SHA1Digest digest;
		this->digest(digest);
		Mpq::SHA1Digest storedDigest;
		this->storedDigest(storedDigest, privateKey);

		if (memcmp(digest.get(), storedDigest.get(), CryptoPP::SHA1::DIGESTSIZE) != 0)
			return false;
	}

	return true;
}

void Mpq::signStrong(Mpq::StrongDigitalSignature &signature, const CryptoPP::RSA::PublicKey &publicKey) const
{
	SHA1Digest digest;
	this->digest(digest);
	// encrypting digest
	CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
	CryptoPP::RandomNumberGenerator rng;
	signature.reset(new byte[strongDigitalSignatureSize]);
	encryptor.Encrypt(rng, digest.get(), CryptoPP::SHA1::DIGESTSIZE, (unsigned char*)signature.get());
	/// TODO set before parameters (2048 RSA key) or provide key pair generation functionality in algorithm.hpp?
}

std::streamsize Mpq::signStrong(const CryptoPP::RSA::PublicKey &publicKey)
{
	Mpq::StrongDigitalSignature signature;
	signStrong(signature, publicKey);

	// sync with archive
	if (!this->m_fileLock.try_lock())
		throw Exception();

	boost::filesystem::ofstream ofstream(path(), std::ios::out | std::ios::binary);
	ofstream.seekp(m_strongDigitalSignaturePosition);
	ofstream.seekp(sizeof(uint32), std::ios::cur); // skip header
	std::streamsize size = 0;
	wc3lib::write(ofstream, signature[0], size, strongDigitalSignatureSize);
	ofstream.close();
	this->m_fileLock.unlock();

	// refresh in heap
	m_strongDigitalSignature.swap(signature);

	return size;
}
#endif

}

}
