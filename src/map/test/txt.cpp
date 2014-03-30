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

//#include <boost/foreach.hpp>

#include "../../spirit.hpp"
#include "../../platform.hpp"
#include "../txt.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

bool isHello(const map::Txt::Pair &pair)
{
	return pair.first == "Hello";
}

bool isHaha(const map::Txt::Pair &pair)
{
	return pair.first == "Haha";
}

BOOST_AUTO_TEST_CASE(TxtSimpleReadTest) {
	spiritTraceLog.open("txtsimplereadtestraces.xml");
	
	BOOST_REQUIRE(spiritTraceLog);
	
	string myTxt =
	"[MySection]\n"
	"Hello = 23"
	;
	
	map::Txt txt;
	std::basic_stringstream<byte> sstream;
	sstream << myTxt;
	//std::cout << sstream.str() << std::endl;
	txt.read(sstream);
	
	BOOST_REQUIRE(txt.sections().size() == 1);
	bool exists = true;
	
	try
	{
		txt.entries("MySection");
	}
	catch (Exception e)
	{
		exists = false;
	}
	
	BOOST_REQUIRE(exists);
	BOOST_REQUIRE(txt.entries("MySection").size() == 1);
	wc3lib::map::Txt::Pairs::iterator begin = txt.sections()[0].entries.begin();
	wc3lib::map::Txt::Pairs::iterator end = txt.sections()[0].entries.end();
	BOOST_REQUIRE(std::find_if(begin, end, isHello) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isHello)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isHello)->second == "23");
}

BOOST_AUTO_TEST_CASE(TxtReadTest) {
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
	bool exists = true;
	
	try
	{
		txt.entries("MySection");
	}
	catch (Exception e)
	{
		exists = false;
	}
	
	BOOST_REQUIRE(exists);
	BOOST_REQUIRE(txt.entries("MySection").size() == 2);
	wc3lib::map::Txt::Pairs::iterator begin = txt.sections()[0].entries.begin();
	wc3lib::map::Txt::Pairs::iterator end = txt.sections()[0].entries.end();
	BOOST_REQUIRE(std::find_if(begin, end, isHello) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isHello)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isHello)->second == "23");
	
	BOOST_REQUIRE(std::find_if(begin, end, isHaha) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isHaha)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isHaha)->second == "12");
	
}

BOOST_AUTO_TEST_CASE(TxtReadTestWithSpacesAtBeginning) {
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
	bool exists = true;
	
	try
	{
		txt.entries("MySection");
	}
	catch (Exception e)
	{
		exists = false;
	}
	
	BOOST_REQUIRE(exists);
	BOOST_REQUIRE(txt.entries("MySection").size() == 2);
	wc3lib::map::Txt::Pairs::iterator begin = txt.sections()[0].entries.begin();
	wc3lib::map::Txt::Pairs::iterator end = txt.sections()[0].entries.end();
	BOOST_REQUIRE(std::find_if(begin, end, isHello) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isHello)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isHello)->second == "23");
	
	BOOST_REQUIRE(std::find_if(begin, end, isHaha) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isHaha)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isHaha)->second == "12");
	
}

bool isBla(const map::Txt::Pair &pair)
{
	return pair.first == "bla";
}

bool isOperatorCompareBoolean(const map::Txt::Pair &pair)
{
	return pair.first == "OperatorCompareBoolean";
}

bool isOperatorCompareDestructibleHint(const map::Txt::Pair &pair)
{
	return pair.first == "OperatorCompareDestructibleHint";
}

BOOST_AUTO_TEST_CASE(TxtReadTriggerDataTest) {
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
	bool exists = true;
	
	try
	{
		//std::cout << "Section: " << txt.sections().front().name << "END" << std::endl;
		txt.entries("bla");
	}
	catch (Exception e)
	{
		exists = false;
	}
	
	BOOST_REQUIRE(exists);
	//std::cout << "Entries count: " << txt.entries("bla").size()  << std::endl;
	BOOST_REQUIRE(txt.entries("bla").size() == 6);
	wc3lib::map::Txt::Pairs::iterator begin = txt.sections()[0].entries.begin();
	wc3lib::map::Txt::Pairs::iterator end = txt.sections()[0].entries.end();
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareBoolean) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isOperatorCompareBoolean)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareBoolean)->second == "\"Boolean Comparison\"");
	
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareDestructibleHint) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isOperatorCompareDestructibleHint)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareDestructibleHint)->second == "");
}

BOOST_AUTO_TEST_CASE(TxtWriteTriggerDataTest) {
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
	sstream.str(""); // clear stream
	txt.write(sstream);
	txt.sections().clear(); // clear Txt for new read
	
	//std::cout << "newly written:\n" << sstream.str() << std::endl;
	
	txt.read(sstream); // read again
	
	/*
	std::cout << "Sections:" << std::endl;
	
	BOOST_FOREACH (map::Txt::Sections::const_reference ref, txt.sections()) {
		std::cout << ref.name << std::endl;
	}
	*/
	
	BOOST_REQUIRE(txt.sections().size() == 1);
	
	bool exists = true;
	
	try
	{
		//std::cout << "Section: " << txt.sections().front().name << "END" << std::endl;
		txt.entries("bla");
	}
	catch (Exception e)
	{
		exists = false;
	}
	
	BOOST_REQUIRE(exists);
	//std::cout << "Entries count: " << txt.entries("bla").size()  << std::endl;
	BOOST_REQUIRE(txt.entries("bla").size() == 6);
	wc3lib::map::Txt::Pairs::iterator begin = txt.sections()[0].entries.begin();
	wc3lib::map::Txt::Pairs::iterator end = txt.sections()[0].entries.end();
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareBoolean) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isOperatorCompareBoolean)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareBoolean)->second == "\"Boolean Comparison\"");
	
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareDestructibleHint) != end);
	//std::cout << "Value: " << std::find_if(begin, end, isOperatorCompareDestructibleHint)->second << std::endl;
	BOOST_REQUIRE(std::find_if(begin, end, isOperatorCompareDestructibleHint)->second == "");
	
	
}
