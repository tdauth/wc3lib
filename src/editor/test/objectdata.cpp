/***************************************************************************
 *   Copyright (C) 2015 by Tamino Dauth                                    *
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

#ifndef WC3_DIR
#error Missing Warcraft III directory definition.
#endif

/**
 * \file
 * Unit Test which tests \ref ObjectData and its sub classes with original Warcraft III data.
 */
#define BOOST_TEST_MODULE ObjectData
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <QCoreApplication>

#include "../mpqprioritylist.hpp"
#include "../objectdata.hpp"
#include "../unitdata.hpp"
#include "../itemdata.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::editor;

 /*
 * Create a Qt Desktop application that the desktop widget can be passed as widget for the synchronous download.
 * TODO start a non GUI application with Qt5 that the test case works without a X11 server.
 */
struct Fixture
{
	Fixture() : app(0, 0)
	{
		BOOST_TEST_MESSAGE( "setup fixture" );
		QUrl war3mpq = QUrl(WC3_DIR) + "/war3.mpq";
		war3mpq.setScheme("mpq");
		std::cerr << war3mpq.toEncoded().constData() << std::endl;
		BOOST_REQUIRE(war3mpq.isValid());
		BOOST_REQUIRE(source.addSource(war3mpq));
	}

	~Fixture()
	{
		BOOST_TEST_MESSAGE( "teardown fixture" );
	}

	QCoreApplication app;
	MpqPriorityList source;
};

/*
 * Tests the unit data with "war3.mpq" which means Warcraft III: Reign of Chaos.
 */
BOOST_FIXTURE_TEST_CASE(ReadUnitDataReignOfChaos, Fixture)
{
	UnitData unitData(&source);
	bool valid = true;

	/*
	 * The object tab data "UI/UnitEditorData.txt" is required for the WE_STRING entries for field types which are used by units.
	 * It has to be load manually before loading the unit data.
	 */
	BOOST_REQUIRE(unitData.objectTabData() != 0);
	BOOST_REQUIRE(source.sharedData().get() != 0);
	BOOST_REQUIRE(source.sharedData()->source() != 0);
	BOOST_REQUIRE(source.sharedData()->sharedObjectData().get() != 0);
	BOOST_REQUIRE(source.sharedData()->sharedObjectData()->source() != 0);
	BOOST_REQUIRE(source.sharedData()->sharedObjectData()->unitEditorData().get() != 0);

	try
	{
		source.sharedData()->sharedObjectData()->unitEditorData()->setSource(&source);
		source.sharedData()->sharedObjectData()->unitEditorData()->load();
	}
	catch (const Exception &e)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(source.sharedData()->sharedObjectData()->unitEditorData()->source() != 0);
	BOOST_REQUIRE(!unitData.objectTabData()->isEmpty());

	/*
	 * Now load the unit data.
	 */
	valid = true;

	try
	{
		unitData.load(QApplication::desktop());
	}
	catch (Exception &e)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);

	/*
	 * Check human peasant values of different data types.
	 */
	BOOST_CHECK(unitData.defaultFieldValue("hpea", "umvs") == "190");
	BOOST_CHECK(unitData.defaultFieldValue("hpea", "umvt") == "foot");
	BOOST_CHECK(unitData.fieldReadableValue("hpea", "", "umvt") == "WESTRING_UE_MOVETYPE_FOOT");
}

/*
 * Tests the item data with "war3.mpq" which means Warcraft III: Reign of Chaos.
 */
BOOST_FIXTURE_TEST_CASE(ReadItemDataReignOfChaos, Fixture)
{
	ItemData itemData(&source);
	bool valid = true;

	/*
	 * Now load the item data.
	 */
	valid = true;

	try
	{
		itemData.load(QApplication::desktop());
	}
	catch (Exception &e)
	{
		valid = false;
	}

	BOOST_REQUIRE(valid);

	/*
	 * Check "Alleria Flute".
	 */
	BOOST_CHECK(itemData.defaultFieldValue("afac", "iabi") == "AIar");
	BOOST_CHECK(itemData.fieldValue("afac", "", "iabi") == "AIar");
}
