/***************************************************************************
 *   Copyright (C) 2013 by Tamino Dauth                                    *
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

#define BOOST_TEST_MODULE TriggersTest
#include <boost/test/unit_test.hpp>

#include "../../spirit.hpp" // enable debug mode

#include <sstream>
#include <iostream>

//#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../triggers.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

BOOST_AUTO_TEST_CASE(TriggerDataReignOfChaos)
{
	ifstream in("TriggerData.txt"); // Warcraft III trigger data

	BOOST_REQUIRE(in);

	map::TriggerData triggerData;

	BOOST_REQUIRE_NO_THROW(triggerData.read(in));

	in.close();

	BOOST_CHECK(strcmp(triggerData.fileName(), "TriggerData.txt") == 0);
	BOOST_CHECK_EQUAL(triggerData.version(), 0); // has no specific version
	BOOST_CHECK_EQUAL(triggerData.latestFileVersion(), 0); // has no specific version

	// data from the file
	BOOST_CHECK_EQUAL(triggerData.categories().size(), 43); // section [TriggerCategories]
	BOOST_REQUIRE_EQUAL(triggerData.types().size(), 133); // section [TriggerTypes]
	// TODO check more sections

	// first type:
	// abilcode=1,1,WESTRING_TRIGTYPE_abilcode,integer
	const map::TriggerData::Type &firstType = *triggerData.types().cbegin()->second;
	BOOST_CHECK(firstType.name() == "abilcode");
	BOOST_CHECK(firstType.canBeGlobal());
	BOOST_CHECK(firstType.canBeCompared());
	BOOST_CHECK(firstType.displayText() == "WESTRING_TRIGTYPE_abilcode");
	BOOST_CHECK(firstType.baseType() != nullptr);
	BOOST_CHECK(firstType.baseType()->name() == "integer");
	BOOST_CHECK(firstType.defaultValue() == "");

	/*
	[DefaultTriggerCategories]
	// Defines categories to be automatically added to new maps
	NumCategories=1

	Category01=WESTRING_INITIALIZATION

	[DefaultTriggers]
	// Defines triggers to be automatically added to new maps
	NumTriggers=1

	// Melee Initialization
	Trigger01Name=WESTRING_MELEEINITIALIZATION
	Trigger01Comment=WESTRING_MELEEINITIALIZATION_COMMENT
	Trigger01Category=1
	Trigger01Events=1
	Trigger01Event01=MapInitializationEvent
	Trigger01Conditions=0
	Trigger01Actions=8
	Trigger01Action01=MeleeStartingVisibility
	Trigger01Action02=MeleeStartingHeroLimit
	Trigger01Action03=MeleeGrantHeroItems
	Trigger01Action04=MeleeStartingResources
	Trigger01Action05=MeleeClearExcessUnits
	Trigger01Action06=MeleeStartingUnits
	Trigger01Action07=MeleeStartingAI
	Trigger01Action08=MeleeInitVictoryDefeat
	*/
	// there is one default trigger for melee maps
	BOOST_REQUIRE(triggerData.defaultTriggerCategories().size() == 1);
	BOOST_REQUIRE(triggerData.defaultTriggers().size() == 1);

	map::TriggerData::DefaultTriggerCategories::const_reference defaultCategory = triggerData.defaultTriggerCategories().front();
	BOOST_CHECK(defaultCategory == "WESTRING_INITIALIZATION");

	map::TriggerData::DefaultTriggers::const_reference defaultTrigger = triggerData.defaultTriggers().front();
	BOOST_CHECK(defaultTrigger.name() == "WESTRING_MELEEINITIALIZATION");
	BOOST_CHECK(defaultTrigger.comment() == "WESTRING_MELEEINITIALIZATION_COMMENT");
	BOOST_CHECK(defaultTrigger.triggerCategory() == "WESTRING_INITIALIZATION");
	BOOST_REQUIRE(defaultTrigger.events().size() == 1);
	BOOST_REQUIRE(defaultTrigger.conditions().size() == 0);
	BOOST_REQUIRE(defaultTrigger.actions().size() == 8);
}

BOOST_AUTO_TEST_CASE(TriggerDataReignOfChaosReadWriteRead)
{
	spiritTraceLog.close();
	spiritTraceLog.open("triggerdata_reign_of_chaos_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("TriggerData.txt"); // Warcraft III trigger data

	BOOST_REQUIRE(in);

	map::TriggerData triggerData;

	BOOST_REQUIRE_NO_THROW(triggerData.read(in));

	in.close();

	BOOST_CHECK(strcmp(triggerData.fileName(), "TriggerData.txt") == 0);
	BOOST_CHECK(triggerData.version() == 0); // has no specific version
	BOOST_CHECK(triggerData.latestFileVersion() == 0); // has no specific version

	// data from the file
	BOOST_CHECK(triggerData.categories().size() == 43); // section [TriggerCategories]
	BOOST_CHECK(triggerData.types().size() == 133); // section [TriggerTypes]
	// TODO check more sections

	// there is one default trigger for melee maps
	BOOST_CHECK(triggerData.defaultTriggerCategories().size() == 1);
	BOOST_CHECK(triggerData.defaultTriggers().size() == 1);

	spiritTraceLog.close();
	spiritTraceLog.open("triggerdata_reign_of_chaos_tmp_out_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ofstream out("TriggerDataTmp.txt");

	BOOST_REQUIRE(out);
	BOOST_REQUIRE_NO_THROW(triggerData.write(out));

	out.close();
	spiritTraceLog.close();
	spiritTraceLog.open("triggerdata_reign_of_chaos_tmp_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	in.open("TriggerDataTmp.txt");

	BOOST_REQUIRE(in);

	triggerData.clear();

	BOOST_REQUIRE(triggerData.categories().empty());
	BOOST_REQUIRE(triggerData.types().empty());
	BOOST_REQUIRE(triggerData.defaultTriggerCategories().empty());
	BOOST_REQUIRE(triggerData.defaultTriggers().empty());

	// TODO check more sections

	BOOST_REQUIRE_NO_THROW(triggerData.read(in));

	in.close();

	BOOST_CHECK_EQUAL(strcmp(triggerData.fileName(), "TriggerData.txt"), 0);
	BOOST_CHECK_EQUAL(triggerData.version(), 0); // has no specific version
	BOOST_CHECK_EQUAL(triggerData.latestFileVersion(), 0); // has no specific version

	// data from the file
	BOOST_CHECK_EQUAL(triggerData.categories().size(), 43); // section [TriggerCategories]
	BOOST_CHECK_EQUAL(triggerData.types().size(), 133); // section [TriggerTypes]
	// TODO check more sections

	// there is one default trigger for melee maps
	BOOST_CHECK_EQUAL(triggerData.defaultTriggerCategories().size(), 1);
	BOOST_CHECK_EQUAL(triggerData.defaultTriggers().size(), 1);
}

BOOST_AUTO_TEST_CASE(WarChasersTriggersSimpleReadTest)
{
	ifstream in("TriggerData.txt"); // Warcraft III trigger data

	BOOST_REQUIRE(in);

	map::TriggerData triggerData;

	BOOST_REQUIRE_NO_THROW(triggerData.read(in));

	in.close();

	ifstream inTriggers("war3map.wtg", std::ios::in | std::ios::binary); // War Chasers triggers
	map::Triggers triggers;

	BOOST_REQUIRE_NO_THROW(triggers.read(inTriggers, triggerData));

	BOOST_CHECK_EQUAL(strcmp(triggers.fileName(), "war3map.wtg"), 0);
	BOOST_CHECK_EQUAL(memcmp(triggers.fileTextId(), "WTG!", 4), 0);
	BOOST_CHECK_EQUAL(triggers.version(), 4); // Reign of Chaos

	// data from the object manager
	BOOST_CHECK_EQUAL(triggers.variables().size(), 54);
	BOOST_CHECK_EQUAL(triggers.triggers().size(), 151);
	BOOST_CHECK_EQUAL(triggers.categories().size(), 19);

	// TODO check single trigger
}

BOOST_AUTO_TEST_CASE(WarChasersTriggersSimpleReadTestWithFrozenThroneTriggerData)
{
	ifstream in("TriggerDataEx.txt"); // Warcraft III: The Frozen Throne trigger data

	BOOST_REQUIRE(in);

	map::TriggerData triggerData;

	BOOST_REQUIRE_NO_THROW(triggerData.read(in));

	in.close();

	ifstream inTriggers("war3map.wtg", std::ios::in | std::ios::binary); // War Chasers triggers
	map::Triggers triggers;

	BOOST_REQUIRE_NO_THROW(triggers.read(inTriggers, triggerData));

	BOOST_CHECK_EQUAL(strcmp(triggers.fileName(), "war3map.wtg"), 0);
	BOOST_CHECK_EQUAL(memcmp(triggers.fileTextId(), "WTG!", 4), 0);
	BOOST_CHECK_EQUAL(triggers.version(), 4); // Reign of Chaos

	// data from the object manager
	BOOST_CHECK_EQUAL(triggers.variables().size(), 54);
	BOOST_CHECK_EQUAL(triggers.triggers().size(), 151);
	BOOST_CHECK_EQUAL(triggers.categories().size(),19);

	// TODO check single trigger
}
