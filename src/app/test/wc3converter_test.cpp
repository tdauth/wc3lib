/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#define BOOST_TEST_MODULE Wc3ConverterTest
#include <boost/test/unit_test.hpp>
#include <cstdlib>

#include "../../config.h"
#include "../../map.hpp"

#ifdef USE_QBLP
#include <QImage>
#endif

#if not defined(BOOST_TEST_DYN_LINK) and not defined(WINDOWS)
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::map;

/**
 * Merges "a.txt" and "b.txt" to a new output file "out.txt".
 */
BOOST_AUTO_TEST_CASE(Wc3ConverterMergeTxt)
{
	BOOST_REQUIRE(boost::filesystem::exists("a.txt"));
	BOOST_REQUIRE(boost::filesystem::exists("b.txt"));

	const int result = system("../wc3converter --overwrite --merge out.txt a.txt b.txt");

	BOOST_REQUIRE(boost::filesystem::exists("out.txt"));

	BOOST_REQUIRE_EQUAL(result, 0);
	Txt txt;
	ifstream in("out.txt");
	BOOST_REQUIRE(in);
	BOOST_REQUIRE_NO_THROW(txt.read(in));

	BOOST_REQUIRE_EQUAL(txt.sections().size(), 3);

	BOOST_CHECK_EQUAL(txt.sections()[0].name, "Hello");
	BOOST_CHECK_EQUAL(txt.sections()[0].entries.size(), 3);

	BOOST_CHECK_EQUAL(txt.sections()[1].name, "CustomSection");
	BOOST_CHECK_EQUAL(txt.sections()[1].entries.size(), 1);

	BOOST_CHECK_EQUAL(txt.sections()[2].name, "CustomSection2");
	BOOST_CHECK_EQUAL(txt.sections()[2].entries.size(), 1);
}

/**
 * Merges "a.slk" and "b.slk" to a new output file "out.slk".
 */
BOOST_AUTO_TEST_CASE(Wc3ConverterMergeSlk)
{
	BOOST_REQUIRE(boost::filesystem::exists("a.slk"));
	BOOST_REQUIRE(boost::filesystem::exists("b.slk"));

	const int result = system("../wc3converter --overwrite --merge out.slk a.slk b.slk");

	BOOST_REQUIRE(boost::filesystem::exists("out.slk"));

	BOOST_REQUIRE_EQUAL(result, 0);
	Slk slk;
	ifstream in("out.slk");
	BOOST_REQUIRE(in);
	BOOST_REQUIRE_NO_THROW(slk.read(in));

	/*
	BOOST_REQUIRE_EQUAL(txt.sections().size(), 3);

	BOOST_CHECK_EQUAL(txt.sections()[0].name, "Hello");
	BOOST_CHECK_EQUAL(txt.sections()[0].entries.size(), 3);

	BOOST_CHECK_EQUAL(txt.sections()[1].name, "CustomSection");
	BOOST_CHECK_EQUAL(txt.sections()[1].entries.size(), 1);

	BOOST_CHECK_EQUAL(txt.sections()[2].name, "CustomSection2");
	BOOST_CHECK_EQUAL(txt.sections()[2].entries.size(), 1);
	*/
}

#ifdef USE_QBLP
BOOST_AUTO_TEST_CASE(Wc3ConverterBlp2Png)
{
	BOOST_REQUIRE(boost::filesystem::exists("BTN_cr_HOLYllllstrenth.blp"));

	const int result = system("../wc3converter --verbose --overwrite BTN_cr_HOLYllllstrenth.png BTN_cr_HOLYllllstrenth.blp");

	BOOST_REQUIRE(boost::filesystem::exists("BTN_cr_HOLYllllstrenth.png"));

	BOOST_REQUIRE_EQUAL(result, 0);

	QImage image;
	BOOST_REQUIRE(image.load("BTN_cr_HOLYllllstrenth.png", "PNG"));
	BOOST_REQUIRE(!image.isNull());
	BOOST_REQUIRE(image.size() == QSize(64, 64));
	BOOST_REQUIRE(image.format() ==  QImage::Format_ARGB32);
	// TODO check all settings

	BOOST_REQUIRE(image.save("BTN_cr_HOLYllllstrenth.jpg", "JPG"));
	// TODO load again with the wc3converter
}
#endif