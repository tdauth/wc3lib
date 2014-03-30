#define BOOST_TEST_MODULE LiteralsTest
#include <boost/test/unit_test.hpp>

#include "../../spirit.hpp" // enable debug mode

// NOTE set this to the number of struct attributes otherwise the limit will be too small!
#define FUSION_MAX_VECTOR_SIZE 20

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../grammar.hpp"
#include "../client.hpp"
#include "../ast.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

struct results {
	std::string identifier;
	int32 integer0;
	std::string integer1; // is a unary operation and not an integer
	int32 integer2;
	int32 integer3;
	int32 integer4;
	int32 integer5;
	float32 real0;
	std::string real1; // is a unary operation and not an integer
	float32 real2;
	float32 real3;
	string stringLiteral;
	bool booleanTrue; // initialize with wrong value to test if it is set properly
	bool booleanFalse;
	
	results() {
		identifier = "";
		integer0 = 0;
		integer1 = "";
		integer2 = 0;
		integer3 = 0;
		integer4 = 0;
		integer5 = 0;
		real0 = 0.0;
		real1 = "";
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
	(std::string, integer1)
	(int32, integer2)
	(int32, integer3)
	(int32, integer4)
	(int32, integer5)
	(float32, real0)
	(std::string, real1)
	(float32, real2)
	(float32, real3)
	(string, stringLiteral)
	(bool, booleanTrue) // initialize with wrong value to test if it is set properly
	(bool, booleanFalse)
)

template <typename Iterator, typename Skipper = client::comment_skipper<Iterator> >
struct MyGrammar : qi::grammar<Iterator, results(), Skipper> {
	MyGrammar(client::jass_grammar<Iterator, Skipper> &grammar) : MyGrammar::base_type(testresults) {
		no_integer_literal %=
			qi::as_string[!grammar.integer_literal]
		;
		
		no_real_literal %=
			qi::as_string[!grammar.real_literal]
		;
		
		testresults =
			+qi::eol >> // TODO initial white spaces should be skipped automatically
			
			grammar.identifier >> +qi::eol >>
			
			grammar.integer_literal >> +qi::eol >>
			no_integer_literal >> +qi::eol >>
			grammar.integer_literal >> +qi::eol >>
			grammar.integer_literal >> +qi::eol >>
			grammar.integer_literal >> +qi::eol >>
			grammar.integer_literal >> +qi::eol >>
			
			grammar.real_literal >> +qi::eol >>
			no_real_literal >> +qi::eol >>
			grammar.real_literal >> +qi::eol >>
			grammar.real_literal >> +qi::eol >>
			
			grammar.string_literal >> +qi::eol >>
			
			grammar.boolean_literal >> +qi::eol >>
			grammar.boolean_literal
		;
		
		no_integer_literal.name("no_integer_literal");
		no_real_literal.name("no_real_literal");
		testresults.name("testresults");
		
		BOOST_SPIRIT_DEBUG_NODES(
			(testresults)
			(no_integer_literal)
			(no_real_literal)
		);
	}
	
	qi::rule<Iterator, results(), Skipper> testresults;
	qi::rule<Iterator, std::string(), Skipper> no_integer_literal;
	qi::rule<Iterator, std::string(), Skipper> no_real_literal;
};

/*
 * Checks all JASS literals which are separated by EOL in the JASS file.
 */
BOOST_AUTO_TEST_CASE(LiteralsTest) {
	const char* jassFile = "literals.j";
	const char* traceFile = "literalstrace.xml";
	
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
		std::cerr << client::expectationFailure(e) << std::endl;
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
