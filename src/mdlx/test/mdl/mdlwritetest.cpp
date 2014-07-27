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

#define BOOST_TEST_MODULE MdlWriteTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

//#include <boost/foreach.hpp>

//#include "../../platform.hpp"
#include "../../platform.hpp"
#include "../../mdlgrammarclient.hpp"
#include "../../mdlgenerator.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::mdlx;

BOOST_AUTO_TEST_CASE(MdlBounds) {
	spiritTraceLog.close();
	spiritTraceLog.open("mdlbounds_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("bounds.mdl");

	BOOST_REQUIRE(in);

	client::MdlGrammar<MdlGrammar::PositionIteratorType, client::CommentSkipper<MdlGrammar::PositionIteratorType>> grammar;
	client::CommentSkipper<MdlGrammar::PositionIteratorType> commentSkipper;

	MdlGrammar::IteratorType first = MdlGrammar::IteratorType(in);
	MdlGrammar::IteratorType last;

	MdlGrammar::ForwardIteratorType forwardFirst = boost::spirit::make_default_multi_pass(first);
	MdlGrammar::ForwardIteratorType forwardLast = boost::spirit::make_default_multi_pass(last); // TODO has to be created? Do we need this iterator to be passed?

	// used for backtracking and more detailed error output
	MdlGrammar::PositionIteratorType position_begin(forwardFirst);
	MdlGrammar::PositionIteratorType position_end;

	Bounds result;

	bool r = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.bounds,
			commentSkipper,
			result
	);

	BOOST_REQUIRE(r);
	BOOST_REQUIRE(first == last);
}

BOOST_AUTO_TEST_CASE(MdlVersion) {
	spiritTraceLog.close();
	spiritTraceLog.open("mdlversion_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("version.mdl");

	BOOST_REQUIRE(in);

	Mdlx *result = 0;
	MdlGrammar grammar;

	BOOST_REQUIRE(grammar.parse(in, result));
	BOOST_REQUIRE(result != 0);

	BOOST_REQUIRE(result->modelVersion()->modelVersion() == 800);
}
