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

#ifndef WC3LIB_JASS_CLIENT_HPP
#define WC3LIB_JASS_CLIENT_HPP

#include "../qi.hpp"
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include "ast.hpp"
#include "report.hpp"

namespace wc3lib
{

namespace jass
{

/*
 * The client namespace structs usually should be hidden from the user but can be useful for unit tests
 * using the single rules.
 * The templates should be marked as external in unit tests to reduce compile time.
 */
namespace client
{

namespace qi = boost::spirit::qi;
namespace karma = boost::spirit::karma;

/**
 * Returns the formatted expectation failure of Boost Spirit \p e as human readable string.
 */
template<typename Iterator>
std::string expectationFailure(const boost::spirit::qi::expectation_failure<Iterator> &e);

/**
 * This annotation function template can be used as handler for on_success for each
 * parsed node to store its current location in the file.
 */
template<typename It>
struct annotation_f {
	typedef void result_type;

	annotation_f(It first) : first(first) {}
	annotation_f(const annotation_f<It> &other) : first(other.first) {}
	annotation_f<It>& operator=(const annotation_f<It> &other) {
		this->first = other.first;

		return *this;
	}

	It first;

	// typename File,
	// File *file,
	template<typename Val, typename First, typename Last>
	void operator()(Val v, First f, Last l) const {
		std::cout << "do annotate" << std::endl;
		//do_annotate(v.location, f, l, first);
	}

	// TODO this version is called!
	void operator()() const {
		//std::cout << "Empty call!" << std::endl;
	}

	template<typename Val>
	void operator()(Val v) const {
		std::cout << "One parameter!" << std::endl;
	}

	private:
		// jass_file *file,
		static void do_annotate(jass_ast_location &li, It f, It l, It first) {
			//namespace classic = boost::spirit::classic;
			//using std::distance;

			/*
			const classic::file_position_base<std::string>& pos = f.get_position();
			li.file_name = pos.file;
			li.line = pos.line;
			li.column = pos.column;
			li.length = distance(f, l);
			*/

			/*
			 * use this for
			 * http://www.boost.org/doc/libs/1_47_0/boost/spirit/home/support/iterators/line_pos_iterator.hpp
			 */
			//li.file = file;
			li.line   = boost::spirit::get_line(f);
			li.column = boost::spirit::get_column(first, f);
			li.length = std::distance(f, l);
		}

		static void do_annotate(...) { }
};

/*
 * Doesn't consume eols since value pairs are separated linewise which therefore can be specified easier in the rules
 */
template<typename Iterator>
struct comment_skipper : public qi::grammar<Iterator> {

	comment_skipper();

	qi::rule<Iterator> skip;
	qi::rule<Iterator> emptyline;
	qi::rule<Iterator> moreemptylines;
	qi::rule<Iterator> emptylines;
	qi::rule<Iterator> comment;
};

template <typename Iterator, typename Skipper = comment_skipper<Iterator> >
struct jass_grammar : qi::grammar<Iterator, jass_ast(), qi::locals<std::string>, Skipper>
{
	jass_grammar();

	/**
	 * \param first The starting iterator used for location information storage. Each AST node has stored its location.
	 * \param ast An AST must be passed which is filled by the input. It can already contain nodes which might be useful for debugging.
	 */
	void prepare(Iterator first, jass_ast &ast, jass_file &current_file);


	/**
	 * Handler function to store location of AST entry.
	 */
	boost::phoenix::function<annotation_f<Iterator> > annotate;

	//----------------------------------------------------------------------
	// Symbols
	//----------------------------------------------------------------------
	qi::rule<Iterator, jass_var_reference(), Skipper> var_reference;
	qi::rule<Iterator, jass_type_reference(), Skipper> type_reference;
	qi::rule<Iterator, jass_function_reference(), Skipper> function_reference;

	qi::rule<Iterator, std::string(), Skipper> identifier;
	qi::rule<Iterator, jass_type_reference(), Skipper> type_nothing;

	//----------------------------------------------------------------------
	// Statements
	//----------------------------------------------------------------------
	qi::rule<Iterator, jass_statement(), Skipper> statement;
	qi::rule<Iterator, jass_statements(), Skipper> statements;
	qi::rule<Iterator, jass_set(), Skipper> set;
	qi::rule<Iterator, jass_function_args(), Skipper> args;
	qi::rule<Iterator, jass_call(), Skipper> call;
	qi::rule<Iterator, jass_conditional_statements(), Skipper> conditional_statements;
	qi::rule<Iterator, jass_ifthenelse(), Skipper> ifthenelse;
	qi::rule<Iterator, jass_loop(), Skipper> loop;
	qi::rule<Iterator, jass_exitwhen(), Skipper> exitwhen;
	qi::rule<Iterator, jass_return(), Skipper> return_statement;
	qi::rule<Iterator, jass_debug(), Skipper> debug_statement;

	//----------------------------------------------------------------------
	// Expressions
	//----------------------------------------------------------------------
	qi::rule<Iterator, jass_expression(), Skipper> expression;

	qi::rule<Iterator, int32(), Skipper> integer_literal;
	qi::rule<Iterator, float32(), Skipper> real_literal;
	qi::rule<Iterator, string(), Skipper> string_literal;
	qi::rule<Iterator, bool(), Skipper> boolean_literal;
	qi::rule<Iterator, fourcc(), Skipper> fourcc_literal;
	qi::rule<Iterator, jass_null(), Skipper> null;

	qi::rule<Iterator, jass_expression(), Skipper> binary_operation_expression;
	qi::rule<Iterator, jass_binary_operator(), Skipper> binary_operator;
	qi::rule<Iterator, jass_binary_operation(), Skipper> binary_operation;
	qi::rule<Iterator, jass_unary_operator(), Skipper> unary_operator;
	qi::rule<Iterator, jass_unary_operation(), Skipper> unary_operation;
	qi::rule<Iterator, jass_function_call(), Skipper> function_call;
	qi::rule<Iterator, jass_array_reference(), Skipper> array_reference;
	qi::rule<Iterator, jass_function_ref(), Skipper> function_ref;
	qi::rule<Iterator, jass_const(), Skipper> constant;
	qi::rule<Iterator, jass_parentheses(), Skipper> parentheses;

	//----------------------------------------------------------------------
	// Local Declarations
	//----------------------------------------------------------------------
	qi::rule<Iterator, jass_var_declaration(), Skipper> var_declaration;
	qi::rule<Iterator, jass_locals(), Skipper> locals;

	//----------------------------------------------------------------------
	// Global Declarations
	//----------------------------------------------------------------------
	qi::rule<Iterator, jass_type(), Skipper> type;
	qi::rule<Iterator, jass_global(), Skipper> global;
	qi::rule<Iterator, jass_globals(), Skipper> globals;

	qi::rule<Iterator, jass_function_parameter(), Skipper> function_parameter;
	qi::rule<Iterator, jass_function_parameters(), Skipper> function_parameters;
	qi::rule<Iterator, jass_function_declaration(), Skipper> function_declaration;
	qi::rule<Iterator, jass_native(), Skipper> native;
	qi::rule<Iterator, jass_natives(), Skipper> natives;

	qi::rule<Iterator, jass_declarations(), Skipper> declarations;

	qi::rule<Iterator, jass_function(), Skipper> function;
	qi::rule<Iterator, jass_functions(), Skipper> functions;

	qi::rule<Iterator, jass_file(), Skipper> file;
	qi::rule<Iterator, jass_ast(), qi::locals<std::string>, Skipper> jass;

	// symbol table for types
	// this symbol table can be used in rules! types will be returned automatically when correct identifiers are found!
	// TODO make the grammar stateless, use locals to improve the performance for multiple grammar instances.
	jass_ast ast;
	jass_file *current_file; // currently parsed file
	/*
	 * Symbols for ALL files:
	 */
	jass_type_declarations type_symbols;
	jass_var_declarations global_symbols;
	jass_function_declarations function_symbols;

	jass_binary_operators_qi binary_operators;
	jass_binary_boolean_operators_qi binary_boolean_operators;
	jass_unary_operators_qi unary_operators;

	/*
	 * Error reports.
	 */
	typedef std::vector<Report> Reports;
	Reports reports;
};

template<typename Iterator>
struct jass_generator : public karma::grammar<Iterator, jass_ast()>
{
	jass_generator();

	/**
	 * \param first The starting iterator used for location information storage. Each AST node has stored its location.
	 * \param ast An AST must be passed which is filled by the input. It can already contain nodes which might be useful for debugging.
	 */
	void prepare(Iterator first, jass_ast &ast, jass_file &current_file);

	//----------------------------------------------------------------------
	// Symbols
	//----------------------------------------------------------------------
	karma::rule<Iterator, jass_var_reference()> var_reference;
	karma::rule<Iterator, jass_type_reference()> type_reference;
	karma::rule<Iterator, jass_function_reference()> function_reference;

	karma::rule<Iterator, string()> identifier;
	karma::rule<Iterator, jass_type_reference()> type_nothing;

	//----------------------------------------------------------------------
	// Statements
	//----------------------------------------------------------------------
	karma::rule<Iterator, jass_statement()> statement;
	karma::rule<Iterator, jass_statements()> statements;
	karma::rule<Iterator, jass_set()> set;
	karma::rule<Iterator, jass_function_args()> args;
	karma::rule<Iterator, jass_call()> call;
	karma::rule<Iterator, jass_conditional_statements()> conditional_statements;
	karma::rule<Iterator, jass_ifthenelse()> ifthenelse;
	karma::rule<Iterator, jass_loop()> loop;
	karma::rule<Iterator, jass_exitwhen()> exitwhen;
	karma::rule<Iterator, jass_return()> return_statement;
	karma::rule<Iterator, jass_debug()> debug_statement;

	//----------------------------------------------------------------------
	// Expressions
	//----------------------------------------------------------------------
	karma::rule<Iterator, jass_expression()> expression;

	karma::rule<Iterator, int32()> integer_literal;
	karma::rule<Iterator, float32()> real_literal;
	karma::rule<Iterator, string()> string_literal;
	karma::rule<Iterator, bool()> boolean_literal;
	karma::rule<Iterator, fourcc()> fourcc_literal;
	karma::rule<Iterator, jass_null()> null;

	karma::rule<Iterator, jass_expression()> binary_operation_expression;
	karma::rule<Iterator, jass_binary_operator()> binary_operator;
	karma::rule<Iterator, jass_binary_operation()> binary_operation;
	karma::rule<Iterator, jass_unary_operator()> unary_operator;
	karma::rule<Iterator, jass_unary_operation()> unary_operation;
	karma::rule<Iterator, jass_function_call()> function_call;
	karma::rule<Iterator, jass_array_reference()> array_reference;
	karma::rule<Iterator, jass_function_ref()> function_ref;
	karma::rule<Iterator, jass_const()> constant;
	karma::rule<Iterator, jass_parentheses()> parentheses;

	//----------------------------------------------------------------------
	// Local Declarations
	//----------------------------------------------------------------------
	karma::rule<Iterator, void(bool)> var_declaration_array;
	karma::rule<Iterator, jass_var_declaration()> var_declaration;
	karma::rule<Iterator, jass_locals()> locals;

	//----------------------------------------------------------------------
	// Global Declarations
	//----------------------------------------------------------------------
	karma::rule<Iterator, jass_type()> type;
	karma::rule<Iterator, jass_types()> types;
	karma::rule<Iterator, jass_global()> global;
	karma::rule<Iterator, jass_globals()> globals;

	karma::rule<Iterator, jass_function_parameter()> function_parameter;
	karma::rule<Iterator, jass_function_parameters()> function_parameters;
	karma::rule<Iterator, jass_function_declaration()> function_declaration;
	karma::rule<Iterator, jass_native()> native;
	karma::rule<Iterator, jass_natives()> natives;

	karma::rule<Iterator, jass_declarations()> declarations;

	karma::rule<Iterator, jass_function()> function;
	karma::rule<Iterator, jass_functions()> functions;

	karma::rule<Iterator, jass_file()> file;
	karma::rule<Iterator, jass_ast()> jass;

	jass_ast ast;
	jass_file *current_file; // currently parsed file

	/*
	 * Symbols for ALL files:
	 */
	//jass_type_declarations type_symbols;
	//jass_var_declarations global_symbols;
	//jass_function_declarations function_symbols;

	jass_binary_operators_karma binary_operators;
	jass_binary_boolean_operators_karma binary_boolean_operators;
	jass_unary_operators_karma unary_operators;
};

}

}

}

#endif
