#define BOOST_TEST_MODULE ConstantsSemantic
#include <boost/test/unit_test.hpp>

#include "../../grammar.hpp"
#include "../../analyzer.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

BOOST_AUTO_TEST_CASE(UnaryOperation) {
	const char* jassFile = "constants_unary_operation.j";
	const char* traceFile = "constants_unary_operation.xml";
	
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
	
	// one variable of corresponding type for each unary operation
	typedef std::vector<jass_unary_operation> Results;
	Results results;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar;
	grammar.prepare(position_begin, ast, current_file);
	
	try {
		valid = boost::spirit::qi::phrase_parse(
				position_begin,
				position_end,
					*qi::eol // TODO initial white spaces should be skipped automatically
					>> (grammar.unary_operation % qi::eol) // parse all unary operations separated by eol
				,
				skipper,
				results
			);

		if (position_begin != position_end) { // fail if we did not get a full match
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<Grammar::PositionIteratorType> e) {
//		std::cerr << client::expectationFailure(e) << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(results.size() == 6);
	
	Analyzer analyzer(grammar.type_symbols, grammar.global_symbols, grammar.function_symbols);
	
	// check expression types
	BOOST_REQUIRE(analyzer.typeName(analyzer.expressionType(results[0].expression)) == "integer");
	
	// TODO get all expression types
	
	bool isConstant = false;
	jass_expression expression;
	expression.variant = results[0];
	jass_const constant = analyzer.evaluateConstantExpression(expression, isConstant);
	
	BOOST_REQUIRE(isConstant);
	BOOST_REQUIRE(constant.whichType() == jass_const::Type::Integer);
	BOOST_REQUIRE(boost::get<int32>(constant.variant) == 10);
	
	expression.variant = results[1];
	constant = analyzer.evaluateConstantExpression(expression, isConstant);
	
	BOOST_REQUIRE(isConstant);
	BOOST_REQUIRE(constant.whichType() == jass_const::Type::Integer);
	BOOST_REQUIRE(boost::get<int32>(constant.variant) == -10);
	
	// real
	expression.variant = results[2];
	constant = analyzer.evaluateConstantExpression(expression, isConstant);
	
	BOOST_REQUIRE(isConstant);
	BOOST_REQUIRE(constant.whichType() == jass_const::Type::Real);
	BOOST_CHECK_CLOSE(boost::get<float32>(constant.variant), 10.40, 0.0001);
	
	expression.variant = results[3];
	constant = analyzer.evaluateConstantExpression(expression, isConstant);
	
	BOOST_REQUIRE(isConstant);
	BOOST_REQUIRE(constant.whichType() == jass_const::Type::Real);
	BOOST_CHECK_CLOSE(boost::get<float32>(constant.variant), -10.40, 0.0001);
	
	// boolean
	expression.variant = results[4];
	constant = analyzer.evaluateConstantExpression(expression, isConstant);
	
	BOOST_REQUIRE(isConstant);
	BOOST_REQUIRE(constant.whichType() == jass_const::Type::Boolean);
	BOOST_REQUIRE(boost::get<bool>(constant.variant) == false);
	
	expression.variant = results[5];
	constant = analyzer.evaluateConstantExpression(expression, isConstant);
	
	BOOST_REQUIRE(isConstant);
	BOOST_REQUIRE(constant.whichType() == jass_const::Type::Boolean);
	BOOST_REQUIRE(boost::get<bool>(constant.variant) == true);
}

BOOST_AUTO_TEST_CASE(BinaryOperation) {
	const char* jassFile = "constants_binary_operation.j";
	const char* traceFile = "constants_binary_operation.xml";
	
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
	
	// one variable of corresponding type for each unary operation
	typedef std::vector<jass_binary_operation> Results;
	Results results;
	
	// grammar has to be allocated until the end of the test because it holds the symbols
	client::comment_skipper<Grammar::PositionIteratorType> skipper;
	client::jass_grammar<Grammar::PositionIteratorType> grammar;
	grammar.prepare(position_begin, ast, current_file);
	
	try {
		valid = boost::spirit::qi::phrase_parse(
				position_begin,
				position_end,
					*qi::eol // TODO initial white spaces should be skipped automatically
					>> (grammar.binary_operation % qi::eol) // parse all unary operations separated by eol
				,
				skipper,
				results
			);

		if (position_begin != position_end) { // fail if we did not get a full match
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<Grammar::PositionIteratorType> e) {
//		std::cerr << client::expectationFailure(e) << std::endl;
	}
	
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(results.size() == 20);
	
	Analyzer analyzer(grammar.type_symbols, grammar.global_symbols, grammar.function_symbols);
	
	// check expression types
	BOOST_REQUIRE(analyzer.typeName(analyzer.expressionType(results[0].first_expression)) == "integer");
	
	// TODO get all expression types etc.
	
	bool isConstant = false;
	jass_expression expression;
	expression.variant = results[0];
	jass_const constant = analyzer.evaluateConstantExpression(expression, isConstant);
	
	BOOST_REQUIRE(isConstant);
	BOOST_REQUIRE(constant.whichType() == jass_const::Type::Integer);
	BOOST_REQUIRE(boost::get<int32>(constant.variant) == 20);
	
	expression.variant = results[1];
	constant = analyzer.evaluateConstantExpression(expression, isConstant);
	
	BOOST_REQUIRE(isConstant);
	BOOST_REQUIRE(constant.whichType() == jass_const::Type::Integer);
	BOOST_REQUIRE(boost::get<int32>(constant.variant) == 0);
	
	// TODO finish for all expressions
}
