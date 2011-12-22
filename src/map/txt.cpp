/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

#include "txt.hpp"

namespace wc3lib
{

namespace map
{
/*
namespace client
{

using namespace boost::spirit;
//using namespace boost::spirit::qi;
using qi::double_;
using qi::phrase_parse;
using standard::space;
using boost::phoenix::ref;

template <typename Iterator>
struct KeyValueSquence : qi::grammar<Iterator, Txt::Pairs>
{
	qi::rule<Iterator, Txt::Pairs()> query;
	qi::rule<Iterator, std::pair<map::string, map::string>()> pair;
	qi::rule<Iterator, map::string> key, value;

	KeyValueSquence() : KeyValueSquence::base_type(query)
	{
		query =  pair >> *(qi::eol >> pair);
		pair  =  key >> -('=' >> value);
		key   =  standard::char_("a-zA-Z_") >> *standard::char_("a-zA-Z_0-9");
		value = +standard::char_("a-zA-Z_0-9");
	}
};



template <typename Iterator>
struct SectionRule : qi::grammar<Iterator, Txt::Section>
{
	qi::rule<Iterator, Txt::Section> query;
	qi::rule<Iterator, map::string> name;
	qi::rule<Iterator, Txt::Pairs()> entries;

	SectionRule() : SectionRule::base_type(query)
	{
		query =  name >> qi::eol >> entries;
		name  =  standard::char_('[') >> standard::string >> standard::char_(']');
		entries = KeyValueSquence<Iterator>();
	}
};

template <typename Iterator>
bool parse(Iterator first, Iterator last, Txt::Sections &sections)
{
	typedef std::set<string> Sections;
	string section;
	typedef std::pair<string, string> Pair;
	string key;
	string value;

	// , space for the third parameter?
	qi::rule<Iterator, string> commentRule =
	standard::char_("//") >> standard::string
	;

	qi::rule<Iterator, string> sectionRule =
	standard::char_('[') >> standard::string[insert(ref(sections), boost::spirit::_1)][ref(section) = boost::spirit::_1] >> standard::char_(']')
	;

	//rule<Iterator,

	qi::rule<Iterator, Pair> pairRule =
	standard::string[ref(key) = _1] >> '=' >> standard::string[ref(value) = _1]
	;

	bool r = phrase_parse(
	first,
	last,
	(

		commentRule |
		sectionRule[ref(section) = _1] |
		(standard::string[ref(key) = _1] >> '=' >> standard::string[ref(value) = _1])
	),
	space
	);

	if (first != last) // fail if we did not get a full match
		return false;

	return r;
}

}

Txt::Entries Txt::entries(const wc3lib::map::string section) const
{
}

std::streamsize Txt::read(InputStream &istream) throw (Exception)
{
	// NOTE read http://www.boost.org/doc/libs/1_48_0/libs/spirit/doc/html/spirit/support/multi_pass.html
	// Do we need bidirectional iterator type for proper backtracing?
	typedef std::istreambuf_iterator<byte> IteratorType;

	boost::spirit::multi_pass<IteratorType> first = boost::spirit::make_default_multi_pass(IteratorType(istream));

	if (!parse(first,
	boost::spirit::make_default_multi_pass(IteratorType()),
	sections()))
		throw Exception(_("Parsing error."));

	return 0;
}

std::streamsize Txt::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}
*/

}

}
