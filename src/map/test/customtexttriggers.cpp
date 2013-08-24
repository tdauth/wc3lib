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

#define BOOST_TEST_MODULE CustomTextTriggersTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

//#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../customtexttriggers.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

BOOST_AUTO_TEST_CASE(SimpleReadTest) {
	ifstream in("war3map.wct", ifstream::in | ifstream::binary); // War Chasers
	
	BOOST_REQUIRE(in);
	
	map::CustomTextTriggers customTextTriggers;
	
	bool valid = true;
	
	try {
		customTextTriggers.read(in);
	}
	catch (...) {
		valid = false;
	}
	
	BOOST_REQUIRE(valid);
}

BOOST_AUTO_TEST_CASE(ReadWriteReadTest) {
	ifstream in("war3map.wtg", ifstream::in | ifstream::binary); // War Chasers
	
	BOOST_REQUIRE(in);
	
	map::CustomTextTriggers customTextTriggers;
	
	bool valid = true;
	
	try {
		customTextTriggers.read(in);
	}
	catch (...) {
		valid = false;
	}
	
	BOOST_REQUIRE(valid);
	
	ofstream out("war3map.wctout", ifstream::out | ifstream::binary);
	
	try {
		customTextTriggers.write(out);
	}
	catch (...) {
		valid = false;
	}
	
	BOOST_REQUIRE(valid);
	
	customTextTriggers.triggerTexts().clear(); // ensure it's empty!
	
	try {
		customTextTriggers.read(in);
	}
	catch (...) {
		valid = false;
	}
	
	BOOST_REQUIRE(valid);
}
