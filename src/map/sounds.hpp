/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_SOUNDS_HPP
#define WC3LIB_MAP_SOUNDS_HPP

#include "platform.hpp"
#include "sound.hpp"

namespace wc3lib
{

namespace map
{

class Sounds : public FileFormat
{
	public:
		typedef boost::shared_ptr<Sound> SoundPtr;
		typedef std::vector<SoundPtr> SoundVector;

		Sounds(class W3m *w3m);

		virtual std::streamsize read(InputStream &istream) throw (Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

		virtual uint32 version() const;
		virtual const byte* fileName() const;
		virtual const byte* fileTextId() const;
		virtual uint32 latestFileVersion() const;

		class W3m* w3m() const;
		SoundVector sounds();
		const SoundVector& sounds() const;

	protected:
		class W3m *m_w3m;
		uint32 m_version;
		SoundVector m_sounds;
};

inline class W3m* Sounds::w3m() const
{
	return m_w3m;
}

inline uint32 Sounds::version() const
{
	return m_version;
}

inline const byte* Sounds::fileName() const
{
	return "war3map.w3s";
}

inline const byte* Sounds::fileTextId() const
{
	return "";
}

inline uint32 Sounds::latestFileVersion() const
{
	return 1;
}

inline Sounds::SoundVector Sounds::sounds()
{
	return m_sounds;
}

inline const Sounds::SoundVector& Sounds::sounds() const
{
	return m_sounds;
}

}

}

#endif
