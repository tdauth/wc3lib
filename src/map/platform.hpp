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

#ifndef WC3LIB_MAP_PLATFORM_HPP
#define WC3LIB_MAP_PLATFORM_HPP

#include <boost/array.hpp>
#include <boost/detail/scoped_enum_emulation.hpp>

#include "../platform.hpp"
#include "../format.hpp"
#include "../vertex.hpp"

namespace wc3lib
{

namespace map
{

/// Versions (file versions) are usually being saved as int32 values.
//typedef int32 version;
typedef uint32_t id;

/**
 * Converts an ID to a readable string. Useful for comparisons and display.
 */
inline string idToString(id value) {
	const std::size_t size = sizeof(id);
	char result[size + 1];
	result[size] = '\0';
	memcpy(result, &value, size);
	
	return result;
}

/// Usually only some formats have a customized version (e. g. map shadow - "war3map.shd")
class FileFormat : public Format
{
	public:
		FileFormat();

		virtual const byte* fileName() const = 0;
		/**
		 * \return Returns the format's id in form of ASCII text with length of \ref id!
		 */
		virtual const byte* fileTextId() const = 0;
		virtual id fileId() const
		{
			return *reinterpret_cast<const id*>(fileTextId());
		}
		virtual uint32 latestFileVersion() const = 0;
		virtual uint32 version() const
		{
			return m_version;
		}

		virtual std::streamsize read(InputStream &istream) throw (Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

	protected:
		uint32 m_version;
};

inline FileFormat::FileFormat() : m_version(0)
{

}

inline std::streamsize FileFormat::read(FileFormat::InputStream &istream) throw (Exception)
{
	id fileId;
	std::streamsize size = 0;
	wc3lib::read(istream, fileId, size);

	if (fileId != this->fileId())
		throw Exception(_("Unknown file id!"));

	wc3lib::read(istream, this->m_version, size);

	if (version() != latestFileVersion())
		std::cerr << boost::format(_("Warning in file \"%1%\": %2% is not the latest file version %3%")) % fileName() % version() % latestFileVersion() << std::endl;

	return size;
}

inline std::streamsize FileFormat::write(FileFormat::OutputStream &ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->fileId(), size);
	wc3lib::write(ostream, this->m_version, size);

	if (version() != latestFileVersion())
		std::cerr << boost::format(_("Warning in file \"%1%\": %2% is not the latest file version %3%")) % fileName() % version() % latestFileVersion() << std::endl;

	return size;
}

/**
* Custom Types
*
* Sometimes, an integer and one or several flags can share bytes. This is the case in the W3E file format: the water level and 2 flags are using the same group of 4 bytes. How? the 2 highest bit are used for the flags, the rest is reserved for the water level (the value range is just smaller). Sometimes a byte can contain two or more different data.
*
*/

typedef std::vector<string> List;

template<typename T = int32>
class BasePosition : public boost::array<T, 2>, public Format, public boost::operators<BasePosition<T> >
{
	public:
		BasePosition()
		{
			(*this)[0] = 0;
			(*this)[1] = 0;
		}

		BasePosition(T x, T y)
		{
			(*this)[0] = x;
			(*this)[1] = y;
		}

		void setX(T x)
		{
			(*this)[0] = x;
		}

		T x() const
		{
			return (*this)[0];
		}

		void setY(T y)
		{
			(*this)[1] = y;
		}

		T y() const
		{
			return (*this)[1];
		}

		virtual std::streamsize read(InputStream &istream) throw (Exception)
		{
			std::streamsize size = 0;
			wc3lib::read(istream, (*this)[0], size);
			wc3lib::read(istream, (*this)[1], size);

			return size;
		}

		virtual std::streamsize write(OutputStream &ostream) const throw (Exception)
		{
			std::streamsize size = 0;
			wc3lib::write(ostream, (*this)[0], size);
			wc3lib::write(ostream, (*this)[1], size);

			return size;
		}

		bool operator==(const BasePosition &position) const
		{
			return x() == position.x() && y() == position.y();
		}

		// FIXME
		bool operator<(const BasePosition &position) const
		{
			//return x() < position.x() && y() < position.y();
			return sqrt(pow((x() - position.x()), 2) + pow((y() - position.y()), 2)) < 0;
			//return (y() / x() - position.y() /  position.x()) < 0;
		}
};

typedef BasePosition<int32> Position;
typedef BasePosition<float32> FloatPosition;

BOOST_SCOPED_ENUM_START(MapFlags) /// \todo C++11 : int32
{
	HideMinimapInPreviewScreens = 0x0001,
	ModifyAllyPriorities = 0x0002,
	MeleeMap = 0x0004,
	PlayableMapSizeWasLargeAndHasNeverBeenReducedToMedium = 0x0008,
	MaskedAreaArePartiallyVisible = 0x0010,
	FixedPlayerSettingForCustomForces = 0x0020,
	UseCustomForces = 0x0040,
	UseCustomTechtree = 0x0080,
	UseCustomAbilities = 0x0100,
	UseCustomUpgrades = 0x0200,
	MapPropertiesMenuOpenedAtLeastOnceSinceMapCreation = 0x0400,
	ShowWaterWavesOnCliffShores = 0x0800,
	ShowWaterWavesOnRollingShores = 0x1000
};
BOOST_SCOPED_ENUM_END

}

}

#endif
