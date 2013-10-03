#define BOOST_TEST_MODULE ListfileTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

//#include <boost/foreach.hpp>

#include "../listfile.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

/*
 2.7 LISTFILE
 The listfile is a very simple extension to the MoPaQ format that contains the file paths of (most) files in the archive. The languages and platforms of the files are not stored in the listfile. The listfile is contained in the file "(listfile)" (default language and platform), and is simply a text file with file paths separated by ';', 0Dh, 0Ah, or some combination of these. The file "(listfile)" may not be listed in the listfile.
 */
BOOST_AUTO_TEST_CASE(ListfileEntriesTest) {
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

BOOST_AUTO_TEST_CASE(ListfileDirEntriesTestTopLevel) {
	stringstream sstream;
	sstream <<
	"bla1;"
	"bla2;"
	"bla3;"
	"bla4\\bla\\bla\\bla"
	;
	
	mpq::Listfile::Entries entries = mpq::Listfile::dirEntries(sstream.str(), "", false, true); // empty directory means top level directory
	
	BOOST_REQUIRE(entries.size() == 4);
	
	BOOST_REQUIRE(entries[0] == "bla1");
	BOOST_REQUIRE(entries[1] == "bla2");
	BOOST_REQUIRE(entries[2] == "bla3");
	BOOST_REQUIRE(entries[3] == "bla4\\");
}

BOOST_AUTO_TEST_CASE(ListfileDirEntriesTestTopLevelRecursive) {
	stringstream sstream;
	sstream <<
	"bla1;"
	"bla2;"
	"bla3;"
	"bla4\\bla\\bla\\bla"
	;
	
	mpq::Listfile::Entries entries = mpq::Listfile::dirEntries(sstream.str(), "", true, true); // empty directory means top level directory
	
	BOOST_REQUIRE(entries.size() == 7);
	
	BOOST_REQUIRE(entries[0] == "bla1");
	BOOST_REQUIRE(entries[1] == "bla2");
	BOOST_REQUIRE(entries[2] == "bla3");
	BOOST_REQUIRE(entries[3] == "bla4\\bla\\bla\\bla"); // the file itself
	// the order of directories is not specified!
	BOOST_REQUIRE(std::find(entries.begin(), entries.end(), "bla4\\") != entries.end());
	BOOST_REQUIRE(std::find(entries.begin(), entries.end(), "bla4\\bla\\") != entries.end());
	BOOST_REQUIRE(std::find(entries.begin(), entries.end(), "bla4\\bla\\bla\\") != entries.end());
}
