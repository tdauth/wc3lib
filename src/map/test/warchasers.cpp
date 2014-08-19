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

#define BOOST_TEST_MODULE WarChasersTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/cast.hpp>
//#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../w3m.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

/**
 * \file
 * This test tries to open the War Chasers map, a standard Warcraft III: Reign of Chaos scenario.
 */

BOOST_AUTO_TEST_CASE(ReadTest)
{
	map::W3m map;

	bool valid = true;

	try
	{
		map.open("(4)WarChasers.w3m");
	}
	catch (std::exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);

	/*
	 * NOTE
	 * the mini map had decompression issues
	 */
	mpq::MpqFile *file = map.findFile("war3mapMap.blp");

	BOOST_REQUIRE(file != 0);
	ofstream outInfo("minimap.info");

	BOOST_REQUIRE(outInfo);

	outInfo << mpq::archiveInfo(map, true, true);
	outInfo << mpq::fileInfo(*file, true, true);
	outInfo.close();
	ofstream outMinimap("war3mapMap.blp");

	BOOST_REQUIRE(outMinimap);
	valid = true;

	try
	{
		file->writeData(outMinimap);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		valid = false;
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);
}
