#ifndef WC3LIB_JASS_GENERATOR_HPP
#define WC3LIB_JASS_GENERATOR_HPP

#include <ostream>

#include "../spirit.hpp"

#include <boost/spirit/include/karma.hpp>

#include "client.hpp"

namespace wc3lib
{

namespace jass
{

class Generator
{
	public:
		typedef std::ostreambuf_iterator<char> IteratorType;
		typedef client::jass_generator<IteratorType> JassGrammar;
};

}

}

#endif
