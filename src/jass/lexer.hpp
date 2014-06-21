/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#ifndef WC3LIB_JASS_LEXER_HPP
#define WC3LIB_JASS_LEXER_HPP

#include "../spirit.hpp"
#include "../platform.hpp"
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

namespace wc3lib
{

namespace jass
{

namespace lex = boost::spirit::lex;

enum class Token
{
	If,
	Then,
	Else,
	Function,
	Max
};

/**
 * \brief JASS lexer for tokenizing JASS files.
 *
 * Boost Spirit offers the optional usage of a separate lexer.
 * Using a separate lexer/scanner might improve the performance since when applying grammar rules
 * the input is already tokenized and the lookup does not need to be very big.
 */
template <typename Lexer>
struct lexer : lex::lexer<Lexer>
{
	lexer() {
		using boost::spirit::lex::_start;
		using boost::spirit::lex::_end;
		using boost::spirit::lex::_pass;
		using boost::phoenix::ref;
		//using boost::phoenix::construct;

		/*
		 * Patterns can be refered.
		 */
		this->self.add_pattern
			("DIGIT", "[0-9]")
			("LETTER", "[a-zA-Z]")
		//	("WHITESPACES", "[ \t]+")
		;

		/*
		 * Define all tokens as class attributes that they can be accessed by the parser.
		 */
		//comments = "//.*";
		//whitespaces = "{WHITESPACES}";

		plus = "+";
		minus = "-";
		multiply = "*";
		divide = "/";
		less_than = "<";
		greater_than = ">";
		assign = "=";
		unequal = "!=";
		and_op = "&";

		if_keyword = "if";
		else_keyword = "else";
		function_keyword = "function";
		takes_keyword = "takes";
		returns_keyword = "returns";
		nothing_keyword = "nothing";
		endfunction_keyword = "endfunction";

		//integer ="{DIGIT}+";
		id = "{LETTER}({DIGIT}|{LETTER})+";

		/*
		 * add all tokens
		 */
		//this->self +=
			//comments
		//	whitespaces
		//; //[_pass = lex::pass_flags::pass_ignore]; // Kommentare werden übersprungen

		// operators
		/*
		this->self +=
			plus
			| minus
			| multiply
			| divide
			| less_than
			| greater_than
			| assign
			| unequal
			| and_op
		;
		*/

		// keywords
		this->self +=
			if_keyword
			| else_keyword
			| function_keyword
			| takes_keyword
			| returns_keyword
			| nothing_keyword
			| endfunction_keyword
		;

		this->self.add
			//(integer)
			(id)
		;
	}

	/*
	 * Alle Tokentypen besitzen ein Attribut.
	 */

	// Kommentare liefern kein Ergebnis. Sie sollen übersprungen werden.
	lex::token_def<> comments;
	lex::token_def<> whitespaces;

	// Operatoren liefern ein einzelnes Zeichen zurück.
	lex::token_def<char> plus;
	lex::token_def<char> minus;
	lex::token_def<char> multiply;
	lex::token_def<char> divide;
	lex::token_def<char> less_than;
	lex::token_def<char> greater_than;
	lex::token_def<char> assign;
	lex::token_def<char> unequal;
	lex::token_def<char> and_op;

	// Schlüsselwörter müssen nichts speichern. Ihr Auftreten alleine reicht aus, um den Parser zu steuern.
	lex::token_def<Token> if_keyword;
	lex::token_def<Token> then_keword;
	lex::token_def<Token> else_keyword;
	lex::token_def<Token> function_keyword;
	lex::token_def<Token> takes_keyword;
	lex::token_def<Token> returns_keyword;
	lex::token_def<Token> nothing_keyword;
	lex::token_def<Token> endfunction_keyword;

	lex::token_def<int32> integer;
	lex::token_def<string> id;
};

typedef lex::lexertl::token<
		boost::spirit::istream_iterator
		//char const*, boost::mpl::vector<std::string>
		> token_type;
typedef lex::lexertl::lexer<token_type> lexer_type;

template<typename Iterator, typename F>
bool tokenize(Iterator first, Iterator last, lexer<lexer_type> &l, F f)
{
	typedef lexer<lexer_type>::iterator_type iterator_type;

	lexer<lexer_type> lex;

	return lex::tokenize(first, last, l, f);
}

/*
bool tokenize(const string &value, lexer<lexer_type> &l)
{
	return tokenize(value.cbegin(), value.cend(), l);
}
*/

template<typename F>
bool tokenize(istream &in, lexer<lexer_type> &l, F f)
{
	boost::spirit::istream_iterator first(in);
	boost::spirit::istream_iterator last;

	return tokenize(first, last, l, f);
}

}

}

#endif
