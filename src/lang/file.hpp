/***************************************************************************
 *   Copyright (C) 2008, 2009 by Tamino Dauth                              *
 *   tamino@cdauth.de                                                      *
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

#ifndef WC3LIB_LANG_FILE_HPP
#define WC3LIB_LANG_FILE_HPP

#include <istream>

#include <FlexLexer.h>

#include "../exception.hpp"
#include "position.hpp"

namespace wc3lib
{

namespace lang
{

class Parser;
class SourceFile;
class Language;

/**
* Provides functionality of a code file which is treated by a Parser instance.
* This class is able to handle the whole scripting languages syntax and to parse it.
*/
class File : public yyFlexLexer
{
	public:
		File();
		/**
		* @return Returns number of parsed lines.
		*/
		std::string::size_type parse(class Parser &parser, class SourceFile &sourceFile, std::istream &istream) throw (class Exception);

		/**
		* @returns Thread-safe boolean value which indicates if file is currently being used for parsing.
		*/
		bool parses() const;
		const class Parser& parser() const throw (class Exception);
		const class SourceFile& sourceFile() const throw (class Exception);
		/**
		* Required by Bison parser.
		* @todo Replace and set in class function if possbile.
		*/
		void setLanguage(const class Language &language) throw (class Exception);
		/**
		* Returns the current interpreted language in file.
		*/
		const class Language& language() const throw (class Exception);
		/**
		* Returns the current position of parser in file.
		*/
		const class Position& position() const throw (class Exception);

	protected:
		friend class Parser;

		volatile bool m_parses;
		class Parser *m_parser;
		class SourceFile *m_sourceFile;
		const class Language *m_language;
		class Position *m_position;
};

inline bool File::parses() const
{
	return this->m_parses;
}

inline const class Parser& File::parser() const  throw (class Exception)
{
	if (!this->parses())
		throw Exception("Is not parsing.");

	return *this->m_parser;
}

inline const class SourceFile& File::sourceFile() const throw (class Exception)
{
	if (!this->parses())
		throw Exception("Is not parsing.");

	return *this->m_sourceFile;
}

inline void File::setLanguage(const class Language &language) throw (class Exception)
{
	if (!this->parses())
		throw Exception("Is not parsing.");

	this->m_language = &language;
}

inline const class Language& File::language() const throw (class Exception)
{
	if (!this->parses())
		throw Exception("Is not parsing.");

	return *this->m_language;
}

inline const class Position& File::position() const throw (class Exception)
{
	if (!this->parses())
		throw Exception("Is not parsing.");

	return *this->m_position;
}

}

}

#endif
