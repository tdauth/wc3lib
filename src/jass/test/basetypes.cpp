#define BOOST_TEST_MODULE BaseTypesTest
#include <boost/test/unit_test.hpp>

#include "../../spirit.hpp" // enable debug mode

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../client.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

/*
 * This test tries to find all six base types of JASS from the input file.
 * The symbol table should add them in the first place so that they are available for ALL JASS inputs.
 */
BOOST_AUTO_TEST_CASE(BaseTypesTest) {
	const char* jassFile = "basetypes.j";
	const char* traceFile = "basetypestrace.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.open(traceFile);
	
	BOOST_REQUIRE(spiritTraceLog);
	
	Grammar::ForwardIteratorType first = boost::spirit::make_default_multi_pass(Grammar::IteratorType(in));
	Grammar::ForwardIteratorType last;
	
	// used for backtracking and more detailed error output
	typedef boost::spirit::classic::position_iterator2<Grammar::ForwardIteratorType> PositionIteratorType;
	PositionIteratorType position_begin(first, last);
	PositionIteratorType position_end;
	
	jass_ast ast;
	
	jass_file current_file;
	current_file.path = jassFile;
	
	ast.files.push_back(current_file);
	
	BOOST_REQUIRE(ast.files.size() == 1);
	
	bool valid = false;
	std::vector<jass_type> result;
	
	try
	{
		namespace phoenix = boost::phoenix;
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;
		
		client::comment_skipper<PositionIteratorType> skipper;
		client::jass_grammar<PositionIteratorType> grammar(ast, current_file);
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			(grammar.type_symbols % qi::eol), // parse all base types separated by eol
			skipper,
			result
			);

		if (position_begin != position_end) // fail if we did not get a full match
		{
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> e)
	{
		std::cerr << client::expectationFailure(e) << std::endl;
	}
	
	/*
	 * for debugging output
	BOOST_FOREACH(std::string &value, result) {
		std::cerr << value << std::endl;
	}
	*/
	
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(result.size() == 6); // there are 6 different base types which all should be found in the symbol table by default
	//BOOST_REQUIRE(result[4] == "typeblaextendsinteger"); // all blanks have to be skipped
	BOOST_REQUIRE(result[0].identifier == "integer");
	BOOST_REQUIRE(result[1].identifier == "boolean");
	BOOST_REQUIRE(result[2].identifier == "code");
	BOOST_REQUIRE(result[3].identifier == "real");
	BOOST_REQUIRE(result[4].identifier == "string");
	BOOST_REQUIRE(result[5].identifier == "handle");
}
