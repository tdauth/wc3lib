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

/**
 * \file
 * Unit Test which reads all team color files.
 * Those files have been analyzed by external tools:
 * - size is 8x8
 * - 4 MIP maps
 * - BLP1
 * - JPEG compression
 * - JPEG header size: 624 bytes
 */
#define BOOST_TEST_MODULE TeamColorsTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include "../blp.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

/*
 * Red
 */
BOOST_AUTO_TEST_CASE(TeamColor00) {
	ifstream in("TeamColor00.blp");

	BOOST_REQUIRE(in);

	blp::Blp texture;
	bool valid = true;

	try {
		texture.read(in);
	} catch (std::exception &e) {
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(texture.format() == blp::Blp::Format::Blp1);
	BOOST_REQUIRE(texture.compression() == blp::Blp::Compression::Jpeg);
	BOOST_REQUIRE(texture.mipMaps().size() == 4);
	BOOST_REQUIRE(texture.mipMaps()[0].width() == 8);
	BOOST_REQUIRE(texture.mipMaps()[0].height() == 8);
	BOOST_REQUIRE(texture.mipMaps()[0].colorAt(0, 0).argb() == 0xFF0303); // red
}

BOOST_AUTO_TEST_CASE(TeamColor00Write) {
	ifstream in("TeamColor00.blp");

	BOOST_REQUIRE(in);

	blp::Blp texture;
	bool valid = true;

	try {
		texture.read(in);
	} catch (std::exception &e) {
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	in.close();

	BOOST_REQUIRE(valid);

	ofstream out("TeamColor00tmp.blp");

	BOOST_REQUIRE(out);

	try {
		texture.write(out);
	} catch (std::exception &e) {
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	out.close();

	BOOST_REQUIRE(valid);

	// now read again
	in.open("TeamColor00tmp.blp");

	BOOST_REQUIRE(in);

	try {
		texture.read(in);
	} catch (std::exception &e) {
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);

	BOOST_REQUIRE(texture.format() == blp::Blp::Format::Blp1);
	BOOST_REQUIRE(texture.compression() == blp::Blp::Compression::Jpeg);
	BOOST_REQUIRE(texture.mipMaps().size() == 4);
	BOOST_REQUIRE(texture.mipMaps()[0].width() == 8);
	BOOST_REQUIRE(texture.mipMaps()[0].height() == 8);
	BOOST_REQUIRE(texture.mipMaps()[0].colorAt(0, 0).argb() == 0xFF0303); // red
}
