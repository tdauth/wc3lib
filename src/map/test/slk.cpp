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

#define BOOST_TEST_MODULE SlkTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/chrono.hpp>

#include "../../spirit.hpp"
#include "../../platform.hpp"
#include "../slk.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

BOOST_AUTO_TEST_CASE(UnitAbilities)
{
	spiritTraceLog.close();
	spiritTraceLog.open("unitabilities_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("UnitAbilities.slk");

	BOOST_REQUIRE(in);

	map::Slk slk;

	bool valid = true;

	try
	{
		slk.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(slk.columns() == 7);
	BOOST_REQUIRE(slk.rows() == 325);
	std::cerr << "Cell 0x0:" << slk.cell(0, 0) << std::endl;
	BOOST_REQUIRE(slk.cell(0, 0) == "\"unitAbilID\"");
}

BOOST_AUTO_TEST_CASE(UnitBalance)
{
	spiritTraceLog.close();
	spiritTraceLog.open("unitbalance_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("UnitBalance.slk");

	BOOST_REQUIRE(in);

	map::Slk slk;

	bool valid = true;

	try
	{
		slk.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(slk.columns() == 44);
	BOOST_REQUIRE(slk.rows() == 467);
	BOOST_REQUIRE(slk.cell(0, 0) == "\"unitBalanceID\"");
}

BOOST_AUTO_TEST_CASE(UnitData)
{
	spiritTraceLog.close();
	spiritTraceLog.open("unitdata_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("UnitData.slk");

	BOOST_REQUIRE(in);

	map::Slk slk;

	bool valid = true;

	try
	{
		slk.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(slk.columns() == 34);
	BOOST_REQUIRE(slk.rows() == 467);
	BOOST_REQUIRE(slk.cell(0, 0) == "\"unitID\"");

	/*
	 * Moon Priestess entry uses F records to set the position before
	 * specifying the values with K records.
	 */
	BOOST_REQUIRE(slk.cell(66, 2) == "\"HeroMoonPriestess\"");
}

BOOST_AUTO_TEST_CASE(UnitMetaData)
{
	spiritTraceLog.close();
	spiritTraceLog.open("unitmetadata_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("UnitMetaData.slk");

	BOOST_REQUIRE(in);

	map::Slk slk;

	bool valid = true;

	try
	{
		slk.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	// 17 columns
	// 118 rows
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(slk.columns() == 17);
	BOOST_REQUIRE(slk.rows() == 118);
	BOOST_REQUIRE(slk.cell(0, 0) == "\"ID\"");
	BOOST_REQUIRE(slk.cell(0, 1) == "\"field\"");
}

BOOST_AUTO_TEST_CASE(UnitMetaDataLibreOffice)
{
	spiritTraceLog.close();
	spiritTraceLog.open("unitmetadatalibreoffice_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("UnitMetaDataExportedByLibreOffice.slk");

	BOOST_REQUIRE(in);

	map::Slk slk;

	bool valid = true;

	try
	{
		slk.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	// 17 columns
	// 118 rows
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(slk.columns() == 17);
	BOOST_REQUIRE(slk.rows() == 118);
	BOOST_REQUIRE(slk.cell(0, 0) == "\"ID\"");
	BOOST_REQUIRE(slk.cell(0, 1) == "\"field\"");
}

BOOST_AUTO_TEST_CASE(UnitWeapons)
{
	spiritTraceLog.close();
	spiritTraceLog.open("unitweapons_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("UnitWeapons.slk");

	BOOST_REQUIRE(in);

	map::Slk slk;

	bool valid = true;

	try
	{
		slk.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(slk.columns() == 67);
	BOOST_REQUIRE(slk.rows() == 317);
	BOOST_REQUIRE(slk.cell(0, 0) == "\"unitWeapID\"");
	BOOST_REQUIRE(slk.cell(0, 1) == "\"sortWeap\"");
}

/*
 * Frozen Thronw
 */
BOOST_AUTO_TEST_CASE(UnitMetaDataEX)
{
	spiritTraceLog.close();
	spiritTraceLog.open("unitmetadataex_traces.xml");

	BOOST_REQUIRE(spiritTraceLog);

	ifstream in("UnitMetaDataEx.slk");

	BOOST_REQUIRE(in);

	map::Slk slk;

	bool valid = true;

	try
	{
		slk.read(in);
	}
	catch (Exception e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(slk.columns() == 23);
	BOOST_REQUIRE(slk.rows() == 249);
	BOOST_REQUIRE(slk.cell(0, 0) == "\"ID\"");
	BOOST_REQUIRE(slk.cell(0, 1) == "\"field\"");
}
