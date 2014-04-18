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

Analyzer::Analyzer(const jass_type_declarations& types, const jass_function_declarations& functions) : types(types), functions(functions)
{
}

std::vector<const  jass_var_declaration* > Analyzer::leakingDeclarations(const jass_ast& ast, const Destructors &destructors) const
{
	typedef std::set<const jass_var_declaration*>  Declarations;
	Declarations allDeclarations;
	
	// get all var declarations which have destructors
	BOOST_FOREACH(jass_files::const_reference ref, ast.files) {
		BOOST_FOREACH(jass_globals::const_reference global, ref.declarations.globals) {
			
			std::pair<Destructors::const_iterator, Destructors::const_iterator> range = destructors.equal_range(boost::get<jass_type*>(global.declaration.type));
			
			if (range.first != destructors.end()) { // has destructor
				allDeclarations.insert(&global.declaration);
			}
		}
		
		BOOST_FOREACH(jass_functions::const_reference function, ref.functions) {
			BOOST_FOREACH(jass_locals::const_reference local, function.locals) {
				std::pair<Destructors::const_iterator, Destructors::const_iterator> range = destructors.equal_range(boost::get<jass_type*>(local.type));
				
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
					const jass_call &call = boost::get<const jass_call&>(statement.variant);
					
					/*
					 * Destructors only have the variable as parameter.
					 * TODO resolve functions which return the variable directly.
					 */
					if (call.arguments.size() == 1) {
						const jass_var_reference *var = 0;
						
						if (call.arguments[0].get().variant.type() == typeid(jass_array_reference)) {
							const jass_array_reference &ref = boost::get<const jass_array_reference&>(call.arguments[0].get().variant);
							
							var = &ref.var;
						} else if (call.arguments[0].get().variant.type() == typeid(jass_var_reference)) {
							const jass_var_reference &ref = boost::get<const jass_var_reference&>(call.arguments[0].get().variant);
							
							var = &ref;
						}
						
						
						if (var != 0) {
							
							const jass_var_declaration *declaration = boost::get<const jass_var_declaration*>(*var);
							Declarations::iterator declarationsIterator = allDeclarations.find(declaration);
							
							// has been declared anywhere
							if (declarationsIterator != allDeclarations.end()) {
								const jass_type *type = boost::get<const jass_type*>(declaration->type);
								
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
	if (function.type() == typeid(jass_function_declaration*)) {
		return boost::get<const jass_function_declaration*>(function)->identifier;
	}
	else if (function.type() == typeid(std::string)) {
		return boost::get<std::string>(function);
	}
	
	return "";
}


std::string Analyzer::typeName(const jass_type_reference& type) const
{
	if (type.type() == typeid(jass_type*)) {
		return boost::get<const jass_type*>(type)->identifier;
	}
	else if (type.type() == typeid(std::string)) {
		return boost::get<std::string>(type);
	}
	
	return "";
}

bool Analyzer::typeReferenceIsOfBaseType(const jass_type_reference& type, const jass_type_reference& baseType) const
{
	/*
	 * If "type" is of type "baseType" OR any of "type's" parent types is of type "baseType" we
	 * return true since the types are compatible.
	 */
	if (type.type() == typeid(jass_type*)) {
		const jass_type *typeRef = boost::get<const jass_type*>(type);
		
		if (baseType.type() == typeid(jass_type*)) {
			if (typeRef->identifier == boost::get<const jass_type*>(baseType)->identifier) {
				return true;
			}
		} else if (baseType.type() == typeid(std::string)) {
			if (typeRef->identifier == boost::get<std::string>(baseType)) {
				return true;
			}
		}
		
		// check parent types
		if (typeRef->has_parent()) {
			return typeReferenceIsOfBaseType(typeRef->parent, baseType);
		}
	/*
	 * Otherwise we only have the identifier of the type and can compare it to the identifier of
	 * the base type.
	 */
	} else if (type.type() == typeid(std::string)) {
		const std::string identifier = boost::get<std::string>(type);
		
		if (baseType.type() == typeid(jass_type*)) {
			if (identifier == boost::get<const jass_type*>(baseType)->identifier) {
				return true;
			}
		} else if (baseType.type() == typeid(std::string)) {
			if (identifier == boost::get<std::string>(baseType)) {
				return true;
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

jass_type_reference Analyzer::constType(const jass_const& constant) const
{
	const jass_type *result = 0;
	
	if (constant.variant.type() == typeid(int32)) {
		result = types.find("integer");
	} else if (constant.variant.type() == typeid(float32)) {
		result = types.find("real");
	} else if (constant.variant.type() == typeid(fourcc)) {
		return jass_type_reference("fourcc");
	} else if (constant.variant.type() == typeid(bool)) {
		result = types.find("boolean");
	} else if (constant.variant.type() == typeid(string)) {
		result = types.find("string");
	} else if (constant.variant.type() == typeid(jass_null)) {
		return jass_type_reference("null");
	}
	
	return jass_type_reference(result);
}

jass_type_reference Analyzer::functionReferenceType(const jass_function_reference &function) const
{
	if (function.type() == typeid(const jass_function_declaration*)) {
		return boost::get<const jass_function_declaration*>(function)->return_type;
	}
	
	throw Exception();
}

jass_type_reference Analyzer::functionCallType(const jass_function_call& call) const
{
	return functionReferenceType(call.function);
}

jass_type_reference Analyzer::expressionType(const jass_expression& expression) const
{
	if (expression.variant.type() == typeid(jass_const)) {
		return constType(boost::get<const jass_const>(expression.variant));
	// always use the first expression for binary operations
	} else if (expression.variant.type() == typeid(jass_binary_operation)) {
		return expressionType(boost::get<jass_binary_operation>(expression.variant).first_expression);
	} else if (expression.variant.type() == typeid(jass_unary_operation)) {
		return expressionType(boost::get<jass_unary_operation>(expression.variant).expression);
	} else if (expression.variant.type() == typeid(jass_function_call)) {
		return functionCallType(boost::get<jass_function_call>(expression.variant));
		
		//return expressionType(boost::get<jass_function_call>(expression.variant).function);
	}
	
	throw Exception();
}

void Analyzer::checkUnaryOperation(const jass_unary_operation& operation, Analyzer::Reports &reports) const
{
	const jass_type_reference type = expressionType(operation.expression);
	
	switch (operation.op) {
		case jass_unary_operator::Not: {
			if (!typeReferenceIsOfBaseType(type, "boolean")) {
				reports.push_back(Report(&operation, _("Boolean operator on non boolean expression.")));
			}
			
			break;
		}
		
		case jass_unary_operator::Plus:
		case jass_unary_operator::Minus: {
			if (!typeReferenceIsOfBaseType(type, "integer") && !typeReferenceIsOfBaseType(type, "real")) {
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
			if (!typeReferenceIsOfBaseType(type, "integer")
				&& !typeReferenceIsOfBaseType(type, "real")
				&& !typeReferenceIsOfBaseType(type, "string")
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
			if (!typeReferenceIsOfBaseType(type, "integer")
				&& !typeReferenceIsOfBaseType(type, "real")
				) {
				reports.push_back(Report(&expression, _("Invalid operator.")));
			}
			
			break;
		}
		
		case jass_binary_operator::Equal:
		case jass_binary_operator::NotEqual:  {
			if (!typeReferenceIsOfBaseType(type, "integer")
				&& !typeReferenceIsOfBaseType(type, "real")
				&& !typeReferenceIsOfBaseType(type, "string")
				&& !typeReferenceIsOfBaseType(type, "boolean")
				&& !typeReferenceIsOfBaseType(type, "fourcc")
				) {
				reports.push_back(Report(&expression, _("Invalid operator.")));
			}
			
			break;
		}
		
		
		case jass_binary_operator::And:
		case jass_binary_operator::Or: {
			if (!typeReferenceIsOfBaseType(type, "boolean")) {
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
	
	if (!typesAreCompatible(firstType, secondType)) {
		reports.push_back(Report(&operation, _("Incompatible operands.")));
	}
}

void Analyzer::checkFunctionCall(const jass_function_call& call, Analyzer::Reports& reports) const
{
	if (call.function.type() == typeid(jass_function_declaration*)) {
		const jass_function_declaration *declaration = boost::get<jass_function_declaration*>(call.function);
		
		if (declaration->parameters.size() != call.arguments.size()) {
			reports.push_back(Report(&call.arguments, _("Invalid count of parameters.")));
		}
		
		const std::size_t min = std::min(declaration->parameters.size(), call.arguments.size());
		
		for (std::size_t i = 0; i < min; i++) {
			const jass_type_reference argType = expressionType(call.arguments[i].get());
			
			const jass_type_reference &parameterType = declaration->parameters[i].type;
			
			
		}
	} else {
		reports.push_back(Report(&call, _("Missing function declaration.")));
	}
}

}

}
