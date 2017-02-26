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
	BOOST_CHECK_EQUAL(input.entries()[2].comment, "Abilities"); // The comment stays the same.
	BOOST_CHECK_EQUAL(input.entries()[2].value, "Dies");

	// There is no translated value for this entry.
	BOOST_CHECK_EQUAL(input.entries()[3].key, 4);
	BOOST_CHECK_EQUAL(input.entries()[3].comment, "");
	BOOST_CHECK_EQUAL(input.entries()[3].value, "Another sentence");
}

BOOST_AUTO_TEST_CASE(Wc3TransGermanWithItself)
{
	// Translates "war3map_untranslated_en.wts" into German by using the already translated file. The result is war3map_de.wts since it is already translated and the last entry which does not exist in the original file should be used anyway.
	system("../wc3trans war3map_en.wts war3map_de.wts war3map_de.wts out2.wts");

	MapStrings input;
	ifstream in("out2.wts");

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

	// This value does not exist in the original, so it is simply kept.
	BOOST_CHECK_EQUAL(input.entries()[3].key, 66);
	BOOST_CHECK_EQUAL(input.entries()[3].comment, "");
	BOOST_CHECK_EQUAL(input.entries()[3].value, "Order 66");
}

BOOST_AUTO_TEST_CASE(Wc3TransUpdate)
{
	// Translates "war3map_untranslated_en.wts" into German by using the already translated file. The result is war3map_de.wts but without the entries which do not exist in "war3map_en.wts" since --update is used.
	system("../wc3trans --update war3map_en.wts war3map_de.wts war3map_de.wts out3.wts");

	MapStrings input;
	ifstream in("out3.wts");

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
	BOOST_CHECK_EQUAL(input.entries()[2].comment, "Abilities"); // The comment is used from the source with --update
	BOOST_CHECK_EQUAL(input.entries()[2].value, "Dies");

	// Since --update is used, this entry replaces the old.
	BOOST_CHECK_EQUAL(input.entries()[3].key, 4);
	BOOST_CHECK_EQUAL(input.entries()[3].comment, "");
	BOOST_CHECK_EQUAL(input.entries()[3].value, "My friends are dead.");
}

BOOST_AUTO_TEST_CASE(Wc3TransSuggest)
{
	system("../wc3trans --update --suggest war3map_en.wts war3map_de.wts war3map_untranslated_en.wts out4.wts");

	MapStrings input;
	ifstream in("out4.wts");

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
	BOOST_CHECK_EQUAL(input.entries()[2].comment, "Abilities"); // The comment is used from the source with --update
	BOOST_CHECK_EQUAL(input.entries()[2].value, "Dies");

	BOOST_CHECK_EQUAL(input.entries()[3].key, 4);
	BOOST_CHECK_EQUAL(input.entries()[3].comment, "");
	BOOST_CHECK_EQUAL(input.entries()[3].value, "Another sentence"); // "Hello" is most similar to "My friends are dead." Therefore the translation "Hallo" is suggested.
}
