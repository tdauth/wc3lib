#define BOOST_TEST_MODULE TypesTest
#include <boost/test/unit_test.hpp>

//#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../grammar.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

BOOST_AUTO_TEST_CASE(TypesTest) {
	const char* jassFile = "types.j";
	const char* traceFile = "typestrace.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	Grammar::traceLog.open(traceFile);
	
	BOOST_REQUIRE(Grammar::traceLog);
	
	Grammar grammar;
	jass_ast ast;
	bool valid = false;
	
	try
	{
		valid = grammar.parse(in, ast, jassFile);
	}
	catch(const Exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	BOOST_REQUIRE(valid);
}
