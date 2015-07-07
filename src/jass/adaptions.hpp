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

#ifndef WC3LIB_JASS_ADAPTIONS_HPP
#define WC3LIB_JASS_ADAPTIONS_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "ast.hpp"

/*
 * All parsed structures have to be defined manually for Fusion.
 * Only add attributes which should be parsed!
 */
BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_type_reference,
	(wc3lib::jass::jass_type_reference_variant, variant)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_var_reference,
	(wc3lib::jass::jass_var_reference_variant, variant)
)

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_function_reference,
	(wc3lib::jass::jass_function_reference_variant, variant)
)

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
	(wc3lib::jass::jass_ifthenelse_elseifs, elseifexpressions)
	(wc3lib::jass::jass_ifthenelse_else, elseexpression)
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
	(wc3lib::jass::jass_return_expression, expression)
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
	(std::string, path)
	(wc3lib::jass::jass_declarations, declarations)
	(wc3lib::jass::jass_functions, functions)
)


BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::jass::jass_ast,
	(wc3lib::jass::jass_files, files)
)

#endif
