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

#include "lexer.hpp"

namespace wc3lib
{

namespace jass
{

enum class Token {
	Comment,
	Newline,
	ID,
	String,
	Real,
	Integer
};

template <typename Lexer>
struct lexer : lex::lexer<Lexer>
{
	lexer() {
		using boost::spirit::lex::_start;
		using boost::spirit::lex::_end;
		using boost::spirit::lex::_pass;
		using boost::phoenix::ref;
		using boost::phoenix::construct;

		/*
		 * Patterns can be refered.
		 */
		this->self.add_pattern
			("DIGIT", "[0-9]")
			("LETTER", "[a-zA-Z]")
			("WHITESPACES", "[ \t]+")
		;

		/*
		 * Define all tokens as class attributes that they can be accessed by the parser.
		 */
		comments = "//.*";
		whitespaces = "{WHITESPACES}";

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

		integer ="{DIGIT}+";
		id = "{LETTER}({DIGIT}|{LETTER})";

		/*
		 * add all tokens
		 */
		this->self +=
			comments
			| whitespaces
		; //[_pass = lex::pass_flags::pass_ignore]; // Kommentare werden übersprungen

		// operators
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

		// keywords
		this->self +=
			if_keyword
			| else_keyword
		;

		this->self.add
			("[ \t]+") // whitespaces
			(integer)
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
	lex::token_def<> if_keyword;
	lex::token_def<> then_keword;
	lex::token_def<> else_keyword;

	lex::token_def<int32> integer;
	lex::token_def<string> id;
};

template<typename Iterator>
bool tokenize(Iterator first, Iterator last, lexer<lexer_type> &lexer)
{
	typedef lexer<lexer_type>::iterator_type iterator_type;

	lexer<lexer_type> lex;

	return lex::tokenize(first, last, lexer);
}

bool tokenize(const string &value, lexer<lexer_type> &lexer)
{
	return tokenize(value.begin(), value.end(), lexer);
}

bool tokenize(istream &in, lexer<lexer_type> &l)
{
	boost::spirit::istream_iterator first(in);
	boost::spirit::istream_iterator last;

	return tokenize(first, last, l);
}

}

}
