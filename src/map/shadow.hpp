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

#include <boost/array.hpp>

#include "platform.hpp"
#include "tilepoint.hpp"

namespace wc3lib
{

namespace map
{

/**
 * "war3map.shd" is usally the map's shadow map file.
 * Each tilepoint of the map has exactly 16 shadow points.
 * Use class key to get shadow data of some point.
 * \note You should refresh the shadow map if you change map size!
 * \sa Tilepoint, Environment
 */
class Shadow : public FileFormat
{
	public:
		class Key : public boost::array<int32, 3>
		{
			public:
				Key(int32 x, int32 y, int32 point);
				int32 x() const;
				int32 y() const;
				int32 point() const;

				/**
				 * This operator is required since map is stored sorted.
				 * Note that this should provide correct sorting (width * height * point).
				 */
				bool operator<(const Key &other) const;
				bool operator==(const Key &other) const;
		};

		BOOST_SCOPED_ENUM_START(Type) /// \todo C++11 : byte
		{
			NoShadow = 0x00,
			HasShadow = 0xFF
		};
		BOOST_SCOPED_ENUM_END

		typedef std::map<class Key, BOOST_SCOPED_ENUM(Type)> Data;

		Shadow(class W3m *w3m);
		virtual ~Shadow();

		class W3m* w3m() const;
		Data& data();
		const Data& data() const;

		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual const byte* fileTextId() const;
		virtual const byte* fileName() const;
		virtual uint32 latestFileVersion() const;
		virtual uint32 version() const;

		BOOST_SCOPED_ENUM(Type) type(const class Key &key) const;
		/**
		 * Just for one single layer point.
		 */
		bool containsShadow(const class Key &key) const;
		/**
		 * Checks all 16 layer points and returns true if any of them contains shadow.
		 */
		bool containsShadow(int32 x, int32 y) const;
		/**
		 * Does also check all 16 layer points.
		 */
		bool tilepointContainsShadow(const class Tilepoint &tilepoint) const;

		static const int32 shadowPointsPerTileset;

	protected:
		class W3m *m_w3m;

		/**
		1byte can have 2 values:
		0x00 = no shadow
		0xFF = shadow
		*/
		Data m_data;
};

inline Shadow::Key::Key(int32 x, int32 y, int32 point)
{
	(*this)[0] = x;
	(*this)[1] = y;
	(*this)[2] = std::min(point, Shadow::shadowPointsPerTileset - 1);
}

inline int32 Shadow::Key::x() const
{
	return (*this)[0];
}

inline int32 Shadow::Key::y() const
{
	return (*this)[1];
}

inline int32 Shadow::Key::point() const
{
	return (*this)[2];
}

inline bool Shadow::Key::operator<(const Key &other) const
{
	return this->y() < other.y() || (this->y() == other.y() && this->x() < other.x()) || (this->y() == other.y() && this->x() == other.x() && this->point() < other.point());
}

inline bool Shadow::Key::operator==(const Key &other) const
{
	return this->x() == other.x() && this->y() == other.y() && this->point() == other.point();
}

inline class W3m* Shadow::w3m() const
{
	return this->m_w3m;
}

inline Shadow::Data &Shadow::data()
{
	return this->m_data;
}

inline const Shadow::Data &Shadow::data() const
{
	return this->m_data;
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

inline uint32 Shadow::version() const
{
	return 0;
}

inline BOOST_SCOPED_ENUM(Shadow::Type) Shadow::type(const Shadow::Key &key) const
{
	Data::const_iterator iterator = this->m_data.find(key);

	if (iterator == this->m_data.end())
		return Shadow::Type::NoShadow;

	return iterator->second;
}

inline bool Shadow::containsShadow(const Key &key) const
{
	return this->type(key) == Shadow::Type::HasShadow;
}

inline bool Shadow::containsShadow(int32 x, int32 y) const
{
	for (int32 point = 0; point < Shadow::shadowPointsPerTileset; ++point)
	{
		class Key key(x, y, point);

		if (this->containsShadow(key))
			return true;
	}

	return false;
}

inline bool Shadow::tilepointContainsShadow(const class Tilepoint &tilepoint) const
{
	return this->containsShadow(tilepoint.x(), tilepoint.y());
}

}

}

#endif
