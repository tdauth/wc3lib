/***************************************************************************
 *   Copyright (C) 2008 by Tamino Dauth                                    *
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

#ifndef WC3LIB_LANG_PARSER_HPP
#define WC3LIB_LANG_PARSER_HPP

#include <list>

#include <boost/filesystem.hpp>

#include "../exception.hpp"

namespace wc3lib
{

namespace lang
{


/**
* Provides methods for parsing code files.
* Contains lists which hold all parsed tokens.
*/
class Parser
{
	public:
		Parser();
		~Parser();

		/**
		* Parses a single source file with given path \p path and adds its tokens to the parser's lists.
		* \param sourceFileLanguage Each source file can contain code of a different scripting language. If this value is 0 parser's current language is used. Note that the language can be changed in the file by the scripting language itself.
		*/
		std::size_t parse(const boost::filesystem::path &path, const class Language *sourceFileLanguage = 0) throw (class Exception);
		std::size_t parse(const std::list<boost::filesystem::path> &paths, const class Language *initialSourceFileLanguage = 0) throw (class Exception);
		/**
		* Some tokens has to be initialized after parsing all tokens.
		*/
		void initTokens();

		void listSyntaxErrors(std::ostream &ostream);


		/**
		* @return Returns current parsed code file.
		*/
		const class File& file() const;
		/**
		* Use this to set the parser's initial scripting language (used for source files etc.).
		* Parser's default language is always JASS.
		*/
		void setLanguage(const class Language *language);
		/**
		* @return Returns the current scripting language of the parser. Note that parsers can analyse various scripting languages (also in the same file).
		*/
		const class Language* language() const;
		const std::list<class SyntaxError*>& syntaxErrors() const;
		/**
		* @return Returns all parsed tokens.
		*/
		const std::list<class Token*>& tokens() const;

	protected:
		void addSyntaxError(const std::string &message);

		class File *m_file;
		const class Language *m_language;
		std::list<class SyntaxError*> m_syntaxErrors;
		std::list<class Token*> m_tokens;
};

}

}

#endif
