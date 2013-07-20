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
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "txt.hpp"

// Only use in global namespace!
BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::map::Txt::Section,
	(wc3lib::string, name)
	(wc3lib::map::Txt::Pairs, entries)
)

namespace wc3lib
{

namespace map
{

namespace client
{

using namespace boost::spirit;
//using namespace boost::spirit::qi;
using qi::double_;
using qi::phrase_parse;
using standard::space;
using boost::phoenix::ref;

//typedef BOOST_TYPEOF(space | lit("//") >> *(standard::char_ - qi::eol) >> qi::eol) SkipperType;

/*
 * Doesn't skip eols since value pairs are separated linewise which therefore can be specified easier in the rules
 */
template<typename Iterator>
struct CommentSkipper : public qi::grammar<Iterator> {

	qi::rule<Iterator> skip;
	
	CommentSkipper() : CommentSkipper::base_type(skip, "PL/0")
	{
		skip = ascii::blank | lit("//") >> *(standard::char_ - qi::eol) >> qi::eol;
	}
};

template <typename Iterator, typename Skipper = CommentSkipper<Iterator> >
struct KeyValueSquence : qi::grammar<Iterator, Txt::Pairs(), Skipper>
{
	//Txt::Pairs::value_type
	qi::rule<Iterator, Txt::Pairs(), Skipper> query; // NOTE first rule used as parameter for base_type does always need the skipper type of the grammar
	qi::rule<Iterator, std::pair<string, string>(), Skipper> pair;
	qi::rule<Iterator, string()> key, value;

	KeyValueSquence() : KeyValueSquence::base_type(query)
	{
		query =  pair > *(+qi::eol > pair > *qi::eol) > *qi::eol; // use only > for backtracking
		pair  =  key > '=' > value; // -('=' >> value)
		key   =  standard::char_("a-zA-Z_") > *standard::char_("a-zA-Z_0-9");
		value = +standard::char_("a-zA-Z_0-9");
	}
};

typedef std::istreambuf_iterator<byte> IteratorType;
typedef boost::spirit::multi_pass<IteratorType> MultiPassIteratorType;

template struct KeyValueSquence<MultiPassIteratorType>;

template <typename Iterator, typename Skipper = CommentSkipper<Iterator> >
struct SectionRule : qi::grammar<Iterator, Txt::Section(), Skipper>
{
	qi::rule<Iterator, Txt::Section(), Skipper> query;
	qi::rule<Iterator, string()> name;
	qi::rule<Iterator, Txt::Pairs(), Skipper> entries;
	
	KeyValueSquence<Iterator, Skipper> keyValueSequence;

	SectionRule() : SectionRule::base_type(query)
	{
		query =  name > -qi::eol > -entries;
		name  =  standard::char_('[') >> standard::char_("a-zA-Z_") >> *standard::char_("a-zA-Z_0-9") >> standard::char_(']');
		entries = keyValueSequence;
	}
};

template struct SectionRule<MultiPassIteratorType>;

template <typename Iterator>
bool parse(Iterator first, Iterator last, Txt::Sections &sections)
{
	SectionRule<Iterator> sectionGrammar;
	CommentSkipper<Iterator> commentSkipper;
	std::vector<Txt::Section> tmpSections;
	
	bool r = boost::spirit::qi::phrase_parse(
	first,
	last,
	(*qi::eol >> -(sectionGrammar >> *(+qi::eol >> sectionGrammar >> *qi::eol)) >> *qi::eol ), // TODO maybe it's too complex, exception is thrown from struct adaption
	// comment skipper
	commentSkipper,
	tmpSections // , sections store into "sections"!
	);

	if (first != last) // fail if we did not get a full match
		return false;
	
	// TODO temporary workaround, add sections directly from heap to vector
	BOOST_FOREACH(std::vector<Txt::Section>::const_reference ref, tmpSections) {
		std::auto_ptr<Txt::Section> s(new Txt::Section());
		s->name = ref.name;
		s->entries = ref.entries;
		sections.push_back(s);
	}

	return r;
}

}

const Txt::Pairs& Txt::entries(const string section) const
{
	for (int i = 0; i < this->sections().size(); ++i)
	{
		if (this->sections()[i].name == section)
		{
			return this->sections()[i].entries;
		}
	}

	throw Exception();
}

std::streamsize Txt::read(InputStream &istream) throw (Exception)
{
	// NOTE read http://www.boost.org/doc/libs/1_48_0/libs/spirit/doc/html/spirit/support/multi_pass.html
	// Do we need bidirectional iterator type for proper backtracing?
	typedef std::istreambuf_iterator<byte> IteratorType;
	typedef boost::spirit::multi_pass<IteratorType> ForwardIteratorType;

	ForwardIteratorType first = boost::spirit::make_default_multi_pass(IteratorType(istream));
	ForwardIteratorType last;
	
	namespace classic = boost::spirit::classic;
	typedef classic::position_iterator2<ForwardIteratorType> PositionIteratorType;
	PositionIteratorType position_begin(first, last);
	PositionIteratorType position_end;

	try
	{
		if (!client::parse(position_begin,
		position_end,
		this->sections()))
			throw Exception(_("Parsing error."));
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> e)
	{
		const classic::file_position_base<std::string>& pos = e.first.get_position();
		std::stringstream msg;
		msg <<
		"parse error at file " << pos.file <<
		" line " << pos.line << " column " << pos.column << std::endl <<
		"'" << e.first.get_currentline() << "'" << std::endl <<
		std::setw(pos.column) << " " << "^- here";
		
		throw Exception(msg.str());
	}


	return 0;
}

std::streamsize Txt::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

}

}
