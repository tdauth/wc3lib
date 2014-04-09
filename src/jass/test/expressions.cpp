#define BOOST_TEST_MODULE ExpressionsTest
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

struct LiteralsResults {
	std::string identifier;
	int32 integer0;
	std::string integer1; // is a unary operation and not an integer
	int32 integer2;
	int32 integer3;
	int32 integer4;
	int32 integer5;
	float32 real0;
	std::string real1; // is a unary operation and not an integer
	std::string real2; // is integer because of missing dot
	float32 real3;
	float32 real4;
	string stringLiteral0;
	string stringLiteral1;
	string stringLiteral2;
	bool booleanTrue; // initialize with wrong value to test if it is set properly
	bool booleanFalse;
	fourcc fourcc0;
	fourcc fourcc1;
	jass_null null;
	
	LiteralsResults() {
		identifier = "";
		integer0 = 0;
		integer1 = "";
		integer2 = 0;
		integer3 = 0;
		integer4 = 0;
		integer5 = 0;
		real0 = 0.0;
		real1 = "";
		real2 = "";
		real3 = 0.0;
		real4 = 0.0;
		stringLiteral0 = "";
		stringLiteral1 = "";
		stringLiteral2 = "";
		booleanTrue = false; // initialize with wrong value to test if it is set properly
		booleanFalse = true;
	}
};

BOOST_FUSION_ADAPT_STRUCT(
	LiteralsResults,
	(std::string, identifier)
	(int32, integer0)
	(std::string, integer1)
	(int32, integer2)
	(int32, integer3)
	(int32, integer4)
	(int32, integer5)
	(float32, real0)
	(std::string, real1)
	(std::string, real2)
	(float32, real3)
	(float32, real4)
	(string, stringLiteral0)
	(string, stringLiteral1)
	(string, stringLiteral2)
	(bool, booleanTrue) // initialize with wrong value to test if it is set properly
	(bool, booleanFalse)
	(fourcc, fourcc0)
	(fourcc, fourcc1)
	(wc3lib::jass::jass_null, null)
)

template <typename Iterator, typename Skipper = client::comment_skipper<Iterator> >
struct LiteralsGrammar : qi::grammar<Iterator, LiteralsResults(), Skipper> {
	LiteralsGrammar(client::jass_grammar<Iterator, Skipper> &grammar) : LiteralsGrammar::base_type(testresults) {
		no_integer_literal %=
			qi::lexeme[
				(+qi::char_("-0-9"))
			]
			- grammar.integer_literal
		;
		
		no_real_literal %=
			qi::lexeme[
				(+qi::char_("-0-9\\."))
			]
			- grammar.real_literal
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
			no_real_literal >> +qi::eol >>
			grammar.real_literal >> +qi::eol >>
			grammar.real_literal >> +qi::eol >>
			
			grammar.string_literal >> +qi::eol >>
			grammar.string_literal >> +qi::eol >>
			grammar.string_literal >> +qi::eol >>
			
			grammar.boolean_literal >> +qi::eol >>
			grammar.boolean_literal >> +qi::eol >>
			
			grammar.fourcc_literal >> +qi::eol >>
			grammar.fourcc_literal >> +qi::eol
			
			>> grammar.null
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
	
	qi::rule<Iterator, LiteralsResults(), Skipper> testresults;
	qi::rule<Iterator, std::string(), Skipper> no_integer_literal;
	qi::rule<Iterator, std::string(), Skipper> no_real_literal;
};

/*
 * Checks all JASS literals which are separated by EOL in the JASS file.
 */
BOOST_AUTO_TEST_CASE(Literals) {
	const char* jassFile = "expressions_literals.j";
	const char* traceFile = "expressions_literals_trace.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	LiteralsResults r;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<PositionIteratorType> skipper;
	client::jass_grammar<PositionIteratorType> grammar(ast, current_file);
	LiteralsGrammar<PositionIteratorType> testGrammar(grammar);
	
	try {
		valid = boost::spirit::qi::phrase_parse(
				position_begin,
				position_end,
				testGrammar, // parse all base types separated by eol
				skipper,
				// results
				r
			);

		if (position_begin != position_end) { // fail if we did not get a full match
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> e) {
		std::cerr << client::expectationFailure(e) << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	
	BOOST_REQUIRE(r.identifier == "This_is_a_valid123_Identifier");
	
	BOOST_REQUIRE(r.integer0 == 200);
	BOOST_REQUIRE(r.integer1 == "-5");
	BOOST_REQUIRE(r.integer2 == 195267);
	BOOST_REQUIRE(r.integer3 == 195267);
	BOOST_REQUIRE(r.integer4 == 720083);
	BOOST_REQUIRE(r.integer5 == 231);
	
	BOOST_CHECK_CLOSE(r.real0, 4.34, 0.0001);
	BOOST_REQUIRE(r.real1 == "-12.");
	BOOST_REQUIRE(r.real2 == "12");
	BOOST_CHECK_CLOSE(r.real3, 0.23, 0.0001);
	BOOST_REQUIRE(r.real4 == 0);
	
	BOOST_REQUIRE(r.stringLiteral0 == "this is a string \\\"like this\\\" \\n\\r");
	BOOST_REQUIRE(r.stringLiteral1 == "");
	BOOST_REQUIRE(r.stringLiteral2 == "!§$%&/()=?+-/*,.-;:_~#'|<>äöüß");
	
	BOOST_REQUIRE(r.booleanTrue == true);
	BOOST_REQUIRE(r.booleanFalse == false);
	
	BOOST_REQUIRE(r.fourcc0.size() == 4);
	BOOST_REQUIRE(to_string(r.fourcc0) == "AFDe");
	
	BOOST_REQUIRE(r.fourcc1.size() == 4);
	BOOST_REQUIRE(to_string(r.fourcc1) == "2342");
}

/*
 * Checks all JASS binary operators which are separated by EOL in the JASS file.
 */
BOOST_AUTO_TEST_CASE(BinaryOperators) {
	const char* jassFile = "expressions_binary_operators.j";
	const char* traceFile = "expressions_binary_operators_trace.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	
	// one variable of corresponding type for each binary operator
	std::vector<BOOST_SCOPED_ENUM(jass_binary_operator)> r;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<PositionIteratorType> skipper;
	client::jass_grammar<PositionIteratorType> grammar(ast, current_file);
	
	try {
		valid = boost::spirit::qi::phrase_parse(
				position_begin,
				position_end,
				
					+qi::eol // TODO initial white spaces should be skipped automatically
					>> (grammar.binary_operator % qi::eol)
			
				, // parse all base types separated by eol
				skipper,
				// results
				r
			);

		if (position_begin != position_end) { // fail if we did not get a full match
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> e) {
		std::cerr << client::expectationFailure(e) << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	
	BOOST_REQUIRE(r.size() == 12);
	BOOST_REQUIRE(r[0] == jass_binary_operator::Plus);
	BOOST_REQUIRE(r[1] == jass_binary_operator::Minus);
	BOOST_REQUIRE(r[2] == jass_binary_operator::Multiply);
	BOOST_REQUIRE(r[3] == jass_binary_operator::Divide);
	BOOST_REQUIRE(r[4] == jass_binary_operator::Greater);
	BOOST_REQUIRE(r[5] == jass_binary_operator::Less);
	BOOST_REQUIRE(r[6] == jass_binary_operator::Equal);
	BOOST_REQUIRE(r[7] == jass_binary_operator::NotEqual);
	BOOST_REQUIRE(r[8] == jass_binary_operator::GreaterEqual);
	BOOST_REQUIRE(r[9] == jass_binary_operator::LessEqual);
	BOOST_REQUIRE(r[10] == jass_binary_operator::And);
	BOOST_REQUIRE(r[11] == jass_binary_operator::Or);
}

/*
 * Checks all JASS unary operators which are separated by EOL in the JASS file.
 */
BOOST_AUTO_TEST_CASE(UnaryOperators) {
	const char* jassFile = "expressions_unary_operators.j";
	const char* traceFile = "expressions_unary_operators_trace.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	
	// one variable of corresponding type for each binary operator
	std::vector<BOOST_SCOPED_ENUM(jass_unary_operator)> r;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<PositionIteratorType> skipper;
	client::jass_grammar<PositionIteratorType> grammar(ast, current_file);
	
	try {
		valid = boost::spirit::qi::phrase_parse(
				position_begin,
				position_end,
				
					+qi::eol // TODO initial white spaces should be skipped automatically
					>> (grammar.unary_operator % qi::eol)
			
				, // parse all base types separated by eol
				skipper,
				// results
				r
			);

		if (position_begin != position_end) { // fail if we did not get a full match
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> e) {
		std::cerr << client::expectationFailure(e) << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	
	BOOST_REQUIRE(r.size() == 3);
	BOOST_REQUIRE(r[0] == jass_unary_operator::Plus);
	BOOST_REQUIRE(r[1] == jass_unary_operator::Minus);
	BOOST_REQUIRE(r[2] == jass_unary_operator::Not);
}

/*
 * This test tries to initialize a binary expression.
 * At the beginning of development an error occured because of endless default initialisation
 * of the variant type of jass_expression until the first variant type to be constructed
 * became jass_const with an default integer value.
 */
BOOST_AUTO_TEST_CASE(ExpressionDefaultAssignment) {
	jass_expression expression;
	BOOST_REQUIRE(expression.variant.which() == 0); // is jass_const
	BOOST_REQUIRE(expression.variant.type() == typeid(jass_const));
	const jass_const const_value = boost::get<const jass_const>(expression.variant);
	BOOST_REQUIRE(const_value.variant.which() == 0); // is int32
	BOOST_REQUIRE(const_value.variant.type() == typeid(int32));
	const int32 const_int = boost::get<const int32>(const_value.variant);
	BOOST_REQUIRE(const_int == 0); // default int32 value
}

/*
 * Checks all JASS unary operators which are separated by EOL in the JASS file.
 */
BOOST_AUTO_TEST_CASE(ArrayReferences) {
	const char* jassFile = "expressions_array_references.j";
	const char* traceFile = "expressions_array_references_trace.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	
	// one variable of corresponding type for each binary operator
	std::vector<jass_array_reference> r;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<PositionIteratorType> skipper;
	client::jass_grammar<PositionIteratorType> grammar(ast, current_file);
	
	try {
		valid = boost::spirit::qi::phrase_parse(
				position_begin,
				position_end,
				
					+qi::eol // TODO initial white spaces should be skipped automatically
					>> (grammar.array_reference % qi::eol)
			
				, // parse all base types separated by eol
				skipper,
				// results
				r
			);

		if (position_begin != position_end) { // fail if we did not get a full match
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> e) {
		std::cerr << client::expectationFailure(e) << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	
	BOOST_REQUIRE(r.size() == 2);
	
	BOOST_REQUIRE(r[0].var.type() == typeid(string));
	BOOST_REQUIRE(boost::get<string>(r[0].var) == "bla");
	BOOST_REQUIRE(r[0].index.variant.type() == typeid(jass_const));
	BOOST_REQUIRE(boost::get<jass_const>(r[0].index.variant).variant.type() == typeid(int32));
	BOOST_REQUIRE(boost::get<int32>(boost::get<jass_const>(r[0].index.variant).variant) == 10);
	
	BOOST_REQUIRE(r[1].var.type() == typeid(string));
	BOOST_REQUIRE(boost::get<string>(r[1].var) == "Hello_123");
	BOOST_REQUIRE(r[1].index.variant.type() == typeid(jass_binary_operation));
	BOOST_REQUIRE(boost::get<jass_binary_operation>(r[1].index.variant).first_expression.variant.type() == typeid(jass_const));
	BOOST_REQUIRE(boost::get<jass_const>(boost::get<jass_binary_operation>(r[1].index.variant).first_expression.variant).variant.type() == typeid(int32));

	BOOST_REQUIRE(boost::get<int32>(boost::get<jass_const>(boost::get<jass_binary_operation>(r[1].index.variant).first_expression.variant).variant) == 10);
	BOOST_REQUIRE(boost::get<jass_binary_operation>(r[1].index.variant).op == jass_binary_operator::Plus);
	BOOST_REQUIRE(boost::get<jass_binary_operation>(r[1].index.variant).second_expression.variant.type() == typeid(jass_const));
	BOOST_REQUIRE(boost::get<jass_const>(boost::get<jass_binary_operation>(r[1].index.variant).second_expression.variant).variant.type() == typeid(int32));

	BOOST_REQUIRE(boost::get<int32>(boost::get<jass_const>(boost::get<jass_binary_operation>(r[1].index.variant).second_expression.variant).variant) == 2);
}

/*
 * Checks all JASS unary operations which are separated by EOL in the JASS file.
 */
BOOST_AUTO_TEST_CASE(UnaryOperations) {
	const char* jassFile = "expressions_unary_operations.j";
	const char* traceFile = "expressions_unary_operations_trace.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	
	// one variable of corresponding type for each binary operator
	std::vector<jass_unary_operation> r;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<PositionIteratorType> skipper;
	client::jass_grammar<PositionIteratorType> grammar(ast, current_file);
	
	try {
		valid = boost::spirit::qi::phrase_parse(
				position_begin,
				position_end,
				
					+qi::eol // TODO initial white spaces should be skipped automatically
					>> (grammar.unary_operation % qi::eol)
			
				, // parse all base types separated by eol
				skipper,
				// results
				r
			);

		if (position_begin != position_end) { // fail if we did not get a full match
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> e) {
		std::cerr << client::expectationFailure(e) << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	
	BOOST_REQUIRE(r.size() == 6);
	//BOOST_REQUIRE(r[0] == jass_unary_operator::Plus);
	//BOOST_REQUIRE(r[1] == jass_unary_operator::Minus);
	//BOOST_REQUIRE(r[2] == jass_unary_operator::Not);
}

BOOST_AUTO_TEST_CASE(BinaryOperations) {
	const char* jassFile = "expressions_binary_operations.j";
	const char* traceFile = "expressions_binary_operations_trace.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	spiritTraceLog.close();
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
	
	// one variable of corresponding type for each binary operator
	std::vector<jass_binary_operation> r;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<PositionIteratorType> skipper;
	client::jass_grammar<PositionIteratorType> grammar(ast, current_file);
	
	try {
		valid = boost::spirit::qi::phrase_parse(
				position_begin,
				position_end,
				
					+qi::eol // TODO initial white spaces should be skipped automatically
					>> (grammar.binary_operation % qi::eol)
			
				, // parse all base types separated by eol
				skipper,
				// results
				r
			);

		if (position_begin != position_end) { // fail if we did not get a full match
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> e) {
		std::cerr << client::expectationFailure(e) << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	
	BOOST_REQUIRE(r.size() == 15);
	//BOOST_REQUIRE(r[0] == jass_unary_operator::Plus);
	//BOOST_REQUIRE(r[1] == jass_unary_operator::Minus);
	//BOOST_REQUIRE(r[2] == jass_unary_operator::Not);
}