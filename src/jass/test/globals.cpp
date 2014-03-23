#define BOOST_TEST_MODULE GlobalsTest
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

//#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../grammar.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

BOOST_AUTO_TEST_CASE(GlobalsTest) {
	ifstream in("globals.j");
	
	BOOST_REQUIRE(in);
	
	Grammar grammar;
	
	jass_ast ast;
	bool valid = false;
	
	try
	{
		valid = grammar.parse(in, ast);
	}
	catch(const Exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	std::cout << "Count of globals: " << ast.globals.size() << std::endl;
	
	BOOST_REQUIRE(valid);
}
