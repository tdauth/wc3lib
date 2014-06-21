#define BOOST_TEST_MODULE LexerTest
#include <boost/test/unit_test.hpp>

#include "../../lexer.hpp"

#include <boost/bind.hpp>

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

typedef std::multimap<int, string> Map;

/*
 * We count all kinds of tokens to compare the resulting numbers with the expected
 * numbers.
 */
struct counter
{
	typedef bool result_type;

	template<typename T>
	bool operator () (const T &t, Map &c) const {
		c.insert(Map::value_type(t.id(), "bla"));
		//c[token_id] = t.value();
		std::cerr << "Token: " << t.id() << std::endl;

		return true;
	}
};

BOOST_AUTO_TEST_CASE(Lexer) {
	const char* jassFile = "lexer.j";
	const char* traceFile = "lexer_traces.xml";

	ifstream in(jassFile);

	BOOST_REQUIRE(in);

	spiritTraceLog.open(traceFile);

	BOOST_REQUIRE(spiritTraceLog);

	lexer<lexer_type> l;

	/*
	boost::spirit::istream_iterator first(in);
	boost::spirit::istream_iterator last;

	lexer_type::iterator_type iter = l.begin(first, last);
	lexer_type::iterator_type end = l.end();

	while (iter != end && token_is_valid(*iter))
	{
		++iter;
	}
	*/

	Map c;
	const bool result = tokenize(in, l, boost::bind(counter(), _1, boost::ref(c)));
	BOOST_REQUIRE(result);

	BOOST_REQUIRE(c.find(l.function_keyword.id()) != c.end());
}
