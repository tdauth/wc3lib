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

#define BOOST_TEST_MODULE WarChasersTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/cast.hpp>
//#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../w3m.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

/**
 * \file
 * This test tries to open the War Chasers map, a standard Warcraft III: Reign of Chaos scenario.
 */

BOOST_AUTO_TEST_CASE(ReadTest)
{
	map::W3m map;

	bool valid = true;

	try
	{
		map.open("(4)WarChasers.w3m");
	}
	catch (std::exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);

	/*
	 * NOTE
	 * the mini map had decompression issues
	 */
	mpq::File file = map.findFile("war3mapMap.blp");

	BOOST_REQUIRE(file.isValid());
	ofstream outInfo("minimap.info");

	BOOST_REQUIRE(outInfo);

	outInfo << mpq::archiveInfo(map, true, true);
	outInfo << mpq::fileInfo(file, true, true);
	outInfo.close();
	ofstream outMinimap("war3mapMap.blp");

	BOOST_REQUIRE(outMinimap);
	valid = true;

	try
	{
		file.writeData(outMinimap);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		valid = false;
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);
}

BOOST_AUTO_TEST_CASE(Cameras)
{
	map::W3m map;

	bool valid = true;

	try
	{
		map.open("(4)WarChasers.w3m");
	}
	catch (std::exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);

	/*
	 * Cameras
	 */
	bool validCameras = true;

	try
	{
		map.readFileFormat(map.cameras().get());
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		validCameras = false;
	}
	catch (...)
	{
		validCameras = false;
	}

	BOOST_REQUIRE(validCameras);
	BOOST_REQUIRE(map.cameras()->cameras().size() == 6);
	const map::Camera &cam0 = map.cameras()->cameras()[0];
	BOOST_REQUIRE(cam0.name() == "CamStart1");
	BOOST_CHECK_CLOSE(cam0.targetX(), -7718.41, 0.001);
	BOOST_CHECK_CLOSE(cam0.targetY(), -9039.14, 0.001);
	BOOST_CHECK_CLOSE(cam0.zOffset(), 0.00, 0.001);
	BOOST_CHECK_CLOSE(cam0.rotation(), 90.0, 0.001);
	BOOST_CHECK_CLOSE(cam0.angleOfAttack(), 304.00, 0.001);
	BOOST_CHECK_CLOSE(cam0.distance(), 1790.91, 0.001);
	BOOST_CHECK_CLOSE(cam0.roll(), 0.00, 0.001);
	BOOST_CHECK_CLOSE(cam0.fieldOfView(), 70.00, 0.001);
	BOOST_CHECK_CLOSE(cam0.farZ(), 5000.00, 0.001);

	const map::Camera &cam1 = map.cameras()->cameras()[1];
	BOOST_REQUIRE(cam1.name() == "Camera 002");
	BOOST_CHECK_CLOSE(cam1.targetX(), 2595.08, 0.001);
	BOOST_CHECK_CLOSE(cam1.targetY(), -8045.40, 0.001);
	BOOST_CHECK_CLOSE(cam1.zOffset(), 0.00, 0.001);
	BOOST_CHECK_CLOSE(cam1.rotation(), 133.47, 0.001);
	BOOST_CHECK_CLOSE(cam1.angleOfAttack(), 314.55, 0.001);
	BOOST_CHECK_CLOSE(cam1.distance(), 1996.50, 0.001);
	BOOST_CHECK_CLOSE(cam1.roll(), 0.00, 0.001);
	BOOST_CHECK_CLOSE(cam1.fieldOfView(), 70.00, 0.001);
	BOOST_CHECK_CLOSE(cam1.farZ(), 5000.00, 0.001);
}

BOOST_AUTO_TEST_CASE(Rects)
{
	map::W3m map;

	bool valid = true;

	try
	{
		map.open("(4)WarChasers.w3m");
	}
	catch (std::exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);

	/*
	 * Rects
	 */
	bool validRects = true;

	try
	{
		map.readFileFormat(map.rects().get());
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		validRects = false;
	}
	catch (...)
	{
		validRects = false;
	}

	BOOST_REQUIRE(validRects);
	// Object Manager lists this number
	BOOST_REQUIRE(map.rects()->rects().size() == 151);
	const map::Rect &rect0 = map.rects()->rects()[0];
	BOOST_REQUIRE(rect0.name() == "ALL SPIDERS");
	BOOST_CHECK_CLOSE(rect0.left(), -8000, 0.001);
	BOOST_CHECK_CLOSE(rect0.right(), -6400, 0.001);
	BOOST_CHECK_CLOSE(rect0.bottom(), 2048, 0.001);
	BOOST_CHECK_CLOSE(rect0.top(), 4512, 0.001);
	BOOST_CHECK(rect0.color() == 0xFFFFFF);
	BOOST_CHECK(!rect0.hasWeatherEffect());
	BOOST_CHECK(!rect0.hasSound());
}

BOOST_AUTO_TEST_CASE(CustomUnits)
{
	map::W3m map;

	bool valid = true;

	try
	{
		map.open("(4)WarChasers.w3m");
	}
	catch (std::exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);

	/*
	 * Custom Units
	 */
	bool validCustomUnits = true;

	try
	{
		map.readFileFormat(map.customUnits().get());
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		validCustomUnits = false;
	}
	catch (...)
	{
		validCustomUnits = false;
	}

	BOOST_REQUIRE(validCustomUnits);
	// Object Manager lists this number
	BOOST_REQUIRE(map.customUnits()->originalTable().size() == 122);
	BOOST_REQUIRE(map.customUnits()->customTable().size() == 25);
	const map::CustomUnits::Unit &unit0 = map.customUnits()->originalTable()[0];
	BOOST_CHECK(map::idToString(unit0.originalId()) == "hmtt");
	BOOST_CHECK(map::idToString(unit0.customId()) == "");
}

