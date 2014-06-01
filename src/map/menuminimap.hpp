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

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"
#include "../color.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief The menu minimap of a Warcraft III map which contains special markers.
 */
class MenuMinimap : public FileFormat
{
	public:
		class Mark : public Vertex2d<int32>
		{
			public:
				enum class IconType : int32
				{
					GoldMine,
					House,
					PlayerStart
				};

				Mark();

				virtual std::streamsize read(InputStream& istream) throw (Exception);
				virtual std::streamsize write(OutputStream& ostream) const throw (Exception);

				IconType iconType() const;
				const Bgra& color() const;

			protected:
				IconType m_iconType;
				Bgra m_color;
		};

		/// Ordered by their coordinates.
		typedef boost::ptr_vector<Mark> Marks;

		virtual std::streamsize read(InputStream &istream) throw (Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

		virtual const byte* fileName() const;
		virtual const byte* fileTextId() const;
		virtual uint32 latestFileVersion() const;

		Marks& marks();
		const Marks& marks() const;

	protected:
		Marks m_marks;
};

inline MenuMinimap::Mark::IconType MenuMinimap::Mark::iconType() const
{
	return m_iconType;
}

inline const Bgra& MenuMinimap::Mark::color() const
{
	return m_color;
}

inline const byte* MenuMinimap::fileName() const
{
	return "war3map.mmp";
}

inline const byte* MenuMinimap::fileTextId() const
{
	return "";
}

inline uint32 MenuMinimap::latestFileVersion() const
{
	return 0;
}

inline MenuMinimap::Marks& MenuMinimap::marks()
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
