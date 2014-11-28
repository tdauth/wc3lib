#ifndef WC3LIB_MDLX_MDXGRAMMARCLIENT_HPP
#define WC3LIB_MDLX_MDXGRAMMARCLIENT_HPP

#include "../qi.hpp"
#include "platform.hpp"
#include "mdlx.hpp"

namespace wc3lib
{

namespace mdlx
{

namespace client
{

namespace qi = boost::spirit::qi;

template <typename Iterator>
class MdxGrammar : public qi::grammar<Iterator, Mdlx*()>
{
	MdxGrammar();

	qi::rule<Iterator, long32()> integer_literal;
	qi::rule<Iterator, float32()> real_literal;
	qi::rule<Iterator, string()> string_literal;
	qi::rule<Iterator, VertexData()> vertexData;
};

}

}

}

#endif
