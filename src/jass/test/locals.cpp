#define BOOST_TEST_MODULE LocalsTest
#include <boost/test/unit_test.hpp>

#include "../../spirit.hpp" // enable debug mode

#include <boost/cast.hpp>
//#include <boost/foreach.hpp>

#include "../client.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

BOOST_AUTO_TEST_CASE(LocalsTest) {
	const char* jassFile = "locals.j";
	const char* traceFile = "locals_traces.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.open(traceFile);
	
	BOOST_REQUIRE(spiritTraceLog);
	
	Grammar::ForwardIteratorType first = boost::spirit::make_default_multi_pass(Grammar::IteratorType(in));
	Grammar::ForwardIteratorType last;
	
	// used for backtracking and more detailed error output
	Grammar::PositionIteratorType position_begin(first);
	Grammar::PositionIteratorType position_end;
	
	jass_ast ast;
	
	jass_file current_file;
	current_file.path = jassFile;
	
	ast.files.push_back(current_file);
	
	BOOST_REQUIRE(ast.files.size() == 1);
	
	bool valid = false;
	jass_locals result;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar(position_begin, ast, current_file);
	
	try {
		namespace qi = boost::spirit::qi;
		
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.locals,
			skipper,
			result
		);

		if (position_begin != position_end) // fail if we did not get a full match
		{
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<Grammar::PositionIteratorType> e)
	{
//		std::cerr << client::expectationFailure(e) << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(result.size() == 6);
}
