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

/**
 * \defgroup txt TXT files
 *
 * \section txtgrammar Grammar of TXT Files
 * [section name] // comment
 * key0 = value0
 * key1 = value1
 */

#include <map>

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief Provides access to one single .txt file's entries such as "UI/MiscUI.txt" or "UI/TriggerData.txt".
 *
 * All sections can be accessed by \ref sections() which returns a vector of \ref Section instances.
 *
 * Uses Boost Spirit library for parsing the file.
 *
 * \ingroup txt
 * \sa TriggerData TriggerStrings
 */
class Txt : public Format
{
	public:
		/// We cannot use a map here since keys are not necessarily unique! For example "UI/TriggerData.txt" has multiple pairs with the same key for different options of trigger data.
		typedef std::pair<string, string> Pair;
		typedef std::vector<Pair> Pairs;

		/**
		 * One single section a .txt file usually indicated by a [section name] expression.
		 * Each section has its name plus any number number of key value pairs.
		 *
		 * \ingroup txt
		 */
		struct Section
		{
			string name;
			Pairs entries;
		};

		/**
		 * Type for all sections stored in a .txt file.
		 *
		 * \ingroup txt
		 */
		typedef std::vector<Section> Sections;

		Sections& sections();
		const Sections& sections() const;
		/**
		 * Returns all key value entries of section \p section.
		 * \param section An existing section ([bla]) of the TXT file.
		 * \throws Exception Throws an exception if section \p section does not exist.
		 * \note Since sections are stored in a vector for more efficency while reading a TXT file this search has a complexity of O(n).
		 * \note \p section is not necessarily unique! The first section with the name will be returned.
		 */
		const Pairs& entries(const string &section) const;

		virtual std::streamsize read(InputStream &istream) throw (Exception) override;
		virtual std::streamsize write(OutputStream &ostream) const throw (Exception) override;

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
