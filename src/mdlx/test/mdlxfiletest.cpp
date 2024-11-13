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

#define BOOST_TEST_MODULE MdlxFileTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include "../platform.hpp"
#include "../mdlx.hpp"
#include "../mdlgrammar.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::mdlx;

namespace
{

void verifyOrcExp(const Mdlx &model)
{
	/*
	 * Sequences 2 {
	 * GlobalSequences 17 {
	 * Textures 23 {
	 * Materials 26 {
	 * TextureAnims 1 {
	 * PivotPoints 168 {
	 * 4 lights
	 * 12 helpers
	 * 1 camera
	 * 3 event objects
	 * 6 geoset animations
	 * ?? geosets
	 * ?? bones
	 */
	BOOST_CHECK_EQUAL(model.model().name(), "Orc_Exp");
	BOOST_CHECK_EQUAL(model.modelVersion(), 800);
	BOOST_CHECK_EQUAL(model.sequences().size(), 2);
	BOOST_CHECK_EQUAL(model.sequences()[0].name(), "Stand");
	BOOST_CHECK_EQUAL(model.sequences()[1].name(), "Birth");
	BOOST_CHECK_EQUAL(model.globalSequences().size(), 17);
}

}

BOOST_AUTO_TEST_CASE(Orc_Exp_Mdx)
{
	ifstream in("Orc_Exp.mdx");

	BOOST_REQUIRE(in);

	Mdlx model;
	bool valid = true;
	std::size_t size = 0;

	try
	{
		size = model.read(in);
	}
	catch (Exception &e)
	{
		valid = false;
		std::cerr << e.what() << std::endl;
	}

	in.close();

	BOOST_REQUIRE(valid);

	verifyOrcExp(model);

	ofstream out("Orc_Exp_out.mdx");

	BOOST_REQUIRE(out);

	valid = true;
	size = 0;

	try
	{
		size = model.write(out);
	}
	catch (Exception &e)
	{
		valid = false;
		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);

	model = Mdlx();
	valid = true;
	size = 0;
	in.open("Orc_Exp_out.mdx");

	BOOST_REQUIRE(in);

	try
	{
		size = model.read(in);
	}
	catch (Exception &e)
	{
		valid = false;
		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);

	verifyOrcExp(model);
}

BOOST_AUTO_TEST_CASE(Orc_Exp_Mdlx)
{
	spiritTraceLog.close();
	spiritTraceLog.open("mdlorc_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("Orc_Exp.mdl");

	BOOST_REQUIRE(in);

	MdlGrammar grammar;
	Mdlx model;

	const std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	BOOST_REQUIRE(grammar.parse(in, model));
	const std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	const std::chrono::system_clock::duration duration = end - start;

	BOOST_TEST_MESSAGE(boost::str(boost::format("Orc time: %1") % duration.count()));

	verifyOrcExp(model);
}
