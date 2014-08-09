/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_PATHMAP_HPP
#define WC3LIB_MAP_PATHMAP_HPP

#include <boost/multi_array.hpp>

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief Stores data from a pathmap file ("war3map.wpm") which contains information about the pathability of each tilepoint from the environment (\ref Environment).
 *
 * \ingroup pathing
 * \ingroup environment
 */
class Pathmap : public FileFormat
{
	public:
		enum class Type : uint8
		{
			Walk = 0x02,
			Fly = 0x04,
			Build = 0x08,
			Blight = 0x20,
			Water = 0x40,
			Unknown = 0x80
		};

		/**
		 * \brief For each tilepoint there exists an entry which defines the pathability of the tilepoint.
		 * Therefore all entries are stored in a two-dimensional array like the tilepoints in \ref Environment.
		 * It uses the X-coordinate as the first index and the Y-coordinate as the second one.
		 */
		typedef boost::multi_array<Type, 2> Tilepoints;

		Pathmap();
		virtual ~Pathmap();

		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual const byte* fileTextId() const;
		virtual const byte* fileName() const;
		virtual uint32 latestFileVersion() const;

		virtual uint32 version() const;
		int32 width() const;
		int32 height() const;

		Tilepoints& tilepoints();
		const Tilepoints& tilepoints() const;

		Type type(int32 x, int32 y) const;

	protected:
		uint32 m_version;
		int32 m_width;
		int32 m_height;
		Tilepoints m_tilepoints;
};

inline const byte* Pathmap::fileTextId() const
{
	return "MP3W";
}

inline const byte* Pathmap::fileName() const
{
	return "war3map.wpm";
}

inline uint32 Pathmap::latestFileVersion() const
{
	return 0;
}

inline uint32 Pathmap::version() const
{
	return this->m_version;
}

inline int32 Pathmap::width() const
{
	return this->m_width;
}

inline int32 Pathmap::height() const
{
	return this->m_height;
}

inline Pathmap::Tilepoints& Pathmap::tilepoints()
{
	return this->m_tilepoints;
}

inline const Pathmap::Tilepoints& Pathmap::tilepoints() const
{
	return this->m_tilepoints;
}

inline Pathmap::Type Pathmap::type(int32 x, int32 y) const
{
	return this->m_tilepoints[x][y];
}

inline constexpr bool operator&(Pathmap::Type x, Pathmap::Type y)
{
	return static_cast<bool>(static_cast<uint8>(x) & static_cast<uint8>(y));
}

inline constexpr Pathmap::Type operator|(Pathmap::Type x, Pathmap::Type y)
{
	return static_cast<Pathmap::Type>(static_cast<uint8>(x) | static_cast<uint8>(y));
}

}

}

#endif
