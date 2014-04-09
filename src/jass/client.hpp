#ifndef WC3LIB_JASS_CLIENT_HPP
#define WC3LIB_JASS_CLIENT_HPP

#include "grammar.hpp"

#include "../qi.hpp"

#include "ast.hpp"

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

/**
 * Returns the formatted expectation failure of Boost Spirit \p e as human readable string.
 */
template<typename Iterator>
std::string expectationFailure(const boost::spirit::qi::expectation_failure<Iterator> &e);

/*
 * Doesn't consume eols since value pairs are separated linewise which therefore can be specified easier in the rules
 */
template<typename Iterator>
struct comment_skipper : public qi::grammar<Iterator> {
	
	comment_skipper();
	
	qi::rule<Iterator> skip;
	qi::rule<Iterator> emptyline;
	qi::rule<Iterator> emptylines;
	qi::rule<Iterator> comment;
};

template <typename Iterator, typename Skipper = comment_skipper<Iterator> >
struct jass_grammar : qi::grammar<Iterator, jass_ast(), qi::locals<std::string>, Skipper>
{
	/**
	 * \param ast An AST must be passed which is filled by the input. It can already contain nodes which might be useful for debugging.
	 */
	jass_grammar(jass_ast &ast, jass_file &current_file);
	
	// symbols
	qi::rule<Iterator, jass_var_reference(), Skipper> var_reference;
	qi::rule<Iterator, jass_type_reference(), Skipper> type_reference;
	qi::rule<Iterator, jass_function_reference(), Skipper> function_reference;
	
	qi::rule<Iterator, std::string(), Skipper> identifier;

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
	/* qi::rule<Iterator, jass_debug(), Skipper> debug; */
	
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
	jass_ast ast;
	jass_file current_file; // currently parsed file
	/*
	 * Symbols for ALL files:
	 */
	jass_type_declarations type_symbols;
	jass_var_declarations global_symbols;
	jass_function_declarations function_symbols;
	
	jass_binary_operators binary_operators;
	jass_binary_boolean_operators binary_boolean_operators;
	jass_unary_operators unary_operators;
};

}

}

}

#endif
