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

#if not defined(BOOST_TEST_DYN_LINK) and not defined(WINDOWS)
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

#ifndef WC3_DIR
#error Define WC3_DIR.
#endif

BOOST_AUTO_TEST_CASE(Validate)
{
	const std::string wc3Dir = WC3_DIR;
	const std::string command = std::string("../wc3objectvalidate " + wc3Dir + " war3map_de.wts ObjectData.w3o");
	std::cout << "Running command: " << command << std::endl;
	const int result = system(command.c_str());

	BOOST_REQUIRE_EQUAL(result, 0);
}
