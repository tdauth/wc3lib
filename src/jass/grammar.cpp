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

#include <vector>

#include "client.hpp" // defines debug, has to be included first!
#include "ast.hpp"
#include "grammar.hpp"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

#include "../platform.hpp"
#include "../i18n.hpp"

namespace wc3lib
{

namespace jass
{

namespace client {

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace classic = boost::spirit::classic;
/**
 * The Unicode namespace can be used for UTF-8 string and comments parsing in JASS.
 */
namespace unicode = boost::spirit::qi::unicode;

/**
 * This function can be called whenever an \ref boost::spirit::qi::expectation_failure occurs to
 * print it in a readable format.
 * It returns a formatted string containing file path, line and column.
 */
template<typename Iterator>
std::string expectationFailure(const jass_file *file, const boost::spirit::qi::expectation_failure<Iterator> &e) {
	//const classic::file_position_base<std::string>& pos = e.first.get_position();
	//std::stringstream msg;
	//msg
	//<< std::setw(boost::spirit::get_column(e.first)) << " " << _("^- here");

	return boost::str(
		boost::format(_("Parse error at file %1%:%2%")) // :%3%:\n%4%
		% file->path
		% boost::spirit::get_line(e.first)
		//% boost::spirit::get_column(e.first)
		//% e.first.get_currentline()
		//% msg.str()
	);
}

/**
 * Specialization for classic position iterator type.
 */
template<>
std::string expectationFailure<Grammar::ClassicPositionIteratorType>(const jass_file *file, const boost::spirit::qi::expectation_failure<Grammar::ClassicPositionIteratorType> &e) {
	const classic::file_position_base<std::string>& pos = e.first.get_position();
	std::stringstream msg;
	msg
	<< std::setw(pos.column) << " " << _("^- here");

	return boost::str(
		boost::format(_("Parse error at file %1%:%2%:%3%:\n%4%"))
		% pos.file
		% pos.line
		% pos.column
		% e.first.get_currentline()
		% msg.str()
	);
}

inline void add_type_to_symbol_table(jass_type_declarations &declarations, jass_type &type) {
	declarations.add(type.identifier, type);
}

/**
 * This functions searches for identifier \p value in the symbol table \p types.
 * If type is not found the identifier string is returned.
 * Otherwise the type pointer is returned.
 */
inline jass_type_reference get_type_symbol(jass_type_declarations &types, const std::string &value) {
	jass_type *type = types.find(value.c_str());
	
	if (type == 0) {
		return value;
	}
	
	return type;
}

inline void add_var_to_symbol_table(jass_var_declarations &declarations, jass_var_declaration &var) {
	declarations.add(var.identifier, var);
}

inline jass_var_reference get_var_symbol(jass_var_declarations &vars, const std::string &value) {
	jass_var_declaration *var = vars.find(value.c_str());
	
	if (var == 0) {
		return value;
	}
	
	return var;
}

inline void add_function_to_symbol_table(jass_function_declarations &declarations, jass_function_declaration &function) {
	declarations.add(function.identifier, function);
}

inline jass_function_reference get_function_symbol(jass_function_declarations &functions, const std::string &value) {
	jass_function_declaration *function = functions.find(value.c_str());
	
	if (function == 0) {
		return function;
	}
	
	return function;
}

/*
 * Doesn't consume eols since value pairs are separated linewise which therefore can be specified easier in the rules
 */
template<typename Iterator>
comment_skipper<Iterator>::comment_skipper() : comment_skipper<Iterator>::base_type(skip, "comments and blanks")
{
	using qi::lit;
	using ascii::char_;
	using ascii::blank;
	using qi::eol;
	using qi::eoi;
	using qi::eps;
	
	/*
	 * Comments may use UTF-8 characters.
	 */
	comment %=
		lit("//") >> *(unicode::char_ - eol)
	;
	
	emptyline %=
		+blank >> -comment // blanks only optionally followed by a comment
		| comment // one comment only
	;
	
	moreemptylines %=
		(eol >> -emptyline >> &eol)
	;
	
	emptylines %=
		// do not consume the eol of the last empty line because it is the eol between all skipped empty lines and the first one
		+moreemptylines
	;
	
	skip %= 
		emptylines
		| comment
		| blank // check blank as last value
	;
	
	emptyline.name("emptyline");
	moreemptylines.name("moreemptylines");
	emptylines.name("emptylines");
	comment.name("comment");
	skip.name("skip");
	
	BOOST_SPIRIT_DEBUG_NODES(
		(emptyline)
		(moreemptylines)
		(emptylines)
		(comment)
		(skip)
	);
}

template <typename Iterator, typename Skipper >
jass_grammar<Iterator, Skipper>::jass_grammar(Iterator first, jass_ast &ast, jass_file &current_file)
: jass_grammar<Iterator, Skipper>::base_type(jass, "jass")
	, ast(ast)
	, current_file(&current_file)
	, annotate(first)
{
	using qi::eps;
	using qi::int_parser;
	using qi::double_;
	using qi::lit;
	using qi::oct;
	using qi::hex;
	using qi::true_;
	using qi::false_;
	using qi::_val;
	using qi::lexeme;
	using qi::eol;
	using qi::eoi;
	using qi::matches;
	using qi::attr_cast;
	using qi::as_string;
	using qi::as;
	using qi::repeat;
	using qi::on_error;
	using qi::fail;
	using ascii::char_;
	using ascii::string;
	using namespace qi::labels;

	//using phoenix::construct;
	using phoenix::val;
	using phoenix::at_c;
	using phoenix::push_back;
	using phoenix::ref;
	
	// TODO get locals of current function as well (including function parameters which are also locals)!
	var_reference =
		identifier[_val = phoenix::bind(&get_var_symbol, ref(global_symbols), _1)]
	;
	
	type_reference =
		identifier[_val = phoenix::bind(&get_type_symbol, ref(type_symbols), _1)]
	;
	
	// gets natives as well
	function_reference =
		identifier[_val = phoenix::bind(&get_function_symbol, ref(function_symbols), _1)]
	;
	
	identifier %= 
		lexeme[
			char_("a-zA-Z")
			>> *char_("a-zA-Z_0-9")
			/*
			 * TODO This expression cannot be resolved properly by Boost Spirit
			>> -(*char_("a-zA-Z_0-9") 
			> char_("a-zA-Z0-9"))
			*/
		]
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
		//| debug
	;
	
	statements %=
		statement % eol
	;
	
	set %=
		lit("set")
		>> (
			array_reference // check array_reference first, otherwise var_reference does always succeed
			| var_reference
			)
		>> lit('=')
		>> expression
	;

	args %=
		expression % lit(',')
	;
	
	call %=
		lit("call")
		>> function_reference
		>> lit('(')
		>> -args
		>> lit(')')
	;
	
	conditional_statements %=
		expression >> lit("then")
		>> -(eol >> statements)
	;
	
	ifthenelse %=
		lit("if") >> conditional_statements
		>> 
		*(
			eol >> lit("elseif") >> conditional_statements
		)
		>>
		-(
			eol >> lit("else")
			>> -(eol >> statements)
		)
		>> eol >> lit("endif")
	;
	
	loop %=
		lit("loop")
		>> -(eol >> statements)
		>> eol >> lit("endloop")
	;

	exitwhen %=
		lit("exitwhen") >> expression
	;

	return_statement %=
		lit("return") >> -expression
	;

	debug_statement %=
		lit("debug")
		>> (
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
		lexeme[ 
			(lit('$') | qi::no_case["0x"]) 
			>> hex 
		] // TODO custom type int32?
		| lexeme[ 
			lit('0')
			>> oct
		] // TODO custom type int32?
		| qi::uint_parser<int32>()
	;
	
	real_literal %=
		qi::real_parser<float32, qi::strict_ureal_policies<float32> >() // parse unsigned! if no dot is available it should be a "integer_literal" therefore use strict policies
	;
	
	string_literal %=
		lexeme[
			lit("\"")
			>>
			*(
				unicode::char_ - char_('\"') - char_('\\') 
				| lit('\\')[push_back(_val, '\\')] >> unicode::char_
			)
			>> lit("\"")
		]
	;
	
	boolean_literal %=
		true_
		| false_
	;
		
	fourcc_literal %=
		as<fourcc>()[
			lit('\'')
			>> repeat(4)[
				char_
			]
			>> lit('\'')
		]
	;
	
	null %=
		as<jass_null>()[
			lit("null")[_val = jass_null()]
		]
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

	binary_operation %=
		(binary_operation_expression
		>> binary_operator
		>> expression)
		/*| ( // "and" and "or" can combine all expressions without parentheses
			expression
			>> binary_boolean_operators
			>> expression
		)*/
	;
	
	unary_operator %=
		unary_operators
	;

	unary_operation %=
		unary_operator
		>> expression
	;
	
	function_call %=
		function_reference
		>> lit('(')
		>> -args
		>> lit(')')
	;
	
	array_reference %=
		var_reference
		>> lit('[')
		>> expression
		>> lit(']')
	;
	
	function_ref %=
		lit("function")
		>> function_reference
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
		lit('(') >> expression >> lit(')')
	;

	//----------------------------------------------------------------------
	// Local Declarations
	//----------------------------------------------------------------------
	// TODO use %= and no semantic actions
	var_declaration =
		type_reference[at_c<0>(_val) = _1]
		>> qi::matches[lit("array")][at_c<1>(_val) = _1]
		>> identifier[at_c<2>(_val) = _1]
		>> -(lit('=') >> expression[at_c<3>(_val) = _1])
	;
	
	locals %=
		(lit("local") >> var_declaration) % eol
	;
	
	//----------------------------------------------------------------------
	// Global Declarations
	//----------------------------------------------------------------------
	type %=
		lit("type") >> identifier >>
		lit("extends") >> type_reference // parent type is not optional!
	;
	
	// do not use %= since we have to set the booleans in semantic actions
	global %=
		qi::matches[lit("constant")]
		>> var_declaration
	;
	
	globals %=
		lit("globals") >> +eol >>
		global % +eol >> // % operator is used to generate std::vectors!
		+eol >>
		lit("endglobals") // >> finish // end of line or input is always expected after "endglobals"!
	;
	
	function_parameter %=
		type_reference
		>> identifier
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
		>> lit("returns")
		>> 
		(
			string("nothing")[at_c<2>(_val) = _1]
			| type_reference[at_c<2>(_val) = _1]
		)
	;
	
	native %=
		qi::matches[lit("constant")]
		>> lit("native")
		>> function_declaration
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
		>> lit("function")
		>> function_declaration
		>> -(
			eol
			>> locals[at_c<2>(_val) = _1]
		)
		
		>> -(
			eol
			>> statements[at_c<3>(_val) = _1]
		)
		>> eol >> lit("endfunction")
	;

	functions %=
		function % eol
	;

	
	file %=
		*eol >>
		-declarations >>
		*eol >>
		-functions >>
		*eol
		;
	
	/*
	 * Parses the current file
	 */
	jass = 
		eps[_val = ast] >>
		file
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

	on_error<fail>
	(
		file,
		std::cerr
			<< val("Error! Expecting ")
			<< _4                               // what failed?
			<< val(" here: \"")
			// TODO Phoenix construct in Boost 1.55
			//<< construct<std::string>(_3, _2)   // iterators to error-pos, end
			<< val("\"")
			<< std::endl
	);
	
	/*
	 * Store location info on success.
	 * https://stackoverflow.com/questions/19612657/boostspirit-access-position-iterator-from-semantic-actions
	 */
	auto set_location_info = annotate(_val, current_file, _1, _3);
	//qi::on_success(identifier, set_location_info);
	//qi::on_success(string_literal, set_location_info);
	//qi::on_success(integer_literal, set_location_info);
	
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

template <typename Iterator>
bool parse(Iterator first, Iterator last, jass_ast &ast, jass_file &current_file)
{
	jass_grammar<Iterator> grammar(first, ast, current_file);
	comment_skipper<Iterator> commentSkipper;
	bool r = false;
	
	try {
		r = boost::spirit::qi::phrase_parse(
			first,
			last,
			grammar,
			commentSkipper,
			ast // store result into the passed ast
		);
	} 
	catch(const boost::spirit::qi::expectation_failure<Iterator> &e) {
		throw Exception(
				client::expectationFailure(grammar.current_file, e)
			);
	}

	if (first != last) // fail if we did not get a full match
	{
		return false;
	}

	return r;
}

/*
 * http://programmers.stackexchange.com/questions/177546/how-do-you-handle-increasingly-long-compile-times-when-working-with-templates
 * used for explicit instanciation which reduces compile time for targets which use rules from "client.hpp" because they do not need to include "grammar.cpp"
 * as long as they use the iterator type PositionIteratorType
 */
template std::string expectationFailure<Grammar::PositionIteratorType>(const jass_file *file, const boost::spirit::qi::expectation_failure<Grammar::PositionIteratorType> &e);
template class comment_skipper<Grammar::PositionIteratorType>;
template class jass_grammar<Grammar::PositionIteratorType, comment_skipper<Grammar::PositionIteratorType> >;
template bool parse<Grammar::PositionIteratorType>(Grammar::PositionIteratorType first, Grammar::PositionIteratorType last, jass_ast &ast, jass_file &current_file);
// ClassicPositionIteratorType
template std::string expectationFailure<Grammar::ClassicPositionIteratorType>(const jass_file *file, const boost::spirit::qi::expectation_failure<Grammar::ClassicPositionIteratorType> &e);
template class comment_skipper<Grammar::ClassicPositionIteratorType>;
template class jass_grammar<Grammar::ClassicPositionIteratorType, comment_skipper<Grammar::ClassicPositionIteratorType> >;
template bool parse<Grammar::ClassicPositionIteratorType>(Grammar::ClassicPositionIteratorType first, Grammar::ClassicPositionIteratorType last, jass_ast &ast, jass_file &current_file);

}

bool Grammar::parse(Grammar::InputStream& istream, jass_ast& ast, jass_file &file)
{
	return this->parse(IteratorType(istream), IteratorType(), ast, file);
}

bool Grammar::parse(IteratorType first, IteratorType last, jass_ast& ast, jass_file &file)
{
	ForwardIteratorType forwardFirst = boost::spirit::make_default_multi_pass(first);
	ForwardIteratorType forwardLast = boost::spirit::make_default_multi_pass(last); // TODO has to be created? Do we need this iterator to be passed?
	
	// used for backtracking and more detailed error output
	PositionIteratorType position_begin(forwardFirst);
	PositionIteratorType position_end;
	bool result = client::parse(position_begin, position_end, ast, file);
	
	
	return result;
}

bool Grammar::parse(Grammar::InputStream& istream, jass_ast& ast, const std::string& fileName)
{
	jass_file file;
	file.path = fileName;
	ast.files.push_back(file);
	
	return this->parse(IteratorType(istream), IteratorType(), ast, file);
}

}

}

/*
 * All parsed structures have to be defined manually for Fusion.
 * Only add attributes which should be parsed!
 */

//----------------------------------------------------------------------
// Statements
//----------------------------------------------------------------------
BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_statement,
	(wc3lib::jass::jass_statement_variant, variant)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_set,
	(wc3lib::jass::jass_set_var, var)
	(boost::recursive_wrapper<wc3lib::jass::jass_expression>, expression)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_call,
	(wc3lib::jass::jass_function_reference, function)
	(wc3lib::jass::jass_function_args, arguments)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_conditional_statements,
	(boost::recursive_wrapper<wc3lib::jass::jass_expression>, expression)
	(wc3lib::jass::jass_statements, statements)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_ifthenelse,
	(wc3lib::jass::jass_conditional_statements, ifexpression)
	(wc3lib::jass::jass_elseifs, elseifexpressions)
	(wc3lib::jass::jass_statements, elseexpression)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_loop,
	(wc3lib::jass::jass_statements, statements)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_exitwhen,
	(boost::recursive_wrapper<wc3lib::jass::jass_expression>, expression)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_return,
	(boost::recursive_wrapper<wc3lib::jass::jass_expression>, expression)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_debug,
	(wc3lib::jass::jass_debug_statement, statement)
)

//----------------------------------------------------------------------
// Expressions
//----------------------------------------------------------------------
BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_expression,
	(wc3lib::jass::jass_expression_variant, variant)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_binary_operation,
	(wc3lib::jass::jass_expression, first_expression)
	(BOOST_SCOPED_ENUM(wc3lib::jass::jass_binary_operator), op)
	(wc3lib::jass::jass_expression, second_expression)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_unary_operation,
	(BOOST_SCOPED_ENUM(wc3lib::jass::jass_unary_operator), op)
	(wc3lib::jass::jass_expression, expression)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_function_call,
	(wc3lib::jass::jass_function_reference, function)
	(wc3lib::jass::jass_function_args, arguments)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_array_reference,
	(wc3lib::jass::jass_var_reference, var)
	(wc3lib::jass::jass_expression, index)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_function_ref,
	(wc3lib::jass::jass_function_reference, function)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_const,
	(wc3lib::jass::jass_const_variant, variant)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_parentheses,
	(wc3lib::jass::jass_expression, expression)
)

//----------------------------------------------------------------------
// Local Declarations
//----------------------------------------------------------------------
BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_var_declaration,
	(wc3lib::jass::jass_type_reference, type)
	(bool, is_array)
	(wc3lib::string, identifier)
	(wc3lib::jass::jass_assignment, assignment)
)

//----------------------------------------------------------------------
// Global Declarations
//----------------------------------------------------------------------
BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_type,
	(wc3lib::string, identifier)
	(wc3lib::jass::jass_type_reference, parent)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_global,
	(bool, is_constant) //don't expect is_constant for the struct, it is set manually in the semantic action because we need a boolean value
	(wc3lib::jass::jass_var_declaration, declaration)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_function_parameter,
	(wc3lib::jass::jass_type_reference, type)
	(wc3lib::string, identifier)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_function_declaration,
	(wc3lib::string, identifier)
	(wc3lib::jass::jass_function_parameters, parameters)
	(wc3lib::jass::jass_type_reference, return_type)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_native,
	(bool, is_constant)
	(wc3lib::jass::jass_function_declaration, declaration)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_declarations,
	(wc3lib::jass::jass_types, types)
	(wc3lib::jass::jass_globals, globals)
	(wc3lib::jass::jass_natives, natives)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_function,
	(bool, is_constant)
	(wc3lib::jass::jass_function_declaration, declaration)
	(wc3lib::jass::jass_locals, locals)
	(wc3lib::jass::jass_statements, statements)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_file,
	(wc3lib::jass::jass_declarations, declarations)
	(wc3lib::jass::jass_functions, functions)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_ast,
	// (client::jass_types, types)
	// TODO do we have to fill these?
	//(wc3lib::jass::jass_files, files)
)
//]
