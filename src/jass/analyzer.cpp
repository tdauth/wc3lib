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

std::vector<const  jass_var_declaration* > Analyzer::leakingDeclarations(const jass_ast& ast, std::multimap<jass_type*, std::string> destructors)
{
	std::set<const jass_var_declaration*> allDeclarations;
	
	// get all var declarations which have destructors
	BOOST_FOREACH(jass_files::const_reference ref, ast.files) {
		BOOST_FOREACH(jass_globals::const_reference global, ref.declarations.globals) {
			if (destructors.find(boost::get<jass_type*>(global.declaration.type)) != destructors.end()) { // has destructor
				allDeclarations.insert(&global.declaration);
			}
		}
		
		BOOST_FOREACH(jass_functions::const_reference function, ref.functions) {
			BOOST_FOREACH(jass_locals::const_reference local, function.locals) {
				if (destructors.find(boost::get<jass_type*>(local.type)) != destructors.end()) { // has destructor
					allDeclarations.insert(&local);
				}
			}
		}
	}
	
	// get all function calls of destructors
	BOOST_FOREACH(jass_files::const_reference ref, ast.files) {
		BOOST_FOREACH(jass_functions::const_reference function, ref.functions) {
			
			BOOST_FOREACH(jass_statements::const_reference statement, function.statements) {
				
				// TODO
				// is jass_call
				// check function id and compare to destructor, then compare parameters and get vars
				
				// if its a call or set and contains expressions of functions (write helper function which extracts all called functions with parameters into a vector)
				// all functions must be checked!
				
				// if parameter of destructor is a function itself or another variable etc. IT MUST BE RESOLVED completely!!!
				
				// if any destructor for a variable of the corresponding type is found, remove it from all Declarations
			}
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

void Analyzer::checkBinaryOperation(const jass_binary_operation& operation, Analyzer::Reports& reports) const
{
	const jass_type_reference firstType = expressionType(operation.first_expression);
	const jass_type_reference secondType = expressionType(operation.second_expression);
	
	switch (operation.op) {
		case jass_binary_operator::Plus: {
			if (!typeReferenceIsOfBaseType(firstType, "integer")
				&& !typeReferenceIsOfBaseType(firstType, "real")
				&& !typeReferenceIsOfBaseType(firstType, "string")
				) {
				reports.push_back(Report(&operation.first_expression, _("Invalid operator.")));
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
			if (!typeReferenceIsOfBaseType(firstType, "integer")
				&& !typeReferenceIsOfBaseType(firstType, "real")
				) {
				reports.push_back(Report(&operation.first_expression, _("Invalid operator.")));
			}
			
			break;
		}
		
		case jass_binary_operator::Equal:
		case jass_binary_operator::NotEqual:  {
			if (!typeReferenceIsOfBaseType(firstType, "integer")
				&& !typeReferenceIsOfBaseType(firstType, "real")
				&& !typeReferenceIsOfBaseType(firstType, "string")
				&& !typeReferenceIsOfBaseType(firstType, "boolean")
				&& !typeReferenceIsOfBaseType(firstType, "fourcc")
				) {
				reports.push_back(Report(&operation, _("Invalid operator.")));
			}
			
			break;
		}
		
		
		case jass_binary_operator::And:
		case jass_binary_operator::Or: {
			if (!typeReferenceIsOfBaseType(firstType, "boolean")) {
				reports.push_back(Report(&operation, _("Invalid operator.")));
			}
			
			break;
		}
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
