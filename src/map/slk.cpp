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

#include <boost/spirit/include/karma.hpp>

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/bind.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/operator.hpp>
#include <boost/spirit/home/phoenix/container.hpp>
#include <boost/spirit/home/phoenix/object.hpp>

#include <boost/foreach.hpp>

#include "slk.hpp"

namespace wc3lib
{

namespace map
{

namespace client
{

namespace qi = boost::spirit::qi;
namespace karma = boost::spirit::karma;
namespace phoenix = boost::phoenix;
namespace ascii = boost::spirit::ascii;
namespace classic = boost::spirit::classic;

/**
 * The Unicode namespace can be used for UTF-8 string and comments parsing in a TXT file.
 */
namespace unicode = boost::spirit::qi::unicode;

using boost::phoenix::ref;

/**
 * Skips all records which contain formatting rules.
 * Formatting is not required we only need the cell content.
 */
template<typename Iterator>
struct RecordSkipper : public qi::grammar<Iterator> {

	RecordSkipper() : RecordSkipper<Iterator>::base_type(skip, "formatting records")
	{
		using qi::lit;
		using unicode::char_;
		using qi::int_;
		using ascii::blank;
		using qi::eol;
		using qi::eoi;
		using qi::eps;

		/*
		 * Comments may use UTF-8 characters.
		 */
		id_record %=
			lit("ID")
			>> lit(";P")
			>> +char_
			>> -lit(";N")
			>> -lit(";E")
		;

		p_record %=
			lit('P')
			>> *char_
		;

		skip_record %=
			id_record
			| p_record
		;

		skip %=
			+(eol >> -skip_record >> &eol)
		;

		id_record.name("id_record");
		p_record.name("p_record");
		skip_record.name("skip_record");
		skip.name("skip");

		BOOST_SPIRIT_DEBUG_NODES(
			(id_record)
			(p_record)
			(skip_record)
			(skip)
		);
	}

	qi::rule<Iterator> id_record;
	qi::rule<Iterator> b_record;
	qi::rule<Iterator> p_record;
	qi::rule<Iterator> skip_record;
	qi::rule<Iterator> skip;
};

typedef std::pair<Slk::Table::size_type, Slk::Table::size_type> SlkSize;

template <typename Iterator, typename Skipper = RecordSkipper<Iterator> >
struct SlkGrammar : qi::grammar<Iterator, Slk::Table(), Skipper>
{
	SlkGrammar() : SlkGrammar::base_type(cells, "slk grammar")
	{
		using qi::lit;
		using ascii::char_;
		using ascii::blank;
		using qi::lexeme;
		using qi::byte_;
		using qi::int_;
		using qi::eol;
		using qi::eoi;
		using qi::eps;
		using qi::_val;
		using phoenix::push_back;
		using phoenix::new_;

		// TODO whenever a B record appears, resize the cells array!
		b_record %=
			lit('B')
			>> lit(";X")
			>> int_
			>> lit(";Y")
			>> int_
			// TODO support ;D0 0 117 16
		;

		/*
		c_record =
			lit('C')[_val = ""]
			>> lit(";X")
			>> int_
			>> lit(";Y")
			>> int_

			>>
			-(
				lit(";E")
				>> +char_
			)

			>>
			-(
				lit(";K")
				>> +char_[_val = _1]
			)

			// TODO all possible fields!
			// TODO store only the cell value and the index!
		;

		cells =
			c_record % eol
			>> e_record
		;
		*/

		b_record.name("b_record");
		c_record.name("c_record");
		e_record.name("e_record");
		cells.name("cells");

		BOOST_SPIRIT_DEBUG_NODES(
			(b_record)
			(c_record)
			(e_record)
			(cells)
		);
	}

	qi::rule<Iterator, SlkSize(), Skipper> b_record;
	qi::rule<Iterator, Slk::Cell(), Skipper> c_record;
	qi::rule<Iterator, Skipper> e_record;
	qi::rule<Iterator, Slk::Table(), Skipper> cells;
};

template <typename Iterator>
struct SlkGenerator : karma::grammar<Iterator, Slk::Table()>
{
	SlkGenerator() : SlkGenerator::base_type(cells, "slk generator")
	{
		using karma::eps;
		using karma::lit;
		using karma::int_;
		using karma::int_generator;

		/*
		 TODO print maximum size
		b_record %=
			lit('B')
			>> lit(";X")
			>> int_generator<Slk::Table::size_type>()
			>> lit(";Y")
			>> int_generator<Slk::Table::size_type>()
		;
		*/

		/*
		cells =
			eps
			>> b_record
		;
		*/

		b_record.name("b_record");
		cells.name("cells");

		BOOST_SPIRIT_DEBUG_NODES(
			(b_record)
			(cells)
		);
	}

	karma::rule<Iterator, SlkSize()> b_record;
	karma::rule<Iterator, Slk::Table()> cells;
};

template <typename Iterator>
bool parse(Iterator first, Iterator last, Slk::Table &table)
{
	SlkGrammar<Iterator> grammar;
	RecordSkipper<Iterator> recordSkipper;

	bool r = boost::spirit::qi::phrase_parse(
		first,
		last,
		grammar,
		recordSkipper,
		table
	);

	if (first != last) // fail if we did not get a full match
	{
		return false;
	}

	return r;
}

}

std::streamsize Slk::read(InputStream &istream) throw (class Exception)
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
		if (!client::parse(position_begin,
		position_end,
		this->table()))
		{
			throw Exception(_("Parsing error."));
		}
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> &e)
	{
		const classic::file_position_base<std::string> &pos = e.first.get_position();
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

std::streamsize Slk::write(OutputStream &ostream) const throw (class Exception)
{
	typedef std::ostream_iterator<byte> Iterator;
	client::SlkGenerator<Iterator> generator;

	return boost::spirit::karma::generate(Iterator(ostream), generator, this->table());
}

}

}
