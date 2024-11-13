/***************************************************************************
 *   Copyright (C) 2024 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MDLX_SOUNDTRACK
#define WC3LIB_MDLX_SOUNDTRACK

#include "platform.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * \note These objects were only used at some point before Warcraft 3 released.
 */
class SoundTrack : public Format
{
	public:
		static const std::size_t nameSize = 260;

		SoundTrack();
		virtual ~SoundTrack();

		void setName(const string &name);
		void setName(const byte name[nameSize]);
		/**
		 * \return Returns name with constant length of \ref nameSize.
		 */
		const byte* name() const;
		void setVolume(float32 volume);
		float32 volume() const;
		void setPitch(float32 pitch);
		float32 pitch() const;
		void setFlags(long32 flags);
		long32 flags() const;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

	protected:
		byte m_name[nameSize];
		float32 m_volume;
		float32 m_pitch;
		long32 m_flags;
};

inline void SoundTrack::setName(const string &name)
{
	if (name.size() + 1 > SoundTrack::nameSize)
	{
		throw std::runtime_error("");
	}

	memset(this->m_name, 0, nameSize); // init name with 0 byte
	memcpy(this->m_name, name.c_str(), name.size() + 1); // copy with 0 terminating character
}

inline void SoundTrack::setName(const byte name[SoundTrack::nameSize])
{
	memcpy(this->m_name, name, sizeof(byte) * SoundTrack::nameSize);
}

inline const byte* SoundTrack::name() const
{
	return this->m_name;
}

inline void SoundTrack::setVolume(float32 volume)
{
	this->m_volume = volume;
}

inline float32 SoundTrack::volume() const
{
	return this->m_volume;
}

inline void SoundTrack::setPitch(float32 pitch)
{
	this->m_pitch = pitch;
}

inline float32 SoundTrack::pitch() const
{
	return this->m_pitch;
}

inline void SoundTrack::setFlags(long32 flags)
{
	this->m_flags = flags;
}

inline long32 SoundTrack::flags() const
{
	return this->m_flags;
}

}

}

#endif
