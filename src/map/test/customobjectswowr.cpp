/***************************************************************************
 *   Copyright (C) 2023 by Tamino Dauth                                    *
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

#define BOOST_TEST_MODULE CustomObjectsCollectionWoWReforgedTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/cast.hpp>
//#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../customobjectscollection.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

BOOST_AUTO_TEST_CASE(Units)
{
	std::ifstream in("reforged/war3map.w3u");
	
	const map::CustomObjects customUnits(map::CustomObjects::Type::Units);
	customUnits.read(in);
	
	BOOST_REQUIRE_EQUAL(customUnits.type(), map::CustomObjects::Type::Units);
	BOOST_REQUIRE_EQUAL(customUnits.fileName(), "war3map.w3u");
	BOOST_REQUIRE_EQUAL(customUnits.customTable().size(), 1);
	const map::CustomObjects::Object *object = boost::polymorphic_cast<map::CustomObjects::Object*>(&customUnits.customTable().at(0));
	BOOST_REQUIRE_EQUAL(object->type(), map::CustomObjects::Type::Units);
	BOOST_REQUIRE_EQUAL(map::idToString(object->originalId()), "hpea"); // footman id
	BOOST_REQUIRE_EQUAL(map::idToString(object->customId()), "h000");
	BOOST_REQUIRE_EQUAL(object->modifications().size(), 1); // only name field changed
	const map::CustomObjects::Modification *modification = boost::polymorphic_cast<const map::CustomObjects::Modification*>(&object->modifications().at(0));
	BOOST_REQUIRE_EQUAL(map::idToString(modification->valueId()), "unam"); // the values id are taken from "UnitMetaData.slk" first column
	BOOST_REQUIRE_EQUAL(modification->value().type(), map::Value::Type::String);
	BOOST_REQUIRE_EQUAL(modification->value().toString(), "My Unit");
}
