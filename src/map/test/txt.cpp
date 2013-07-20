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

#include "../../platform.hpp"
#include "../txt.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

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
	std::cout << sstream.str() << std::endl;
	txt.read(sstream);
	
	BOOST_REQUIRE(txt.sections().size() == 1);
	BOOST_REQUIRE(txt.entries("MySection").size() == 2);
	BOOST_REQUIRE(txt.sections()[0].name == "Hello");
	
}
