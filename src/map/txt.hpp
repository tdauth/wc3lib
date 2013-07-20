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

#ifndef WC3LIB_MAP_TXT_HPP
#define WC3LIB_MAP_TXT_HPP

#include <map>

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief Provides access to one single *.txt file's entries such as "UI/MiscUI.txt" or "UI/TriggerData.txt"
 * Uses Boost Spirit library for parsing the file.
 * 
 * \section txtgrammar Grammar of TXT Files
 * [section name] // comment
 * key0 = value0
 * key1 = value1
 * 
 * 
 * \sa TriggerData
 */
class Txt : public Format
{
	public:
		typedef std::map<string, string> Pairs;

		struct Section
		{
			string name;
			Pairs entries;
		};

		typedef boost::ptr_vector<Section> Sections;

		Sections& sections();
		const Sections& sections() const;
		/**
		 * Returns all key value entries of section \p section.
		 * \param section An existing section ([bla]) of the TXT file.
		 * \throws Exception Throws an exception if section \p section does not exist.
		 * \note Since sections are stored in a vector for more efficency while reading a TXT file this search has a complexity of O(n).
		 */
		const Pairs& entries(const string section) const;

		virtual std::streamsize read(InputStream &istream) throw (Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

	private:
		Sections m_sections;
};

inline Txt::Sections& Txt::sections()
{
	return this->m_sections;
}

inline const Txt::Sections& Txt::sections() const
{
	return this->m_sections;
}

}

}

#endif // TXT_HPP
