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
 * Unit Test which tests BLP/Qt color conversion functions.
 */
#define BOOST_TEST_MODULE Colors
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include "../platform.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::editor;

BOOST_AUTO_TEST_CASE(BlpToQt)
{
	// blp::color has the format RGBA
	const blp::color value = 0xFFAACC00;
	// QRgb has the format ARGB
	const QRgb rgb = colorToArgb(value);
	BOOST_CHECK(qRed(rgb) == 0xFF);
	BOOST_CHECK(qGreen(rgb) == 0xAA);
	BOOST_CHECK(qBlue(rgb) == 0xCC);
	BOOST_CHECK(qAlpha(rgb) == 0x00);
}

BOOST_AUTO_TEST_CASE(QtToBlp)
{
	// QRgb has the format ARGB
	const QRgb rgb = 0x00FFAACC;
	// blp::color has the format RGBA
	const blp::color value = argbToColor(rgb);
	BOOST_CHECK(blp::red(value) == 0xFF);
	BOOST_CHECK(blp::green(value) == 0xAA);
	BOOST_CHECK(blp::blue(value) == 0xCC);
	BOOST_CHECK(blp::alpha(value) == 0x00);
}
