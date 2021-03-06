/***************************************************************************
 *   Copyright (C) 2015 by Tamino Dauth                                    *
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

#define BOOST_TEST_MODULE JasstransTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <fstream>

#include <boost/filesystem.hpp>

#include "../../map.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

//using namespace wc3lib;

BOOST_AUTO_TEST_CASE(Extract)
{
	std::cout << "Working directory: " << boost::filesystem::current_path() << std::endl;

	if (boost::filesystem::exists("strings.wts"))
	{
		BOOST_REQUIRE(boost::filesystem::remove("strings.wts"));
	}

	BOOST_REQUIRE(std::system("../jasstrans ./jass.j -f -o ./war3map.wts -O war3map.j") == 0);

	std::ifstream in("war3map.wts");

	BOOST_REQUIRE(in);
	wc3lib::map::MapStrings mapStrings;
	bool valid = true;

	try
	{
		mapStrings.read(in);
	}
	catch (const std::exception &e)
	{
		valid = false;
		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(mapStrings.entries().size() == 2);
	BOOST_CHECK(mapStrings.entries()[0].key == 0);
	BOOST_CHECK(mapStrings.entries()[0].value == "This is my string.");
	BOOST_CHECK(mapStrings.entries()[0].comment.empty());

	BOOST_CHECK(mapStrings.entries()[1].key == 1);
	BOOST_CHECK(mapStrings.entries()[1].value == "Hello world!");
	BOOST_CHECK(mapStrings.entries()[1].comment.empty());
}
