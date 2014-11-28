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

#define BOOST_TEST_MODULE InterpolationTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

//#include <boost/foreach.hpp>

//#include "../../platform.hpp"
#include "../../platform.hpp"
#include "../../mdlgrammar.hpp"
#include "../../interpolator.hpp"
#include "../../geosetanimation.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::mdlx;

BOOST_AUTO_TEST_CASE(GeosetAnim)
{
	spiritTraceLog.close();
	spiritTraceLog.open("geosetanim_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("geosetanim.mdl");

	BOOST_REQUIRE(in);

	Mdlx result;
	MdlGrammar grammar;
	BOOST_REQUIRE(grammar.parse(in, result));
	BOOST_REQUIRE(result.geosetAnimations().size() == 2);

	/*
	 * first animation uses no interpolation which means that the directly bounding value is returned.
	 */
	const GeosetAnimation &animation = result.geosetAnimations().front();

	BOOST_REQUIRE(animation.alphas().properties().size() == 3);
	BOOST_REQUIRE(animation.alphas().lineType() == LineType::DontInterpolate);
	BOOST_REQUIRE(animation.alphas().properties()[0].frame() == 0);
	BOOST_CHECK_CLOSE(animation.alphas().properties()[0].values()[0], 0.0, 0.0001);
	BOOST_REQUIRE(animation.alphas().properties()[1].frame() == 100);
	BOOST_CHECK_CLOSE(animation.alphas().properties()[1].values()[0], 1.0, 0.0001);
	BOOST_REQUIRE(animation.alphas().properties()[2].frame() == 200);
	BOOST_CHECK_CLOSE(animation.alphas().properties()[2].values()[0], 2.0, 0.0001);


	// TODO check values
	Interpolator<1, float32> interpolator;
	interpolator.setAnimatedProperties(&animation.alphas());

	BOOST_REQUIRE(interpolator.animatedProperties() == &animation.alphas());
	BOOST_CHECK_CLOSE(interpolator.calculate(0)[0], 0.0, 0.0001);
	BOOST_CHECK_CLOSE(interpolator.calculate(100)[0], 1.0, 0.0001);
	BOOST_CHECK_CLOSE(interpolator.calculate(200)[0], 2.0, 0.0001);
}
