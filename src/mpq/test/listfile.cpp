#define BOOST_TEST_MODULE ListfileTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/foreach.hpp>

#include "../listfile.hpp"

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

BOOST_AUTO_TEST_CASE(CaseSensitiveEntries)
{
	stringstream sstream;
	sstream <<
	"Abilities"
	";abilities"
	";abilities\\Hans"
	;

	mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 3);

	mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveEntries(entries, "", false);

	/*
	for (std::size_t i = 0; i < uniqueEntries.size(); ++i)
	{
		std::cerr << uniqueEntries[i] << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 2);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities");
}

BOOST_AUTO_TEST_CASE(CaseSensitiveEntriesRecursive)
{
	stringstream sstream;
	sstream <<
	"Abilities\\Hans"
	";abilities\\Peter"
	";abilities\\PeTeR"
	";abILIties\\UI\\test"
	";abilities\\ui\\test2"
	;

	mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 5);

	mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveEntries(entries, "", true);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_CHECK(uniqueEntries.size() == 5);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities\\Hans");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[2] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[3] == "Abilities\\UI\\test");
	BOOST_REQUIRE(uniqueEntries[4] == "Abilities\\UI\\test2");
}

BOOST_AUTO_TEST_CASE(CaseSensitiveEntriesRecursiveExtension)
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

	mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 6);

	mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveEntries(entries, "", true);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_CHECK(uniqueEntries.size() == 6);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities\\Hans");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[2] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[3] == "Abilities\\UI\\test");
	BOOST_REQUIRE(uniqueEntries[4] == "Abilities\\UI\\test2");
	BOOST_REQUIRE(uniqueEntries[5] == "Abilities\\UI\\test3.txt");
}
