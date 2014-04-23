/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#include "sound.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize Sound::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::readString(istream, this->m_name, size);
	wc3lib::readString(istream, this->m_file, size);
	string eax;
	wc3lib::readString(istream, eax, size);

	/*
	 * Music files seem to have an empty EAX when default value is set.
	 * For example music "Human1.mp3".
	 */
	if (eax.empty()) {
		this->m_eaxEffects = EAX::UseDefault;
	}
	else if (eax == "DefaultEAXON")
		this->m_eaxEffects = EAX::Default;
	else if (eax == "CombatSoundsEAX")
		this->m_eaxEffects = EAX::Combat;
	else if (eax == "KotoDrumsEAX")
		this->m_eaxEffects = EAX::Drums;
	else if (eax == "SpellsEAX")
		this->m_eaxEffects = EAX::Spells;
	else if (eax == "MissilesEAX")
		this->m_eaxEffects = EAX::Missiles;
	else if (eax == "HeroAcksEAX")
		this->m_eaxEffects = EAX::HeroSpeech;
	else if (eax == "DoodadsEAX")
		this->m_eaxEffects = EAX::Doodads;
	else
		throw Exception(boost::format(_("Sound: Unknown EAX effect \"%1%\".")) % eax);

	int32 flags = 0;
	wc3lib::read(istream, flags, size);
	this->m_flags = (BOOST_SCOPED_ENUM(Flags))flags;
	wc3lib::read(istream, this->m_fadeInRate, size);
	wc3lib::read(istream, this->m_fadeOutRate, size);
	wc3lib::read(istream, this->m_volume, size);
	wc3lib::read(istream, this->m_pitch, size);
	wc3lib::read(istream, this->m_unknown0, size);
	wc3lib::read(istream, this->m_unknown1, size);
	int32 channel = 0;
	wc3lib::read(istream, channel, size);
	this->m_channel = (BOOST_SCOPED_ENUM(Channel))channel;
	wc3lib::read(istream, this->m_minDistance, size);
	wc3lib::read(istream, this->m_maxDistance, size);
	wc3lib::read(istream, this->m_distanceCutoff, size);
	wc3lib::read(istream, this->m_unknown2, size);
	wc3lib::read(istream, this->m_unknown3, size);
	wc3lib::read(istream, this->m_unknown4, size);
	wc3lib::read(istream, this->m_unknown5, size);
	wc3lib::read(istream, this->m_unknown6, size);
	wc3lib::read(istream, this->m_unknown7, size);

	return size;
}

std::streamsize Sound::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::writeString(ostream, this->name(), size);
	wc3lib::writeString(ostream, this->file(), size);
	string eax;

	switch (this->m_eaxEffects)
	{
		case EAX::UseDefault:
			eax = "";
			
			break;
		
		case EAX::Default:
			eax = "DefaultEAXON";

			break;

		case EAX::Combat:
			eax = "CombatSoundsEAX";

			break;

		case EAX::Drums:
			eax = "KotoDrumsEAX";

			break;

		case EAX::Spells:
			eax = "SpellsEAX";

			break;

		case EAX::Missiles:
			eax = "MissilesEAX";

			break;

		case EAX::HeroSpeech:
			eax = "HeroAcksEAX";

			break;

		case EAX::Doodads:
			eax = "DoodadsEAX";

			break;
	}

	wc3lib::writeString(ostream, eax, size);
	wc3lib::write<int32>(ostream, this->flags(), size);
	wc3lib::write(ostream, this->fadeInRate(), size);
	wc3lib::write(ostream, this->fadeOutRate(), size);
	wc3lib::write(ostream, this->volume(), size);
	wc3lib::write(ostream, this->pitch(), size);
	wc3lib::write(ostream, this->unknown0(), size);
	wc3lib::write(ostream, this->unknown1(), size);
	wc3lib::write<int32>(ostream, this->channel(), size);
	wc3lib::write(ostream, this->minDistance(), size);
	wc3lib::write(ostream, this->maxDistance(), size);
	wc3lib::write(ostream, this->distanceCutoff(), size);
	wc3lib::write(ostream, this->unknown2(), size);
	wc3lib::write(ostream, this->unknown3(), size);
	wc3lib::write(ostream, this->unknown4(), size);
	wc3lib::write(ostream, this->unknown5(), size);
	wc3lib::write(ostream, this->unknown6(), size);
	wc3lib::write(ostream, this->unknown7(), size);

	return size;
}

}

}
