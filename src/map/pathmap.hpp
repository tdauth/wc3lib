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

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

class Pathmap : public FileFormat
{
	public:
		BOOST_SCOPED_ENUM_START(Type)
		{
			Walk = 0x02,
			Fly = 0x04,
			Build = 0x08,
			Blight = 0x20,
			Water = 0x40,
			Unknown = 0x80
		};
		BOOST_SCOPED_ENUM_END

		typedef std::map<Position, BOOST_SCOPED_ENUM(Type)> Data;

		Pathmap(class W3m *w3m);
		virtual ~Pathmap();

		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual const char8* fileTextId() const;
		virtual const char8* fileName() const;
		virtual int32 latestFileVersion() const;

		virtual int32 version() const;
		int32 width() const;
		int32 height() const;
		BOOST_SCOPED_ENUM(Type) type(const Position &position) const throw (class Exception);

	protected:
		class W3m *m_w3m;
		int32 m_version;
		int32 m_width;
		int32 m_height;
		Data m_data;
};

inline const char8* Pathmap::fileTextId() const
{
	return "MP3W";
}

inline const char8* Pathmap::fileName() const
{
	return "war3map.wpm";
}

inline int32 Pathmap::latestFileVersion() const
{
	return 0;
}

inline int32 Pathmap::version() const
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

inline BOOST_SCOPED_ENUM(Pathmap::Type) Pathmap::type(const Position &position) const throw (class Exception)
{
	Data::const_iterator iterator = this->m_data.find(position);

	if (iterator == this->m_data.end())
		throw Exception(boost::format(_("No pathing found at positon (%1%|%2%).")) % position.x() % position.y());

	return iterator->second;
}

}

}

#endif
