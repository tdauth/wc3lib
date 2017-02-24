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

#include <iomanip>

#include "../qi.hpp"

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/phoenix/operator.hpp>

#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

#include "mapstrings.hpp"

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
 * The Unicode namespace can be used for UTF-8 string and comments.
 */
namespace unicode = boost::spirit::qi::unicode;

using boost::phoenix::ref;

void appendString(string &var, string &other)
{
	var += other;
}

/**
 * \ingroup parsers
 */
template <typename Iterator, typename Skipper >
struct StringsGrammar : qi::grammar<Iterator, MapStrings::Entries(), Skipper>
{
	StringsGrammar() : StringsGrammar::base_type(pairs, "strings grammar")
	{
		using qi::lit;
		using qi::no_skip;
		using ascii::char_;
		using ascii::blank;
		using qi::eol;
		using qi::eoi;
		using qi::eps;
		using qi::int_;
		using qi::_val;
		using qi::lexeme;
		using qi::byte_;
		using namespace qi::labels;

		using phoenix::at_c;
		using phoenix::push_back;
		using phoenix::bind;

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

		comment %=
			lit("//")
			// skip the initial spaces only
			> lexeme[
				*(unicode::char_ - eol)
			]
		;

		skipped =
			+(
				-(
					comment[phoenix::bind(&appendString, phoenix::ref(_val), phoenix::ref(_1))]
					| unicode::blank
				)
				>> qi::eol
			)
		;

		pairs =
			-bomMarker
			>> (*pair)[_val = _1]
			>> -skipped
		;

		/*
		 * Append all comment strings before the first { character to the comment of the entry.
		 */
		pair =
			-skipped[phoenix::bind(&appendString, at_c<1>(_val), phoenix::ref(_1))]
			>> lit("STRING")
			>> -skipped[phoenix::bind(&appendString, at_c<1>(_val), phoenix::ref(_1))]
			>> qi::uint_parser<int32>()[at_c<0>(_val) = _1]
			>> -skipped[phoenix::bind(&appendString, at_c<1>(_val), phoenix::ref(_1))]
			>> lit('{')
			>> -qi::eol
			>> value[at_c<2>(_val) = _1]
			>> qi::eol
			>> lit('}')
		;

		// values can be empty or all characters except the closing ones
		value %=
			no_skip[
				*(
					(unicode::char_- lit('}') - qi::eol)
					// only take eol if it is not the last one before the closing bracket
					| (qi::eol >> &(unicode::char_- lit('}')))
				)
			]
		;

		/*
		 * Define all names:
		 */
		bomMarker.name("bomMarker");
		comment.name("comment");
		skipped.name("name");
		value.name("value");
		pair.name("pair");
		pairs.name("pairs");

		BOOST_SPIRIT_DEBUG_NODES(
			(bomMarker)
			(comment)
			(skipped)
			(value)
			(pair)
			(pairs)
		);
	}

	qi::rule<Iterator> bomMarker;
	qi::rule<Iterator, string(), Skipper> comment;
	qi::rule<Iterator, string(), Skipper> skipped;
	qi::rule<Iterator, string()> value;
	qi::rule<Iterator, MapStrings::Entry(), Skipper> pair;
	qi::rule<Iterator, MapStrings::Entries(), Skipper> pairs;
};

template <typename Iterator>
bool parse(Iterator first, Iterator last, MapStrings::Entries &entries)
{
	/*
	 * Use static const instances to improve the performance.
	 * But it has to be stateless: http://stackoverflow.com/questions/16918831/how-to-benchmark-boost-spirit-parser
	 */
	static const StringsGrammar<Iterator, ascii::blank_type> grammar;

	bool r = boost::spirit::qi::phrase_parse(
		first,
		last,
		grammar,
		// skip blank lines
		ascii::blank,
		entries
	);

	if (first != last) // fail if we did not get a full match
	{
		return false;
	}

	return r;
}

}

std::streamsize MapStrings::read(InputStream &istream)
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
		if (!client::parse(position_begin, position_end, this->entries()))
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

std::streamsize MapStrings::write(OutputStream &ostream) const
{
	return this->write(ostream, true);
}

std::streamsize MapStrings::write(OutputStream &ostream, bool useSpaces) const
{
	std::size_t i = 0;

	/**
	 * Writes STRING entries into the the output file ostream.
	 * TODO Use Karma to generate the output not basic code.
	 */
	BOOST_FOREACH(Entries::const_reference ref, this->entries())
	{
		if (useSpaces && i > 0)
		{
			ostream << std::endl;
		}

		ostream << "STRING " << ref.key << std::endl;

		// The comment is added after STRING for war3map.wts files by the World Editor.
		if (!ref.comment.empty())
		{
			ostream << "// " << ref.comment << std::endl;
		}

		ostream
		<< '{' << std::endl
		<< ref.value << std::endl
		<< '}' << std::endl
		;

		++i;
	}

	return 0;
}

}

}

// Only use in global namespace!
BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::map::MapStrings::Entry,
	(int, key)
	(wc3lib::string, comment)
	(wc3lib::string, value)
)
