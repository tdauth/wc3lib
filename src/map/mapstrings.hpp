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

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief Map strings file "war3map.wts".
 *
 * \todo Implement Boost Qi parser and Karma generator!
 */
class MapStrings : public FileFormat
{
	public:
		struct Entry
		{
			string key;
			string value;
		};

		typedef boost::ptr_vector<Entry> Entries;

		virtual const byte* fileName() const override;
		virtual const byte* fileTextId() const override;
		virtual uint32 latestFileVersion() const override;

		virtual std::streamsize read(InputStream& istream) throw (Exception) override;
		virtual std::streamsize write(OutputStream& ostream) const throw (Exception) override;

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

}

}

#endif
