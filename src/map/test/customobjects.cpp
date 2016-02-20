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

#define BOOST_TEST_MODULE CustomObjectsCollectionTest
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

		ifstream in("customobjects.w3o", ifstream::in | ifstream::binary);

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

BOOST_AUTO_TEST_CASE(Units)
{
	BOOST_REQUIRE(strcmp(MyConfig::collection.fileExtension(), ".w3o") == 0);
	BOOST_REQUIRE(MyConfig::valid);
	BOOST_REQUIRE(MyConfig::collection.hasUnits());
	BOOST_REQUIRE(MyConfig::collection.units().get() != 0);
	BOOST_REQUIRE(MyConfig::collection.units()->type() == map::CustomObjects::Type::Units);
	BOOST_REQUIRE(strcmp(MyConfig::collection.units()->fileName(), "war3map.w3u") == 0);
	BOOST_REQUIRE(MyConfig::collection.units()->customTable().size() == 1);
	const map::CustomObjects::Object *object = boost::polymorphic_cast<map::CustomObjects::Object*>(&MyConfig::collection.units()->customTable().at(0));
	BOOST_REQUIRE(object->type() == map::CustomObjects::Type::Units);
	BOOST_REQUIRE(map::idToString(object->originalId()) == "hpea"); // footman id
	BOOST_REQUIRE(map::idToString(object->customId()) == "h000");
	BOOST_REQUIRE(object->modifications().size() == 1); // only name field changed
	const map::CustomObjects::Modification *modification = boost::polymorphic_cast<const map::CustomObjects::Modification*>(&object->modifications().at(0));
	BOOST_REQUIRE(map::idToString(modification->valueId()) == "unam"); // the values id are taken from "UnitMetaData.slk" first column
	BOOST_REQUIRE(modification->value().type() == map::Value::Type::String);
	BOOST_REQUIRE(modification->value().toString() == "My Unit");
}

BOOST_AUTO_TEST_CASE(Items)
{
	BOOST_REQUIRE(strcmp(MyConfig::collection.fileExtension(), ".w3o") == 0);
	BOOST_REQUIRE(MyConfig::valid);
	BOOST_REQUIRE(MyConfig::collection.hasItems());
	BOOST_REQUIRE(MyConfig::collection.items().get() != 0);
	BOOST_REQUIRE(MyConfig::collection.items()->type() == map::CustomObjects::Type::Items);
	BOOST_REQUIRE(strcmp(MyConfig::collection.items()->fileName(), "war3map.w3t") == 0);
	BOOST_REQUIRE(MyConfig::collection.items()->customTable().size() == 1);
	const map::CustomObjects::Object *object = boost::polymorphic_cast<map::CustomObjects::Object*>(&MyConfig::collection.items()->customTable().at(0));
	BOOST_REQUIRE(object->type() == map::CustomObjects::Type::Items);
	BOOST_REQUIRE(map::idToString(object->originalId()) == "tkno"); // tome of power
	BOOST_REQUIRE(map::idToString(object->customId()) == "I000");
	BOOST_REQUIRE(object->modifications().size() == 1); // only name field changed
	const map::CustomObjects::Modification *modification = boost::polymorphic_cast<const map::CustomObjects::Modification*>(&object->modifications().at(0));
	BOOST_REQUIRE(map::idToString(modification->valueId()) == "unam"); // the values id are taken from "UnitMetaData.slk" first column
	BOOST_REQUIRE(modification->value().type() == map::Value::Type::String);
	BOOST_REQUIRE(modification->value().toString() == "My Item");
}

BOOST_AUTO_TEST_CASE(Destructibles)
{
	BOOST_REQUIRE(strcmp(MyConfig::collection.fileExtension(), ".w3o") == 0);
	BOOST_REQUIRE(MyConfig::valid);
	BOOST_REQUIRE(MyConfig::collection.hasDestructibles());
	BOOST_REQUIRE(MyConfig::collection.destructibles().get() != 0);
	BOOST_REQUIRE(MyConfig::collection.destructibles()->type() == map::CustomObjects::Type::Destructibles);
	BOOST_REQUIRE(strcmp(MyConfig::collection.destructibles()->fileName(), "war3map.w3b") == 0);
	BOOST_REQUIRE(MyConfig::collection.destructibles()->customTable().size() == 1);
	const map::CustomObjects::Object *object = boost::polymorphic_cast<map::CustomObjects::Object*>(&MyConfig::collection.destructibles()->customTable().at(0));
	BOOST_REQUIRE(object->type() == map::CustomObjects::Type::Destructibles);
	BOOST_REQUIRE(map::idToString(object->originalId()) == "DTrf"); // first destructible
	BOOST_REQUIRE(map::idToString(object->customId()) == "B000");
	BOOST_REQUIRE(object->modifications().size() == 1); // only name field changed
	const map::CustomObjects::Modification *modification = boost::polymorphic_cast<const map::CustomObjects::Modification*>(&object->modifications().at(0));
	BOOST_REQUIRE(map::idToString(modification->valueId()) == "bnam"); // the values id are taken from "DestructableMetaData.slk" first column
	BOOST_REQUIRE(modification->value().type() == map::Value::Type::String);
	BOOST_REQUIRE(modification->value().toString() == "My Destructible");
}

BOOST_AUTO_TEST_CASE(Doodads)
{
	BOOST_REQUIRE(strcmp(MyConfig::collection.fileExtension(), ".w3o") == 0);
	BOOST_REQUIRE(MyConfig::valid);
	BOOST_REQUIRE(MyConfig::collection.hasDoodads());
	BOOST_REQUIRE(MyConfig::collection.doodads().get() != 0);
	BOOST_REQUIRE(MyConfig::collection.doodads()->type() == map::CustomObjects::Type::Doodads);
	BOOST_REQUIRE(strcmp(MyConfig::collection.doodads()->fileName(), "war3map.w3d") == 0);
	BOOST_REQUIRE(MyConfig::collection.doodads()->customTable().size() == 1);
	const map::CustomObjects::Object *object = boost::polymorphic_cast<map::CustomObjects::Object*>(&MyConfig::collection.doodads()->customTable().at(0));
	BOOST_REQUIRE(object->type() == map::CustomObjects::Type::Doodads);
	BOOST_REQUIRE(map::idToString(object->originalId()) == "ASbl"); // first doodad
	BOOST_REQUIRE(map::idToString(object->customId()) == "D000");
	BOOST_REQUIRE(object->modifications().size() == 1); // only name field changed
	const map::CustomObjects::Modification *modification = boost::polymorphic_cast<const map::CustomObjects::Modification*>(&object->modifications().at(0));
	BOOST_REQUIRE(map::idToString(modification->valueId()) == "dnam"); // the values id are taken from "Doodads/DoodadMetaData.slk" first column
	BOOST_REQUIRE(modification->value().type() == map::Value::Type::String);
	BOOST_REQUIRE(modification->value().toString() == "My Doodad");
}

BOOST_AUTO_TEST_CASE(Abilities)
{
	BOOST_REQUIRE(strcmp(MyConfig::collection.fileExtension(), ".w3o") == 0);
	BOOST_REQUIRE(MyConfig::valid);
	BOOST_REQUIRE(MyConfig::collection.hasAbilities());
	BOOST_REQUIRE(MyConfig::collection.abilities().get() != 0);
	BOOST_REQUIRE(MyConfig::collection.abilities()->type() == map::CustomObjects::Type::Abilities);
	BOOST_REQUIRE(strcmp(MyConfig::collection.abilities()->fileName(), "war3map.w3a") == 0);
	BOOST_REQUIRE(MyConfig::collection.abilities()->customTable().size() == 1);
	const map::CustomObjects::Object *object = boost::polymorphic_cast<map::CustomObjects::Object*>(&MyConfig::collection.abilities()->customTable().at(0));
	BOOST_REQUIRE(object->type() == map::CustomObjects::Type::Abilities);
	BOOST_REQUIRE(map::idToString(object->originalId()) == "AUcb"); // first ability
	BOOST_REQUIRE(map::idToString(object->customId()) == "A000");
	BOOST_REQUIRE(object->modifications().size() == 1); // only name field changed
	const map::CustomObjects::Modification *modification = boost::polymorphic_cast<const map::CustomObjects::Modification*>(&object->modifications().at(0));
	BOOST_REQUIRE(map::idToString(modification->valueId()) == "anam"); // the values id are taken from "AbilityMetaData.slk" first column
	BOOST_REQUIRE(modification->value().type() == map::Value::Type::String);
	BOOST_REQUIRE(modification->value().toString() == "My Ability");
}

BOOST_AUTO_TEST_CASE(Buffs)
{
	BOOST_REQUIRE(strcmp(MyConfig::collection.fileExtension(), ".w3o") == 0);
	BOOST_REQUIRE(MyConfig::valid);
	BOOST_REQUIRE(MyConfig::collection.hasBuffs());
	BOOST_REQUIRE(MyConfig::collection.buffs().get() != 0);
	BOOST_REQUIRE(MyConfig::collection.buffs()->type() == map::CustomObjects::Type::Buffs);
	BOOST_REQUIRE(strcmp(MyConfig::collection.buffs()->fileName(), "war3map.w3h") == 0);
	BOOST_REQUIRE(MyConfig::collection.buffs()->customTable().size() == 1);
	const map::CustomObjects::Object *object = boost::polymorphic_cast<map::CustomObjects::Object*>(&MyConfig::collection.buffs()->customTable().at(0));
	BOOST_REQUIRE(object->type() == map::CustomObjects::Type::Buffs);
	BOOST_REQUIRE(map::idToString(object->originalId()) == "BUcb"); // first ability
	BOOST_REQUIRE(map::idToString(object->customId()) == "B000");
	BOOST_REQUIRE(object->modifications().size() == 1); // only name field changed
	const map::CustomObjects::Modification *modification = boost::polymorphic_cast<const map::CustomObjects::Modification*>(&object->modifications().at(0));
	BOOST_REQUIRE(map::idToString(modification->valueId()) == "fnam"); // the values id are taken from "Units\AbilityBuffMetaData.slk" first column NOTE the file is not listed in the listfile
	BOOST_REQUIRE(modification->value().type() == map::Value::Type::String);
	BOOST_REQUIRE(modification->value().toString() == "My Buff");
}

BOOST_AUTO_TEST_CASE(Upgrades)
{
	BOOST_REQUIRE(strcmp(MyConfig::collection.fileExtension(), ".w3o") == 0);
	BOOST_REQUIRE(MyConfig::valid);
	BOOST_REQUIRE(MyConfig::collection.hasUpgrades());
	BOOST_REQUIRE(MyConfig::collection.upgrades().get() != 0);
	BOOST_REQUIRE(MyConfig::collection.upgrades()->type() == map::CustomObjects::Type::Upgrades);
	BOOST_REQUIRE(strcmp(MyConfig::collection.upgrades()->fileName(), "war3map.w3q") == 0);
	BOOST_REQUIRE(MyConfig::collection.upgrades()->customTable().size() == 1);
	const map::CustomObjects::Object *object = boost::polymorphic_cast<map::CustomObjects::Object*>(&MyConfig::collection.upgrades()->customTable().at(0));
	BOOST_REQUIRE(object->type() == map::CustomObjects::Type::Upgrades);
	BOOST_REQUIRE(map::idToString(object->originalId()) == "Rhme"); // first upgrade
	BOOST_REQUIRE(map::idToString(object->customId()) == "R000");
	BOOST_REQUIRE(object->modifications().size() == 1); // only name field changed
	const map::CustomObjects::Modification *modification = boost::polymorphic_cast<const map::CustomObjects::Modification*>(&object->modifications().at(0));
	BOOST_REQUIRE(map::idToString(modification->valueId()) == "gnam"); // the values id are taken from "UpgradeMetaData.slk" first column
	BOOST_REQUIRE(modification->value().type() == map::Value::Type::String);
	BOOST_REQUIRE(modification->value().toString() == "My Upgrade");
}

/*
 * Test whether the copy constructors and the polymorphic cloning works without crashes and losing data.
 */
BOOST_AUTO_TEST_CASE(Cloning)
{
	BOOST_REQUIRE(strcmp(MyConfig::collection.fileExtension(), ".w3o") == 0);
	BOOST_REQUIRE(MyConfig::valid);
	BOOST_REQUIRE(MyConfig::collection.hasUnits());
	BOOST_REQUIRE(MyConfig::collection.units().get() != 0);
	BOOST_REQUIRE(MyConfig::collection.units()->type() == map::CustomObjects::Type::Units);
	BOOST_REQUIRE(strcmp(MyConfig::collection.units()->fileName(), "war3map.w3u") == 0);
	BOOST_REQUIRE(MyConfig::collection.units()->customTable().size() == 1);

	map::CustomObjectsCollection clonedCollection;
	map::CustomObjects *units = new map::CustomObjects(*MyConfig::collection.units());
	BOOST_REQUIRE(units != nullptr);
	clonedCollection.units().reset(units);
	BOOST_REQUIRE(clonedCollection.hasUnits());
	BOOST_REQUIRE(clonedCollection.units().get() != nullptr);
	BOOST_REQUIRE(clonedCollection.units()->type() == map::CustomObjects::Type::Units);
	BOOST_REQUIRE(strcmp(clonedCollection.units()->fileName(), "war3map.w3u") == 0);
	BOOST_REQUIRE(clonedCollection.units()->customTable().size() == 1);
}

BOOST_AUTO_TEST_SUITE_END()
