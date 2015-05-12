/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#define BOOST_TEST_MODULE MapStringsTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>
#include <chrono>

#include "../../spirit.hpp"
#include "../../platform.hpp"
#include "../mapstrings.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

/*
 * German strings of War Chasers, a Warcraft III: Reign of Chaos scenario map by Blizzard Entertainment.
 */
BOOST_AUTO_TEST_CASE(WarChasersRead)
{
	spiritTraceLog.close();
	spiritTraceLog.open("mapstrings_warchasers_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("war3map.wts");

	BOOST_REQUIRE(in);

	map::MapStrings strings;

	bool valid = true;

	try
	{
		strings.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(!strings.entries().empty());
	BOOST_CHECK(strings.entries()[0].key == 2);
	BOOST_CHECK(strings.entries()[0].comment == "");
	//std::cerr << "Value: " << strings.entries()[0].value << std::endl;
	BOOST_CHECK(strings.entries()[0].value == "Spieler 1");
	
	BOOST_CHECK(strings.entries().back().key == 178);
	BOOST_CHECK(strings.entries().back().comment == "");
	//std::cerr << "Value: " << strings.entries()[0].value << std::endl;
	BOOST_CHECK(strings.entries().back().value == "Dungeon-Bewohner");
}

BOOST_AUTO_TEST_CASE(WarChasersReadWriteRead)
{
	spiritTraceLog.close();
	spiritTraceLog.open("mapstrings_warchasers_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("war3map.wts");

	BOOST_REQUIRE(in);

	map::MapStrings strings;

	bool valid = true;

	try
	{
		strings.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(!strings.entries().empty());
	BOOST_CHECK(strings.entries()[0].key == 2);
	BOOST_CHECK(strings.entries()[0].comment == "");
	//std::cerr << "Value: " << strings.entries()[0].value << std::endl;
	BOOST_CHECK(strings.entries()[0].value == "Spieler 1");
	
	BOOST_CHECK(strings.entries().back().key == 178);
	BOOST_CHECK(strings.entries().back().comment == "");
	//std::cerr << "Value: " << strings.entries()[0].value << std::endl;
	BOOST_CHECK(strings.entries().back().value == "Dungeon-Bewohner");
	
	ofstream out("war3mapout.wts");
	BOOST_REQUIRE(out);
	
	try
	{
		strings.write(out);
	}
	catch (const Exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}
	
	out.close();
	
	in.close();
	in.open("war3mapout.wts");
	
	BOOST_REQUIRE(in);
	
	strings.clear();
	
	try
	{
		strings.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(!strings.entries().empty());
	BOOST_CHECK(strings.entries()[0].key == 2);
	BOOST_CHECK(strings.entries()[0].comment == "");
	//std::cerr << "Value: " << strings.entries()[0].value << std::endl;
	BOOST_CHECK(strings.entries()[0].value == "Spieler 1");
	
	BOOST_CHECK(strings.entries().back().key == 178);
	BOOST_CHECK(strings.entries().back().comment == "");
	//std::cerr << "Value: " << strings.entries()[0].value << std::endl;
	BOOST_CHECK(strings.entries().back().value == "Dungeon-Bewohner");
}

/*
 * German strings of Skibbi's Castle TD, a The Frozen Throne map.
 * This string file includes comments.
 */
BOOST_AUTO_TEST_CASE(SkibbisCastleTD)
{
	spiritTraceLog.close();
	spiritTraceLog.open("mapstrings_skibbiscastletd_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("SkibisCastleTD/war3map.wts");

	BOOST_REQUIRE(in);

	map::MapStrings strings;

	bool valid = true;

	try
	{
		strings.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(!strings.entries().empty());
	BOOST_REQUIRE(strings.entries()[0].key == 0);
	//std::cerr << "Comment: " << strings.entries()[0].comment << std::endl;
	// initial spaces of comments are skipped
	BOOST_REQUIRE(strings.entries()[0].comment == "Units: h02K (D-String), Name (Name)");
	//std::cerr << "Value: " << strings.entries()[0].value << std::endl;
	BOOST_REQUIRE(strings.entries()[0].value == "D-Kette");
}
