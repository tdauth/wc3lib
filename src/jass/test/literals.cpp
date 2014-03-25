#define BOOST_TEST_MODULE LiteralsTest
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>

//#define BOOST_SPIRIT_DEBUG // enable debugging for Qi

// NOTE set this to the number of struct attributes otherwise the limit will be too small!
#define FUSION_MAX_VECTOR_SIZE 20

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

struct results {
	std::string identifier;
	int32 integer0;
	int32 integer1;
	int32 integer2;
	int32 integer3;
	int32 integer4;
	int32 integer5;
	float32 real0;
	float32 real1;
	float32 real2;
	float32 real3;
	string stringLiteral;
	bool booleanTrue; // initialize with wrong value to test if it is set properly
	bool booleanFalse;
	
	results() {
		identifier = "";
		integer0 = 0;
		integer1 = 0;
		integer2 = 0;
		integer3 = 0;
		integer4 = 0;
		integer5 = 0;
		real0 = 0.0;
		real1 = 0.0;
		real2 = 0.0;
		real3 = 0.0;
		stringLiteral = "";
		booleanTrue = false; // initialize with wrong value to test if it is set properly
		booleanFalse = true;
	}
};

BOOST_FUSION_ADAPT_STRUCT(
	results,
	(std::string, identifier)
	(int32, integer0)
	(int32, integer1)
	(int32, integer2)
	(int32, integer3)
	(int32, integer4)
	(int32, integer5)
	(float32, real0)
	(float32, real1)
	(float32, real2)
	(float32, real3)
	(string, stringLiteral)
	(bool, booleanTrue) // initialize with wrong value to test if it is set properly
	(bool, booleanFalse)
)

template <typename Iterator, typename Skipper = client::comment_skipper<Iterator> >
struct MyGrammar : qi::grammar<Iterator, results(), Skipper> {
	MyGrammar(client::jass_grammar<Iterator, Skipper> &grammar) : MyGrammar::base_type(testresults) {
		testresults =
				+qi::eol >>
				grammar.identifier >> +qi::eol >>
				
				grammar.integer_literal >> +qi::eol >>
				grammar.integer_literal >> +qi::eol >>
				grammar.integer_literal >> +qi::eol >>
				grammar.integer_literal >> +qi::eol >>
				grammar.integer_literal >> +qi::eol >>
				grammar.integer_literal >> +qi::eol >>
				
				grammar.real_literal >> +qi::eol >>
				grammar.real_literal >> +qi::eol >>
				grammar.real_literal >> +qi::eol >>
				grammar.real_literal >> +qi::eol >>
				
				grammar.string_literal >> +qi::eol >>
				
				grammar.boolean_literal >> +qi::eol >>
				grammar.boolean_literal
			;
	}
	qi::rule<Iterator, results(), Skipper> testresults;
};

/*
 * Checks all JASS literals which are separated by EOL in the JASS file.
 */
BOOST_AUTO_TEST_CASE(LiteralsTest) {
	const char* jassFile = "literals.j";
	const char* traceFile = "literalstrace.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	Grammar::traceLog.open(traceFile);
	
	BOOST_REQUIRE(Grammar::traceLog);
	
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
	
	jass_file current_file;
	current_file.path = jassFile;
	
	ast.files.push_back(current_file);
	
	BOOST_REQUIRE(ast.files.size() == 1);
	
	bool valid = false;
	
	// one variable of corresponding type for each literal in the correct order
	results r;
	
	try
	{
		namespace phoenix = boost::phoenix;
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;
		
		client::comment_skipper<PositionIteratorType> skipper;
		client::jass_grammar<PositionIteratorType> grammar(ast, current_file);
		MyGrammar<PositionIteratorType> testGrammar(grammar);
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			testGrammar, // parse all base types separated by eol
			skipper,
			
				// results
				r
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
	
	/*
	 * for debugging output
	BOOST_FOREACH(std::string &value, result) {
		std::cerr << value << std::endl;
	}
	*/
	
	BOOST_REQUIRE(valid);
	//BOOST_REQUIRE(result.size() == 6); // there are 6 different base types which all should be found in the symbol table by default
	//BOOST_REQUIRE(result[4] == "typeblaextendsinteger"); // all blanks have to be skipped
}
