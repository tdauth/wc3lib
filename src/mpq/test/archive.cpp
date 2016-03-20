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

#ifndef WC3_DIR
#error Define WC3_DIR.
#endif

using namespace wc3lib;
using namespace wc3lib::mpq;

/**
 * Constructs a basic MPQ archive with one block and one hash entry (basically one file) and tests the basic IO methods of the class Archive.
 */
BOOST_AUTO_TEST_CASE(WriteReadHeaderAndTables)
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
	// TODO extended block table offset?
	archive.setHashTableOffset(loadedHeader.hashTableOffset);

	// compare everything with block entry
	size = 0;
	BOOST_REQUIRE(archive.readBlockTable(in, 1, size));
	BOOST_REQUIRE(size == sizeof(blockTableEntry));
	BOOST_REQUIRE(archive.blocks().size() == 1);
	const Block &loadedBlock = archive.blocks()[0];
	const BlockTableEntry loadedBlockTableEntry = loadedBlock.toBlockTableEntry();
	BOOST_REQUIRE(loadedBlockTableEntry.blockOffset == blockTableEntry.blockOffset);
	// the loaded block table entry has to be the exact same as the written
	BOOST_REQUIRE(memcmp(&loadedBlockTableEntry, &blockTableEntry, sizeof(blockTableEntry)) == 0);

	// check methods of class Block
	BOOST_REQUIRE(!loadedBlock.empty());
	// TODO check more

	// compare everything with hash entry
	size = 0;
	BOOST_REQUIRE(archive.readHashTable(in, 1, size));
	BOOST_REQUIRE(size == sizeof(hashTableEntry));
	BOOST_REQUIRE(archive.hashes().size() == 1);
	const Hash &loadedHash = *archive.hashes().begin()->second;
	const HashTableEntry loadedHashTableEntry = loadedHash.toHashTableEntry();
	// the loaded hash table entry has to be the exact same as the written
	BOOST_REQUIRE(memcmp(&loadedHashTableEntry, &hashTableEntry, sizeof(hashTableEntry)) == 0);

	// check methods of class Hash
	BOOST_REQUIRE(!loadedHash.empty());
	BOOST_REQUIRE(!loadedHash.deleted());
	BOOST_REQUIRE(loadedHash.block() == &loadedBlock);
	BOOST_REQUIRE(loadedHash.index() == 0);
	// TODO check more
}

BOOST_AUTO_TEST_CASE(ReadMPQMasterArchive)
{
	mpq::Archive archive;
	BOOST_REQUIRE(archive.open("mpqmaster_mpq1_no_extended_attributes.mpq") > 0);
	// TODO find file
}

BOOST_AUTO_TEST_CASE(ReadWar3Archive)
{
	boost::filesystem::path wc3DirPath = WC3_DIR;
	std::cerr << "wc3 dir" << wc3DirPath << std::endl;
	BOOST_REQUIRE(boost::filesystem::exists(wc3DirPath));
	BOOST_REQUIRE(boost::filesystem::is_directory(wc3DirPath));

	const boost::filesystem::path war3Path = wc3DirPath / "war3.mpq";

	BOOST_REQUIRE(boost::filesystem::exists(war3Path));

	Archive war3Archive;
	BOOST_REQUIRE(war3Archive.open(war3Path) > 0);
	BOOST_REQUIRE(war3Archive.format() == Archive::Format::Mpq1);
	BOOST_REQUIRE(war3Archive.containsListfileFile());

	const File listfileFile = war3Archive.findFile("(listfile)");
	BOOST_REQUIRE(listfileFile.isValid());
}

BOOST_AUTO_TEST_CASE(ReadWar3XArchive)
{
	boost::filesystem::path wc3DirPath = WC3_DIR;
	std::cerr << "wc3 dir" << wc3DirPath << std::endl;
	BOOST_REQUIRE(boost::filesystem::exists(wc3DirPath));
	BOOST_REQUIRE(boost::filesystem::is_directory(wc3DirPath));

	const boost::filesystem::path war3XPath = wc3DirPath / "war3x.mpq";

	BOOST_REQUIRE(boost::filesystem::exists(war3XPath));

	Archive war3XArchive;
	BOOST_REQUIRE(war3XArchive.open(war3XPath) > 0);
	BOOST_REQUIRE(war3XArchive.format() == Archive::Format::Mpq1);
	BOOST_REQUIRE(war3XArchive.containsListfileFile());

	const File listfileFile = war3XArchive.findFile("(listfile)");
	BOOST_REQUIRE(listfileFile.isValid());
}

BOOST_AUTO_TEST_CASE(CreateEmptyArchive)
{
	Archive archive;

	BOOST_REQUIRE(archive.create("emptyarchive.mpq", 1, 1) > 0);

	BOOST_CHECK(archive.isOpen());

	BOOST_CHECK_EQUAL(archive.blocks().size(), 1);
	BOOST_CHECK_EQUAL(archive.hashes().size(), 1);

	BOOST_CHECK(boost::filesystem::is_regular("emptyarchive.mpq"));

	BOOST_CHECK_EQUAL(archive.size(), sizeof(Header) + sizeof(BlockTableEntry) + sizeof(HashTableEntry)); // size includes the size of the header
}

BOOST_AUTO_TEST_CASE(AddFileUncompressed)
{
	if (boost::filesystem::exists("addfile.mpq"))
	{
		boost::filesystem::remove("addfile.mpq");
	}

	Archive archive;
	archive.create("addfile.mpq", 1, 1);

	BOOST_REQUIRE(archive.isOpen());
	BOOST_REQUIRE_EQUAL(archive.blocks().size(), 1);
	BOOST_REQUIRE_EQUAL(archive.hashes().size(), 1);

	string data = "Hello World!";

	File file = archive.addFile("test.txt", data.c_str(), data.size());

	BOOST_REQUIRE(file.isValid());

	archive.close();

	// make sure the archive has been written
	BOOST_REQUIRE(boost::filesystem::exists("addfile.mpq"));

	archive.open("addfile.mpq");

	BOOST_REQUIRE(archive.isOpen());
	BOOST_REQUIRE_EQUAL(archive.blocks().size(), 1);
	BOOST_REQUIRE_EQUAL(archive.hashes().size(), 1);

	file = archive.findFile("test.txt");

	BOOST_REQUIRE(file.isValid());

	stringstream sstream;
	file.decompress(sstream);
	data = sstream.str();

	BOOST_REQUIRE_EQUAL(data, "Hello World!");
}

BOOST_AUTO_TEST_CASE(AddFileUncompressedEncrypted)
{
	if (boost::filesystem::exists("addfile.mpq"))
	{
		boost::filesystem::remove("addfile.mpq");
	}

	Archive archive;
	archive.create("addfile.mpq", 1, 1);

	BOOST_REQUIRE(archive.isOpen());
	BOOST_REQUIRE_EQUAL(archive.blocks().size(), 1);
	BOOST_REQUIRE_EQUAL(archive.hashes().size(), 1);

	string data = "Hello World!";

	File file = archive.addFile("test.txt", data.c_str(), data.size(), Sector::Compression::Uncompressed, Block::Flags::IsEncrypted);

	BOOST_REQUIRE(file.isValid());

	archive.close();

	// make sure the archive has been written
	BOOST_REQUIRE(boost::filesystem::exists("addfile.mpq"));

	archive.open("addfile.mpq");

	BOOST_REQUIRE(archive.isOpen());
	BOOST_REQUIRE_EQUAL(archive.blocks().size(), 1);
	BOOST_REQUIRE_EQUAL(archive.hashes().size(), 1);

	file = archive.findFile("test.txt");

	BOOST_REQUIRE(file.isValid());

	stringstream sstream;
	file.decompress(sstream);
	data = sstream.str();

	BOOST_REQUIRE_EQUAL(data, "Hello World!");
}

BOOST_AUTO_TEST_CASE(RemoveFile)
{
	if (boost::filesystem::exists("removefile.mpq"))
	{
		boost::filesystem::remove("removefile.mpq");
	}

	Archive archive;
	archive.create("removefile.mpq", 1, 1);

	BOOST_REQUIRE(archive.isOpen());
	BOOST_REQUIRE_EQUAL(archive.blocks().size(), 1);
	BOOST_REQUIRE_EQUAL(archive.hashes().size(), 1);

	string data = "Hello World!";

	File file = archive.addFile("test.txt", data.c_str(), data.size());

	BOOST_REQUIRE(file.isValid());

	archive.close();

	// make sure the archive has been written
	BOOST_REQUIRE(boost::filesystem::exists("removefile.mpq"));

	archive.open("removefile.mpq");

	BOOST_REQUIRE(archive.isOpen());
	BOOST_REQUIRE_EQUAL(archive.blocks().size(), 1);
	BOOST_REQUIRE_EQUAL(archive.hashes().size(), 1);

	file = archive.findFile("test.txt");

	BOOST_REQUIRE(file.isValid());

	stringstream sstream;
	file.decompress(sstream);
	data = sstream.str();

	BOOST_REQUIRE_EQUAL(data, "Hello World!");

	BOOST_REQUIRE(archive.removeFile(file));

	// TODO check everything
}
