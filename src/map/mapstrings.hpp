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

#ifndef WC3LIB_MAP_STRINGS_HPP
#define WC3LIB_MAP_STRINGS_HPP

#include <vector>

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief Map strings file "war3map.wts".
 *
 * The grammar of a map strings file consists of string entries with numbers:
 * \code
 * STRING 23
 * {
 * My Text
 * }
 * \endcode
 *
 * \todo Implement Boost Qi parser and Karma generator!
 */
class MapStrings : public FileFormat
{
	public:
		/**
		 * \brief A single string entry for a translatable string in a \ref MapStrings file.
		 *
		 * Each string is identified by its unique integer key.
		 * The value itself is the translation of the string for the current language of the map.
		 * The comment is an optional string which may contain additional information of the location of the string in the map.
		 * For example for object data it might be something like "// Units: h02M (Stealth Maiden), Name (Name)" in Frozen Throne. This helps
		 * to find the source of the string.
		 */
		struct Entry
		{
			int key;
			/**
			 * The comment might be important since it usually contains information about the string's location.
			 */
			string comment;
			string value;

			Entry() : key(0)
			{
			}
		};

		/**
		 * \brief All entries of a map strings file.
		 *
		 * The entries are stored into a vector in the order of ther occurence in the file.
		 */
		typedef std::vector<Entry> Entries;

		virtual const byte* fileName() const override;
		virtual const byte* fileTextId() const override;
		virtual uint32 latestFileVersion() const override;

		virtual std::streamsize read(InputStream& istream) override;
		virtual std::streamsize write(OutputStream& ostream) const override;

		/**
		 * \return Returns all string entries of the file.
		 *
		 * @{
		 */
		Entries& entries();
		const Entries& entries() const;
		/**
		 * @}
		 */

	private:
		Entries m_entries;
};

inline const byte* MapStrings::fileName() const
{
	return "war3map.wts";
}

inline const byte* MapStrings::fileTextId() const
{
	return "";
}

inline uint32 MapStrings::latestFileVersion() const
{
	return 0;
}

inline MapStrings::Entries& MapStrings::entries()
{
	return this->m_entries;
}

inline const MapStrings::Entries& MapStrings::entries() const
{
	return this->m_entries;
}

}

}

#endif
