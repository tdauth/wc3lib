#define BOOST_TEST_MODULE AttributesTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/scoped_ptr.hpp>

#include "../archive.hpp"
#include "../attributes.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

/*
 * Read file "ladik_mpq1_all_extended_attributes.txt" for information about the content of the archive.
 * The file contains information displayed by Ladiks MPQ Editor.
 */
BOOST_AUTO_TEST_CASE(LadikMpq1AllExtendedAttributes)
{
	mpq::Archive archive;
	bool success = true;

	try
	{
		archive.open("ladik_mpq1_all_extended_attributes.mpq");
	}
	catch (Exception &e)
	{
		success = false;
	}

	BOOST_REQUIRE(success);
	BOOST_REQUIRE(archive.format() == mpq::Archive::Format::Mpq1);
	// TODO wrong size of hash table?
	//std::cerr << "Hashes size: " << archive.hashes().size() << std::endl;
	BOOST_REQUIRE(archive.hashes().size() == 4096);
	BOOST_REQUIRE(archive.blocks().size() == 2);
	BOOST_REQUIRE(archive.sectorSize() == 4096);

	mpq::File testfile = archive.findFile("testfile.txt");
	BOOST_REQUIRE(testfile.isValid());
	BOOST_REQUIRE(testfile.compressedSize() == 12);
	BOOST_REQUIRE(testfile.locale() == mpq::File::Locale::Neutral);
	// TODO test file hash and block data

	success = true;
	stringstream data;

	try
	{
		testfile.writeData(data);
	}
	catch (Exception &e)
	{
		std::cerr << e.what() << std::endl;
		success = false;
	}

	BOOST_REQUIRE(success);
	const string dataString = data.str();
	std::cerr << "Data string:" << dataString << std::endl;
	std::cerr << "Data string size:" << dataString.size() << std::endl;
	BOOST_REQUIRE(dataString == "Test");

	BOOST_REQUIRE(!archive.containsListfileFile());
	BOOST_REQUIRE(!archive.containsSignatureFile());
	BOOST_REQUIRE(archive.containsAttributesFile());

	mpq::Attributes attributes = archive.attributesFile();
	BOOST_REQUIRE(attributes.isValid());

	int32 version = 0;
	mpq::Attributes::ExtendedAttributes extendedAttributes = mpq::Attributes::ExtendedAttributes::None;
	mpq::Attributes::Crc32s crcs;
	mpq::Attributes::FileTimes fileTimes;
	mpq::Attributes::Md5s md5s;

	try
	{
		attributes.attributes(version, extendedAttributes, crcs, fileTimes, md5s);
	}
	catch (Exception &e)
	{
		success = false;
	}

	BOOST_REQUIRE(success);

	BOOST_REQUIRE(extendedAttributes & mpq::Attributes::ExtendedAttributes::FileCrc32s);
	BOOST_REQUIRE(extendedAttributes & mpq::Attributes::ExtendedAttributes::FileTimeStamps);
	BOOST_REQUIRE(extendedAttributes & mpq::Attributes::ExtendedAttributes::FileMd5s);

	// contains CRC32 with valid index
	BOOST_REQUIRE(crcs.size() > testfile.block()->index());
	BOOST_REQUIRE(fileTimes.size() > testfile.block()->index());
	BOOST_REQUIRE(md5s.size() > testfile.block()->index());

	const mpq::MD5Checksum currentMd5 = mpq::Attributes::md5(dataString.c_str(), dataString.size());
	const mpq::MD5Checksum storedMd5 = md5s[testfile.block()->index()];
	std::cerr << "Current: " << currentMd5 << " Stored: " << storedMd5 << std::endl;
	BOOST_REQUIRE(currentMd5 == storedMd5);

	const mpq::CRC32 currentCrc32 = mpq::Attributes::crc32(dataString.c_str(), dataString.size());
	const mpq::CRC32 storedCrc32 = crcs[testfile.block()->index()];
	std::cerr << "Current: " << currentCrc32 << " Stored: " << storedCrc32 << std::endl;
	BOOST_REQUIRE(currentCrc32 == storedCrc32);

	// TODO test time stamp by manual comparison
	const mpq::FILETIME storedFiletime = fileTimes[testfile.block()->index()];
	time_t time;
	BOOST_REQUIRE(storedFiletime.toTime(time));
	const std::tm *localtime = std::localtime(&time);
	// 20.8.2014 12:18
	BOOST_REQUIRE(localtime->tm_year == 2014 - 1990);
	BOOST_REQUIRE(localtime->tm_mday == 20);
	BOOST_REQUIRE(localtime->tm_mon == 7);
	BOOST_REQUIRE(localtime->tm_hour == 11);
	BOOST_REQUIRE(localtime->tm_min == 17);
}

/*
 * NOTE has no file time stamps.
 */
BOOST_AUTO_TEST_CASE(Crc32s)
{
	mpq::Archive archive;
	bool success = true;

	try
	{
		archive.open("testattributes.mpq");
	}
	catch (Exception &e)
	{
		success = false;
	}

	BOOST_REQUIRE(success);
	BOOST_REQUIRE(archive.containsListfileFile());
	BOOST_REQUIRE(archive.containsAttributesFile());
	int32 version = 0;
	mpq::Attributes::ExtendedAttributes extendedAttributes = mpq::Attributes::ExtendedAttributes::None;
	mpq::Attributes::Crc32s crcs;
	mpq::Attributes::FileTimes fileTimes;
	mpq::Attributes::Md5s md5s;
	mpq::Attributes attributes = archive.attributesFile();
	BOOST_REQUIRE(attributes.isValid());

	try
	{
		attributes.attributes(version, extendedAttributes, crcs, fileTimes, md5s);
	}
	catch (Exception &e)
	{
		success = false;
	}

	BOOST_REQUIRE(success);
	BOOST_REQUIRE(extendedAttributes & mpq::Attributes::ExtendedAttributes::FileCrc32s);
	BOOST_REQUIRE(!(extendedAttributes & mpq::Attributes::ExtendedAttributes::FileTimeStamps));
	BOOST_REQUIRE(!(extendedAttributes & mpq::Attributes::ExtendedAttributes::FileMd5s));

	mpq::File file = archive.findFile("test.txt");

	BOOST_REQUIRE(file.isValid());
	// contains CRC32 with valid index
	BOOST_REQUIRE(crcs.size() > file.block()->index());
	BOOST_REQUIRE(fileTimes.size() == 0);
	BOOST_REQUIRE(md5s.size() == 0);

	stringstream data;

	try
	{
		file.writeData(data);
	}
	catch (Exception &e)
	{
		success = false;
	}

	BOOST_REQUIRE(success);

	const string dataString = data.str();

	/*
	const mpq::MD5 currentMd5 = mpq::Attributes::md5(dataString.c_str(), dataString.size());
	const mpq::MD5 storedMd5 = md5s[file.block()->index()];
	std::cerr << "Current: " << currentMd5 << " Stored: " << storedMd5 << std::endl;
	BOOST_REQUIRE(currentMd5 == storedMd5);
	*/

	const mpq::CRC32 currentCrc32 = mpq::Attributes::crc32(dataString.c_str(), dataString.size());
	const mpq::CRC32 storedCrc32 = crcs[file.block()->index()];
	std::cerr << "Current: " << currentCrc32 << " Stored: " << storedCrc32 << std::endl;
	BOOST_REQUIRE(currentCrc32 == storedCrc32);
}
