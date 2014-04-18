#define BOOST_TEST_MODULE StatementsTest
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

BOOST_AUTO_TEST_CASE(Set) {
	const char* jassFile = "statements_set.j";
	const char* traceFile = "statements_set_traces.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	std::vector<jass_set> result;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar(position_begin, ast, current_file);
	
	try {
		using boost::spirit::qi::eol;
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.set % qi::eol,
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
	BOOST_REQUIRE(result.size() == 10);
}

BOOST_AUTO_TEST_CASE(Call) {
	const char* jassFile = "statements_call.j";
	const char* traceFile = "statements_call_traces.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	std::vector<jass_call> result;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar(position_begin, ast, current_file);
	
	try {
		using boost::spirit::qi::eol;
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.call % qi::eol,
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
	BOOST_REQUIRE(result.size() == 3);
}

BOOST_AUTO_TEST_CASE(IfThenElse) {
	const char* jassFile = "statements_ifthenelse.j";
	const char* traceFile = "statements_ifthenelse_traces.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	std::vector<jass_ifthenelse> result;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar(position_begin, ast, current_file);
	
	try {
		using boost::spirit::qi::eol;
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.ifthenelse % qi::eol,
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
	BOOST_REQUIRE(result.size() == 7);
}

BOOST_AUTO_TEST_CASE(Loop) {
	const char* jassFile = "statements_loop.j";
	const char* traceFile = "statements_loop_traces.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	std::vector<jass_loop> result;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar(position_begin, ast, current_file);
	
	try {
		using boost::spirit::qi::eol;
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.loop % qi::eol,
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
	BOOST_REQUIRE(result.size() == 2);
}

BOOST_AUTO_TEST_CASE(Exitwhen) {
	const char* jassFile = "statements_exitwhen.j";
	const char* traceFile = "statements_exitwhen_traces.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	std::vector<jass_exitwhen> result;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar(position_begin, ast, current_file);
	
	try {
		using boost::spirit::qi::eol;
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.exitwhen % qi::eol,
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
}

BOOST_AUTO_TEST_CASE(Return) {
	const char* jassFile = "statements_return.j";
	const char* traceFile = "statements_return_traces.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	std::vector<jass_return> result;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar(position_begin, ast, current_file);
	
	try {
		using boost::spirit::qi::eol;
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.return_statement % qi::eol,
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
	BOOST_REQUIRE(result.size() == 5);
}

BOOST_AUTO_TEST_CASE(Debug) {
	const char* jassFile = "statements_debug.j";
	const char* traceFile = "statements_debug_traces.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	std::vector<jass_debug> result;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar(position_begin, ast, current_file);
	
	try {
		using boost::spirit::qi::eol;
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			grammar.debug_statement % qi::eol,
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
}
