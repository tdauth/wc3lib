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

#ifndef WC3LIB_MAP_SHADOW_HPP
#define WC3LIB_MAP_SHADOW_HPP

#include <boost/multi_array.hpp>

#include "platform.hpp"
#include "tilepoint.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief Stores information about a map's shadow map (file "war3map.shd").
 *
 * Each tilepoint of the map has exactly 16 shadow points. The number can be refered using \ref Shadow::shadowPointsPerTileset.
 * This means a tilepoint is divided into 4 * 4 shadow points which can either have shadow or not (\ref Shadow::Type).
 *
 * \note You should refresh the shadow map if you change map size!
 *
 * \sa Tilepoint
 * \sa Environment
 *
 * \ingroup environment
 */
class Shadow : public FileFormat
{
	public:
		enum class Type : uint8
		{
			NoShadow = 0x00,
			HasShadow = 0xFF
		};

		/**
		 * Each tilepoint from map environment is divided into 16 different shadow points (4 * 4).
		 * Therefore the shadow file stores 16 layer points per tilepoint from environment (\ref Environment) in a three-dimensional array.
		 * The first index is the X-coordinate, the second one the Y-coordinate and the third one a number between 0 and 15
		 * for the corresponding layer point.
		 */
		typedef boost::multi_array<Type, 3> Tilepoints;
		typedef Tilepoints::array_view<1>::type View;
		typedef Tilepoints::const_array_view<1>::type ConstView;

		/**
		 * The shadow file needs the width and height of the map as information.
		 * It does not store width and height itself.
		 */
		Shadow(int32 width, int32 height);
		virtual ~Shadow();

		/**
		 * Reads a shadow file of size \ref width() * \ref height() * \ref shadowPointsPerTileset.
		 */
		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual const byte* fileTextId() const;
		virtual const byte* fileName() const;
		virtual uint32 latestFileVersion() const;

		/**
		 * Expected shadow file size. This is used when the shadow file is read from
		 * any input stream to calculate its size.
		 *
		 * @{
		 */
		void setWidth(int32 width);
		int32 width() const;
		void setHeight(int32 height);
		int32 height() const;
		/**
		 * @}
		 */

		Tilepoints& tilepoints();
		const Tilepoints& tilepoints() const;

		/**
		 * Returns the type of a shadow point of tilepoint at (\p x | \p y) with index \p index.
		 */
		Type type(int32 x, int32 y, int32 index) const;

		/**
		 * Returns a sub view with all 16 shadow points for one single tilepoint at (\p x | \p y).
		 * @{
		 */
		View tilepoint(int32 x, int32 y);
		ConstView tilepoint(int32 x, int32 y) const;
		/**
		 * @}
		 */

		/**
		 * The number of flags per one single tileset.
		 */
		static const int32 shadowPointsPerTileset;

	protected:
		int32 m_width;
		int32 m_height;
		Tilepoints m_tilepoints;
};

inline void Shadow::setWidth(int32 width)
{
	this->m_width = width;
}

inline int32 Shadow::width() const
{
	return this->m_width;
}

inline void Shadow::setHeight(int32 height)
{
	this->m_height = height;
}

inline int32 Shadow::height() const
{
	return this->m_height;
}

inline const byte* Shadow::fileTextId() const
{
	return "";
}

inline const byte* Shadow::fileName() const
{
	return "war3map.shd";
}

inline uint32 Shadow::latestFileVersion() const
{
	return 0;
}

inline Shadow::Tilepoints& Shadow::tilepoints()
{
	return this->m_tilepoints;
}

inline const Shadow::Tilepoints& Shadow::tilepoints() const
{
	return this->m_tilepoints;
}

inline Shadow::Type Shadow::type(int32 x, int32 y, int32 index) const
{
	return this->tilepoints()[x][y][index];
}

inline Shadow::View Shadow::tilepoint(int32 x, int32 y)
{
	typedef Tilepoints::index_range range;
	Tilepoints::index_gen indices;
	Tilepoints::array_view<1>::type myview = this->tilepoints()[indices[x][y][range(0, this->tilepoints().shape()[2])] ];

	return myview;
}

inline Shadow::ConstView Shadow::tilepoint(int32 x, int32 y) const
{
	typedef Tilepoints::index_range range;
	Tilepoints::index_gen indices;
	Tilepoints::const_array_view<1>::type myview = this->tilepoints()[indices[x][y][range(0, this->tilepoints().shape()[2])] ];

	return myview;
}

}

}

#endif
