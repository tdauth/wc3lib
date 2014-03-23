#ifndef WC3LIB_JASS_GRAMMAR_HPP
#define WC3LIB_JASS_GRAMMAR_HPP

#include <iterator>

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

#include "../platform.hpp"
#include "../exception.hpp"
#include "ast.hpp"

namespace wc3lib
{

namespace jass
{

class Grammar {
	public:
		typedef std::basic_istream<byte> InputStream;
		typedef std::istreambuf_iterator<byte> IteratorType;
		typedef boost::spirit::multi_pass<IteratorType> ForwardIteratorType;
		
		bool parse(InputStream &istream, jass_ast &ast);
		bool parse(IteratorType first, IteratorType last, jass_ast &ast);
};

}

}

#endif
