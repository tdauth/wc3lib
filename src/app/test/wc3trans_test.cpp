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

#define BOOST_TEST_MODULE Wc3TransTest
#include <boost/test/unit_test.hpp>
#include <cstdlib>

#include "../../map.hpp"

#if not defined(BOOST_TEST_DYN_LINK) and not defined(WINDOWS)
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::map;

BOOST_AUTO_TEST_CASE(Wc3TransGerman)
{
	// Translates "war3map_untranslated_en.wts" into German by using the relation between "war3map_en.wts" and "war3map_de.wts".
	system("../wc3trans war3map_en.wts war3map_de.wts war3map_untranslated_en.wts out.wts");

	MapStrings input;
	ifstream in("out.wts");

	BOOST_REQUIRE(in);
	BOOST_CHECK_NO_THROW(input.read(in));

	BOOST_REQUIRE_EQUAL(input.entries().size(), 4);

	BOOST_CHECK_EQUAL(input.entries()[0].key, 1);
	BOOST_CHECK_EQUAL(input.entries()[0].comment, "");
	BOOST_CHECK_EQUAL(input.entries()[0].value, "Hallo");

	BOOST_CHECK_EQUAL(input.entries()[1].key, 2);
	BOOST_CHECK_EQUAL(input.entries()[1].comment, "");
	BOOST_CHECK_EQUAL(input.entries()[1].value, "Welt");

	BOOST_CHECK_EQUAL(input.entries()[2].key, 3);
	BOOST_CHECK_EQUAL(input.entries()[2].comment, "Fähigkeiten"); // even the comment is translated?
	BOOST_CHECK_EQUAL(input.entries()[2].value, "Dies");

	// There is no translated value for this entry
	BOOST_CHECK_EQUAL(input.entries()[3].key, 4);
	BOOST_CHECK_EQUAL(input.entries()[3].comment, "");
	BOOST_CHECK_EQUAL(input.entries()[3].value, "Another sentence");
}
