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

/**
 * \file
 * Unit Test which tests \ref wc3lib::editor::MetaData class.
 */
#define BOOST_TEST_MODULE MetaData
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/filesystem.hpp>

#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::editor;

BOOST_AUTO_TEST_CASE(Txt)
{
	const boost::filesystem::path current = boost::filesystem::current_path() / "WorldEditStrings.txt";
	MetaData metaData(KUrl(current.c_str()));
	MpqPriorityList source;
	metaData.setSource(&source);
	bool success = true;

	try
	{
		metaData.load();
	}
	catch (Exception &e)
	{
		success = false;

		std::cerr << e.what() << std::endl;
	}


	BOOST_REQUIRE(success);
	BOOST_REQUIRE(metaData.hasTxt());
	BOOST_REQUIRE(!metaData.hasSlk());
	BOOST_REQUIRE(metaData.hasValue("WorldEditStrings", "WESTRING_LOADINGSCREEN_GENERIC"));
	BOOST_REQUIRE(metaData.value("WorldEditStrings", "WESTRING_LOADINGSCREEN_GENERIC") == "Standard");
}

BOOST_AUTO_TEST_CASE(Slk)
{
	const boost::filesystem::path current = boost::filesystem::current_path() / "UnitMetaData.slk";
	MetaData metaData(KUrl(current.c_str()));
	MpqPriorityList source;
	metaData.setSource(&source);
	bool success = true;

	try
	{
		metaData.load();
	}
	catch (Exception &e)
	{
		success = false;

		std::cerr << e.what() << std::endl;
	}


	BOOST_REQUIRE(success);
	BOOST_REQUIRE(!metaData.hasTxt());
	BOOST_REQUIRE(metaData.hasSlk());
	BOOST_REQUIRE(metaData.hasValue("ihtp", "useSpecific"));
	BOOST_REQUIRE(metaData.value("ihtp", "useSpecific") == "");
}
