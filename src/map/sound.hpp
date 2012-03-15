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

#ifndef WC3LIB_MAP_SOUND_HPP
#define WC3LIB_MAP_SOUND_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

class Sound : public Format
{
	public:
		BOOST_SCOPED_ENUM_START(EAX)
		{
			Default,
			Combat,
			Drums,
			Spells,
			Missiles,
			HeroSpeech,
			Doodads
		};
		BOOST_SCOPED_ENUM_END

		BOOST_SCOPED_ENUM_START(Flags) /// \todo C++11 : int32
		{
			Looping = 0x00000001,
			Sound3d = 0x00000002,
			StopWhenOutOfRange = 0x00000004,
			Music = 0x00000008,
			Unknown = 0x00000010
		};
		BOOST_SCOPED_ENUM_END

		BOOST_SCOPED_ENUM_START(Channel) /// \todo C++11 : int32
		{
			General = 0,
			UnitSelection = 1,
			UnitAcknowledgement = 2,
			UnitMovement = 3,
			UnitReady = 4,
			CombatChannel = 5,
			Error = 6,
			MusicChannel = 7,
			UserInterface = 8,
			LoopingMovement = 9,
			LoopingAmbient = 10,
			Animations = 11,
			Constructions = 12,
			Birth = 13,
			Fire = 14
		};
		BOOST_SCOPED_ENUM_END

		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

	protected:
		string m_name; // gg_snd_HumanGlueScreenLoop1
		string m_file; // "Sound\Ambient\HumanGlueScreenLoop1.wav"
		BOOST_SCOPED_ENUM(EAX) m_eaxEffects;
		BOOST_SCOPED_ENUM(Flags) m_flags;
		int32 m_fadeInRate;
		int32 m_fadeOutRate;
		int32 m_volume;
		float32 m_pitch;
		float32 m_unknown0; //  SetSoundDuration             takes sound soundHandle, integer duration??
		int32 m_unknown1; // -1 or 8
		BOOST_SCOPED_ENUM(Channel) m_channel;
		float32 m_minDistance;
		float32 m_maxDistance;
		float32 m_distanceCutoff;
		/*
		// these calls are only valid if the sound was created with 3d enabled
		native SetSoundDistances            takes sound soundHandle, real minDist, real maxDist returns nothing
		native SetSoundConeAngles           takes sound soundHandle, real inside, real outside, integer outsideVolume returns nothing
		native SetSoundConeOrientation      takes sound soundHandle, real x, real y, real z returns nothing
		native SetSoundPosition             takes sound soundHandle, real x, real y, real z returns nothing
		native SetSoundVelocity             takes sound soundHandle, real x, real y, real z returns nothing
		native CreateSoundFilenameWithLabel takes string fileName, boolean looping, boolean is3D, boolean stopwhenoutofrange, integer fadeInRate, integer fadeOutRate, string SLKEntryName returns sound
		native CreateSoundFromLabel         takes string soundLabel, boolean looping, boolean is3D, boolean stopwhenoutofrange, integer fadeInRate, integer fadeOutRate returns sound
		native CreateMIDISound              takes string soundLabel, integer fadeInRate, integer fadeOutRate returns sound
		*/
		float32 m_unknown2; // inside?
		float32 m_unknown3; // outside?
		int32 m_unknown4; // (-1 or 127) outside volume?
		float32 m_unknown5; // sound cone orientation x
		float32 m_unknown6; // sound cone orientation y
		float32 m_unknown7; // sound cone orientation z
};

}

}

#endif
