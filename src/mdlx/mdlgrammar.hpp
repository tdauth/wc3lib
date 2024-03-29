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

#ifndef WC3LIB_MDLX_MDLGRAMMAR_HPP
#define WC3LIB_MDLX_MDLGRAMMAR_HPP

/**
 * \defgroup mdlsupport MDL support
 *
 * wc3lib supports parsing MDL files through an EBNF grammar.
 * The class \ref wc3lib::mdlx::MdlGrammar provides parsing methods.
 */

#include "../spirit.hpp"

#include <iterator>

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include "../platform.hpp"
#include "../exception.hpp"
#include "mdlgrammarclient.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * \brief Parser for MDL files based on Boost Spirit.
 *
 * This class supports parsing an MDL file from an input stream.
 *
 * \ingroup parsers
 */
class MdlGrammar
{
	public:
		typedef std::basic_istream<byte> InputStream;
		typedef std::istreambuf_iterator<byte> IteratorType;
		typedef boost::spirit::multi_pass<IteratorType> ForwardIteratorType;

		/**
		 * Declare iterator types for better error results.
		 * These Position iterators support tracking the current position of the parser.
		 * When an error occurs they their information can be used for better reports.
		 *
		 * Usually you would create objects of theses types from the multi pass iterator \ref ForwardIteratorType and pass
		 * them to the \ref parse() element functions.
		 *
		 * Both types are instanciated in "grammar.cpp" and can be declared as external templates to save compile time.
		 */
		typedef boost::spirit::classic::position_iterator2<MdlGrammar::ForwardIteratorType> ClassicPositionIteratorType;
		typedef boost::spirit::line_pos_iterator<MdlGrammar::ForwardIteratorType> PositionIteratorType;


		typedef client::CommentSkipper<PositionIteratorType> Skipper;
		typedef client::MdlGrammar<PositionIteratorType, Skipper> Grammar;

		MdlGrammar();

		/**
		 * Parses the MDL input from input stream \p istream and stores the result into \p result.
		 * If parsing succeeds it returns true. Otherwise it returns false.
		 */
		bool parse(InputStream &istream, Mdlx &result);
		bool parse(IteratorType first, IteratorType last, Mdlx &result);

	private:
		/*
		 * Internal grammars for MDL grammar and its skipper.
		 * We do not want to create a new grammar instance each time we parse something.
		 * Therefore we use these attributes for all parsing operations.
		 *
		 * Use static attributes to avoid multiple allocations of the same grammar and skipper for multiple instances
		 * of "MdlGrammar".
		 */
		static const Grammar grammar;
		static const Skipper skipper;
};

}

}

#endif
