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

#define BOOST_TEST_MODULE MdxFileReadTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include "../platform.hpp"
#include "../mdlx.hpp"
#include "../mdxgrammarclient.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::mdlx;

BOOST_AUTO_TEST_CASE(Orc)
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

	BOOST_REQUIRE(valid);

	BOOST_CHECK_EQUAL(model.model().name(), "Orc_Exp");
	BOOST_CHECK_EQUAL(model.modelVersion(), 800);
	BOOST_CHECK_EQUAL(model.sequences().size(), 2);
	BOOST_CHECK_EQUAL(model.sequences()[0].name(), "Stand");
	BOOST_CHECK_EQUAL(model.sequences()[1].name(), "Birth");
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
}
