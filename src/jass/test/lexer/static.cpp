#define BOOST_TEST_MODULE LexerTest
#include <boost/test/unit_test.hpp>

#include "../../lexer.hpp"

#include <boost/bind.hpp>
#include <boost/spirit/include/lex_static_lexertl.hpp>

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

/*
 * We count all kinds of tokens to compare the resulting numbers with the expected
 * numbers.
 */
struct counter
{
	typedef bool result_type;

	template<typename T>
	bool operator () (const T &t, std::map<int, int> &c) const {

		const int token_id =  t.id() - lex::min_token_id;
		c[token_id]++;
		std::cerr << "Token: " << token_id << std::endl;

		return true;
	}
};

BOOST_AUTO_TEST_CASE(Lexer)
{
	const char* staticFile = "lexer.hpp";

	ifstream out(staticFile);

	BOOST_REQUIRE(out);

	lexer<lexer_type> l;

	// TODO generate static lexer
	// _dfa
	//BOOST_REQUIRE(lex::lexertl::generate_static(l, out, "lexer"));
}