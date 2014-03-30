#define BOOST_TEST_MODULE TypesTest
#include <boost/test/unit_test.hpp>

#include "../../spirit.hpp" // enable debug mode

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
	
	spiritTraceLog.open(traceFile);
	
	BOOST_REQUIRE(spiritTraceLog);
	
	// grammar has to be allocated until the end of the test because it holds the symbols
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
