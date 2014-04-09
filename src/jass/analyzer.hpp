#ifndef WC3LIB_JASS_ANALYZER_HPP
#define WC3LIB_JASS_ANALYZER_HPP

#include <vector>
#include <map>

#include "ast.hpp"

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
		std::vector<const jass_var_declaration*> leakingDeclarations(const jass_ast &ast, std::multimap<jass_type*, std::string> destructors);
		
		
};

}

}

#endif
