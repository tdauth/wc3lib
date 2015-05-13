/***************************************************************************
 *   Copyright (C) 2015 by Tamino Dauth                                    *
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

#include "generator.hpp"
#include "client.hpp" // defines debug, has to be included first!

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_scope.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

namespace wc3lib
{
	
namespace jass
{

namespace client
{

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
//namespace classic = boost::spirit::classic;

namespace karma_unicode = boost::spirit::karma::unicode;

template <typename Iterator >
void jass_generator<Iterator>::prepare(Iterator first, jass_ast &ast, jass_file &current_file)
{
	this->ast = ast;
	this->current_file = &current_file;
}


template<typename Iterator>
jass_generator<Iterator>::jass_generator() : jass_generator<Iterator>::base_type(jass, "jass")
{
	using karma::eps;
	using karma::lit;
	using karma::eol;
	using karma::_val;
	using karma::_1;
	using karma_unicode::string;
	
	using phoenix::val;
	using phoenix::at_c;
	using phoenix::push_back;
	using phoenix::ref;
	using phoenix::let;
	using phoenix::bind;
	using phoenix::if_;

	identifier %=
		string
	;

	type_nothing %=
		lit("nothing")
	;
	
	//----------------------------------------------------------------------
	// Statements
	//----------------------------------------------------------------------
	statement %=
		set
		| call
		| ifthenelse
		| loop
		| exitwhen
		| return_statement
		| debug_statement
	;

	statements %=
		statement % eol
	;

	/*
	 * Whenever a "set" keyword occurs we expect all three expressions.
	 * Otherwise the line is invalid.
	 */
	set %=
		lit("set")
		<< (
			array_reference // check array_reference first, otherwise var_reference does always succeed
			| var_reference
			)
		<< lit('=')
		<< expression
	;

	args %=
		expression % lit(',')
	;

	call %=
		lit("call")
		<< function_reference
		<< lit('(')
		<< -args
		<< lit(')')
	;

	conditional_statements %=
		expression << lit("then")
		<< -(eol << statements)
	;

	ifthenelse %=
		lit("if") << conditional_statements
		<<
		-(*(
			eol << lit("elseif") << conditional_statements
		))
		<<
		-(
			eol << lit("else")
			<< -(eol << statements)
		)
		<< eol << lit("endif")
	;

	loop %=
		lit("loop")
		<< -(eol << statements)
		<< eol << lit("endloop")
	;

	exitwhen %=
		lit("exitwhen") << expression
	;

	return_statement %=
		lit("return") << -expression
	;

	debug_statement %=
		lit("debug")
		<< (
			set
			| call
			| ifthenelse
			| loop
		)
	;
	
	//----------------------------------------------------------------------
	// Expressions
	//----------------------------------------------------------------------

	expression %=
		binary_operation
		| unary_operation
		| function_call
		| array_reference
		| function_ref
		| constant // check constant before var reference, "true" and "false" are no identifiers!
		| var_reference
		| parentheses
	;

	/*
	* For literals we do not use parsers provided by Boost Spirit to keep the exact grammar definition
	* and to be more flexible in type representation of integers and reals etc.
	*/
	/*
	 * NOTE parse unsigned ints, the sign is considered as unary_operation already!
	 * hex and oct are unsigned as specified by Boost Spirit
	 */
	integer_literal %=
		karma::uint_generator<int32>()
	;

	real_literal %=
		qi::real_parser<float32, qi::strict_ureal_policies<float32> >() // parse unsigned! if no dot is available it should be a "integer_literal" therefore use strict policies
	;

	string_literal %=
		lit("\"")
		<< string
		<< lit("\"")
	;

	boolean_literal %=
		karma::true_
		| karma::false_
	;

	fourcc_literal %=
		lit('\'')
		<< karma::repeat(4)[
			karma::char_
		]
		<< lit('\'')
	;

	null %=
		lit("null")
	;

	// another nested binary operation can only be possible in parentheses, therefore no direct binary_operation is possible
	// parentheses is already considered in expression
	// this avoids endless recursion
	binary_operation_expression %=
		unary_operation
		| function_call
		| array_reference
		| function_ref
		| var_reference
		| constant
		| parentheses
	;

	/*
	 * FIXME
	binary_operator %=
		binary_operators
	;
	*/

	binary_operation %=
		(binary_operation_expression
		<< binary_operator
		<< expression)
		/*| ( // "and" and "or" can combine all expressions without parentheses
			expression
			>> binary_boolean_operators
			>> expression
		)*/
	;

	/*
	 * FIXME
	unary_operator %=
		unary_operators
	;
	*/

	unary_operation %=
		unary_operator
		<< expression
	;

	function_call %=
		function_reference
		<< lit('(')
		<< -args
		<< lit(')')
	;

	array_reference %=
		var_reference
		<< lit('[')
		<< expression
		<< lit(']')
	;

	function_ref %=
		lit("function")
		<< function_reference
	;

	constant %=
		real_literal // has to be checked before integer literal since dot is required
		| integer_literal
		| fourcc_literal
		| boolean_literal
		| string_literal
		| null
	;

	parentheses %=
		lit('(') << expression << lit(')')
	;

	//----------------------------------------------------------------------
	// Local Declarations
	//----------------------------------------------------------------------
	// TODO use %= and no semantic actions
	var_declaration =
		type_reference[at_c<0>(_val) = karma::_1]
		// FIXME
		//<< karma::matches[lit("array")][at_c<1>(_val) = _1]
		<< identifier[at_c<2>(_val) = _1]
		<< -(lit('=') >> expression[at_c<3>(_val) = _1])
	;

	locals %=
		(lit("local") << var_declaration) % eol
	;

	//----------------------------------------------------------------------
	// Global Declarations
	//----------------------------------------------------------------------
	type %=
		lit("type") << identifier <<
		lit("extends") << type_reference // parent type is not optional!
	;

	// do not use %= since we have to set the booleans in semantic actions
	global %=
		qi::matches[lit("constant")]
		<< var_declaration
	;

	globals %=
		lit("globals") << +eol <<
		global % +eol // % operator is used to generate std::vectors!
		<< +eol
		<< lit("endglobals") // >> finish // end of line or input is always expected after "endglobals"!
	;

	function_parameter %=
		type_reference
		<< identifier
	;

	function_parameters %=
		function_parameter % lit(',')
	;

	function_declaration =
		identifier[at_c<0>(_val) = _1] >>
		lit("takes") >>
		(
			lit("nothing")
			| function_parameters[at_c<1>(_val) =_1]
		)
		<< lit("returns")
		<<
		(
			type_nothing[at_c<2>(_val) = _1]
			| type_reference[at_c<2>(_val) = _1]
		)
	;

	native %=
		qi::matches[lit("constant")]
		<< lit("native")
		<< function_declaration
	;

	natives %=
		native % eol
	;

	declarations =
		eps[_val = jass_declarations()] >>
		(
			globals[at_c<1>(_val) = _1]
			//type[push_back(at_c<0>(_val), _1)]
			| type[push_back(at_c<0>(_val), _1)]//[phoenix::bind(&client::add_type_to_symbol_table, type_symbols, _1)]
			| native[push_back(at_c<2>(_val), _1)]//[phoenix::bind(&client::add_function_to_symbol_table, function_symbols, _1)]
		)
		% eol
	;

	function %=
		qi::matches[lit("constant")]
		<< lit("function")
		<< function_declaration
		<< -(
			eol
			<< locals[at_c<2>(_val) = _1]
		)

		<< -(
			eol
			<< statements[at_c<3>(_val) = _1]
		)
		<< eol << lit("endfunction")
	;

	functions %=
		function % eol
	;


	file =
		// skip file path attribute, only required for the generator
		*eol
		<< -declarations[at_c<1>(_val) = _1]
		<< *eol
		<< -functions[at_c<2>(_val) = _1]
		<< *eol
	;

	/*
	 * Parses the current file
	 */
	jass =
		eps[_val = ast]
		<< file[phoenix::push_back(at_c<0>(_val), _1)]
	;

	/*
	 * put all rule names here:
	 */
	// symbols
	var_reference.name("var_reference");
	type_reference.name("type_reference");
	function_reference.name("function_reference");
	identifier.name("identifier");
	//----------------------------------------------------------------------
	// Statements
	//----------------------------------------------------------------------
	statement.name("statement");
	statements.name("statements");
	set.name("set");
	args.name("args");
	call.name("call");
	conditional_statements.name("conditional_statements");
	ifthenelse.name("ifthenelse");
	loop.name("loop");
	exitwhen.name("exitwhen");
	return_statement.name("return_statement");
	debug_statement.name("debug");
	//----------------------------------------------------------------------
	// Expressions
	//----------------------------------------------------------------------
	expression.name("expression");

	integer_literal.name("integer_literal");
	real_literal.name("real_literal");
	string_literal.name("string_literal");
	boolean_literal.name("boolean_literal");
	fourcc_literal.name("fourcc_literal");
	null.name("null");

	binary_operation_expression.name("binary_operation_expression");
	binary_operator.name("binary_operator");
	binary_operation.name("binary_operation");
	unary_operator.name("unary_operator");
	unary_operation.name("unary_operation");
	function_call.name("function_call");
	array_reference.name("array_reference");
	function_ref.name("function_ref");
	constant.name("constant");
	parentheses.name("parentheses");
	//----------------------------------------------------------------------
	// Local Declarations
	//----------------------------------------------------------------------
	var_declaration.name("var_declaration");
	locals.name("locals");
	//----------------------------------------------------------------------
	// Global Declarations
	//----------------------------------------------------------------------
	type.name("type");
	global.name("global");
	globals.name("globals");

	function_parameter.name("function_parameter");
	function_parameters.name("function_parameters");
	function_declaration.name("function_declaration");
	native.name("native");
	natives.name("natives");

	declarations.name("declarations");

	function.name("function");
	functions.name("functions");

	file.name("file");
	jass.name("jass");
	
	BOOST_SPIRIT_DEBUG_NODES(
		// symbols
		(var_reference)
		(type_reference)
		(function_reference)
		(identifier)
		//----------------------------------------------------------------------
		// Statements
		//----------------------------------------------------------------------
		(statement)
		(statements)
		(set)
		(args)
		(call)
		(conditional_statements)
		(ifthenelse)
		(loop)
		(exitwhen)
		(return_statement)
		(debug_statement)
		//----------------------------------------------------------------------
		// Expressions
		//----------------------------------------------------------------------
		(expression)

		(integer_literal)
		(real_literal)
		(string_literal)
		(boolean_literal)
		(fourcc_literal)
		(null)

		(binary_operation_expression)
		(binary_operator)
		(binary_operation)
		(unary_operator)
		(unary_operation)
		(function_call)
		(array_reference)
		(function_ref)
		(constant)
		(parentheses)
		//----------------------------------------------------------------------
		// Local Declarations
		//----------------------------------------------------------------------
		(var_declaration)
		(locals)
		//----------------------------------------------------------------------
		// Global Declarations
		//----------------------------------------------------------------------
		(type)
		(global)
		(globals)

		(function_parameter)
		(function_parameters)
		(function_declaration)
		(native)
		(natives)

		(declarations)

		(function)
		(functions)

		(file)
		(jass)
	);
}

}

Generator::Generator()
{
}

bool Generator::generate(Generator::IteratorType sink, const jass_ast &ast)
{
	return false;
}

bool Generator::generate(Generator::OutputStream &ostream, const jass_ast &ast)
{
	return false;
}

}

}
