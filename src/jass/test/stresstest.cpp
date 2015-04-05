#define BOOST_TEST_MODULE StressTest
#include <boost/test/unit_test.hpp>

/*
 * Include before it is included by the grammar!
 */
#include "../../spirit.hpp" // enable debug mode

/*
 * Undefine debugging stuff since it would create a huge trace file and stop the execution at some point.
 * The stress test should only test the execution time on a huge file.
 */
#undef BOOST_SPIRIT_DEBUG
#undef BOOST_SPIRIT_DEBUG_OUT
#undef BOOST_SPIRIT_LEXERTL_DEBUG

#include <boost/cast.hpp>
//#include <boost/foreach.hpp>
#include <boost/chrono.hpp>

#include "../grammar.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

BOOST_AUTO_TEST_CASE(StressTest)
{
	const char* jassFile = "stresstest.j";
	//const char* traceFile = "stresstest_trace.xml";

	ifstream in(jassFile);

	BOOST_REQUIRE(in);

	//spiritTraceLog.open(traceFile);

	//BOOST_REQUIRE(spiritTraceLog);

	jass_ast ast;
	jass_file file;
	file.path = jassFile;

	Grammar grammar;

	boost::chrono::high_resolution_clock::time_point now = boost::chrono::high_resolution_clock::now();
	const bool valid = grammar.parse(Grammar::IteratorType(in), Grammar::IteratorType(), ast, file);

	BOOST_REQUIRE(valid);

	boost::chrono::high_resolution_clock::time_point finished = boost::chrono::high_resolution_clock::now();
	boost::chrono::high_resolution_clock::duration duration = finished - now;

	std::cout << "Duration: " << duration << std::endl;
}
