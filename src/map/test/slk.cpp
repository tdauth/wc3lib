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

#define BOOST_TEST_MODULE SlkTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/chrono.hpp>

#include "../../spirit.hpp"
#include "../../platform.hpp"
#include "../slk.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

// TODO add small tests before "Unit Meta Data"

BOOST_AUTO_TEST_CASE(UnitMetaData) {
	spiritTraceLog.close();
	spiritTraceLog.open("unitmetadata_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("UnitMetaData.slk");

	BOOST_REQUIRE(in);

	map::Slk slk;

	bool valid = true;

	try
	{
		slk.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	// 17 columns
	// 118 rows
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(slk.table().shape()[0] == 17);
	BOOST_REQUIRE(slk.table().shape()[1] == 118);
}