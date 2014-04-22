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

#define BOOST_TEST_MODULE Wc3MapTest
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
 * For this Unit Test a custom Warcraft III: The Reign of Chaos map has been created with test objects
 * which are queried in the test cases.
 */

BOOST_AUTO_TEST_CASE(Rects) {
	map::W3m map;
	
	bool valid = true;
	
	try {
		map.open("TestMap.w3m");
	}
	catch (...) {
		valid = false;
	}
	
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(map.rects().get() != 0);
	BOOST_REQUIRE(map.rects()->fileName() == "war3map.w3r");
	BOOST_REQUIRE(map.rects()->rects().size() == 3);
}
