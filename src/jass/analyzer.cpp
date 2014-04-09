#include <set>

#include <boost/foreach.hpp>

#include "analyzer.hpp"

namespace wc3lib
{

namespace jass
{

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
		
		BOOST_FOREACH(jass_file::functions::const_reference function, ref.functions) {
			BOOST_FOREACH(jass_locals::const_reference local, function.locals) {
				if (destructors.find(boost::get<jass_type*>(local.type)) != destructors.end()) { // has destructor
					allDeclarations.insert(&local);
				}
			}
		}
	}
	
	// get all function calls of destructors
	BOOST_FOREACH(jass_files::const_reference ref, ast.files) {
		BOOST_FOREACH(jass_file::functions::const_reference function, ref.functions) {
			
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

}

}
