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

#include <set>

#include <boost/foreach.hpp>

#include "analyzer.hpp"
#include "../i18n.hpp"
#include "../exception.hpp"

namespace wc3lib
{

namespace jass
{

Analyzer::Analyzer(const jass_type_declarations& types, const jass_var_declarations &globals, const jass_function_declarations& functions) : types(types), globals(globals), functions(functions)
{
}

std::vector<const jass_var_declaration* > Analyzer::leakingDeclarations(const jass_ast& ast, const Destructors &destructors) const
{
	typedef std::set<const jass_var_declaration*>  Declarations;
	Declarations allDeclarations;

	// get all var declarations which have destructors
	BOOST_FOREACH(jass_files::const_reference ref, ast.files) {
		BOOST_FOREACH(jass_globals::const_reference global, ref.declarations.globals) {

			std::pair<Destructors::const_iterator, Destructors::const_iterator> range = destructors.equal_range(global.declaration.type.variant);

			if (range.first != destructors.end()) { // has destructor
				allDeclarations.insert(&global.declaration);
			}
		}

		BOOST_FOREACH(jass_functions::const_reference function, ref.functions) {
			BOOST_FOREACH(jass_locals::const_reference local, function.locals) {
				std::pair<Destructors::const_iterator, Destructors::const_iterator> range = destructors.equal_range(local.type.variant);

				if (range.first != destructors.end()) { // has destructor
					allDeclarations.insert(&local);
				}
			}
		}
	}

	/*
	 * Create a map which uses the destructor as key and the type as value
	 */
	bool finished = false;

	// get all function calls of destructors
	BOOST_FOREACH(jass_files::const_reference ref, ast.files) {
		BOOST_FOREACH(jass_functions::const_reference function, ref.functions) {

			BOOST_FOREACH(jass_statements::const_reference statement, function.statements) {

				if (statement.variant.type() == typeid(jass_call)) {
					const jass_call &call = boost::get<const jass_call>(statement.variant);

					/*
					 * Destructors only have the variable as parameter.
					 * TODO resolve functions which return the variable directly.
					 */
					if (call.arguments.size() == 1) {
						const jass_var_reference *var = 0;

						if (call.arguments[0].get().variant.type() == typeid(jass_array_reference)) {
							const jass_array_reference &ref = boost::get<const jass_array_reference>(call.arguments[0].get().variant);

							var = &ref.var;
						} else if (call.arguments[0].get().variant.type() == typeid(jass_var_reference)) {
							const jass_var_reference &ref = boost::get<const jass_var_reference>(call.arguments[0].get().variant);

							var = &ref;
						}


						if (var != 0) {

							const jass_var_declaration *declaration = boost::get<const jass_var_declaration*>(var->variant);
							Declarations::iterator declarationsIterator = allDeclarations.find(declaration);

							// has been declared anywhere
							if (declarationsIterator != allDeclarations.end()) {
								const jass_type *type = boost::get<const jass_type*>(declaration->type.variant);

								std::pair<Destructors::const_iterator, Destructors::const_iterator> range = destructors.equal_range(type);

								if (range.first != destructors.end()) {
									const std::string functionName = this->functionName(call.function);

									for (Destructors::const_iterator iterator = range.first; iterator != range.second; ++iterator) {

										// found destructor
										if (functionName == iterator->second) {
											allDeclarations.erase(declarationsIterator);

											break;
										}
									}

									// stop checking statements when all vars where destroyed
									if (allDeclarations.empty()) {
										finished = true;

										break;
									}
								}
							}
						}
					}
				}

				// TODO
				// is jass_call
				// check function id and compare to destructor, then compare parameters and get vars

				// if its a call or set and contains expressions of functions (write helper function which extracts all called functions with parameters into a vector)
				// all functions must be checked!

				// if parameter of destructor is a function itself or another variable etc. IT MUST BE RESOLVED completely!!!

				// if any destructor for a variable of the corresponding type is found, remove it from all Declarations
			}

			if (finished) {
				break;
			}
		}

		if (finished) {
			break;
		}
	}

	// remaining declarations leak!
	// TODO one could oreder allDeclarations by appearance?!
	std::vector<const jass_var_declaration*> result(allDeclarations.size());
	std::size_t i = 0;

	BOOST_FOREACH(std::set<const jass_var_declaration*>::const_reference ref, allDeclarations) {
		result[i] = ref;
		++i;
	}

	return result;
}

Analyzer::Destructors Analyzer::commonjDestructors() const
{
	/*
	 * Generate default destructor list based on the information from "common.j"
	 */
	Destructors destructors;
	destructors.insert(Destructors::value_type("timer", "DestroyTimer"));
	destructors.insert(Destructors::value_type("group", "DestroyGroup"));
	destructors.insert(Destructors::value_type("force", "DestroyForce"));
	destructors.insert(Destructors::value_type("rect", "RemoveRect"));
	destructors.insert(Destructors::value_type("trigger", "DestroyTrigger"));
	destructors.insert(Destructors::value_type("conditionfunc", "DestroyCondition"));
	destructors.insert(Destructors::value_type("filterfunc", "DestroyFilter"));
	destructors.insert(Destructors::value_type("boolexpr", "DestroyBoolExpr"));
	destructors.insert(Destructors::value_type("destructable", "RemoveDestructable"));
	destructors.insert(Destructors::value_type("item", "RemoveItem"));
	destructors.insert(Destructors::value_type("unit", "RemoveUnit"));
	destructors.insert(Destructors::value_type("fogmodifier", "DestroyFogModifier"));
	destructors.insert(Destructors::value_type("dialog", "DialogDestroy"));
	destructors.insert(Destructors::value_type("gamecache", "FlushGameCache"));
	destructors.insert(Destructors::value_type("hashtable", "FlushParentHashtable"));
	destructors.insert(Destructors::value_type("unitpool", "DestroyUnitPool"));
	destructors.insert(Destructors::value_type("itempool", "DestroyItemPool"));
	destructors.insert(Destructors::value_type("texttag", "DestroyTextTag"));
	destructors.insert(Destructors::value_type("quest", "DestroyQuest"));
	destructors.insert(Destructors::value_type("defeatcondition", "DestroyDefeatCondition"));
	destructors.insert(Destructors::value_type("timerdialog", "DestroyTimerDialog"));
	destructors.insert(Destructors::value_type("leaderboard", "DestroyLeaderboard"));
	destructors.insert(Destructors::value_type("multiboard", "DestroyMultiboard"));
	destructors.insert(Destructors::value_type("weathereffect", "RemoveWeatherEffect"));
	destructors.insert(Destructors::value_type("effect", "DestroyEffect"));
	destructors.insert(Destructors::value_type("lightning", "DestroyLightning"));
	destructors.insert(Destructors::value_type("image", "DestroyImage"));
	destructors.insert(Destructors::value_type("ubersplat", "DestroyUbersplat"));

	return destructors;
}

std::vector< const jass_var_declaration* > Analyzer::leakingDeclarations(const jass_ast& ast) const
{
	return this->leakingDeclarations(ast, commonjDestructors());
}

std::string Analyzer::functionName(const jass_function_reference& function) const
{
	switch (function.whichType())
	{
		case jass_function_reference::Type::Declaration:
			return boost::get<const jass_function_declaration*>(function.variant)->identifier;

		case jass_function_reference::Type::String:
			return boost::get<std::string>(function.variant);
	}

	return "";
}


std::string Analyzer::typeName(const jass_type_reference& type) const
{
	switch (type.whichType())
	{
		case jass_type_reference::Type::Declaration:
		{
			return boost::get<const jass_type*>(type.variant)->identifier;
		}

		case jass_type_reference::Type::String:
		{
			return boost::get<std::string>(type.variant);
		}
	}

	return "";
}

bool Analyzer::typeReferenceIsOfBaseType(const jass_type_reference& type, const jass_type_reference& baseType) const
{
	/*
	 * If "type" is of type "baseType" OR any of "type's" parent types is of type "baseType" we
	 * return true since the types are compatible.
	 */
	switch (type.whichType())
	{
		case jass_type_reference::Type::Declaration:
		{
			const jass_type *typeRef = boost::get<const jass_type*>(type.variant);

			switch (baseType.whichType())
			{
				case jass_type_reference::Type::Declaration:
				{
					if (typeRef->identifier == boost::get<const jass_type*>(baseType.variant)->identifier) {
						return true;
					}
				}

				case jass_type_reference::Type::String:
				{
					if (typeRef->identifier == boost::get<const std::string>(baseType.variant)) {
						return true;
					}
				}
			}

			// check parent types
			if (typeRef->has_parent()) {
				return typeReferenceIsOfBaseType(typeRef->parent, baseType);
			}
		}
		/*
		* Otherwise we only have the identifier of the type and can compare it to the identifier of
		* the base type.
		*/
		case jass_type_reference::Type::String:
		{
			const std::string &identifier = boost::get<const std::string>(type.variant);

			switch (baseType.whichType())
			{
				case jass_type_reference::Type::Declaration:
				{
					if (identifier == boost::get<const jass_type*>(baseType.variant)->identifier) {
						return true;
					}
				}

				case jass_type_reference::Type::String:
				{
					if (identifier == boost::get<const std::string>(baseType.variant)) {
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Analyzer::typesAreCompatible(const jass_type_reference& first, const jass_type_reference& second) const
{
	std::string firstTypeName = typeName(first);
	std::string secondTypeName = typeName(second);

	/*
	 * The same type is always compatible!
	 */
	if (firstTypeName == secondTypeName) {
		return true;
	}

	if (
		(firstTypeName == "fourcc" && secondTypeName == "integer")
		|| (firstTypeName == "integer" && secondTypeName == "fourcc")
	) {
		return true;
	}

	// TODO not for comparisons??
	if (
		(firstTypeName == "integer" && secondTypeName == "real")
		|| (firstTypeName == "real" && secondTypeName == "integer")
	) {
		return true;
	}

	// "null" can be used on all types
	if (firstTypeName == "null" || secondTypeName == "null") {
		return true;
	}

	return false;
}

void Analyzer::checkTypeCompatiblity(const jass_ast_node &node, const jass_type_reference &first, const jass_type_reference &second, Reports &reports) const {
	if (!typesAreCompatible(first, second)) {
		reports.push_back(Report(&node, _("Invalid type.")));
	}
}

jass_type_reference Analyzer::handle() const
{
	jass_type_reference result;
	result.variant = types.find("handle");

	return result;
}

jass_type_reference Analyzer::code() const
{
	jass_type_reference result;
	result.variant = types.find("code");

	return result;
}

jass_type_reference Analyzer::boolean() const
{
	jass_type_reference result;
	result.variant = types.find("boolean");

	return result;
}


jass_type_reference Analyzer::integer() const
{
	jass_type_reference result;
	result.variant = types.find("integer");

	return result;
}

jass_type_reference Analyzer::real() const
{
	jass_type_reference result;
	result.variant = types.find("real");

	return result;
}

jass_type_reference Analyzer::string() const
{
	jass_type_reference result;
	result.variant = types.find("string");

	return result;
}

jass_type_reference Analyzer::fourcc() const
{
	jass_type_reference result;
	result.variant = "fourcc";

	return result;
}

jass_type_reference Analyzer::null() const
{
	jass_type_reference result;
	result.variant = "null";

	return result;
}

jass_const Analyzer::evaluateConstantExpression(const jass_expression& expression, bool &isConstant) const
{
	isConstant = true; // set to true so that if a value is returned before the end of the function it is always automatically true

	switch (expression.whichType()) {
		case jass_expression::Type::Constant: {
			const jass_const &constant = boost::get<const jass_const>(expression.variant);

			return constant;
		}

		case jass_expression::Type::Parentheses: {
			const jass_parentheses &parentheses = boost::get<const jass_parentheses>(expression.variant);

			return evaluateConstantExpression(parentheses.expression, isConstant);
		}

		case jass_expression::Type::FunctionCall: {
			const jass_function_call &call = boost::get<const jass_function_call>(expression.variant);

			switch (call.function.whichType())
			{
				case jass_function_reference::Type::Declaration:
				{
					const jass_function_declaration *declaration = boost::get<const jass_function_declaration*>(call.function.variant);
					// TODO get function definition from declaration and get return expression. If return expression is const so return the const expression.
				}
			}
		}

		case jass_expression::Type::UnaryOperation: {
			const jass_unary_operation &unary_operation = boost::get<const jass_unary_operation>(expression.variant);

			const jass_const &constant = evaluateConstantExpression(unary_operation.expression, isConstant);

			if (isConstant) {
				switch (unary_operation.op) {
					case jass_unary_operator::Plus: {
						switch (constant.whichType()) {
							case jass_const::Type::Integer:
							case jass_const::Type::Real: {
								return constant;
							}
						}
					}

					case jass_unary_operator::Minus: {
						switch (constant.whichType()) {
							case jass_const::Type::Integer: {
								jass_const result;
								result.variant = -1 * boost::get<int32>(constant.variant);

								return result;
							}

							case jass_const::Type::Real: {
								jass_const result;
								result.variant = -1 * boost::get<float32>(constant.variant);

								return result;
							}
						}
					}

					case jass_unary_operator::Not: {
						switch (constant.whichType()) {
							case jass_const::Type::Boolean: {
								jass_const result;
								result.variant = !boost::get<bool>(constant.variant);

								return result;
							}
						}
					}
				}
			}
		}

		case jass_expression::Type::BinaryOperation: {
			const jass_binary_operation &binary_operation = boost::get<const jass_binary_operation>(expression.variant);

			const jass_const &firstConstant = evaluateConstantExpression(binary_operation.first_expression, isConstant);

			if (isConstant) {
				const jass_const &secondConstant = evaluateConstantExpression(binary_operation.second_expression, isConstant);

				if (isConstant) {
					if (typesAreCompatible(constType(firstConstant), constType(secondConstant))) {
						switch (binary_operation.op) {
							case jass_binary_operator::And: {
								switch (firstConstant.whichType()) {
									case jass_const::Type::Boolean: {
										jass_const result;
										result.variant = (boost::get<bool>(firstConstant.variant) && boost::get<bool>(secondConstant.variant));

										return result;
									}
								}
							}

							case jass_binary_operator::Or: {
								switch (firstConstant.whichType()) {
									case jass_const::Type::Boolean: {
										jass_const result;
										result.variant = (boost::get<bool>(firstConstant.variant) || boost::get<bool>(secondConstant.variant));

										return result;
									}
								}
							}

							case jass_binary_operator::Equal: {
								switch (firstConstant.whichType()) {
									case jass_const::Type::Boolean: {
										jass_const result;
										result.variant = (boost::get<bool>(firstConstant.variant) == boost::get<bool>(secondConstant.variant));

										return result;
									}
								}
							}

							case jass_binary_operator::NotEqual: {
								switch (firstConstant.whichType()) {
									case jass_const::Type::Boolean: {
										jass_const result;
										result.variant = (boost::get<bool>(firstConstant.variant) != boost::get<bool>(secondConstant.variant));

										return result;
									}
								}
							}

							case jass_binary_operator::Divide: {
								switch (firstConstant.whichType()) {
									case jass_const::Type::Integer: {
										jass_const result;
										result.variant = (boost::get<int32>(firstConstant.variant) / boost::get<int32>(secondConstant.variant));

										return result;
									}
								}
							}

							case jass_binary_operator::Multiply: {
								switch (firstConstant.whichType()) {
									case jass_const::Type::Integer: {
										jass_const result;
										result.variant = (boost::get<int32>(firstConstant.variant) * boost::get<int32>(secondConstant.variant));

										return result;
									}
								}
							}

							case jass_binary_operator::Minus: {
								switch (firstConstant.whichType()) {
									case jass_const::Type::Integer: {
										jass_const result;
										result.variant = (boost::get<int32>(firstConstant.variant) - boost::get<int32>(secondConstant.variant));

										return result;
									}
								}
							}

							case jass_binary_operator::Plus: {
								switch (firstConstant.whichType()) {
									case jass_const::Type::Integer: {
										jass_const result;
										result.variant = (boost::get<int32>(firstConstant.variant) + boost::get<int32>(secondConstant.variant));

										return result;
									}

									case jass_const::Type::String: {
										jass_const result;
										result.variant = (boost::get<const wc3lib::string>(firstConstant.variant) + boost::get<const wc3lib::string>(secondConstant.variant));

										return result;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// if nothing returned, invalid constant!
	isConstant = false;

	return jass_const();
}

jass_type_reference Analyzer::constType(const jass_const& constant) const
{
	const jass_type *result = 0;

	switch (constant.whichType()) {
		case jass_const::Type::Integer: {
			result = types.find("integer");

			break;
		}

		case jass_const::Type::Real: {
			result = types.find("real");

			break;
		}

		case jass_const::Type::Fourcc: {
			jass_type_reference returnValue;
			returnValue.variant = "fourcc";

			return returnValue;
		}

		case jass_const::Type::Boolean: {
			result = types.find("boolean");
		}

		case jass_const::Type::String: {
			result = types.find("string");
		}

		case jass_const::Type::Null: {
			jass_type_reference returnValue;
			returnValue.variant = "null";

			return returnValue;
		}
	}

	jass_type_reference returnValue;
	returnValue.variant = result;

	return returnValue;
}

jass_type_reference Analyzer::functionReferenceType(const jass_function_reference &function) const
{
	if (function.whichType() == jass_function_reference::Type::Declaration) {
		return boost::get<const jass_function_declaration*>(function.variant)->return_type;
	}

	throw Exception();
}

jass_type_reference Analyzer::functionRefType(const jass_function_ref &function) const
{
	jass_type_reference result;
	result.variant = this->types.find("code");;

	return result;
}


jass_type_reference Analyzer::functionCallType(const jass_function_call& call) const
{
	return functionReferenceType(call.function);
}

jass_type_reference Analyzer::variableReferenceType(const jass_var_reference& var) const
{
	switch (var.whichType())
	{
		case jass_var_reference::Type::Declaration:
		{
			const jass_var_declaration *declaration = boost::get<const jass_var_declaration*>(var.variant);
			jass_type_reference result;
			result.variant = declaration->type.variant;

			return result;
		}
	}

	jass_type_reference result;
	result.variant = "";

	return result;
}

jass_type_reference Analyzer::arrayReferenceType(const jass_array_reference& array) const
{
	return variableReferenceType(array.var);
}

jass_type_reference Analyzer::expressionType(const jass_expression& expression) const
{
	switch (expression.whichType()) {
		case jass_expression::Type::Constant: {
			return constType(boost::get<const jass_const>(expression.variant));
		}

		// always use the first expression for binary operations
		case jass_expression::Type::BinaryOperation: {
			return expressionType(boost::get<const jass_binary_operation>(expression.variant).first_expression);
		}

		case jass_expression::Type::UnaryOperation: {
			return expressionType(boost::get<const jass_unary_operation>(expression.variant).expression);
		}

		case jass_expression::Type::FunctionCall: {
			return functionCallType(boost::get<const jass_function_call>(expression.variant));
		}

		case jass_expression::Type::ArrayReference: {
			return arrayReferenceType(boost::get<const jass_array_reference>(expression.variant));
		}

		case jass_expression::Type::VariableReference: {
			return variableReferenceType(boost::get<const jass_var_reference>(expression.variant));
		}

		case jass_expression::Type::FunctionReference: {
			return functionRefType(boost::get<const jass_function_ref>(expression.variant));
		}

		case jass_expression::Type::Parentheses: {
			return expressionType(boost::get<const jass_parentheses>(expression.variant).expression);
		}
	}

	throw Exception();
}

void Analyzer::checkTypeReference(const jass_ast_node &node, const jass_type_reference& type, Analyzer::Reports& reports) const
{
	switch (type.whichType())
	{
		case jass_type_reference::Type::Declaration:
		{
			//globals.find();
			// TODO compare declaration position with expression position!

			break;
		}

		case jass_type_reference::Type::String:
		{
			const std::string &typeName = boost::get<const std::string>(type.variant);

			if (typeName != "nothing") {
				reports.push_back(Report(&node, _("Undeclared indentifier.")));
			}

			break;
		}

		default:
		{
			throw Exception();
		}
	}
}

void Analyzer::checkUnaryOperation(const jass_unary_operation& operation, Analyzer::Reports &reports) const
{
	const jass_type_reference type = expressionType(operation.expression);

	switch (operation.op) {
		case jass_unary_operator::Not: {
			if (!typeReferenceIsOfBaseType(type, this->boolean())) {
				reports.push_back(Report(&operation, _("Boolean operator on non boolean expression.")));
			}

			break;
		}

		case jass_unary_operator::Plus:
		case jass_unary_operator::Minus: {
			if (!typeReferenceIsOfBaseType(type, this->integer()) && !typeReferenceIsOfBaseType(type, this->real())) {
				reports.push_back(Report(&operation, _("Arithmetic operator on non arithmetic expression.")));
			}

			break;
		}
	}
}

void Analyzer::checkBinaryOperatorWithExpression(BOOST_SCOPED_ENUM(jass_binary_operator) op, const jass_expression& expression, Analyzer::Reports& reports) const
{
	const jass_type_reference type = expressionType(expression);

	switch (op) {
		case jass_binary_operator::Plus: {
			if (!typeReferenceIsOfBaseType(type, integer())
				&& !typeReferenceIsOfBaseType(type, real())
				&& !typeReferenceIsOfBaseType(type, string())
				) {
				reports.push_back(Report(&expression, _("Invalid operator.")));
			}

			break;
		}

		case jass_binary_operator::GreaterEqual:
		case jass_binary_operator::LessEqual:
		case jass_binary_operator::Greater:
		case jass_binary_operator::Less:
		case jass_binary_operator::Minus:
		case jass_binary_operator::Multiply:
		case jass_binary_operator::Divide: {
			if (!typeReferenceIsOfBaseType(type, integer())
				&& !typeReferenceIsOfBaseType(type, real())
				) {
				reports.push_back(Report(&expression, _("Invalid operator.")));
			}

			break;
		}

		case jass_binary_operator::Equal:
		case jass_binary_operator::NotEqual:  {
			if (!typeReferenceIsOfBaseType(type, integer())
				&& !typeReferenceIsOfBaseType(type, real())
				&& !typeReferenceIsOfBaseType(type, string())
				&& !typeReferenceIsOfBaseType(type, boolean())
				&& !typeReferenceIsOfBaseType(type, fourcc())
				) {
				reports.push_back(Report(&expression, _("Invalid operator.")));
			}

			break;
		}


		case jass_binary_operator::And:
		case jass_binary_operator::Or: {
			if (!typeReferenceIsOfBaseType(type, boolean())) {
				reports.push_back(Report(&expression, _("Invalid operator.")));
			}

			break;
		}
	}
}

void Analyzer::checkBinaryOperation(const jass_binary_operation& operation, Analyzer::Reports& reports) const
{
	checkBinaryOperatorWithExpression(operation.op, operation.first_expression, reports);
	checkBinaryOperatorWithExpression(operation.op, operation.second_expression, reports);

	const jass_type_reference firstType = expressionType(operation.first_expression);
	const jass_type_reference secondType = expressionType(operation.second_expression);

	checkTypeCompatiblity(operation, firstType, secondType, reports);

	// recursive check
	checkExpression(operation.first_expression, reports);
	checkExpression(operation.second_expression, reports);
}

void Analyzer::checkExpression(const jass_expression& expression, Analyzer::Reports& reports) const
{
	switch (expression.whichType()) {
		case jass_expression::Type::BinaryOperation: {
			const jass_binary_operation &binaryOperation = boost::get<const jass_binary_operation>(expression.variant);

			checkBinaryOperation(binaryOperation, reports);

			break;
		}

		case jass_expression::Type::UnaryOperation: {
			const jass_unary_operation &unaryOperation = boost::get<const jass_unary_operation>(expression.variant);

			checkUnaryOperation(unaryOperation, reports);

			break;
		}

		case jass_expression::Type::FunctionCall: {
			const jass_function_call &call = boost::get<const jass_function_call>(expression.variant);

			checkFunctionCall(call, reports);

			break;
		}

		case jass_expression::Type::Parentheses: {
			const jass_parentheses &parentheses = boost::get<const jass_parentheses>(expression.variant);

			checkExpression(parentheses.expression, reports);

			break;
		}

		case jass_expression::Type::VariableReference: {
			const jass_var_reference &var = boost::get<const jass_var_reference>(expression.variant);

			switch (var.whichType())
			{
				case jass_var_reference::Type::Declaration:
				{
					//globals.find();
					// TODO compare declaration position with expression position!

					break;
				}

				case jass_var_reference::Type::String:
				{
					reports.push_back(Report(&expression, _("Undeclared indentifier.")));

					break;
				}

				default:
				{
					throw Exception();
				}
			}

			break;
		}
	}
}

void Analyzer::checkFunctionCall(const jass_function_call& call, Analyzer::Reports& reports) const
{
	if (call.function.whichType() == jass_function_reference::Type::Declaration) {
		const jass_function_declaration *declaration = boost::get<const jass_function_declaration*>(call.function.variant);

		if (declaration->parameters.size() != call.arguments.size()) {
			reports.push_back(Report(&call.arguments, _("Invalid count of parameters.")));
		}

		const std::size_t min = std::min(declaration->parameters.size(), call.arguments.size());

		for (std::size_t i = 0; i < min; i++) {
			const jass_type_reference argType = expressionType(call.arguments[i].get());

			const jass_type_reference &parameterType = declaration->parameters[i].type;

			checkTypeCompatiblity(call.arguments[i].get(), argType, parameterType, reports);
		}
	} else {
		reports.push_back(Report(&call, _("Missing function declaration.")));
	}
}

void Analyzer::checkArrayReference(const jass_array_reference &array, Analyzer::Reports &reports) const
{
	checkExpression(array.index, reports);

	if (typeName(expressionType(array.index)) != "integer") {
		reports.push_back(Report(&(array.index), _("Expected integer expression.")));
	} else {
		bool isConstant = false;
		const jass_const indexConst = evaluateConstantExpression(array.index, isConstant);

		// we can only check bounds for constant indices
		if (isConstant) {
			if (indexConst.whichType() == jass_const::Type::Integer) {
				const int32 value = boost::get<int32>(indexConst.variant);

				if (value < 0 || value >= 8192) {
					reports.push_back(Report(&(array.index), _("Array index must be between 0 and 8192.")));
				}
			// TODO should never happen!
			} else {
				reports.push_back(Report(&(array.index), _("Expected integer expression (error appeared in constant expression evaluation).")));
			}
		}
	}
}

void Analyzer::checkVarDeclaration(const jass_var_declaration& var, Analyzer::Reports& reports) const
{
	if (var.assignment.is_initialized()) {
		checkExpression(var.assignment.get(), reports);

		if (var.is_array) {
			reports.push_back(Report(&var, _("Arrays can not be initialized.")));
		} else {
			checkTypeCompatiblity(var.assignment.get(), var.type, expressionType(var.assignment.get()), reports);
		}
	} else {
		// TODO check for initialization somewhere and generate warning
	}
}

void Analyzer::checkGlobal(const jass_global& global, Analyzer::Reports& reports) const
{
	checkVarDeclaration(global.declaration, reports);

	if (global.is_constant && !global.declaration.assignment.is_initialized()) {
		reports.push_back(Report(&global, _("Constants must be initialized.")));
	}
}

void Analyzer::checkFunction(const jass_function& function, Analyzer::Reports& reports) const
{
	BOOST_FOREACH(jass_function_parameters::const_reference parameter, function.declaration.parameters) {
		checkTypeReference(parameter, parameter.type, reports);
	}

	checkTypeReference(function.declaration, function.declaration.return_type, reports);

	BOOST_FOREACH(jass_locals::const_reference local, function.locals) {
		checkVarDeclaration(local, reports);
	}

	BOOST_FOREACH(jass_statements::const_reference statement, function.statements) {
		switch (statement.whichType()) {
			case jass_statement::Type::Return: {
				const jass_return &returnStatement = boost::get<const jass_return>(statement.variant);

				if (returnStatement.expression.is_initialized()) {
					const jass_expression &expression = returnStatement.expression.get().get();

					if (typeName(function.declaration.return_type) == "nothing") {
						reports.push_back(Report(&expression, _("Return expression for function without return type.")));
					} else {
						checkTypeCompatiblity(expression, expressionType(expression), function.declaration.return_type, reports);
					}
				}
			}
		}
	}
}

void Analyzer::checkAst(const jass_ast& ast, Analyzer::Reports& reports) const
{
	BOOST_FOREACH(jass_files::const_reference file, ast.files) {
		BOOST_FOREACH(jass_globals::const_reference global, file.declarations.globals) {
			checkGlobal(global, reports);
		}

		BOOST_FOREACH(jass_functions::const_reference function, file.functions) {
			checkFunction(function, reports);
		}
	}
}

}

}
