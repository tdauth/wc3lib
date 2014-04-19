#define BOOST_TEST_MODULE VariantsTest
#include <boost/test/unit_test.hpp>

#include "../../ast.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

/*
 * This test verifies type informations of all variant AST types.
 * For type safety we provide functions called "whichType()" which return enumeration values
 * for each stored type.
 * After assigning the corresponding value "whichType()" must return the proper enumeration value
 * and should never throw an exception.
 */

BOOST_AUTO_TEST_CASE(Const) {
	jass_const value;
	value.variant = 10;
	
	bool exception = false;
	
	try {
		BOOST_REQUIRE(value.whichType() == jass_const::Type::Integer);
	} catch (Exception &e) {
		exception = true;
	}
	
	BOOST_REQUIRE(!exception);
	
	value.variant = 10.4f;
	
	try {
		BOOST_REQUIRE(value.whichType() == jass_const::Type::Real);
	} catch (Exception &e) {
		exception = true;
	}
	
	BOOST_REQUIRE(!exception);
	
	value.variant = "Peter";
	
	try {
		BOOST_REQUIRE(value.whichType() == jass_const::Type::String);
	} catch (Exception &e) {
		exception = true;
	}
	
	BOOST_REQUIRE(!exception);
	
	value.variant = true;
	
	try {
		BOOST_REQUIRE(value.whichType() == jass_const::Type::Boolean);
	} catch (Exception &e) {
		exception = true;
	}
	
	BOOST_REQUIRE(!exception);
	
	value.variant = from_string("AX41");
	
	try {
		BOOST_REQUIRE(value.whichType() == jass_const::Type::Fourcc);
	} catch (Exception &e) {
		exception = true;
	}
	
	BOOST_REQUIRE(!exception);
	
	jass_null null;
	value.variant = null;
	
	try {
		BOOST_REQUIRE(value.whichType() == jass_const::Type::Null);
	} catch (Exception &e) {
		exception = true;
	}
	
	BOOST_REQUIRE(!exception);
}
