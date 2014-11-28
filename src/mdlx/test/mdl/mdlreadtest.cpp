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

#define BOOST_TEST_MODULE MdlReadTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

//#include <boost/foreach.hpp>

//#include "../../platform.hpp"
#include "../../platform.hpp"
#include "../../mdlgrammarclient.hpp"
#include "../../mdlgrammar.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::mdlx;

BOOST_AUTO_TEST_CASE(MdlBounds)
{
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

	BOOST_CHECK_CLOSE(result.minimumExtent()[0], -16849.500000, 0.0001);
	BOOST_CHECK_CLOSE(result.minimumExtent()[1], -3155.110107, 0.0001);
	BOOST_CHECK_CLOSE(result.minimumExtent()[2], -9140.349609, 0.0001);

	BOOST_CHECK_CLOSE(result.maximumExtent()[0], 786.507019, 0.0001);
	BOOST_CHECK_CLOSE(result.maximumExtent()[1], 16107.799805, 0.0001);
	BOOST_CHECK_CLOSE(result.maximumExtent()[2], 5544.810059, 0.0001);

	BOOST_CHECK_CLOSE(result.boundsRadius(), 2324.42, 0.0001);
}

BOOST_AUTO_TEST_CASE(MdlVersion)
{
	spiritTraceLog.close();
	spiritTraceLog.open("mdlversion_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("version.mdl");

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

	long32 result;

	bool r = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.version,
			commentSkipper,
			result
	);

	BOOST_REQUIRE(r);
	BOOST_REQUIRE(first == last);
	BOOST_CHECK(result == 800);
}

BOOST_AUTO_TEST_CASE(MdlModel)
{
	spiritTraceLog.close();
	spiritTraceLog.open("mdlmodel_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("model.mdl");

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

	Mdlx result;

	bool r = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar,
			commentSkipper,
			result
	);

	BOOST_REQUIRE(r);
	BOOST_REQUIRE(first == last);

	BOOST_REQUIRE(strcmp(result.model().name(), "MyModelName") == 0);
	BOOST_REQUIRE(result.model().blendTime() == 150);
	BOOST_CHECK_CLOSE(result.model().bounds().minimumExtent()[0], -16849.500000, 0.0001);
	BOOST_CHECK_CLOSE(result.model().bounds().minimumExtent()[1], -3155.110107, 0.0001);
	BOOST_CHECK_CLOSE(result.model().bounds().minimumExtent()[2], -9140.349609, 0.0001);
	BOOST_CHECK_CLOSE(result.model().bounds().maximumExtent()[0], 786.507019, 0.0001);
	BOOST_CHECK_CLOSE(result.model().bounds().maximumExtent()[1], 16107.799805, 0.0001);
	BOOST_CHECK_CLOSE(result.model().bounds().maximumExtent()[2], 5544.810059, 0.0001);
}

BOOST_AUTO_TEST_CASE(MdlFaces)
{
	spiritTraceLog.close();
	spiritTraceLog.open("mdlfaces_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("faces.mdl");

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

	Geoset::Faces result;

	bool r = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.faces,
			commentSkipper,
			result
	);

	BOOST_REQUIRE(r);
	BOOST_REQUIRE(first == last);

	BOOST_REQUIRE(result.size() == 1);
	const Faces &faces = result.front();
	BOOST_REQUIRE(faces.vertices().size() == 810);
	BOOST_REQUIRE(faces.type() == Faces::Type::Triangles);
	BOOST_REQUIRE(faces.vertices().front() == 0);
	BOOST_REQUIRE(faces.vertices().back() == 154);
}
