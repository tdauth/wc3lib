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
		enum class EAX
		{
			UseDefault, /// If eax value is empty it uses the default value from the corresponding SLK file.
			Default,
			Combat,
			Drums,
			Spells,
			Missiles,
			HeroSpeech,
			Doodads
		};

		enum class Flags : int32
		{
			None = 0, /// Does not use any flags.
			Looping = 0x00000001,
			Sound3d = 0x00000002,
			StopWhenOutOfRange = 0x00000004,
			Music = 0x00000008,
			Unknown = 0x00000010
		};

		enum class Channel : int32
		{
			UseDefault = -1, /// Uses default channel from corresponding SLK file entry.
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

		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		const string& name() const;
		const string& file() const;
		EAX eaxEffects() const;
		Flags flags() const;
		int32 fadeInRate() const;
		int32 fadeOutRate() const;
		int32 volume() const;
		float32 pitch() const;
		float32 unknown0() const;
		int32 unknown1() const;
		Channel channel() const;
		float32 minDistance() const;
		float32 maxDistance() const;
		float32 distanceCutoff() const;
		float32 unknown2() const;
		float32 unknown3() const;
		int32 unknown4() const;
		float32 unknown5() const;
		float32 unknown6() const;
		float32 unknown7() const;

		bool usesDefaultVolume() const;

	protected:
		string m_name; // gg_snd_HumanGlueScreenLoop1
		string m_file; // "Sound\Ambient\HumanGlueScreenLoop1.wav"
		EAX m_eaxEffects;
		Flags m_flags;
		int32 m_fadeInRate;
		int32 m_fadeOutRate;
		int32 m_volume;
		float32 m_pitch;
		float32 m_unknown0; //  SetSoundDuration             takes sound soundHandle, integer duration??
		int32 m_unknown1; // -1 or 8
		Channel m_channel;
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

inline constexpr bool operator&(Sound::Flags x, Sound::Flags y)
{
	return static_cast<bool>(static_cast<int32>(x) & static_cast<int32>(y));
}

inline const string& Sound::name() const
{
	return this->m_name;
}

inline const string& Sound::file() const
{
	return this->m_file;
}

inline Sound::EAX Sound::eaxEffects() const
{
	return this->m_eaxEffects;
}

inline Sound::Flags Sound::flags() const
{
	return this->m_flags;
}

inline int32 Sound::fadeInRate() const
{
	return this->m_fadeInRate;
}

inline int32 Sound::fadeOutRate() const
{
	return this->m_fadeOutRate;
}

inline int32 Sound::volume() const
{
	return this->m_volume;
}

inline float32 Sound::pitch() const
{
	return this->m_pitch;
}

inline float32 Sound::unknown0() const
{
	return this->m_unknown0;
}

inline int32 Sound::unknown1() const
{
	return this->m_unknown1;
}

inline Sound::Channel Sound::channel() const
{
	return this->m_channel;
}

inline float32 Sound::minDistance() const
{
	return this->m_minDistance;
}

inline float32 Sound::maxDistance() const
{
	return this->m_maxDistance;
}

inline float32 Sound::distanceCutoff() const
{
	return this->m_distanceCutoff;
}

inline float32 Sound::unknown2() const
{
	return this->m_unknown2;
}

inline float32 Sound::unknown3() const
{
	return this->m_unknown3;
}

inline int32 Sound::unknown4() const
{
	return this->m_unknown4;
}

inline float32 Sound::unknown5() const
{
	return this->m_unknown5;
}

inline float32 Sound::unknown6() const
{
	return this->m_unknown6;
}

inline float32 Sound::unknown7() const
{
	return this->m_unknown7;
}

inline bool Sound::usesDefaultVolume() const
{
	return this->volume() == -1;
}

}

}

#endif
