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
 * Performes specific analysises on an AST and returns results.
 * Does not performe any modifying operations!
 */
class Analyzer {
	public:
		typedef std::vector<Report> Reports;
		
		Analyzer(const jass_type_declarations &types, const jass_function_declarations &functions);
		
		std::vector<const jass_var_declaration*> leakingDeclarations(const jass_ast &ast, std::multimap<jass_type*, std::string> destructors);
		
		bool typeReferenceIsOfBaseType(const jass_type_reference &type, const jass_type_reference &baseType) const;
		
		jass_type_reference functionReferenceType(const jass_function_reference &function) const;
		jass_type_reference constType(const jass_const &constant) const;
		jass_type_reference functionCallType(const jass_function_call &call) const;
		jass_type_reference expressionType(const jass_expression &expression) const;
		
		/**
		 * \begingroup 
		 */
		void checkUnaryOperation(const jass_unary_operation &operation, Reports &resports) const;
		void checkBinaryOperation(const jass_binary_operation &operation, Reports &reports) const;
		void checkFunctionCall(const jass_function_call &call, Reports &reports) const;
		
	private:
		 const jass_type_declarations &types;
		 const jass_function_declarations &functions;
		
};

}

}

#endif
