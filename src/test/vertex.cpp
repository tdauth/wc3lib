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
 * Tests the basic serialization functions as well as specialized getters.
 */
#define BOOST_TEST_MODULE VertexTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include "../vertex.hpp"

#if not defined(BOOST_TEST_DYN_LINK) and not defined(WINDOWS)
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

BOOST_AUTO_TEST_CASE(Vertex2dTest)
{
	Vertex2d<int32> vertex;

	BOOST_REQUIRE(vertex.size() == 2);

	vertex[0] = 1;
	vertex[1] = 2;

	BOOST_CHECK(vertex.x() == 1);
	BOOST_CHECK(vertex.y() == 2);

	stringstream sstream(std::ios::in | std::ios::out | std::ios::binary);
	const std::streamsize writtenSize = vertex.write(sstream);
	const std::streamsize expectedSize = 2 * sizeof(int32) * sizeof(byte);

	BOOST_REQUIRE(expectedSize == endPosition(sstream));
	BOOST_REQUIRE(expectedSize == writtenSize);

	Vertex2d<int32> result;

	BOOST_REQUIRE(result.size() == 2);

	sstream.seekg(0);

	BOOST_REQUIRE(sstream.tellg() == 0);

	const std::streamsize readSize = result.read(sstream);

	BOOST_REQUIRE(sstream);
	BOOST_CHECK(readSize == expectedSize);
	BOOST_CHECK(result == vertex);
	BOOST_CHECK(result[0] == vertex[0]);
	BOOST_CHECK(result[1] == vertex[1]);
}

BOOST_AUTO_TEST_CASE(Vertex3dTest)
{
	Vertex3d<int32> vertex;

	BOOST_REQUIRE(vertex.size() == 3);

	vertex[0] = 1;
	vertex[1] = 2;
	vertex[2] = 3;

	BOOST_CHECK(vertex.x() == 1);
	BOOST_CHECK(vertex.y() == 2);
	BOOST_CHECK(vertex.z() == 3);

	stringstream sstream(std::ios::in | std::ios::out | std::ios::binary);
	const std::streamsize writtenSize = vertex.write(sstream);
	const std::streamsize expectedSize = 3 * sizeof(int32) * sizeof(byte);

	BOOST_REQUIRE(expectedSize == endPosition(sstream));
	BOOST_REQUIRE(expectedSize == writtenSize);

	Vertex3d<int32> result;

	BOOST_REQUIRE(result.size() == 3);

	sstream.seekg(0);

	BOOST_REQUIRE(sstream.tellg() == 0);

	const std::streamsize readSize = result.read(sstream);

	BOOST_CHECK(readSize == expectedSize);
	BOOST_CHECK(result == vertex);
	BOOST_CHECK(result[0] == vertex[0]);
	BOOST_CHECK(result[1] == vertex[1]);
	BOOST_CHECK(result[2] == vertex[2]);
}

BOOST_AUTO_TEST_CASE(QuaternionTest)
{
	Quaternion<int32> quaternion;

	BOOST_REQUIRE(quaternion.size() == 4);

	quaternion[0] = 1;
	quaternion[1] = 2;
	quaternion[2] = 3;
	quaternion[3] = 4;

	BOOST_CHECK(quaternion.a() == 1);
	BOOST_CHECK(quaternion.b() == 2);
	BOOST_CHECK(quaternion.c() == 3);
	BOOST_CHECK(quaternion.d() == 4);

	stringstream sstream(std::ios::in | std::ios::out | std::ios::binary);
	const std::streamsize writtenSize = quaternion.write(sstream);
	const std::streamsize expectedSize = 4 * sizeof(int32) * sizeof(byte);

	BOOST_REQUIRE(expectedSize == endPosition(sstream));
	BOOST_REQUIRE(expectedSize == writtenSize);

	Quaternion<int32> result;

	BOOST_REQUIRE(result.size() == 4);

	sstream.seekg(0);

	BOOST_REQUIRE(sstream.tellg() == 0);

	const std::streamsize readSize = result.read(sstream);

	BOOST_CHECK(readSize == expectedSize);
	BOOST_CHECK(result == quaternion);
	BOOST_CHECK(result[0] == quaternion[0]);
	BOOST_CHECK(result[1] == quaternion[1]);
	BOOST_CHECK(result[2] == quaternion[2]);
	BOOST_CHECK(result[3] == quaternion[3]);
}
