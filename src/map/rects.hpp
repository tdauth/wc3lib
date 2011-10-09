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

#ifndef WC3LIB_MAP_RECTS_HPP
#define WC3LIB_MAP_RECTS_HPP

#include "platform.hpp"
#include "rect.hpp"

namespace wc3lib
{

namespace map
{

/**
 * "war3map.w3r" file usually contains rects of map.
 */
class Rects : public FileFormat
{
	public:
		typedef boost::shared_ptr<Rect> RectPtr;
		typedef std::map<int32, RectPtr> RectMap;

		Rects(class W3m *w3m);
		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual const char8* fileTextId() const;
		virtual const char8* fileName() const;
		virtual int32 latestFileVersion() const;

		virtual int32 version() const { return m_version; }

		const RectMap& rects() const;
		RectMap& rects();

	protected:
		class W3m *m_w3m;
		int32 m_version;
		RectMap m_rects;
};

inline const char8* Rects::fileTextId() const
{
	return "";
}

inline const char8* Rects::fileName() const
{
	return "war3map.w3r";
}

inline int32 Rects::latestFileVersion() const
{
	return 5;
}

inline const Rects::RectMap& Rects::rects() const
{
	return m_rects;
}

inline Rects::RectMap& Rects::rects()
{
	return m_rects;
}

}

}

#endif
