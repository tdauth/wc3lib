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

#define BOOST_TEST_MODULE SoundsTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include "../../platform.hpp"
#include "../sounds.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

BOOST_AUTO_TEST_CASE(ReadTest) {
	ifstream in("war3map.w3s", ifstream::in | ifstream::binary); // TestMap.w3m
	
	BOOST_REQUIRE(in);
	
	map::Sounds sounds;
	
	bool valid = true;
	
	try {
		sounds.read(in);
	}
	catch (std::exception &e) {
		valid = false;
		std::cerr << e.what() << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	
	BOOST_REQUIRE(sounds.sounds().size() == 3);
	const map::Sound &sound = sounds.sounds().at(0);
	BOOST_REQUIRE(sound.name() == "gg_snd_RainOfFireLoop1");
	BOOST_REQUIRE(sound.file() == "Abilities\\Spells\\Demon\\RainOfFire\\RainOfFireLoop1.wav");
	BOOST_REQUIRE(sound.usesDefaultVolume());
	BOOST_REQUIRE(sound.flags() & map::Sound::Flags::StopWhenOutOfRange);
	BOOST_REQUIRE(sound.flags() & map::Sound::Flags::Sound3d);
	BOOST_REQUIRE(sound.eaxEffects() == map::Sound::EAX::Spells);
	BOOST_REQUIRE(sound.channel() == map::Sound::Channel::Birth || sound.channel() == map::Sound::Channel::UseDefault);
	// TODO check all sounds and properties
}

BOOST_AUTO_TEST_CASE(ReadWriteReadTest) {
	ifstream in("war3map.w3s", istream::in | ifstream::binary); // Reign of Chaos
	
	BOOST_REQUIRE(in);
	
	map::Sounds sounds;
	
	bool valid = true;
	
	try {
		sounds.read(in);
	}
	catch (std::exception &e) {
		valid = false;
		std::cerr << e.what() << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	
	in.close();
	ofstream out("war3map.w3sout", ifstream::out | ifstream::binary);
	
	BOOST_REQUIRE(out);
	
	try {
		sounds.write(out);
	}
	catch (std::exception &e) {
		valid = false;
		std::cerr << e.what() << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	
	out.close(); // flush file stream
	sounds.sounds().clear(); // ensure it's empty!
	
	in.open("war3map.w3sout", ifstream::in | ifstream::binary); // Reign of Chaos, reopen
	
	BOOST_REQUIRE(in);
	
	try {
		sounds.read(in);
	}
	catch (std::exception &e) {
		valid = false;
		std::cerr << e.what() << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	// TODO check all sounds and properties
}
