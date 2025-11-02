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

/**
 * \file
 * \todo Implement Karma based JASS generator.
 */

#include "generator_impl.hpp"
#include "client.hpp" // defines debug, has to be included first!
#include "adaptions.hpp"

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/phoenix/stl.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/scope.hpp>
#include <boost/phoenix/statement.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

namespace wc3lib
{

namespace jass
{

const GeneratorImpl::JassGenerator GeneratorImpl::generator;

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

/**
 * \return Returns true if type \p type_reference is type nothing.
 */
bool is_type_nothing(const jass_type_reference &type_reference)
{
	return type_reference.whichType() == jass_type_reference::Type::String && boost::get<string>(type_reference.variant) == "nothing";
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
	using karma::omit;
	using karma::bool_;

	using phoenix::val;
	using phoenix::at_c;
	using phoenix::push_back;
	using phoenix::pop_back;
	using phoenix::ref;
	using phoenix::let;
	using phoenix::bind;
	using phoenix::if_;

	identifier %=
		string
	;

	type_nothing =
		(karma::omit[karma::bool_(true)] << lit("nothing"))[_1 = phoenix::bind(is_type_nothing, ref(_val))]
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
		expression[_1 = at_c<0>(_val)] << lit("then")
		<< -(eol << statements[_1 = at_c<1>(_val)])
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
		karma::int_generator<int32>()
	;

	real_literal %=
		// karma::strict_ureal_policies<float32>
		karma::real_generator<float32>() // parse unsigned! if no dot is available it should be a "integer_literal" therefore use strict policies
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
		eps[_1 =_val]
		<< lit("null")
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

	binary_operator %=
		binary_operators
	;

	// TODO %=
	binary_operation =
		(binary_operation_expression[_1 = at_c<0>(_val)]
		<< binary_operator[_1 = at_c<1>(_val)]
		<< expression[_1 = at_c<2>(_val)])
		| ( // "and" and "or" can combine all expressions without parentheses
			expression[_1 = at_c<0>(_val)]
			<< binary_boolean_operators[_1 = at_c<1>(_val)]
			<< expression[_1 = at_c<2>(_val)]
		)
	;

	unary_operator %=
		unary_operators
	;

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
	var_declaration_array %=
		(karma::omit[karma::bool_(true)] << lit("array") | "")
	;

	// TODO use %= and no semantic actions
	var_declaration =
		type_reference[_1 = at_c<0>(_val)]
		<< var_declaration_array(at_c<1>(_val))
		<< identifier[_1 = at_c<2>(_val)]
		<< (-(lit('=') << expression))[_1 = at_c<3>(_val)]
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

	types %=
		type % eol
	;

	// do not use %= since we have to set the booleans in semantic actions
	global %=
		(karma::omit[karma::bool_(true)] << lit("constant") | karma::omit[karma::bool_(false)] << lit(""))[_1 = at_c<0>(_val)] // is_constant
		//qi::matches[lit("constant")]
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
		identifier[_1 = at_c<0>(_val)] <<
		lit("takes") <<
		(
			lit("nothing")
			| function_parameters[_1 = at_c<1>(_val)]
		)
		<< lit("returns")
		<<
		(
			type_nothing[_1 = at_c<2>(_val)]
			| type_reference[_1 = at_c<2>(_val)]
		)
	;

	native %=
		(karma::omit[karma::bool_(true)] << lit("constant") | karma::omit[karma::bool_(false)] << lit(""))[_1 = at_c<0>(_val)] // is_constant
		<< lit("native")
		<< function_declaration
	;

	natives %=
		native % eol
	;

	/*
	jass_types types;
	jass_globals globals;
	jass_natives natives;
	*/

	declarations =
		eps[_val = jass_declarations()]
		<< globals[_1 = at_c<1>(_val)]
		<< types[_1 = at_c<0>(_val)]//[phoenix::bind(&client::add_type_to_symbol_table, type_symbols, _1)]
		<< natives[_1 = at_c<2>(_val)]//[phoenix::bind(&client::add_function_to_symbol_table, function_symbols, _1)]
	;

	function %=
		(karma::omit[karma::bool_(true)] << lit("constant") | karma::omit[karma::bool_(false)] << lit(""))[_1 = at_c<0>(_val)] // is_constant
		<< lit("function")
		<< function_declaration
		<< -(
			eol
			<< locals[_1 = at_c<2>(_val)]
		)

		<< -(
			eol
			<< statements[_1 = at_c<3>(_val)]
		)
		<< eol << lit("endfunction")
	;

	functions %=
		function % eol
	;


	file =
		// skip file path attribute, only required for the generator
		*eol
		<< -declarations[_1 = at_c<1>(_val)]
		<< *eol
		<< -functions[_1 = at_c<2>(_val)]
		<< *eol
	;

	/*
	 * Parses the current file
	 */
	jass =
		eps[_val = ast]
		<< (file % eol)[_1 = at_c<0>(_val)]
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
	types.name("types");
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
		(types)
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

GeneratorImpl::GeneratorImpl()
{
}

bool GeneratorImpl::generate(Generator::IteratorType sink, const jass_ast &ast)
{
	return false;
}

bool GeneratorImpl::generate(Generator::OutputStream &ostream, const jass_ast &ast)
{
	return false;
}

}

}
