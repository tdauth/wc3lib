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

#ifndef WC3LIB_JASS_ANALYZER_HPP
#define WC3LIB_JASS_ANALYZER_HPP

#include <vector>
#include <map>

#include "ast.hpp"
#include "report.hpp"

namespace wc3lib
{

namespace jass
{

/**
 * Performs specific analysises on an AST and returns results.
 * It supports all analysises of the basic static semantic of a JASS program so it can be used
 * by any JASS compiler for static semantic checks.
 * 
 * Function \ref checkAst() does a normal static semantic check on a whole AST according to all JASS syntax rules.
 * 
 * \note As it should passively analyse AST nodes it does not perform any modifications! Therefore all parameters are passed const.
 */
class Analyzer {
	public:
		/**
		 * List of reports which contain human readable messages
		 * and the corresponding AST node to inform the user about an error
		 * or warning of one single AST entry.
		 */
		typedef std::vector<Report> Reports;
		
		/**
		 * The analyzer needs symbol tables to get information about declared types, variables and functions.
		 */
		Analyzer(const jass_type_declarations &types, const jass_var_declarations &globals, const jass_function_declarations &functions);
		
		/**
		 *\defgroup LeakAnalysis Static memory leak analysis
		 * @{
		 * 
		 * Checks for proper destruction of leaking objects.
		 */

		/**
		 * Multimap of destructor functions for specific JASS types.
		 * Each JASS type can have multiple destructor functions. For example "rect" has
		 * defined "RemoveRect" in file "common.j" as its destructing function.
		 * 
		 * Destructors are only required for "agent" derived types which might leak.
		 */
		typedef std::multimap<jass_type_reference, std::string> Destructors;	
	
		/**
		 * Analyses all variable declarations in \p ast and returns a list of declarations for which never any
		 * destructor function is called.
		 * The destructors for the corresponding types are defined in \p destructors using the type as key and the function
		 * names as value. One type can have multiple destructors. Therefore a multimap is used.
		 * 
		 * To get the default destructors call \ref commonjDestructors().
		 * 
		 * The resulting vector contains all leaking variable declarations helping the user to find memory leaks in his
		 * program.
		 */
		std::vector<const jass_var_declaration*> leakingDeclarations(const jass_ast &ast, const Destructors &destructors) const;
		
		/**
		 * Returns a multimap of all "common.j" native types which have destructors with their corresponding destructor function.
		 * Usually only one single function is defined to free a native type.
		 */
		Destructors commonjDestructors() const;
		
		/**
		 * Uses the default destructors defined in "common.j" to analyse leaking variable destructions.
		 * For example type "rect" uses destructor "RemoveRect()"
		 */
		std::vector<const jass_var_declaration*> leakingDeclarations(const jass_ast &ast) const;
		/** @} */
		
		std::string functionName(const jass_function_reference &function) const;
		std::string typeName(const jass_type_reference &type) const;
		bool typeReferenceIsOfBaseType(const jass_type_reference &type, const jass_type_reference &baseType) const;
		bool typesAreCompatible(const jass_type_reference &first, const jass_type_reference &second) const;
		void checkTypeCompatiblity(const jass_ast_node &node, const jass_type_reference &first, const jass_type_reference &second, Reports &reports) const;
		
		/**
		 * Evaluates constant expression \p expression and returns its result.
		 * If the expression is not constant \p isConstant is set to false. Otherwise it is set to true.
		 * 
		 * Constant expression evaluation can be useful for static value checks or optimizations.
		 */
		jass_const evaluateConstantExpression(const jass_expression &expression,  bool &isConstant) const;
		
		jass_type_reference functionReferenceType(const jass_function_reference &function) const;
		/**
		 * All function references have type "code".
		 */
		jass_type_reference functionRefType(const jass_function_ref &function) const;
		jass_type_reference constType(const jass_const &constant) const;
		jass_type_reference functionCallType(const jass_function_call &call) const;
		jass_type_reference variableReferenceType(const jass_var_reference &var) const;
		jass_type_reference arrayReferenceType(const jass_array_reference &array) const;
		jass_type_reference expressionType(const jass_expression &expression) const;
		
		/**
		 *\defgroup StaticChecks Static AST Checks
		 * @{
		 * 
		 * Check for static types and semantic of statements, expressions and declarations.
		 * All methods fill \p reports with errors and warnings using the semantic rules of JASS.
		 */
		void checkTypeReference(const jass_ast_node &node, const jass_type_reference &type, Reports &reports) const;
		void checkUnaryOperation(const jass_unary_operation &operation, Reports &reports) const;
		void checkBinaryOperatorWithExpression(BOOST_SCOPED_ENUM(jass_binary_operator) op, const jass_expression &expression, Reports &reports) const;
		void checkBinaryOperation(const jass_binary_operation &operation, Reports &reports) const;
		void checkExpression(const jass_expression &expression, Reports &reports) const;
		void checkFunctionCall(const jass_function_call &call, Reports &reports) const;
		/**
		 * For arrays constant indices expressions can be checked for being between 0 and 8192.
		 */
		void checkArrayReference(const jass_array_reference &array, Reports &reports) const;
		
		void checkVarDeclaration(const jass_var_declaration &var, Reports &reports) const;
		void checkGlobal(const jass_global &global, Reports &reports) const;
		/**
		 * Multiple checks are done on function \p function .
		 * <ul>
		 * <li> valid locals declarations </li>
		 * <li> valid statements </li>
		 * <li> no return statement with expression for functions without return types </li>
		 * </ul>
		 */
		void checkFunction(const jass_function &function, Reports &reports) const;
		void checkAst(const jass_ast &ast, Reports &reports) const;
		/** @} */
		
	private:
		const jass_type_declarations &types;
		const jass_var_declarations &globals;
		const jass_function_declarations &functions;
};

}

}

#endif
