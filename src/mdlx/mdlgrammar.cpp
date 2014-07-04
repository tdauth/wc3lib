/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#include "mdlgrammar.hpp"
#include "mdlgrammarclient.cpp" // we need all actual template implementations

#include "../platform.hpp"
#include "../i18n.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * This function can be called whenever an \ref boost::spirit::qi::expectation_failure occurs to
 * print it in a readable format.
 * It returns a formatted string containing file path, line and column.
 */
template<typename Iterator>
std::string expectationFailure(const boost::spirit::qi::expectation_failure<Iterator> &e) {
	//const classic::file_position_base<std::string>& pos = e.first.get_position();
	//std::stringstream msg;
	//msg
	//<< std::setw(boost::spirit::get_column(e.first)) << " " << _("^- here");

	return boost::str(
		boost::format(_("Parse error: %1%")) // :%3%:\n%4%
		% boost::spirit::get_line(e.first)
		//% boost::spirit::get_column(e.first)
		//% e.first.get_currentline()
		//% msg.str()
	);
}

/**
 * Specialization for classic position iterator type.
 */
template<>
std::string expectationFailure<MdlGrammar::ClassicPositionIteratorType>(const boost::spirit::qi::expectation_failure<MdlGrammar::ClassicPositionIteratorType> &e) {
	const boost::spirit::classic::file_position_base<std::string>& pos = e.first.get_position();
	std::stringstream msg;
	msg
	<< std::setw(pos.column) << " " << _("^- here");

	return boost::str(
		boost::format(_("Parse error at file %1%:%2%:%3%:\n%4%"))
		% pos.file
		% pos.line
		% pos.column
		% e.first.get_currentline()
		% msg.str()
	);
}

MdlGrammar::MdlGrammar()
{
}

bool MdlGrammar::parse(MdlGrammar::InputStream& istream, Mdlx* &result)
{
	return this->parse(IteratorType(istream), IteratorType(), result);
}

bool MdlGrammar::parse(IteratorType first, IteratorType last, Mdlx* &result)
{
	ForwardIteratorType forwardFirst = boost::spirit::make_default_multi_pass(first);
	ForwardIteratorType forwardLast = boost::spirit::make_default_multi_pass(last); // TODO has to be created? Do we need this iterator to be passed?

	// used for backtracking and more detailed error output
	PositionIteratorType position_begin(forwardFirst);
	PositionIteratorType position_end;
	bool failed = false;

	try {
		failed = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			this->grammar,
			this->skipper,
			result
		);
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> &e) {
		throw Exception(expectationFailure(e));
	}

	if (position_begin != position_end) // fail if we did not get a full match
	{
		return false;
	}

	return failed;
}

}

}
