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

#ifndef WC3LIB_MAP_STRINGS_HPP
#define WC3LIB_MAP_STRINGS_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * Class for FDF and WTS file formats.
 */
class Strings : public Format
{
	public:
		typedef std::map<std::size_t, class String*> StringList; // key is string id
		typedef std::pair<std::size_t, class String*> StringListValue;
		typedef std::pair<std::size_t, const class String*> StringListValueConst;

		BOOST_SCOPED_ENUM_START(ConflictResult)
		{
			UseBoth,
			UseFirst,
			UseSecond
		};
		BOOST_SCOPED_ENUM_END

		typedef BOOST_SCOPED_ENUM(ConflictResult) (*ConflictFunction)(const class String &string1, const class String &string2);


		Strings();
		virtual ~Strings();

		const boost::filesystem::path& path() const;
		const StringList& stringList() const;

		/**
		* This function allows you to parse JASS or similar code files. It detects all necessary translation functions and their parameter string values.
		* @note It recognizes line and block comments (// and /*).
		* @param path Each string entry needs a corresponding file path and line for usage comparison.
		* @param ostream Should not be 0 if replace is true. Output stream for replaced version.
		* @param translationFunctions List of translation function identifiers which should be recognized.
		* @param conflictFunction If this value is not 0 function will be called when two strings have the same default string. Use Strings::ConflictResult as return value to solve such conflicts.
		* @return Returns pair with get and put stream sizes.
		*/
		virtual std::pair<std::streamsize, std::streamsize> parse(const boost::filesystem::path &path, std::basic_istream<byte> &istream, std::basic_ostream<byte> *ostream = 0, const bool replace = false, const bool fill = true, const bool ignoreReplacedValues = true, const std::list<std::string> &translationFunctions = std::list<std::string>(1, "GetLocalizedString"), ConflictFunction conflictFunction = 0) throw (class Exception);

		/**
		* Reads WTS format.
		*/
		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		/**
		* Writes WTS format.
		*/
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual std::streamsize readFdf(InputStream &istream) throw (class Exception);
		virtual std::streamsize writeFdf(OutputStream &ostream) const throw (class Exception);

		virtual void list(OutputStream  &ostream) const;

	protected:
		boost::filesystem::path m_path;
		StringList m_stringList;
};

inline const boost::filesystem::path& Strings::path() const
{
	return this->m_path;
}

inline const Strings::StringList& Strings::stringList() const
{
	return this->m_stringList;
}

}

}

#endif
