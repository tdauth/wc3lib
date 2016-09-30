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

#define BOOST_TEST_MODULE PalettedTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include "../blp.hpp"

#if not defined(BOOST_TEST_DYN_LINK) and not defined(WINDOWS)
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

/*
 * BLP1 paletted + alpha
 */
BOOST_AUTO_TEST_CASE(HumanCampaignCastle)
{
	ifstream in("HumanCampaignCastle.blp");

	BOOST_REQUIRE(in);

	blp::Blp texture;
	bool valid = true;

	try
	{
		texture.read(in);
	}
	catch (std::exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(texture.format() == blp::Blp::Format::Blp1);
	BOOST_REQUIRE(texture.compression() == blp::Blp::Compression::Paletted);
	BOOST_REQUIRE(texture.pictureType() == blp::Blp::PictureType::PalettedWithAlpha2);
	BOOST_REQUIRE(texture.mipMaps().size() == 10);
	BOOST_REQUIRE(texture.mipMaps()[0].width() == 512);
	BOOST_REQUIRE(texture.mipMaps()[0].height() == 512);
}

BOOST_AUTO_TEST_CASE(HumanCampaignCastleWrite)
{
	ifstream in("HumanCampaignCastle.blp");

	BOOST_REQUIRE(in);

	blp::Blp texture;
	bool valid = true;

	try
	{
		texture.read(in);
	}
	catch (std::exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	in.close();
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(texture.format() == blp::Blp::Format::Blp1);
	BOOST_REQUIRE(texture.compression() == blp::Blp::Compression::Paletted);
	BOOST_REQUIRE(texture.pictureType() == blp::Blp::PictureType::PalettedWithAlpha2);
	BOOST_REQUIRE(texture.mipMaps().size() == 10);
	BOOST_REQUIRE(texture.mipMaps()[0].width() == 512);
	BOOST_REQUIRE(texture.mipMaps()[0].height() == 512);

	ofstream out("HumanCampaignCastleOut.blp", std::ios::out | std::ios::binary);
	BOOST_REQUIRE(out);

	try
	{
		texture.write(out);
	}
	catch (std::exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	out.close();
	BOOST_REQUIRE(valid);

	in.open("HumanCampaignCastleOut.blp", std::ios::in | std::ios::binary);

	BOOST_REQUIRE(in);
	try
	{
		texture.read(in);
	}
	catch (std::exception &e)
	{
		valid = false;

		std::cerr << e.what() << std::endl;
	}

	in.close();
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(texture.format() == blp::Blp::Format::Blp1);
	BOOST_REQUIRE(texture.compression() == blp::Blp::Compression::Paletted);
	BOOST_REQUIRE(texture.pictureType() == blp::Blp::PictureType::PalettedWithAlpha2);
	BOOST_REQUIRE(texture.mipMaps().size() == 10);
	BOOST_REQUIRE(texture.mipMaps()[0].width() == 512);
	BOOST_REQUIRE(texture.mipMaps()[0].height() == 512);
}
