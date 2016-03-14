#define BOOST_TEST_MODULE ArchiveTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

#include "../archive.hpp"
#include "../algorithm.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::mpq;

/**
 * Constructs a basic MPQ archive with one block and one hash entry (basically one file) and tests the basic IO methods of the class Archive.
 */
BOOST_AUTO_TEST_CASE(WriteReadHeader)
{
	const std::size_t fileSize = 5;

	/*
	 * Initialize a header with one block table entry and one hash table entry.
	 * The block has a file size of 5 bytes.
	 */
	Header header;
	memcpy(header.magic, Archive::identifier, 4);
	header.headerSize = sizeof(header);
	header.archiveSize = sizeof(header) + sizeof(BlockTableEntry) + sizeof(HashTableEntry) + fileSize;
	header.formatVersion = static_cast<uint16>(Archive::Format::Mpq1);
	header.sectorSizeShift = 4096;
	header.hashTableOffset = sizeof(header) + sizeof(BlockTableEntry);
	header.blockTableOffset = sizeof(header);
	header.hashTableEntries = 1;
	header.blockTableEntries = 1;

	/*
	 * The block starts after the hash table entries.
	 */
	BlockTableEntry blockTableEntry;
	blockTableEntry.blockOffset = sizeof(header) + sizeof(BlockTableEntry) + sizeof(HashTableEntry);
	blockTableEntry.blockSize = fileSize;
	blockTableEntry.fileSize = fileSize;
	blockTableEntry.flags = static_cast<uint32>(Block::Flags::IsFile);

	/*
	 * The hash table entry belongs to the only block table entry.
	 */
	HashTableEntry hashTableEntry;
	hashTableEntry.filePathHashA = HashString(Archive::cryptTable(), "test.txt", HashType::NameA);
	hashTableEntry.filePathHashB = HashString(Archive::cryptTable(), "test.txt", HashType::NameB);
	hashTableEntry.locale = static_cast<uint16>(File::Locale::Neutral);
	hashTableEntry.platform = static_cast<uint16>(File::Platform::Default);
	hashTableEntry.fileBlockIndex = 0;


	/*
	 * The custom MPQ archive is written into the file "tmp.mpq".
	 */
	ofstream out("tmp.mpq");
	boost::scoped_array<char> empty(new char[10]);
	memset((void*)empty.get(), 0, 10);
	out.write((const char*)empty.get(), 10); // skip 10 bytes and leave the file empty here
	// write the header structure which is not encrypted
	out.write((const char*)&header, sizeof(header));

	// write encrypted block table
	const std::size_t encryptedBytesSize = sizeof(struct BlockTableEntry);
	boost::scoped_array<byte> encryptedBytes(new byte[encryptedBytesSize]);
	memcpy(encryptedBytes.get(), &blockTableEntry, encryptedBytesSize);
	const uint32 hashValue = HashString(Archive::cryptTable(), "(block table)", HashType::FileKey);
	EncryptData(Archive::cryptTable(), encryptedBytes.get(), encryptedBytesSize, hashValue);
	out.write(encryptedBytes.get(), encryptedBytesSize);

	// write encrypted hash table
	const std::size_t encryptedBytesSizeHash = sizeof(struct HashTableEntry);
	boost::scoped_array<byte> encryptedBytesHash(new byte[encryptedBytesSizeHash]);
	memcpy(encryptedBytesHash.get(), &hashTableEntry, encryptedBytesSizeHash);
	const uint32 hashValueHash = HashString(Archive::cryptTable(), "(hash table)", HashType::FileKey);
	EncryptData(Archive::cryptTable(), encryptedBytesHash.get(), encryptedBytesSizeHash, hashValueHash);
	out.write(encryptedBytesHash.get(), encryptedBytesSizeHash);

	// write the file block sector
	boost::scoped_array<char> fileData(new byte[fileSize]);
	memset(fileData.get(), 0, fileSize);
	out.write(fileData.get(), fileSize);
	out.close();

	mpq::Archive archive;

	// check empty initialized values
	BOOST_REQUIRE(archive.startPosition() == 0);
	BOOST_REQUIRE(archive.blockTableOffset() == 0);
	BOOST_REQUIRE(archive.extendedBlockTableOffset() == 0);
	BOOST_REQUIRE(archive.hashTableOffset() == 0);
	BOOST_REQUIRE(archive.strongDigitalSignaturePosition() == 0);
	BOOST_REQUIRE(archive.blocks().empty());
	BOOST_REQUIRE(archive.hashes().empty());
	BOOST_REQUIRE(archive.sectorSize() == 0);
	BOOST_REQUIRE(!archive.containsListfileFile());
	BOOST_REQUIRE(!archive.containsAttributesFile());
	BOOST_REQUIRE(!archive.containsSignatureFile());
	BOOST_REQUIRE(!archive.hasStrongDigitalSignature());
	BOOST_REQUIRE(!archive.isOpen());

	// load the header structure from the archive
	Header loadedHeader;

	ifstream in("tmp.mpq");
	BOOST_REQUIRE(in);
	uint64 startPosition = 0;
	std::streamsize size = 0;
	BOOST_REQUIRE(archive.readHeader(in, loadedHeader, startPosition, size));
	BOOST_REQUIRE(startPosition == 10);
	BOOST_REQUIRE(size == sizeof(header));
	// compare everything with header
	BOOST_REQUIRE(memcmp(&loadedHeader, &header, sizeof(loadedHeader)) == 0);

	// fix archive's offsets for the usage of the read methods since they use these values to seek to the positions in the stream
	archive.setStartPosition(startPosition);
	archive.setBlockTableOffset(loadedHeader.blockTableOffset);
	// TODO extended
	archive.setHashTableOffset(loadedHeader.hashTableOffset);

	// compare everything with block entry
	size = 0;
	BOOST_REQUIRE(archive.readBlockTable(in, 1, size));
	BOOST_REQUIRE(size == sizeof(blockTableEntry));
	BOOST_REQUIRE(archive.blocks().size() == 1);
	const Block &loadedBlock = archive.blocks()[0];
	const BlockTableEntry loadedBlockTableEntry = loadedBlock.toBlockTableEntry();

	std::cerr << "Header block table offset: " << header.blockTableOffset << std::endl;
	std::cerr << "Loaded header block table offset: " << loadedHeader.blockTableOffset << std::endl;
	std::cerr << "Block table offset: " << archive.blockTableOffset() << std::endl;
	std::cerr << "Non converted offset: " << archive.blocks()[0].blockOffset() << std::endl;
	std::cerr << "Loaded offset: " << loadedBlockTableEntry.blockOffset << std::endl;
	std::cerr << "Original offset: " << blockTableEntry.blockOffset << std::endl;
	BOOST_REQUIRE(loadedBlockTableEntry.blockOffset == blockTableEntry.blockOffset);
	BOOST_REQUIRE(memcmp(&loadedBlockTableEntry, &blockTableEntry, sizeof(blockTableEntry)) == 0);

	// check methods of class Block
	BOOST_REQUIRE(!loadedBlock.empty());
	// TODO check more

	// compare everything with hash entry
	size = 0;
	BOOST_REQUIRE(archive.readHashTable(in, 1, size));
	BOOST_REQUIRE(size == sizeof(hashTableEntry));
	BOOST_REQUIRE(archive.hashes().size() == 1);
	const HashTableEntry loadedHashTableEntry = archive.hashes().begin()->second->cHashData().toEntry();
	std::cerr << "Loaded hash file block index: " << loadedHashTableEntry.fileBlockIndex << std::endl;
	BOOST_REQUIRE(memcmp(&loadedHashTableEntry, &hashTableEntry, sizeof(hashTableEntry)) == 0);
}
