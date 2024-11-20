/***************************************************************************
 *   Copyright (C) 2024 by Tamino Dauth                                    *
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

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::mdlx;

BOOST_AUTO_TEST_CASE(Orc_Exp_Mdx)
{
	ifstream in("Orc_Exp.mdx");

	BOOST_REQUIRE(in);

	wc3lib::mdlx::Mdlx model;
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

	BOOST_TEST_MESSAGE("Writing file Orc_Exp_out.json");

	ofstream out("Orc_Exp_out.json");

	BOOST_REQUIRE(out);

	valid = true;
	size = 0;
	wc3lib::json::Mdlx jsonMdlx(&model);

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

	BOOST_TEST_MESSAGE("Reading file Orc_Exp_out.json");
}
