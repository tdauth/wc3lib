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

/**
 * \brief Different token types of the JASS scripting language.
 *
 * Each token is a std::size_t value and refers as ID for Boost Spirit token instances.
 */
enum class Token : std::size_t
{
	If,
	Then,
	Else,
	Function,
	EndFunction,
	Takes,
	Nothing,
	Returns
};

/**
 * \brief JASS lexer for tokenizing JASS files.
 *
 * Boost Spirit offers the optional usage of a separate lexer.
 * Using a separate lexer/scanner might improve the performance since when applying grammar rules
 * the input is already tokenized and the lookup does not need to be very big.
 *
 * For each token \ref Token is used to determine its ID.
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
			("WHITESPACES", "[ \\t]+")
			("COMMENT", "\\/\\/.*\\n")
		;

		/*
		 * Define all tokens as class attributes that they can be accessed by the parser.
		 */
		whitespaces = "({COMMENT} | {WHITESPACES})+";

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
		; //[_pass = lex::pass_flags::pass_ignore]; // Kommentare werden übersprungen

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
		this->self.add
			(if_keyword, (std::size_t)Token::If)
			(else_keyword, (std::size_t)Token::Else)
			(function_keyword, (std::size_t)Token::Function)
			(takes_keyword, (std::size_t)Token::Takes)
			(returns_keyword, (std::size_t)Token::Returns)
			(nothing_keyword, (std::size_t)Token::Nothing)
			(endfunction_keyword, (std::size_t)Token::EndFunction)
		;

		this->self.add
			//(integer)
			(id)
		;

		this->self("WS") = whitespaces;
	}

	/*
	 * Alle Tokentypen besitzen ein Attribut.
	 */

	/*
	 * Whitespaces should simply be omitted.
	 * TODO Add the possibility to store comments in the AST.
	 */
	lex::token_def<lex::omit> whitespaces;

	// Operators are identified by their token ids as well.
	// We do not need to store anything.
	lex::token_def<> plus;
	lex::token_def<> minus;
	lex::token_def<> multiply;
	lex::token_def<> divide;
	lex::token_def<> less_than;
	lex::token_def<> greater_than;
	lex::token_def<> assign;
	lex::token_def<> unequal;
	lex::token_def<> and_op;

	// Schlüsselwörter müssen nichts speichern. Ihr Auftreten alleine reicht aus, um den Parser zu steuern.
	lex::token_def<> if_keyword;
	lex::token_def<> then_keword;
	lex::token_def<> else_keyword;
	lex::token_def<> function_keyword;
	lex::token_def<> takes_keyword;
	lex::token_def<> returns_keyword;
	lex::token_def<> nothing_keyword;
	lex::token_def<> endfunction_keyword;

	lex::token_def<int32> integer;
	lex::token_def<string> id;
};

/**
 * \brief Corresponding token data type of JASS Scanner \ref lexer.
 */
typedef lex::lexertl::token<
		boost::spirit::istream_iterator
		//char const*, boost::mpl::vector<std::string>
		> token_type;
/**
 * \brief Corresponding lexer data type of JASS Scanner \ref lexer. This type can be passed as the template parameter \p Lexer for \ref lexer.
 */
typedef lex::lexertl::lexer<token_type> lexer_type;

/**
 * \brief Basic tokenizer function which uses Scanner \p l to tokenize the input from the iterator range \p first and \p last.
 *
 * For simpler input stream usage use \ref tokenize(istream&, lexer<lexer_type> &, F).
 */
template<typename Iterator, typename F>
bool tokenize(Iterator first, Iterator last, lexer<lexer_type> &l, F f)
{
	/*
	typedef lexer<lexer_type>::iterator_type iterator_type;

	lexer<lexer_type> lex;
	*/

	return lex::tokenize(first, last, l, f, "WS");
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
