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

#define BOOST_TEST_MODULE TriggersTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

//#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../triggers.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

BOOST_AUTO_TEST_CASE(TriggersSimpleReadTest) {
	ifstream in("TriggerData.txt"); // Warcraft III trigger data
	
	BOOST_REQUIRE(in);
	
	map::TriggerData triggerData;
	
	bool valid = true;
	
	try {
		triggerData.read(in);
	}
	catch (...) {
		valid = false;
	}
	
	BOOST_REQUIRE(valid);
	
	ifstream inTriggers("war3map.wtg", std::ios::in | std::ios::binary); // War Chasers triggers
	map::Triggers triggers(0);
	
	try {
		triggers.read(in, triggerData);
		
	}
	catch (...) {
		valid = false;
	}
	
	BOOST_REQUIRE(valid);
}
