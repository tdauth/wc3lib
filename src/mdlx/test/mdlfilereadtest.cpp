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

#define BOOST_TEST_MODULE MdlFileReadTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>
#include <chrono>

#include <boost/format.hpp>

//#include <boost/foreach.hpp>

//#include "../../platform.hpp"
#include "../platform.hpp"
#include "../mdlgrammar.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::mdlx;

BOOST_AUTO_TEST_CASE(Orc)
{
	spiritTraceLog.close();
	spiritTraceLog.open("mdlorc_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("Orc_Exp.mdl");

	BOOST_REQUIRE(in);

	MdlGrammar grammar;
	Mdlx result;

	const std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	BOOST_REQUIRE(grammar.parse(in, result));
	const std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	const std::chrono::system_clock::duration duration = end - start;

	BOOST_TEST_MESSAGE(boost::str(boost::format("Orc time: %1") % duration.count()));

	BOOST_REQUIRE(strcmp(result.model().name(), "Orc_Exp") == 0);
}
