#define BOOST_TEST_MODULE LexerTest
#include <boost/test/unit_test.hpp>

#include "../../lexer.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

BOOST_AUTO_TEST_CASE(Lexer) {
	const char* jassFile = "lexer.j";
	const char* traceFile = "lexer_traces.xml";

	ifstream in(jassFile);

	BOOST_REQUIRE(in);

	spiritTraceLog.open(traceFile);

	BOOST_REQUIRE(spiritTraceLog);

	lexer<lexer_type> l;
	BOOST_REQUIRE(tokenize(in, l));
}
