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

#define BOOST_TEST_MODULE Wc3ObjectValidateTest
#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <iostream>

#include "../../editor.hpp"

#if not defined(BOOST_TEST_DYN_LINK) and not defined(WINDOWS)
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

#ifndef WC3_DIR
#error Define WC3_DIR.
#endif

using namespace wc3lib;
using namespace wc3lib::editor;

namespace
{

void loadObjectData(ObjectData &objectData, const map::CustomObjects &customObjects, const QUrl &stringsUrl)
{
	objectData.load(nullptr);
	objectData.importCustomObjects(customObjects);
	objectData.applyMapStrings(stringsUrl);
}

}

BOOST_AUTO_TEST_CASE(ValidateManually)
{
	const std::string wc3Dir = WC3_DIR;

	const QDir dir(wc3Dir.c_str());

	BOOST_REQUIRE(dir.exists());

	MpqPriorityList source;
	BOOST_REQUIRE(source.addSource(QUrl::fromLocalFile(WC3_DIR)));

	const QFileInfo fileInfo("./war3map_de.wts");
	BOOST_REQUIRE(fileInfo.exists());
	const QUrl stringsUrl = QUrl::fromLocalFile("./war3map_de.wts");

	map::CustomObjectsCollection collection;
	ifstream in("ObjectData.w3o");

	BOOST_REQUIRE(in);

	BOOST_REQUIRE_NO_THROW(collection.read(in));

	BOOST_REQUIRE(collection.hasUnits());
	BOOST_REQUIRE_NO_THROW(loadObjectData(*source.sharedData()->sharedObjectData()->unitData(), *collection.units(), stringsUrl));

	BOOST_REQUIRE(collection.hasItems());
	BOOST_REQUIRE_NO_THROW(loadObjectData(*source.sharedData()->sharedObjectData()->itemData(), *collection.items(), stringsUrl));

	BOOST_REQUIRE(collection.hasAbilities());
	BOOST_REQUIRE_NO_THROW(loadObjectData(*source.sharedData()->sharedObjectData()->abilityData(), *collection.abilities(), stringsUrl));

	BOOST_REQUIRE(collection.hasDestructibles());
	BOOST_REQUIRE_NO_THROW(loadObjectData(*source.sharedData()->sharedObjectData()->destructableData(), *collection.destructibles(), stringsUrl));

	BOOST_REQUIRE(collection.hasDoodads());
	BOOST_REQUIRE_NO_THROW(loadObjectData(*source.sharedData()->sharedObjectData()->doodadData(), *collection.doodads(), stringsUrl));

	BOOST_REQUIRE(collection.hasBuffs());
	BOOST_REQUIRE_NO_THROW(loadObjectData(*source.sharedData()->sharedObjectData()->buffData(), *collection.buffs(), stringsUrl));

	BOOST_REQUIRE(collection.hasUpgrades());
	BOOST_REQUIRE_NO_THROW(loadObjectData(*source.sharedData()->sharedObjectData()->upgradeData(), *collection.upgrades(), stringsUrl));

	const QStringList errors = source.sharedData()->sharedObjectData()->unitData()->validateTooltipReferences();

	BOOST_REQUIRE_EQUAL(errors.size(), 0);
}

BOOST_AUTO_TEST_CASE(Validate)
{
	const std::string wc3Dir = WC3_DIR;

	const QDir dir(wc3Dir.c_str());

	BOOST_REQUIRE(dir.exists());

	const std::string command = std::string("../wc3objectvalidate \"" + wc3Dir + "\" war3map_de.wts ObjectData.w3o");
	std::cout << "Running command: " << command << std::endl;
	const int result = system(command.c_str());

	BOOST_REQUIRE_EQUAL(result, 0);
}
