/***************************************************************************
 *   Copyright (C) 2013 by Tamino Dauth                                    *
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

#define BOOST_TEST_MODULE TxtTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>
#include <chrono>

//#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../txt.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

bool isHello(const map::Txt::Entry &entry)
{
	return entry.first == "Hello";
}

bool isHaha(const map::Txt::Entry &entry)
{
	return entry.first == "Haha";
}

BOOST_AUTO_TEST_CASE(TxtSimpleReadTest)
{
	string myTxt =
	"\n"
	" // comment "
	"\n"
	"[MySection] // comment  \n"
	"Hello = 23 // comment\n"
	"WESTRING_RACE_SELECTABLE=Auswählbar" // check Umlaute
	;

	map::Txt txt;
	std::basic_stringstream<byte> sstream;
	sstream << myTxt;
	//std::cout << sstream.str() << std::endl;
	txt.read(sstream);

	BOOST_REQUIRE_EQUAL(txt.sections().size(), 1);
	const map::Txt::Section &section = txt.sections()[0];
	BOOST_REQUIRE_EQUAL(section.name, "MySection");
	BOOST_REQUIRE_EQUAL(section.entries.size(), 2);
	BOOST_REQUIRE_EQUAL(txt.sections()[0].entries[0].key(), "Hello");
	BOOST_REQUIRE_EQUAL(txt.sections()[0].entries[0].value(), "23");
}

BOOST_AUTO_TEST_CASE(TxtReadTest)
{
	string myTxt =
	"[MySection]\n"
	"Hello = 23\n"
	"\n"
	"\n"
	"// does our ruin benefit the world?\n"
	"\n"
	"Haha = 12// is there an avenging power in nature?\n"
	"\n"
	;

	map::Txt txt;
	std::basic_stringstream<byte> sstream;
	sstream << myTxt;
	//std::cout << sstream.str() << std::endl;
	txt.read(sstream);

	BOOST_REQUIRE(txt.sections().size() == 1);
	const map::Txt::Section &section = txt.sections()[0];
	BOOST_REQUIRE(section.name == "MySection");
	BOOST_REQUIRE(section.entries.size() == 2);
	wc3lib::map::Txt::Entries::const_iterator begin = section.entries.begin();
	wc3lib::map::Txt::Entries::const_iterator end = section.entries.end();
	BOOST_REQUIRE(std::find_if(begin, end, isHello) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isHello)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isHello)->second == "23");

	BOOST_REQUIRE(std::find_if(begin, end, isHaha) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isHaha)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isHaha)->second == "12");

}

BOOST_AUTO_TEST_CASE(TxtReadTestWithSpacesAtBeginning)
{
	string myTxt =
	"\n\n\n\n "
	"[MySection]// Hey, what's this?\n"
	"Hello = 23\n"
	"\n"
	"\n"
	"// does our ruin benefit the world?\n"
	"\n"
	"Haha = 12// is there an avenging power in nature?\n"
	"\n"
	;

	map::Txt txt;
	std::basic_stringstream<byte> sstream;
	sstream << myTxt;
	//std::cout << sstream.str() << std::endl;
	txt.read(sstream);

	BOOST_REQUIRE(txt.sections().size() == 1);
	const map::Txt::Section &section = txt.sections()[0];
	BOOST_REQUIRE(section.name == "MySection");
	BOOST_REQUIRE(section.entries.size() == 2);
	wc3lib::map::Txt::Entries::const_iterator begin = section.entries.begin();
	wc3lib::map::Txt::Entries::const_iterator end = section.entries.end();
	BOOST_REQUIRE(std::find_if(begin, end, isHello) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isHello)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isHello)->second == "23");

	BOOST_REQUIRE(std::find_if(begin, end, isHaha) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isHaha)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isHaha)->second == "12");

}

bool isBla(const map::Txt::Entry &entry)
{
	return entry.first == "bla";
}

bool isOperatorCompareBoolean(const map::Txt::Entry &entry)
{
	return entry.first == "OperatorCompareBoolean";
}

bool isOperatorCompareDestructibleHint(const map::Txt::Entry &entry)
{
	return entry.first == "OperatorCompareDestructibleHint";
}

BOOST_AUTO_TEST_CASE(TxtReadTriggerDataTest)
{
	string myTxt =
	"//***************************************************************************\n"
	"[bla]\n"

	"OperatorCompareBoolean=\"Boolean Comparison\"\n"
	"OperatorCompareBoolean=~Value,\" \",~Operator,\" \",~Value\n"

	"OperatorCompareBooleanHint = 10\n"
	"\n"
	"OperatorCompareDestructible=\"Destructible Comparison\"\n"
	"OperatorCompareDestructible=~Value,\" \",~Operator,\" \",~Value\n"
	"OperatorCompareDestructibleHint=\n"
	;

	map::Txt txt;
	std::basic_stringstream<byte> sstream;
	sstream << myTxt;
	//std::cout << sstream.str() << std::endl;
	txt.read(sstream);

	BOOST_REQUIRE(txt.sections().size() == 1);
	const map::Txt::Section &section = txt.sections()[0];
	BOOST_REQUIRE(section.name == "bla");
	//std::cout << "Entries count: " << txt.entries("bla").size()  << std::endl;
	BOOST_REQUIRE(section.entries.size() == 6);
	wc3lib::map::Txt::Entries::const_iterator begin = section.entries.begin();
	wc3lib::map::Txt::Entries::const_iterator end = section.entries.end();
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareBoolean) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isOperatorCompareBoolean)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareBoolean)->second == "\"Boolean Comparison\"");

	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareDestructibleHint) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isOperatorCompareDestructibleHint)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareDestructibleHint)->second == "");
}

BOOST_AUTO_TEST_CASE(TxtWriteTriggerDataTest)
{
	string myTxt =
	"//***************************************************************************\n"
	"[bla]\n"

	"OperatorCompareBoolean=\"Boolean Comparison\"\n"
	"OperatorCompareBoolean=~Value,\" \",~Operator,\" \",~Value\n"

	"OperatorCompareBooleanHint = 10\n"
	"\n"
	"OperatorCompareDestructible=\"Destructible Comparison\"\n"
	"OperatorCompareDestructible=~Value,\" \",~Operator,\" \",~Value\n"
	"OperatorCompareDestructibleHint=\n"
	;

	map::Txt txt;
	std::basic_stringstream<byte> sstream;
	sstream << myTxt;
	//std::cout << sstream.str() << std::endl;
	txt.read(sstream);

	BOOST_REQUIRE_EQUAL(txt.sections().size(), 1);

	//std::cout << "test 1" << std::endl;

	sstream = std::basic_stringstream<byte>(std::stringstream::out | std::stringstream::in | std::stringstream::binary);
	txt.write(sstream);

	//ofstream out("bla.txt", std::fstream::out | std::fstream::binary);
	//txt.write(out);
	//out.close();

	//std::cout << "test 2" << std::endl;

	txt.sections().clear(); // clear Txt for new read

	//std::cout << "newly written:\n" << sstream.str() << std::endl;

	BOOST_REQUIRE_EQUAL(txt.sections().size(), 0);

	txt.read(sstream); // read again

	/*
	std::cout << "Sections:" << std::endl;

	BOOST_FOREACH (map::Txt::Sections::const_reference ref, txt.sections()) {
		std::cout << ref.name << std::endl;
	}
	*/

	BOOST_REQUIRE_EQUAL(txt.sections().size(), 1);
	const map::Txt::Section &section = txt.sections()[0];
	BOOST_REQUIRE_EQUAL(section.name, "bla");
	//std::cout << "Entries count: " << txt.entries("bla").size()  << std::endl;
	const map::Txt::Sections::const_iterator iterator = std::find_if(txt.sections().begin(), txt.sections().end(), [](const map::Txt::Section &section) { return section.name == "bla"; });
	BOOST_REQUIRE(iterator != txt.sections().end());
	BOOST_REQUIRE(iterator->entries.size() == 6);
	const wc3lib::map::Txt::Entries::const_iterator begin = iterator->entries.begin();
	const wc3lib::map::Txt::Entries::const_iterator end = iterator->entries.end();
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareBoolean) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isOperatorCompareBoolean)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareBoolean)->second == "\"Boolean Comparison\"");

	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareDestructibleHint) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isOperatorCompareDestructibleHint)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareDestructibleHint)->second == "");
}

BOOST_AUTO_TEST_CASE(EmptySection)
{
	string myTxt =
	"[nnom]\n"
	"\n"
	"// Goblin Laboratory\n"
	"[ngad]\n"
	"Art=ReplaceableTextures\\CommandButtons\\BTNAmmoDump.blp\n"
	"Sellunits=ngsp,nzep,ngir\n"
	;

	map::Txt txt;
	std::basic_stringstream<byte> sstream;
	sstream << myTxt;
	bool valid = true;

	try
	{
		txt.read(sstream);
	}
	catch (const Exception &e)
	{
		valid = false;
		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
}

/*
 * We test the German "WorldEditStrings.txt" file which needs UTF-8 support and contains many many entries.
 * This is the most realistic test since it uses an original file from The Frozen Throne.
 */
BOOST_AUTO_TEST_CASE(WorldEditStrings)
{

	ifstream in("WorldEditStrings.txt");

	BOOST_REQUIRE(in);

	map::Txt txt;
	bool valid = true;
	const std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();

	try
	{
		txt.read(in);
	}
	catch (Exception &e)
	{
		std::cerr << e.what() << std::endl;
		valid = false;
	}

	BOOST_REQUIRE(valid);

	const std::chrono::high_resolution_clock::time_point finished = std::chrono::high_resolution_clock::now();
	const std::chrono::high_resolution_clock::duration duration = finished - now;

	std::cout << "WorldEditStrings.txt Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms" << std::endl;
	std::cout << "WorldEditStrings.txt Duration: " << std::chrono::duration_cast<std::chrono::seconds>(duration).count() << " s" << std::endl;
}

BOOST_AUTO_TEST_CASE(UnitEditorData)
{
	ifstream in("UnitEditorData.txt");

	BOOST_REQUIRE(in);

	map::Txt txt;
	bool valid = true;

	try
	{
		txt.read(in);
	}
	catch (Exception &e)
	{
		std::cerr << e.what() << std::endl;
		valid = false;
	}

	BOOST_REQUIRE(valid);
}

BOOST_AUTO_TEST_CASE(NeutralUnitFunc)
{
	ifstream in("NeutralUnitFunc.txt");

	BOOST_REQUIRE(in);

	map::Txt txt;
	bool valid = true;

	try
	{
		txt.read(in);
	}
	catch (Exception &e)
	{
		std::cerr << e.what() << std::endl;
		valid = false;
	}

	BOOST_REQUIRE(valid);
}

// has a comment starting with one single /: / Stuffed Penguin
BOOST_AUTO_TEST_CASE(ItemFunc)
{
	ifstream in("ItemFunc.txt");

	BOOST_REQUIRE(in);

	map::Txt txt;
	bool valid = true;

	try
	{
		txt.read(in);
	}
	catch (Exception &e)
	{
		std::cerr << e.what() << std::endl;
		valid = false;
	}

	BOOST_REQUIRE(valid);
}

/*
[mdpb]
[mdpb]
Name=Medusa Pebble
Purchase Medusa Pebble
Hotkey=P
Tip=Purchase Medusa |cffffcc00P|rebble
Ubertip="Turns the targeted enemy non-Hero unit into stone when used. A unit turned to stone by the Medusa Pebble is removed from the game permanently."
Description=Turns target unit to stone.
*/
BOOST_AUTO_TEST_CASE(ItemStrings)
{
	ifstream in("ItemStrings.txt");

	BOOST_REQUIRE(in);

	map::Txt txt;
	bool valid = true;

	try
	{
		txt.read(in);
	}
	catch (Exception &e)
	{
		std::cerr << e.what() << std::endl;
		valid = false;
	}

	BOOST_REQUIRE(valid);
}


