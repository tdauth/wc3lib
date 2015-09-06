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

#define BOOST_TEST_MODULE RectsTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include "../../platform.hpp"
#include "../rects.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

BOOST_AUTO_TEST_CASE(ReadTest)
{
	ifstream in("war3map.w3r", ifstream::in | ifstream::binary); // TestMap.w3m

	BOOST_REQUIRE(in);

	map::Rects rects;

	bool valid = true;

	try
	{
		rects.read(in);
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);

	BOOST_REQUIRE(rects.rects().size() == 3);
	const map::Rect &rect = rects.rects().at(0);
	BOOST_REQUIRE(rect.name() == "Blue Rect");
	BOOST_REQUIRE(rect.color() == 0xFF0000); // blue - BGR color!
	BOOST_REQUIRE(!rect.hasWeatherEffect());
	BOOST_REQUIRE(rect.soundName().empty());
	BOOST_CHECK_CLOSE(rect.left(), 1120.0, 0.0001);
	BOOST_CHECK_CLOSE(rect.right(), 1152.0, 0.0001);
	BOOST_CHECK_CLOSE(rect.bottom(), 896.0, 0.0001);
	BOOST_CHECK_CLOSE(rect.top(), 928.0, 0.0001);

	const map::Rect &whiteRect = rects.rects().at(1);
	BOOST_REQUIRE(whiteRect.name() == "White Rect");
	BOOST_REQUIRE(whiteRect.color() == 0xFFFFFF); // white - BGR color!
	BOOST_REQUIRE(!whiteRect.hasWeatherEffect());
	BOOST_REQUIRE(whiteRect.soundName().empty());
	BOOST_CHECK_CLOSE(whiteRect.left(), 1024.0, 0.0001);
	BOOST_CHECK_CLOSE(whiteRect.right(), 1152.0, 0.0001);
	BOOST_CHECK_CLOSE(whiteRect.bottom(), 768.0, 0.0001);
	BOOST_CHECK_CLOSE(whiteRect.top(), 896.0, 0.0001);

	const map::Rect &redRectWithWeathereffect = rects.rects().at(2);
	BOOST_REQUIRE(redRectWithWeathereffect.name() == "Red Rect With Weathereffect");
	BOOST_REQUIRE(redRectWithWeathereffect.color() == 0x0000FF); // red - BGR color!
	BOOST_REQUIRE(redRectWithWeathereffect.hasWeatherEffect());
	BOOST_REQUIRE(map::idToString(redRectWithWeathereffect.weatherEffectId()) == "RAhr"); // taken from "TERRAINART/Weather.slk" Ashenvale Heavy Rain
	BOOST_REQUIRE(redRectWithWeathereffect.hasSound());
	BOOST_REQUIRE(redRectWithWeathereffect.soundName() == "gg_snd_RainOfFireLoop1");
	BOOST_CHECK_CLOSE(redRectWithWeathereffect.left(), 896.0, 0.0001);
	BOOST_CHECK_CLOSE(redRectWithWeathereffect.right(), 1024.0, 0.0001);
	BOOST_CHECK_CLOSE(redRectWithWeathereffect.bottom(), 768.0, 0.0001);
	BOOST_CHECK_CLOSE(redRectWithWeathereffect.top(), 992.0, 0.0001);
}

BOOST_AUTO_TEST_CASE(ReadWriteReadTest)
{
	ifstream in("war3map.w3r", ifstream::in | ifstream::binary); // Reign of Chaos

	BOOST_REQUIRE(in);

	map::Rects rects;

	bool valid = true;

	try
	{
		rects.read(in);
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);

	in.close();
	ofstream out("war3map.w3rout", ifstream::out | ifstream::binary);

	BOOST_REQUIRE(out);

	try
	{
		rects.write(out);
	}
	catch (...)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);

	out.close(); // flush file stream
	rects.rects().clear(); // ensure it's empty!

	in.open("war3map.w3rout", ifstream::in | ifstream::binary); // Reign of Chaos, reopen

	BOOST_REQUIRE(in);

	try
	{
		rects.read(in);

	}
	catch (...) {
		valid = false;
	}

	BOOST_REQUIRE(valid);

	BOOST_REQUIRE(rects.rects().size() == 3);
	const map::Rect &rect = rects.rects().at(0);
	BOOST_REQUIRE(rect.name() == "Blue Rect");
	BOOST_REQUIRE(rect.color() == 0xFF0000); // blue - BGR color!
	BOOST_REQUIRE(!rect.hasWeatherEffect());
	BOOST_REQUIRE(rect.soundName().empty());
	BOOST_CHECK_CLOSE(rect.left(), 1120.0, 0.0001);
	BOOST_CHECK_CLOSE(rect.right(), 1152.0, 0.0001);
	BOOST_CHECK_CLOSE(rect.bottom(), 896.0, 0.0001);
	BOOST_CHECK_CLOSE(rect.top(), 928.0, 0.0001);

	const map::Rect &whiteRect = rects.rects().at(1);
	BOOST_REQUIRE(whiteRect.name() == "White Rect");
	BOOST_REQUIRE(whiteRect.color() == 0xFFFFFF); // white - BGR color!
	BOOST_REQUIRE(!whiteRect.hasWeatherEffect());
	BOOST_REQUIRE(whiteRect.soundName().empty());
	BOOST_CHECK_CLOSE(whiteRect.left(), 1024.0, 0.0001);
	BOOST_CHECK_CLOSE(whiteRect.right(), 1152.0, 0.0001);
	BOOST_CHECK_CLOSE(whiteRect.bottom(), 768.0, 0.0001);
	BOOST_CHECK_CLOSE(whiteRect.top(), 896.0, 0.0001);

	const map::Rect &redRectWithWeathereffect = rects.rects().at(2);
	BOOST_REQUIRE(redRectWithWeathereffect.name() == "Red Rect With Weathereffect");
	BOOST_REQUIRE(redRectWithWeathereffect.color() == 0x0000FF); // red - BGR color!
	BOOST_REQUIRE(redRectWithWeathereffect.hasWeatherEffect());
	BOOST_REQUIRE(map::idToString(redRectWithWeathereffect.weatherEffectId()) == "RAhr"); // taken from "TERRAINART/Weather.slk" Ashenvale Heavy Rain
	BOOST_REQUIRE(redRectWithWeathereffect.hasSound());
	BOOST_REQUIRE(redRectWithWeathereffect.soundName() == "gg_snd_RainOfFireLoop1");
	BOOST_CHECK_CLOSE(redRectWithWeathereffect.left(), 896.0, 0.0001);
	BOOST_CHECK_CLOSE(redRectWithWeathereffect.right(), 1024.0, 0.0001);
	BOOST_CHECK_CLOSE(redRectWithWeathereffect.bottom(), 768.0, 0.0001);
	BOOST_CHECK_CLOSE(redRectWithWeathereffect.top(), 992.0, 0.0001);
}
