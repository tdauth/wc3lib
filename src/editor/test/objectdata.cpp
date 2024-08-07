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
 * Unit Test which tests \ref wc3lib::editor::ObjectData and its sub classes with original Warcraft III data.
 */
#define BOOST_TEST_MODULE ObjectData
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <QCoreApplication>
#include <QDebug>

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
	int args = 0;

	Fixture() : app(args, 0)
	{
		BOOST_TEST_MESSAGE("setup fixture");
		const QDir dir(WC3_DIR);
		std::cerr << "Warcraft III dir is: " << WC3_DIR << std::endl;
		BOOST_REQUIRE(dir.exists());
		BOOST_REQUIRE(dir.isReadable());
		QUrl war3mpq = QString(WC3_DIR) + "/war3.mpq";
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

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

/*
 * Tests the unit data with "war3.mpq" which means Warcraft III: Reign of Chaos.
 */
BOOST_AUTO_TEST_CASE(ReadUnitDataReignOfChaos)
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
		qDebug() << e.what();

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
		unitData.load(nullptr);
	}
	catch (const Exception &e)
	{
		qDebug() << e.what();

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
BOOST_AUTO_TEST_CASE(ReadItemDataReignOfChaos)
{
	ItemData itemData(&source);
	bool valid = true;

	/*
	 * Now load the item data.
	 */
	valid = true;

	try
	{
		itemData.load(nullptr);
	}
	catch (const Exception &e)
	{
		qDebug() << e.what();

		valid = false;
	}

	BOOST_REQUIRE(valid);

	/*
	 * Check "Alleria Flute".
	 */
	BOOST_CHECK_EQUAL(itemData.defaultFieldValue("afac", "iabi").toStdString(), "AIar");
	BOOST_CHECK_EQUAL(itemData.fieldValue("afac", "", "iabi").toStdString(), "AIar");
}

BOOST_AUTO_TEST_CASE(Validate)
{
	UnitData unitData(&source);

	BOOST_REQUIRE_NO_THROW(unitData.load(nullptr));

	const QStringList errors = unitData.validateTooltipReferences();

	BOOST_REQUIRE(errors.isEmpty());
}

BOOST_AUTO_TEST_SUITE_END()