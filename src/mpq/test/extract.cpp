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

#define BOOST_TEST_MODULE ExtractTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>
#include <chrono>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include "../archive.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

#ifndef WC3_DIR
#error Define WC3_DIR.
#endif

using namespace wc3lib;

/**
 * Extracts the file "Units\OrcUpgradeStrings.txt" from War3Patch.mpq
 * which led to some errors. This has to work otherwise object data cannot be inspected.
 */
BOOST_AUTO_TEST_CASE(ExtractOrcUpgradeStrings)
{
	/*
	 * Copy original War3Patch.mpq.
	 * Since the original War3Patch.mpq may not be distributed due to licensing, this test can only be run if Warcraft III: The Frozen Throne is installed.
	 * The installation directory must be specified in "WC3_DIR" which is an environment variable.
	 */
	boost::filesystem::path wc3DirPath = WC3_DIR;
	std::cerr << "wc3 dir" << wc3DirPath << std::endl;
	BOOST_REQUIRE(boost::filesystem::exists(wc3DirPath));
	BOOST_REQUIRE(boost::filesystem::is_directory(wc3DirPath));

	const boost::filesystem::path war3Path = wc3DirPath / "War3xlocal.mpq";

	BOOST_REQUIRE(boost::filesystem::exists(war3Path));

	const  boost::filesystem::path workingDir = boost::filesystem::current_path();

	BOOST_REQUIRE(boost::filesystem::exists(workingDir));
	BOOST_REQUIRE(boost::filesystem::is_directory(workingDir));

	const  boost::filesystem::path target = workingDir / "War3xlocal.mpq";

	/*
	 * Overwrite old copy to get a fresh unmodified file.
	 */
	if (boost::filesystem::exists(target))
	{
		BOOST_REQUIRE(boost::filesystem::is_regular_file(target));
		BOOST_REQUIRE(boost::filesystem::remove(target));
	}

	std::cerr << "Copying " << war3Path << " to " << target << std::endl;

	boost::system::error_code ec;
	boost::filesystem::copy(war3Path, target, ec);

	std::cerr << "Error code: " << ec.message() << std::endl;

	BOOST_REQUIRE(!ec);

	mpq::Archive archive;
	archive.open(target);

	BOOST_REQUIRE(archive.isOpen());

	mpq::File file = archive.findFile("Units\\OrcUpgradeStrings.txt");

	BOOST_REQUIRE(file.isValid());

	stringstream tmpStream0;
	bool valid = true;

	try
	{
		file.decompress(tmpStream0);
	}
	catch (const Exception &e)
	{
		std::cerr << e.what() << std::endl;
		valid = false;
	}

	BOOST_REQUIRE(valid);

	archive.close();
}