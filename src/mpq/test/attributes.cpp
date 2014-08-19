#define BOOST_TEST_MODULE AttributesTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/scoped_ptr.hpp>

#include "../mpq.hpp"
#include "../attributes.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

BOOST_AUTO_TEST_CASE(Attributes) {
	mpq::Mpq archive;
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
	mpq::Attributes::Crc32s crcs;
	mpq::Attributes::Md5s md5s;
	mpq::Attributes::FileTimes fileTimes;

	try
	{
		archive.attributesFile()->attributes(crcs, fileTimes, md5s);
	}
	catch (Exception &e)
	{
		success = false;
	}

	BOOST_REQUIRE(success);

	mpq::MpqFile *file = archive.findFile("test.txt");

	BOOST_REQUIRE(file != 0);
	// contains CRC32 with valid index
	BOOST_REQUIRE(crcs.size() > file->block()->index());
	BOOST_REQUIRE(fileTimes.size() > file->block()->index());
	BOOST_REQUIRE(md5s.size() > file->block()->index());

	stringstream data;

	try
	{
		file->writeData(data);
	}
	catch (Exception &e)
	{
		success = false;
	}

	BOOST_REQUIRE(success);

	string dataString = data.str();

	const mpq::MD5 currentMd5 = mpq::Attributes::md5(dataString.c_str(), dataString.size());
	const mpq::MD5 storedMd5 = md5s[file->block()->index()];
	std::cerr << "Current: " << currentMd5 << " Stored: " << storedMd5 << std::endl;
	BOOST_REQUIRE(currentMd5 == storedMd5);

	const mpq::CRC32 currentCrc32 = mpq::Attributes::crc32(dataString.c_str(), dataString.size());
	const mpq::CRC32 storedCrc32 = crcs[file->block()->index()];
	std::cerr << "Current: " << currentCrc32 << " Stored: " << storedCrc32 << std::endl;
	BOOST_REQUIRE(currentCrc32 == storedCrc32);

	// TODO test time stamp by manual comparison
}
