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

#ifndef WC3LIB_LANG_TOKEN_HPP
#define WC3LIB_LANG_TOKEN_HPP

#include <string>

#include "position.hpp"

namespace wc3lib
{

namespace lang
{

/**
* Provides access to a parsed token.
* Parser holds various containers of all of its parsed tokens.
* Each token has some corresponding information about its language, position and scope.
* This class should be inherited by all other token type classes.
*/
class Token
{
	public:
		Token(class Parser *parser, const class Position &position, class Scope &scope, const std::string &expression);
		virtual ~Token();

		virtual void init() = 0; /// Some tokens has to be initialized after finding all tokens of all files.
		class Parser* parser() const;
		const class Language* language() const;
		const class Position& position() const;
		const class Scope& scope() const;
		const std::string& expression() const;

	protected:
		class Parser *m_parser;
		const class Language *m_language;
		class Position m_position;
		class Scope *m_scope;
		std::string m_expression;

	private:
		Token(const class Token&);
		class Token& operator=(const Token&);
};

}

}

#endif
