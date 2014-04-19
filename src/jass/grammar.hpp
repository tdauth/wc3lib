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

#ifndef WC3LIB_JASS_GRAMMAR_HPP
#define WC3LIB_JASS_GRAMMAR_HPP

#include "../spirit.hpp"

#include <iterator>

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include "../platform.hpp"
#include "../exception.hpp"
#include "ast.hpp"
#include "client.hpp"

namespace wc3lib
{

namespace jass
{

/**
 * The Grammar contains all rules of the JASS syntax. It provides parse functions which create the
 * resulting AST.
 */
class Grammar {
	public:
		typedef std::basic_istream<byte> InputStream;
		typedef std::istreambuf_iterator<byte> IteratorType;
		typedef boost::spirit::multi_pass<IteratorType> ForwardIteratorType;
		
		/**
		 * Declare iterator types for better error results.
		 * These Position iterators support tracking the current position of the parser.
		 * When an error occurs they their information can be used for better reports.
		 * 
		 * Usually you would create objects of theses types from the multi pass iterator \ref ForwardIteratorType and pass
		 * them to the \ref parse() element functions.
		 * 
		 * Both types are instanciated in "grammar.cpp" and can be declared as external templates to save compile time.
		 */
		typedef boost::spirit::classic::position_iterator2<Grammar::ForwardIteratorType> ClassicPositionIteratorType;
		typedef boost::spirit::line_pos_iterator<Grammar::ForwardIteratorType> PositionIteratorType;
		
		bool parse(InputStream &istream, jass_ast &ast, jass_file &file);
		bool parse(IteratorType first, IteratorType last, jass_ast &ast, jass_file &file);
		
		/**
		 * Creates the parsed file automatically using \p fileName as file path.
		 * The created file will be added to the AST.
		 */
		bool parse(InputStream &istream, jass_ast &ast, const std::string &fileName = "JASS file");
		
		/**
		 * \defgroup GrammarSymbols Grammar Symbols
		 *  @{
		 * 
		 * Return symbol tables from parsed input.
		 */
		const jass_type_declarations& typeSymbols() const;
		const jass_var_declarations& globalSymbols() const;
		const jass_function_declarations& functionSymbols() const;
		/** @} */
		
	private:
		/*
		 * Internal grammars for JASS grammar and its skipper.
		 * We do not want to create a new grammar instance each time we parse something.
		 * Therefore we use these attributes for all parsing operations.
		 */
		client::jass_grammar<PositionIteratorType, client::comment_skipper<PositionIteratorType> > grammar;
		client::comment_skipper<PositionIteratorType> skipper;
};

inline const jass_type_declarations& Grammar::typeSymbols() const
{
	return grammar.type_symbols;
}

inline const jass_var_declarations& Grammar::globalSymbols() const
{
	return grammar.global_symbols;
}

inline const jass_function_declarations& Grammar::functionSymbols() const
{
	return grammar.function_symbols;
}



}

}

#endif
