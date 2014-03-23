#define BOOST_TEST_MODULE CommentsTest
#include <boost/test/unit_test.hpp>

//#define BOOST_SPIRIT_DEBUG // enable debugging for Qi

#include <fstream>
#include <iostream>
#include <iomanip>

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../grammar.cpp"
#include "../ast.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

/*
 * This test parses lines and skips all comments starting with //
 * End of line characters should still be at the end of the strings to detect the lines.
 * All lines are stored in a vector.
 * 
 * https://stackoverflow.com/questions/22591094/boost-spirit-new-line-and-end-of-input
 */
BOOST_AUTO_TEST_CASE(CommentsTest) {
	ifstream in("comments.j");
	
	BOOST_REQUIRE(in);
	
	typedef std::istreambuf_iterator<byte> IteratorType;
	typedef boost::spirit::multi_pass<IteratorType> ForwardIteratorType;

	ForwardIteratorType first = boost::spirit::make_default_multi_pass(IteratorType(in));
	ForwardIteratorType last;
	
	// used for backtracking and more detailed error output
	namespace classic = boost::spirit::classic;
	typedef classic::position_iterator2<ForwardIteratorType> PositionIteratorType;
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
	
		/*
		 * The skipper has no attributes nor does it need a skipper.
		 */
		bool valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			//  | &qi::eoi
			*qi::eol >> (*(qi::char_ - (qi::eol)) % (+qi::eol)) >> qi::eoi,
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
		const classic::file_position_base<std::string>& pos = e.first.get_position();
		std::stringstream msg;
		msg <<
		"parse error at file " << pos.file <<
		" line " << pos.line << " column " << pos.column << std::endl <<
		"'" << e.first.get_currentline() << "'" << std::endl <<
		std::setw(pos.column) << " " << "^- here";
		
		std::cerr << msg.str() << std::endl;
	}
	
	BOOST_FOREACH(std::string &value, result) {
		std::cerr << value << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(result.size() == 5); // number of parsed lines, lines with comments only contain the line break and therefore result in empty strings (eol is not stored in the string)
}
