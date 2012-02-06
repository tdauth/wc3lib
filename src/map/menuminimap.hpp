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

#ifndef WC3LIB_MAP_MENUMINIMAP_HPP
#define WC3LIB_MAP_MENUMINIMAP_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

class MenuMinimap : public FileFormat
{
	public:
		class Mark : public Position
		{
			public:
				BOOST_SCOPED_ENUM_START(IconType) /// \todo C++11 : int32
				{
					GoldMine,
					House,
					PlayerStart
				};
				BOOST_SCOPED_ENUM_END

				Mark(int32 x, int32 y);

				virtual std::streamsize read(InputStream& istream) throw (Exception);
				virtual std::streamsize write(OutputStream& ostream) const throw (Exception);

				BOOST_SCOPED_ENUM(IconType) iconType() const;
				const Bgra& color() const;

			protected:
				BOOST_SCOPED_ENUM(IconType) m_iconType;
				Bgra m_color;
		};

		typedef boost::shared_ptr<Mark> MarkPtr;
		/// Ordered by their coordinates.
		typedef std::set<MarkPtr> Marks;

		MenuMinimap(class W3m *w3m);

		virtual std::streamsize read(InputStream &istream) throw (Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

		virtual int32 version() const;
		virtual const char8* fileName() const;
		virtual const char8* fileTextId() const;
		virtual int32 latestFileVersion() const;

		class W3m* w3m() const;
		Marks marks();
		const Marks& marks() const;

	protected:
		class W3m *m_w3m;
		int32 m_version;
		Marks m_marks;
};

inline BOOST_SCOPED_ENUM(MenuMinimap::Mark::IconType) MenuMinimap::Mark::iconType() const
{
	return m_iconType;
}

inline const wc3lib::map::Bgra& MenuMinimap::Mark::color() const
{
	return m_color;
}

inline class W3m* MenuMinimap::w3m() const
{
	return m_w3m;
}

inline int32 MenuMinimap::version() const
{
	return m_version;
}

inline const char8* MenuMinimap::fileName() const
{
	return "war3map.mmp";
}

inline const char8* MenuMinimap::fileTextId() const
{
	return "";
}

inline int32 MenuMinimap::latestFileVersion() const
{
	return 0;
}

inline MenuMinimap::Marks MenuMinimap::marks()
{
	return m_marks;
}

inline const MenuMinimap::Marks& MenuMinimap::marks() const
{
	return m_marks;

}

}

}

#endif
