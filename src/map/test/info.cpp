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

#define BOOST_TEST_MODULE InfoTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include "../../platform.hpp"
#include "../info.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

BOOST_AUTO_TEST_CASE(InfoReadTest)
{
	ifstream in("war3map.w3i");

	BOOST_REQUIRE(in);

	map::Info info;
	bool valid = true;

	try
	{
		info.read(in);
	}
	catch (const Exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(info.players().size() == 5);
	BOOST_REQUIRE(info.forces().size() == 2);
	// TODO check the numbers
	//BOOST_REQUIRE(info.upgradeAvailabilities().size() == 0);
	//BOOST_REQUIRE(info.techAvailabilities().size() == 9); // 9 units are not available for night elves (enemies)
	//BOOST_REQUIRE(info.techAvailabilities()[0].playerMask() == 11); // enemies TODO 11th bit must be set 1
	BOOST_REQUIRE(info.randomUnitTables().size() == 0); // no custom tables
}

BOOST_AUTO_TEST_CASE(InfoReadWriteReadTest)
{
	ifstream in("war3map.w3i");

	BOOST_REQUIRE(in);

	map::Info info;
	bool valid = true;

	try
	{
		info.read(in);
	}
	catch (const Exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(info.players().size() == 5);
	BOOST_REQUIRE(info.forces().size() == 2);
	// TODO check the numbers
	//BOOST_REQUIRE(info.upgradeAvailabilities().size() == 0);
	//BOOST_REQUIRE(info.techAvailabilities().size() == 9); // 9 units are not available for night elves (enemies)
	//BOOST_REQUIRE(info.techAvailabilities()[0].playerMask() == 11); // enemies TODO 11th bit must be set 1
	BOOST_REQUIRE(info.randomUnitTables().size() == 0); // no custom tables

	const char *outFileName = "war3map.w3iout";
	ofstream out(outFileName);

	BOOST_REQUIRE(out);

	valid = true;

	try
	{
		info.write(out);
	}
	catch (const Exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);

	out.close();
	in.close();
	info.clear();
	in.open(outFileName);

	BOOST_REQUIRE(in);

	valid = true;

	try
	{
		info.read(in);
	}
	catch (const Exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(info.players().size() == 5);
	BOOST_REQUIRE(info.forces().size() == 2);
	// TODO check the numbers
	//BOOST_REQUIRE(info.upgradeAvailabilities().size() == 0);
	//BOOST_REQUIRE(info.techAvailabilities().size() == 9); // 9 units are not available for night elves (enemies)
	//BOOST_REQUIRE(info.techAvailabilities()[0].playerMask() == 11); // enemies TODO 11th bit must be set 1
	BOOST_REQUIRE(info.randomUnitTables().size() == 0); // no custom tables

	// TODO check all stuff again
}
