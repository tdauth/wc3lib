#define BOOST_TEST_MODULE CommentsTest
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
 * This test parses lines and skips all comments starting with //
 * Empty lines with blanks and comments only should be fully skipped and no end of line character should appear for them!
 * Finally all non-empty lines are stored in a vector.
 * 
 * https://stackoverflow.com/questions/22591094/boost-spirit-new-line-and-end-of-input
 */
BOOST_AUTO_TEST_CASE(CommentsTest) {
	ifstream in("comments.j");
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.open("commentstraces.xml");
	
	BOOST_REQUIRE(spiritTraceLog);
	
	Grammar::ForwardIteratorType first = boost::spirit::make_default_multi_pass(Grammar::IteratorType(in));
	Grammar::ForwardIteratorType last;
	
	// used for backtracking and more detailed error output
	typedef boost::spirit::classic::position_iterator2<Grammar::ForwardIteratorType> PositionIteratorType;
	PositionIteratorType position_begin(first, last);
	PositionIteratorType position_end;
	jass_ast ast;
	bool valid = false;
	std::vector<std::string> result;
	
	try
	{
		namespace phoenix = boost::phoenix;
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;
		
		client::comment_skipper<PositionIteratorType> skipper;
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			//  | &qi::eoi
			*qi::eol >> (*(qi::char_ - (qi::eol)) % (qi::eol)), // each valid expression is separated by one eol TODO initial empty lines should be skipped automatically as well!
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
	
	// for debugging output
	BOOST_FOREACH(std::string &value, result) {
		std::cerr << value << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(result.size() == 4); // number of parsed lines, lines with comments only contain the line break and therefore result in empty strings (eol is not stored in the string)
	//BOOST_REQUIRE(result[0] == "\n"); // NOTE The first line is ignored due to the grammar which starts with a non EOL character!
	BOOST_REQUIRE(result[0] == "globals");
	BOOST_REQUIRE(result[1] == "integerbla=10"); // all blanks have to be skipped
	BOOST_REQUIRE(result[2] == "endglobals");
	//BOOST_REQUIRE(result[3] == ""); // NOTE Empty line for comment/blanks only line! This line should be skipped automatically and not appear at all so don't check!
	BOOST_REQUIRE(result[3] == "typeblaextendsinteger"); // all blanks have to be skipped
}
