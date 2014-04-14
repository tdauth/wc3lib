#define BOOST_TEST_MODULE CommonAiTest
#include <boost/test/unit_test.hpp>

#include "../../spirit.hpp" // enable debug mode

#include <boost/cast.hpp>
//#include <boost/foreach.hpp>

#include "../grammar.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

BOOST_AUTO_TEST_CASE(CommonAiTest) {
	const char* jassFile = "common.ai";
	const char* traceFile = "commonai_traces.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.open(traceFile);
	
	BOOST_REQUIRE(spiritTraceLog);
	
	jass_ast ast;
	jass_file file;
	file.path = jassFile;
	
	Grammar grammar;
	
	const bool valid = grammar.parse(Grammar::IteratorType(in), Grammar::IteratorType(), ast, file);
	
	BOOST_REQUIRE(valid);
}
