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

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_scope.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

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
struct RecordSkipper : public qi::grammar<Iterator>
{
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
		 * ; has to be escaped by using two ; characters
		 */
		literal %=
			(
				(char_ - eol) >> lit(";;")
				| (char_ - lit(';') - eol)

			)
		;

		p_field %=
			lit(";P")
			>> *literal
		;

		f_field %=
			lit(";F")
			>> *literal
		;

		m_field %=
			lit(";M")
			>> *literal
		;

		l_field %=
			lit(";L")
			>> *literal
		;

		s_field %=
			lit(";S")
			>> *literal
		;

		v_field %=
			lit(";V")
			>> *literal
		;

		g_field %=
			lit(";G")
			>> *literal
		;

		d_field %=
			lit(";D")
			>> *literal
		;


		w_field %=
			lit(";W")
			>> *literal
		;

		e_field %=
			lit(";E")
			>> *literal
		;

		k_field %=
			lit(";K")
			>> *literal
		;

		id_record %=
			lit("ID")
			>> p_field
			>> -lit(";N")
			>> -lit(";E")
		;

		p_record %=
			lit('P')
			>>
			+(
				p_field
				// NOTE is not listed on Wikipedia but present in Warcraft's SLK files
				| f_field
				| m_field
				| e_field
				| s_field
				| l_field
			)
		;

		f_record %=
			lit('F')
			>> +(p_field | d_field | m_field | w_field)
		;

		o_record %=
			lit('O')
			>> +(l_field | s_field | d_field | v_field | k_field | g_field)
		;

		skip_record %=
			id_record
			| p_record
			| f_record
			| o_record
		;

		skip_lines %=
			+(eol >> -skip_record >> &eol)
		;

		skip %=
			skip_lines
			| skip_record
		;

		literal.name("literal");
		p_field.name("p_field");
		f_field.name("f_field");
		m_field.name("m_field");
		d_field.name("d_field");
		l_field.name("l_field");
		s_field.name("s_field");
		v_field.name("v_field");
		g_field.name("g_field");
		w_field.name("w_field");
		e_field.name("e_field");
		k_field.name("k_field");
		id_record.name("id_record");
		p_record.name("p_record");
		f_record.name("f_record");
		skip_record.name("skip_record");
		skip_lines.name("skip_lines");
		skip.name("skip");

		BOOST_SPIRIT_DEBUG_NODES(
			(literal)
			(p_field)
			(f_field)
			(m_field)
			(d_field)
			(l_field)
			(s_field)
			(v_field)
			(g_field)
			(w_field)
			(e_field)
			(k_field)
			(id_record)
			(p_record)
			(f_record)
			(skip_record)
			(skip_lines)
			(skip)
		);
	}

	qi::rule<Iterator> literal;

	qi::rule<Iterator> p_field;
	qi::rule<Iterator> f_field;
	qi::rule<Iterator> m_field;
	qi::rule<Iterator> d_field;
	qi::rule<Iterator> l_field;
	qi::rule<Iterator> s_field;
	qi::rule<Iterator> v_field;
	qi::rule<Iterator> g_field;
	qi::rule<Iterator> w_field;
	qi::rule<Iterator> e_field;
	qi::rule<Iterator> k_field;

	qi::rule<Iterator> id_record;
	qi::rule<Iterator> b_record;
	qi::rule<Iterator> p_record;
	qi::rule<Iterator> f_record;
	qi::rule<Iterator> o_record;
	qi::rule<Iterator> skip_record;
	qi::rule<Iterator> skip_lines;
	qi::rule<Iterator> skip;
};

typedef std::pair<Slk::Table::size_type, Slk::Table::size_type> SlkSize;

struct CellData
{
	CellData() : x(0), y(0)
	{
	}

	Slk::Table::size_type x, y;
	Slk::Cell cell;
	// TODO save optional description (;A)
	Slk::Cell description;
};

void assignX(SlkSize &size, int x)
{
	size.first = x;

	std::cerr << "Assigning X: " << x << std::endl;
}

void assignY(SlkSize &size, int y)
{
	size.second = y;

	std::cerr << "Assigning Y: " << y << std::endl;
}

void resizeTable(Slk::Table &table, const SlkSize &size)
{
	table.resize(boost::extents[size.first][size.second]);
	std::cerr << "Resizing table: " << size.first << "x" << size.second << std::endl;
}

void setCell(Slk::Table &table, const CellData &cell)
{
	const Slk::Table::size_type x = cell.x - 1;
	const Slk::Table::size_type y = cell.y - 1;

	if (x < table.shape()[0] && y < table.shape()[1])
	{
		// if the value is continued append it
		table[x][y] = table[x][y]  + cell.cell;
	}
	else
	{
		std::cerr << boost::format(_("Cell data %1%|%2% with value \"%3%\" and description \"%4%\" is out of range for table with size %5%|%6%."))
			% cell.x
			% cell.y
			% cell.cell
			% cell.description
			% table.shape()[0]
			% table.shape()[1]
			<< std::endl;
	}
}

/**
 * The actual Qi based grammar for SLK files.
 * All SLK files contain records separated by eol characters.
 * There is different types of records from which only need those on which
 * cell values depend on.
 */
template <typename Iterator, typename Skipper = RecordSkipper<Iterator> >
struct SlkGrammar : qi::grammar<Iterator, Skipper>
{
	void prepare(Slk::Table &result)
	{
		this->result = result;
	}

	/*
	 * SLK starts counting at 1 so initialize values with 1.
	 */
	SlkGrammar(Slk::Table &result) : SlkGrammar::base_type(cells, "slk grammar"), result(result), row(1), column(1)
	{
		using qi::eps;
		using qi::int_parser;
		using qi::double_;
		using qi::int_;
		using qi::lit;
		using qi::oct;
		using qi::hex;
		using qi::true_;
		using qi::false_;
		using qi::_val;
		using qi::lexeme;
		using qi::eol;
		using qi::eoi;
		using qi::matches;
		using qi::attr_cast;
		using qi::as_string;
		using qi::as;
		using qi::repeat;
		using qi::on_error;
		using qi::on_success;
		using qi::retry;
		using qi::fail;
		using ascii::char_;
		using ascii::string;
		using namespace qi::labels;


		using phoenix::val;
		using phoenix::at_c;
		using phoenix::push_back;
		using phoenix::ref;
		using phoenix::let;
		using phoenix::bind;
		using phoenix::if_;

		/*
		 * ; has to be escaped by using two ; characters
		 */
		literal %=
			+(
				(char_ - eol) >> &lit(";;")
				| (char_ - lit(';') - eol)

			)
		;

		d_field %=
			lit(";D")
			>> literal
		;

		/*
		 * B records define the actual size of the spreadsheet.
		 * They contain the number of columns and rows.
		 *
		 * NOTE Do not use %= assignment since it overwrites the pair's values in wrong order.
		 */
		b_record =
			lit('B')
			>>
			(
				(
					lit(";Y")
					>> int_[phoenix::bind(&assignY, phoenix::ref(_val), phoenix::ref(_1))]
					>> lit(";X")
					>> int_[phoenix::bind(&assignX, phoenix::ref(_val), phoenix::ref(_1))]
				)
				| (
					lit(";X")
					>> int_[phoenix::bind(&assignX, phoenix::ref(_val), phoenix::ref(_1))]
					>> lit(";Y")
					>> int_[phoenix::bind(&assignY, phoenix::ref(_val), phoenix::ref(_1))]
				)
			)
			// Warcraft III files have D fields
			>> -d_field
		;

		c_record =
			lit('C')[at_c<0>(_val) = ref(column)][at_c<1>(_val) = ref(row)] // use current row by default
			>>
			// the whole position might be skipped in this case we use the last used X and Y
			-(
				// if row value is present (Y) set the current row to that value
				-(
					lit(";Y")
					>> int_[at_c<1>(_val) = _1][ref(row) = _1]
				)
				>> lit(";X")
				>> int_[at_c<0>(_val) = _1]
			)

			>>
			-(
				lit(";E")
				>> literal
			)

			>>
			-(
				(
					lit(";K")
					>> literal[at_c<2>(_val) = _1]
				)
				// in some cases descriptions appear as ;A fields
				| (
					lit(";A")
					>> literal[at_c<3>(_val) = _1]
				)
			)

			// TODO all possible fields!
			// TODO store only the cell value and the index!
		;

		e_record =
			lit('E')
		;

		record =
			b_record[phoenix::bind(&resizeTable, phoenix::ref(result), phoenix::ref(_1))]
			| c_record[phoenix::bind(&setCell, phoenix::ref(result), phoenix::ref(_1))]
		;

		cells =
			eps[ref(column) = 1][ref(row) = 1]
			>> *eol
			>> record % eol
			>> eol >> e_record >> +eol
		;

		literal.name("literal");
		d_field.name("d_field");
		b_record.name("b_record");
		c_record.name("c_record");
		e_record.name("e_record");
		cells.name("cells");

		BOOST_SPIRIT_DEBUG_NODES(
			(literal)
			(d_field)
			(b_record)
			(c_record)
			(e_record)
			(cells)
		);
	}

	qi::rule<Iterator, Slk::Cell(), Skipper> literal;
	qi::rule<Iterator, Skipper> d_field;

	qi::rule<Iterator, SlkSize(), Skipper> b_record;
	qi::rule<Iterator, CellData(), Skipper> c_record;
	qi::rule<Iterator, Skipper> e_record;
	qi::rule<Iterator, Skipper> record;
	qi::rule<Iterator, Skipper> cells;

	Slk::Table result;
	Slk::Table::size_type row;
	Slk::Table::size_type column;
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
	SlkGrammar<Iterator> grammar(table);
	RecordSkipper<Iterator> recordSkipper;

	bool r = boost::spirit::qi::phrase_parse(
		first,
		last,
		grammar,
		recordSkipper
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

BOOST_FUSION_ADAPT_STRUCT(
	wc3lib::map::client::CellData,
	(wc3lib::map::Slk::Table::size_type, x)
	(wc3lib::map::Slk::Table::size_type, y)
	(wc3lib::map::Slk::Cell, cell)
	(wc3lib::map::Slk::Cell, description)
)
