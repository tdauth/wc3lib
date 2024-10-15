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
		 * The key can be accessed using Entry::first and the value using Entry::second of the pair.
		 *
		 * \note We cannot use a map here since keys are not necessarily unique! For example "UI/TriggerData.txt" has multiple pairs with the same key for different options of trigger data.
		 *
		 * \sa wc3lib::map::Txt::Sections wc3lib::map::Txt::Entries wc3lib::map::Txt::Section
		 */
		class Entry : public std::pair<string, string>
		{
			public:
				/**
				 * Creates an empty key value entry.
				 */
				Entry();
				/**
				 * Creates a new entry.
				 * \param key The key of the entry.
				 * \param value The value of the entry.
				 */
				Entry(const string &key, const string &value);

				/**
				 * Sets they key of the entry.
				 * \param key The key of the entry.
				 */
				void setKey(const string &key);
				/**
				 * \return Returns the key of the entry.
				 */
				const string& key() const;
				/**
				 * Sets the value of the entry.
				 * \param value The value of the entry.
				 */
				void setValue(const string &value);
				/**
				 * \return Retunrs the value of the entry.
				 */
				const string& value() const;
		};
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
			/**
			 * The name of the section.
			 */
			string name;
			/**
			 * All entries of the section.
			 */
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
		 * Sets sections to \p sections.
		 */
		void setSections(const Sections &sections);
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

		virtual std::streamsize read(InputStream &istream) override;
		/**
		 * Use the flag std::fstream::binary even when opening a string stream.
		 * \param ostream The output stream where the TXT data is written to.
		 * \param useSpaces If this value is true, whitespaces are used around the = character. Otherwise none are used. Whitespace characters around the = character might lead to errors (for example when generating the file "UI/TriggerData.txt").
		 * \return Returns the number of written bytes.
		 */
		virtual std::streamsize write(OutputStream &ostream, bool useSpaces) const;
		virtual std::streamsize write(OutputStream &ostream) const override
		{
			// Dont use whitespaces by default since it might lead to errors on parsing.
			return write(ostream, false);
		}

	private:
		Sections m_sections;
};

inline Txt::Entry::Entry()
{
}

inline Txt::Entry::Entry(const string &key, const string &value) : std::pair<string, string>(key, value)
{
}

inline void Txt::Entry::setKey(const string &key)
{
	this->first = key;
}

inline const string& Txt::Entry::key() const
{
	return this->first;
}

inline void Txt::Entry::setValue(const string &value)
{
	this->second = value;
}

inline const string& Txt::Entry::value() const
{
	return this->second;
}

inline void Txt::setSections(const Txt::Sections &sections)
{
	this->m_sections = sections;
}

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
