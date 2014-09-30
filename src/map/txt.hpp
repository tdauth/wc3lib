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
 * [section name] // comment<br/>
 * key0 = value0<br/>
 * key1 = value1<br/>
 */

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
		/**
		 * \brief One single entry in a section has a key and a corresponding value.
		 *
		 * The key follows a "=" character and everything following this character including whitespaces etc. is the corresponding value.
		 * The key is not necessarily unique.
		 *
		 * The key can be accessed using \ref first and the value using \ref second of the pair.
		 *
		 * \note We cannot use a map here since keys are not necessarily unique! For example "UI/TriggerData.txt" has multiple pairs with the same key for different options of trigger data.
		 *
		 * \sa wc3lib::map::Txt::Sections wc3lib::map::Txt::Entries wc3lib::map::Txt::Section
		 */
		typedef std::pair<string, string> Entry;
		/**
		 * \brief All entries of a section are stored in a single vector.
		 *
		 * \sa wc3lib::map::Txt::Sections wc3lib::map::Txt::Section wc3lib::map::Txt::Entry
		 */
		typedef std::vector<Entry> Entries;

		/**
		 * \brief A .txt section is indicated by its name (which is not necessarily unique) and its entries which can be empty as well.
		 *
		 * One single section a .txt file usually indicated by a [section name] expression.
		 * Each section has its name plus any number number of key value pairs called entries.
		 *
		 * \ingroup txt
		 *
		 * \sa wc3lib::map::Txt::Sections wc3lib::map::Txt::Entries wc3lib::map::Txt::Entry
		 */
		struct Section
		{
			string name;
			Entries entries;
		};

		/**
		 * \brief All sections of a .txt file are stored in a single vector.
		 *
		 * \ingroup txt
		 *
		 * \sa wc3lib::map::Txt::Section wc3lib::map::Txt::Entries wc3lib::map::Txt::Entry
		 */
		typedef std::vector<Section> Sections;

		/**
		 * \return Returns all sections of the .txt file.
		 *
		 * @{
		 */
		Sections& sections();
		const Sections& sections() const;
		/**
		 * @}
		 */

		/**
		 * Returns all key value entries of section \p section.
		 * \param section An existing section ([bla]) of the TXT file.
		 * \throws Exception Throws an exception if section \p section does not exist.
		 * \note Since sections are stored in a vector for more efficency while reading a TXT file this search has a complexity of O(n).
		 * \note \p section is not necessarily unique! The first section with the name will be returned.
		 * \todo Remove this member function and use \ref wc3lib::editor::MetaData for abstract access.
		 * \deprecated
		 */
		const Entries& entries(const string &section) const;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

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
