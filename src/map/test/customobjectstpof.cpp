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

#define BOOST_TEST_MODULE CustomObjectsCollectionTestThePowerOfFire
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

/**
 * \file
 * For this Unit Test a custom objects file has been created with one custom object
 * per category with a changed named field only.
 */

struct MyConfig
{
	MyConfig()
	{
		BOOST_TEST_MESSAGE("global setup");

		ifstream in("customobjectstpof.w3o", ifstream::in | ifstream::binary);

		if (in)
		{
			valid = true;

			try
			{
				collection.read(in);
			}
			catch (Exception &e)
			{
				valid = false;
				std::cerr << e.what() << std::endl;
			}
		}
		else
		{
			valid = false;
		}
	}

	~MyConfig()
	{
		BOOST_TEST_MESSAGE("global teardown");

		collection.clear();
		valid = true;
	}

	static map::CustomObjectsCollection collection;
	static bool valid;
};

map::CustomObjectsCollection MyConfig::collection;
bool MyConfig::valid = true;

BOOST_FIXTURE_TEST_SUITE(TestSuite, MyConfig)

BOOST_AUTO_TEST_CASE(Abilities)
{
	BOOST_REQUIRE(strcmp(MyConfig::collection.fileExtension(), ".w3o") == 0);
	BOOST_REQUIRE(MyConfig::valid);
	BOOST_REQUIRE(MyConfig::collection.hasAbilities());
	BOOST_REQUIRE(MyConfig::collection.abilities().get() != 0);
	BOOST_REQUIRE(MyConfig::collection.abilities()->type() == map::CustomObjects::Type::Abilities);
	BOOST_REQUIRE(strcmp(MyConfig::collection.abilities()->fileName(), "war3map.w3a") == 0);

	const map::CustomObjects::Object *concentrationAbility = nullptr;

	for (map::CustomObjects::Table::const_iterator iterator = MyConfig::collection.abilities()->customTable().begin(); iterator != MyConfig::collection.abilities()->customTable().end(); ++iterator)
	{
		const map::CustomObjects::Object *object = boost::polymorphic_cast<const map::CustomObjects::Object*>(&(*iterator));

		if (map::idToString(object->customId()) == "A02A")
		{
			concentrationAbility = object;

			break;
		}
	}

	BOOST_REQUIRE(concentrationAbility != nullptr);

	const map::CustomObjects::Modification *aherModification = nullptr;

	for (map::CustomObjects::Object::Modifications::const_iterator iterator = concentrationAbility->modifications().begin(); iterator != concentrationAbility->modifications().end(); ++iterator)
	{
		const map::CustomObjects::Modification *modification = boost::polymorphic_cast<const map::CustomObjects::Modification*>(&(*iterator));

		if (map::idToString(modification->valueId()) == "aher")
		{
			aherModification = modification;

			break;
		}
	}

	BOOST_REQUIRE(aherModification != nullptr);
	BOOST_REQUIRE(aherModification->value().type() == map::Value::Type::Integer);
	BOOST_REQUIRE(!aherModification->value().toInteger());
}

BOOST_AUTO_TEST_SUITE_END()
