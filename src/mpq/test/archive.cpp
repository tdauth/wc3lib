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
	Header header;
	memcpy(header.magic, Archive::identifier, 4);
	header.headerSize = sizeof(header);
	header.archiveSize = sizeof(header) + sizeof(BlockTableEntry) + sizeof(HashTableEntry) + 5;
	header.formatVersion = static_cast<uint16>(Archive::Format::Mpq1);
	header.sectorSizeShift = 4096;
	header.hashTableOffset = sizeof(header) + sizeof(BlockTableEntry);
	header.blockTableOffset = sizeof(header);
	header.hashTableEntries = 1;
	header.blockTableEntries = 1;

	BlockTableEntry blockTableEntry;
	blockTableEntry.blockOffset = sizeof(header) + sizeof(BlockTableEntry) + sizeof(HashTableEntry);
	blockTableEntry.blockSize = 5;
	blockTableEntry.fileSize = 5;
	blockTableEntry.flags = static_cast<uint32>(Block::Flags::IsFile);

	HashTableEntry hashTableEntry;
	hashTableEntry.filePathHashA = HashString(Archive::cryptTable(), "test.txt", HashType::NameA);
	hashTableEntry.filePathHashB = HashString(Archive::cryptTable(), "test.txt", HashType::NameB);
	hashTableEntry.locale = static_cast<uint16>(File::Locale::Neutral);
	hashTableEntry.platform = static_cast<uint16>(File::Platform::Default);
	hashTableEntry.fileBlockIndex = 0;


	ofstream out("tmp.mpq");
	boost::scoped_array<char> empty(new char[10]);
	memset((void*)empty.get(), 5, 10);
	out.write((const char*)empty.get(), 10); // skip 10 bytes and leave the file empty here
	out.write((const char*)&header, sizeof(header));
	out.write((const char*)&blockTableEntry, sizeof(blockTableEntry));
	out.write((const char*)&hashTableEntry, sizeof(hashTableEntry));
	// TODO write the file block sector
	out.close();

	mpq::Archive archive;

	BOOST_REQUIRE(archive.blocks().empty());
	BOOST_REQUIRE(archive.hashes().empty());

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

	// compare everything with block entry
	size = 0;
	BOOST_REQUIRE(archive.readBlockTable(in, 1, size));
	BOOST_REQUIRE(size == sizeof(blockTableEntry));
	BOOST_REQUIRE(archive.blocks().size() == 1);
	BOOST_REQUIRE(memcmp(&archive.blocks()[0], &blockTableEntry, sizeof(blockTableEntry)) == 0);

	// compare everything with hash entry
	size = 0;
	BOOST_REQUIRE(archive.readHashTable(in, 1, size));
	BOOST_REQUIRE(size == sizeof(hashTableEntry));
	BOOST_REQUIRE(archive.hashes().size() == 1);
	BOOST_REQUIRE(memcmp(&archive.hashes().begin()->second, &hashTableEntry, sizeof(hashTableEntry)) == 0);
}
