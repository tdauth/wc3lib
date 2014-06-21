#define BOOST_TEST_MODULE GlobalsTest
#include <boost/test/unit_test.hpp>

#include "../../spirit.hpp" // enable debug mode

#include <boost/cast.hpp>
//#include <boost/foreach.hpp>

#include "../client.hpp"
#include "../grammar.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

BOOST_AUTO_TEST_CASE(GlobalsTest) {
	const char* jassFile = "globals.j";
	const char* traceFile = "globalstraces.xml";

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
	jass_globals result;

	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar;
	grammar.prepare(position_begin, ast, current_file);

	try {

		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.globals,
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
	BOOST_REQUIRE(result.size() == 4);

	BOOST_REQUIRE(result[0].is_constant == false);
	BOOST_REQUIRE(result[0].declaration.type.whichType() == jass_type_reference::Type::Declaration);
	BOOST_REQUIRE(boost::get<const jass_type*>(result[0].declaration.type.variant) != 0); // TODO boost::get throws exception on fail
	BOOST_REQUIRE(boost::get<const jass_type*>(result[0].declaration.type.variant)->identifier == "boolean");
	BOOST_REQUIRE(result[0].declaration.is_array == true);
	BOOST_REQUIRE(result[0].declaration.identifier == "myBoolean_123");
	BOOST_REQUIRE(!result[0].declaration.assignment.is_initialized());

	BOOST_REQUIRE(result[1].is_constant == true);
	BOOST_REQUIRE(result[1].declaration.type.whichType() == jass_type_reference::Type::Declaration);
	BOOST_REQUIRE(boost::get<const jass_type*>(result[1].declaration.type.variant) != 0); // TODO boost::get throws exception on fail
	BOOST_REQUIRE(boost::get<const jass_type*>(result[1].declaration.type.variant)->identifier == "integer");
	BOOST_REQUIRE(result[1].declaration.is_array == false);
	BOOST_REQUIRE(result[1].declaration.identifier == "My_Constant");
	BOOST_REQUIRE(result[0].declaration.assignment.is_initialized());
	BOOST_REQUIRE(result[0].declaration.assignment.get().whichType() == jass_expression::Type::Constant);
	BOOST_REQUIRE(boost::get<const jass_const&>(result[0].declaration.assignment.get().variant).whichType() == jass_const::Type::Integer);
	BOOST_REQUIRE(boost::get<int32>(boost::get<const jass_const&>(result[0].declaration.assignment.get().variant).variant) == 10);

	// TODO check other globals
}
