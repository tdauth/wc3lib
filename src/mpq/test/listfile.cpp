#define BOOST_TEST_MODULE ListfileTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/foreach.hpp>

#include "../listfile.hpp"
#include "../archive.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

/*
 2.7 LISTFILE
 The listfile is a very simple extension to the MoPaQ format that contains the file paths of (most) files in the archive. The languages and platforms of the files are not stored in the listfile. The listfile is contained in the file "(listfile)" (default language and platform), and is simply a text file with file paths separated by ';', 0Dh, 0Ah, or some combination of these. The file "(listfile)" may not be listed in the listfile.
 */
BOOST_AUTO_TEST_CASE(ListfileEntriesTest)
{
	stringstream sstream;
	sstream <<
	"bla1;"
	"bla2\n"
	"bla3\r"
	// ;
	"bla4;\n\r"
	"bla5;\r\n"
	"bla6;\n"
	"bla7;\r"
	// line break
	"bla8\n\r;"
	"bla9\n;\r"
	"bla10\n;"
	"bla11\n\r"
	// carriage return
	"bla12\r\n;"
	"bla13\r;\n"
	"bla14\r\n"
	"bla15\r;"
	"end"
	;

	mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 16);
	BOOST_REQUIRE(entries[0] == "bla1");
	BOOST_REQUIRE(entries[1] == "bla2");

	// ...

	BOOST_REQUIRE(entries[6] == "bla7");

	// ...

	BOOST_REQUIRE(entries.back() == "end");
}

BOOST_AUTO_TEST_CASE(CaseFileSensitiveEntries)
{
	stringstream sstream;
	sstream <<
	"Abilities"
	";abilities"
	";abilities\\Hans" // must be excluded since it is not recursive
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 3);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveFileEntries(entries, "", false);

	/*
	for (std::size_t i = 0; i < uniqueEntries.size(); ++i)
	{
		std::cerr << uniqueEntries[i] << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 2);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities");
	BOOST_REQUIRE(uniqueEntries[1] == "abilities");
}

BOOST_AUTO_TEST_CASE(CaseSensitiveFileEntriesRecursive)
{
	stringstream sstream;
	sstream <<
	"Abilities\\Hans"
	";abilities\\Peter"
	";abilities\\PeTeR"
	";abILIties\\UI\\test"
	";abilities\\ui\\test2"
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 5);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveFileEntries(entries, "", true);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 5);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities\\Hans");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[2] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[3] == "Abilities\\UI\\test");
	BOOST_REQUIRE(uniqueEntries[4] == "Abilities\\UI\\test2");
}

BOOST_AUTO_TEST_CASE(CaseSensitiveFileEntriesRecursiveExtension)
{
	stringstream sstream;
	sstream <<
	"Abilities\\Hans"
	";abilities\\Peter"
	";abilities\\PeTeR"
	";abILIties\\UI\\test"
	";abilities\\ui\\test2"
	";abilities\\ui\\test3.txt"
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 6);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveFileEntries(entries, "", true);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 6);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities\\Hans");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[2] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[3] == "Abilities\\UI\\test");
	BOOST_REQUIRE(uniqueEntries[4] == "Abilities\\UI\\test2");
	BOOST_REQUIRE(uniqueEntries[5] == "Abilities\\UI\\test3.txt");
}

BOOST_AUTO_TEST_CASE(CaseSensitiveDirEntriesRecursive)
{
	stringstream sstream;
	sstream <<
	"Abilities\\Hans\\bla"
	";abilities\\Peter\\blu"
	";abilities\\Peter\\bla"
	";abilities\\PeTeR\\bli"
	";abILIties\\UI\\test"
	";abilities\\ui\\test2"
	";abilities\\ui\\test3.txt"
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 7);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveDirEntries(entries, "", true);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 4);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities\\Hans");
	BOOST_REQUIRE(uniqueEntries[2] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[3] == "Abilities\\UI");
}

BOOST_AUTO_TEST_CASE(CaseSensitiveDirEntriesWithPrefix)
{
	stringstream sstream;
	sstream <<
	"Abilities\\Hans\\bla"
	";abilities\\Peter\\blu"
	";abilities\\PeTeR\\bli"
	";abILIties\\UI\\test"
	";abILIties\\UI\\test1"
	";abilities\\ui\\test2"
	";abilities\\ui\\test3.txt"
	";test\\ui\\test3.txt"
	";test\\ui\\anothersubdir\\myfile.txt"
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 9);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveDirEntries(entries, "Abilities\\", false);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 3);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities\\Hans");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[2] == "Abilities\\UI");
}

BOOST_AUTO_TEST_CASE(ExistingEntriesWithPrefix)
{
	mpq::Archive archive;
	bool success = true;

	try
	{
		archive.open("mpqmaster_mpq1_no_extended_attributes.mpq");
	}
	catch (Exception &e)
	{
		std::cerr << e.what() << std::endl;

		success = false;
	}

	BOOST_REQUIRE(success);
	stringstream sstream;
	sstream <<
	"test\\testfile.txt"
	";testfile.txt"
	;

	const mpq::Listfile::Entries listfileEntries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(listfileEntries.size() == 2);

	const mpq::Listfile::Entries entries = mpq::Listfile::existingEntries(listfileEntries, archive, "test\\", true);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, entries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(entries.size() == 1);

	BOOST_REQUIRE(entries[0] == "test\\testfile.txt");
}

BOOST_AUTO_TEST_CASE(DirPath)
{
	BOOST_CHECK(mpq::Listfile::dirPath("Bla") == "");
	BOOST_CHECK(mpq::Listfile::dirPath("Bla\\bla") == "Bla");
	BOOST_CHECK(mpq::Listfile::dirPath("Bla\\bla\\bla.txt") == "Bla\\bla");
}

BOOST_AUTO_TEST_CASE(FileName)
{
	const string filePath1 = "UI\\peter\\test.txt";
	BOOST_CHECK(mpq::Listfile::fileName(filePath1) == "test.txt");
	const string filePath2 = "UI\\peter\\";
	BOOST_CHECK(mpq::Listfile::fileName(filePath2) == "");
}
