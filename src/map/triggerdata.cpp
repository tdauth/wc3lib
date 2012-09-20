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

#include <set>

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "triggerdata.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize TriggerData::Category::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Category::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Type::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Type::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Parameter::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Parameter::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Function::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Function::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Call::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Call::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::DefaultTrigger::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::DefaultTrigger::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

	/*
namespace client
{

//using namespace boost::spirit;
using namespace boost::spirit::qi;
namespace qi = boost::spirit::qi;
using qi::double_;
using boost::spirit::standard::char_;
using qi::phrase_parse;
using boost::spirit::standard::space;
using boost::phoenix::ref;
using boost::phoenix::insert;

typedef std::pair<string, string> Pair;
typedef std::map<string, Pair> Pairs;

template <typename Iterator>
struct KeysGrammar : grammar<Iterator, Pairs, space_type>
{
	rule<Iterator, Pairs, space_type> query;
	//rule<Iterator,
	rule<Iterator, Pair, space_type> pair;
	rule<Iterator, string, space_type> key, value;

	KeysGrammar() : KeysGrammar::base_type(query)
	{
		query %= *(pair | (lit("//") >> *char_())); // skip comments


		pair  %=  lexeme[key >> -('=' >> value)] >> *(lit("//") >> *char_()) >> eol; // skip spaces

		rule<Iterator, string, space_type> expression = char_("a-zA-Z_") >> *char_("a-zA-Z_0-9");

		key   %= expression;
		value %= expression;
	}
};

struct Section
{
	string name;
	Pairs entries;
};

template <typename Iterator>
struct SectionGrammar : grammar<Iterator, Section, space_type>
{
	rule<Iterator, Section> query;
	rule<Iterator, string> name;
	rule<Iterator, Pairs> entries;

	SectionGrammar() : SectionGrammar::base_type(query)
	{
		query =  name >> eol >> entries;
		name  =  lexeme[char_('[') >> standard::string >> standard::char_(']')];
		entries = KeysGrammar<Iterator>();
	}
};

template <typename Iterator>
struct SectionsGrammar : grammar<Iterator, Section, space_type>
{
	rule<Iterator, Section> query;
	rule<Iterator, string> name;
	rule<Iterator, Pairs> entries;

	SectionsGrammar() : SectionsGrammar::base_type(query)
	{
		query =  name >> eol >> entries;
		name  =  lexeme[char_('[') >> standard::string >> standard::char_(']')];
		entries = KeyValueSquence<Iterator>();
	}
};

template <typename Iterator>
bool parse(Iterator first, Iterator last)
{
	typedef std::set<string> Sections;
	Sections sections;
	string section;
	typedef std::pair<string, string> Pair;
	string key;
	string value;

	rule<Iterator, string, space_type> commentRule =
	standard::char_("//") >> standard::string
	;

	rule<Iterator, string, space_type> sectionRule =
	standard::char_('[') >> standard::string[insert(ref(sections), _1)][ref(section) = _1] >> standard::char_(']')
	;

	rule<Iterator,

	rule<Iterator, Pair, space_type> pairRule =
	standard::string[ref(key) = _1] >> '=' >> standard::string[ref(value) = _1]
	;

	bool r = phrase_parse(
	first,
	last,
	(

		commentRule |
		sectionRule[insert(ref(sections), _1] |
		(standard::string[ref(key) = _1] >> '=' >> standard::string[ref(value) = _1])
	),
	space
	);

	if (first != last) // fail if we did not get a full match
		return false;

	return r;
}

}
*/

std::streamsize TriggerData::read(InputStream &istream) throw (Exception)
{
/*
	typedef boost::spirit::basic_istream_iterator<byte, std::char_traits<byte> > istream_iterator;

	istream_iterator first = istream.begin();
	istream_iterator last = istream.end();
*/

	/*
	typedef std::istreambuf_iterator<byte> base_iterator_type;
	boost::spirit::multi_pass<base_iterator_type> first =
        boost::spirit::make_default_multi_pass(base_iterator_type(istream));
	*/

/*
	boost::spirit::qi::grammar grammar(istream, "");
	int id = 0;
	std::string defaultString;
	std::string value;
	stream_parser parser;

	if (first != last)
		throw Exception(_("Parsing error."));
*/
	return 0;
}

std::streamsize TriggerData::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

}

}
