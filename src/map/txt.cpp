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

#include <sstream>
#include <iomanip>

//#include <boost/spirit/include/phoenix.hpp>
#include "../qi.hpp"

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_scope.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <boost/foreach.hpp>

#include "txt.hpp"

// Only use in global namespace!
BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::map::Txt::Section,
	(wc3lib::string, name)
	(wc3lib::map::Txt::Entries, entries)
)

namespace wc3lib
{

namespace map
{

namespace client
{

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;
namespace ascii = boost::spirit::ascii;
namespace classic = boost::spirit::classic;

/**
 * The Unicode namespace can be used for UTF-8 string and comments parsing in a TXT file.
 */
namespace unicode = boost::spirit::qi::unicode;

using boost::phoenix::ref;

/*
 * Doesn't consume eols since value pairs are separated linewise which therefore can be specified easier in the rules
 */
template<typename Iterator>
struct CommentSkipper : public qi::grammar<Iterator> {

	CommentSkipper() : CommentSkipper<Iterator>::base_type(skip, "comments and blanks")
	{
		using qi::lit;
		using ascii::char_;
		using ascii::blank;
		using qi::eol;
		using qi::eoi;
		using qi::eps;

		/*
		 * Eat all lines which do not have a section or key.
		 */
		emptyline %=
			// Units/ItemStrings.txt contains lines with no meaning
			// Units/ItemFunc.txt has a comment starting with one single / (/ Stuffed Penguin)
			+(unicode::char_ - lit('[') - lit('=') - eol)
		;

		/*
		 * Comments may use UTF-8 characters.
		 * Comments usually start with // but there are exceptions. The exceptions are caught by rule "emptyline"
		 * This rule is only used in lines at the end.
		 */
		comment %=
			lit("//") > *(unicode::char_ - eol)
		;

		emptylines %=
			// do not consume the eol of the last empty line because it is the eol between all skipped empty lines and the first one
			+(eol >> -(comment | emptyline) >> &eol)
		;

		skip %=
			emptylines
			| comment
			| blank // check blank as last value
		;

		emptyline.name("emptyline");
		moreemptylines.name("moreemptylines");
		emptylines.name("emptylines");
		comment.name("comment");
		skip.name("skip");

		BOOST_SPIRIT_DEBUG_NODES(
			(emptyline)
			(moreemptylines)
			(emptylines)
			(comment)
			(skip)
		);
	}

	qi::rule<Iterator> skip;
	qi::rule<Iterator> emptyline;
	qi::rule<Iterator> moreemptylines;
	qi::rule<Iterator> emptylines;
	qi::rule<Iterator> comment;
};

template <typename Iterator, typename Skipper = CommentSkipper<Iterator> >
struct KeyValueSquence : qi::grammar<Iterator, Txt::Entries(), Skipper>
{
	KeyValueSquence() : KeyValueSquence::base_type(pairs, "key value sequence")
	{
		using qi::lit;
		using qi::no_skip;
		using ascii::char_;
		using ascii::blank;
		using qi::eol;
		using qi::eoi;
		using qi::eps;

		// use only > for non backtracking expectations to get more specific error results

		pairs %=
			pair % qi::eol
		;

		pair %=
			key
			> lit('=')
			> value
		;

		key %=
			+(unicode::char_ - lit('=') - lit('[') - qi::eol)
		;

		// values can be empty or all characters except eol which indicates the and of the value, eoi is the end of the stream and "//" starts a comment!
		value %=
			no_skip[
				*(unicode::char_ - qi::eol - lit("//"))
			]
		;

		/*
		 * Define all names:
		 */
		pairs.name("pairs");
		pair.name("pair");
		key.name("key");
		value.name("value");

		BOOST_SPIRIT_DEBUG_NODES(
			(pairs)
			(pair)
			(key)
			(value)
		);
	}

	qi::rule<Iterator, Txt::Entries(), Skipper> pairs; // NOTE first rule used as parameter for base_type does always need the skipper type of the grammar
	qi::rule<Iterator, Txt::Entry(), Skipper> pair;
	qi::rule<Iterator, string(), Skipper> key;
	qi::rule<Iterator, string()> value; // only skip blanks at beginning and at the end
};

template <typename Iterator, typename Skipper = CommentSkipper<Iterator> >
struct SectionGrammar : qi::grammar<Iterator, Txt::Section(), Skipper>
{
	SectionGrammar() : SectionGrammar::base_type(query, "section grammar")
	{
		using qi::lit;
		using ascii::char_;
		using ascii::blank;
		using qi::eol;
		using qi::eoi;
		using qi::eps;
		using qi::_val;
		using phoenix::push_back;
		using phoenix::new_;

		query %=
			name
			>> -(qi::eol >> entries)
		;

		name %=
			lit('[')
			> char_("a-zA-Z_")
			> *char_("a-zA-Z_0-9")
			> lit(']')
		;

		entries %=
			keyValueSequence
		;

		query.name("query");
		name.name("name");
		entries.name("entries");

		BOOST_SPIRIT_DEBUG_NODES(
			(query)
			(name)
			(entries)
		);
	}

	qi::rule<Iterator, Txt::Section(), Skipper> query;
	qi::rule<Iterator, string(), Skipper> name;
	qi::rule<Iterator, Txt::Entries(), Skipper> entries;

	KeyValueSquence<Iterator, Skipper> keyValueSequence;
};

template <typename Iterator, typename Skipper = CommentSkipper<Iterator> >
struct TxtGrammar : qi::grammar<Iterator, Txt::Sections(), Skipper>
{
	TxtGrammar() : TxtGrammar::base_type(sections, "txt grammar")
	{
		using qi::lit;
		using ascii::char_;
		using ascii::blank;
		using qi::lexeme;
		using qi::byte_;
		using qi::eol;
		using qi::eoi;
		using qi::eps;
		using qi::_val;
		using phoenix::push_back;
		using phoenix::new_;

		/*
		 * https://en.wikipedia.org/wiki/Byte_order_mark
		 * https://en.wikipedia.org/wiki/Byte_order_mark#UTF-8
		 */
		bomMarker %=
			lexeme[
				byte_(0xEF)
				> byte_(0xBB)
				> byte_(0xBF)
			]
		;

		sections %=
			-bomMarker
			>> *eol // TODO do we have to consume eols at the beginning or does the skipper handle them?
			>> (sectionGrammar % eol)
			>> *eol
		;

		bomMarker.name("bom_marker");
		sections.name("sections");

		BOOST_SPIRIT_DEBUG_NODES(
			(bomMarker)
			(sections)
		);
	}

	qi::rule<Iterator, Skipper> bomMarker;
	qi::rule<Iterator, Txt::Sections(), Skipper> sections;

	SectionGrammar<Iterator, Skipper> sectionGrammar;
};

template <typename Iterator>
bool parse(Iterator first, Iterator last, Txt::Sections &sections)
{
	/*
	 * Use static const instances to improve the performance.
	 * But it has to be stateless: http://stackoverflow.com/questions/16918831/how-to-benchmark-boost-spirit-parser
	 */
	static const TxtGrammar<Iterator> grammar;
	static const CommentSkipper<Iterator> commentSkipper;

	bool r = boost::spirit::qi::phrase_parse(
		first,
		last,
		grammar,
		// comment skipper
		commentSkipper,
		sections //sections store into "sections"!
	);

	if (first != last) // fail if we did not get a full match
	{
		return false;
	}

	return r;
}

}

const Txt::Entries& Txt::entries(const string &section) const
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

std::streamsize Txt::read(InputStream &istream)
{
	typedef std::istreambuf_iterator<byte> IteratorType;
	typedef boost::spirit::multi_pass<IteratorType> ForwardIteratorType;

	ForwardIteratorType first = boost::spirit::make_default_multi_pass(IteratorType(istream));
	ForwardIteratorType last;

	// used for backtracking and more detailed error output
	namespace classic = boost::spirit::classic;
	typedef classic::position_iterator2<ForwardIteratorType> PositionIteratorType;
	PositionIteratorType position_begin(first, last);
	PositionIteratorType position_end;

	try
	{
		if (!client::parse(position_begin, position_end, this->sections()))
		{
			throw Exception(_("Parsing error."));
		}
	}
	catch (const boost::spirit::qi::expectation_failure<PositionIteratorType> &e)
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

std::streamsize Txt::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	std::string out;

	BOOST_FOREACH(Sections::const_reference ref, sections())
	{
		ostringstream osstream;
		osstream << "[" << ref.name << "]\n";
		out =  osstream.str();
		wc3lib::writeString(ostream, out, size, out.length());

		BOOST_FOREACH(Entries::const_reference keyValuePair, ref.entries)
		{
			osstream.str(""); // flush
			osstream << keyValuePair.first << " = " << keyValuePair.second << "\n";
			out =  osstream.str();
			wc3lib::writeString(ostream, out, size, out.length());
		}
	}

	return size;
}

}

}
