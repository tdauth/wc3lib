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

#ifndef WC3LIB_MDLX_MDLGRAMMARCLIENT_HPP
#define WC3LIB_MDLX_MDLGRAMMARCLIENT_HPP

#include "../qi.hpp"
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include <boost/spirit/include/karma.hpp>

#include "mdlx.hpp"
#include "model.hpp"
#include "version.hpp"

namespace wc3lib
{

namespace mdlx
{

/*
 * The client namespace structs usually should be hidden from the user but can be useful for unit tests
 * using the single rules.
 * The templates should be marked as external in unit tests to reduce compile time.
 */
namespace client
{

namespace qi = boost::spirit::qi;
namespace karma = boost::spirit::karma;

/**
 * Doesn't consume eols since value pairs are separated linewise which therefore can be specified easier in the rules.
 * MDL supports single line comments started by "//"
 */
template<typename Iterator>
struct CommentSkipper : public qi::grammar<Iterator> {

	CommentSkipper();

	qi::rule<Iterator> skip;
	qi::rule<Iterator> emptyline;
	qi::rule<Iterator> moreemptylines;
	qi::rule<Iterator> emptylines;
	qi::rule<Iterator> comment;
};

/**
 * We use unique_ptr types which release their pointers on successfull passing
 * to avoid memory leaks whenever a rule fails which has already allocated a value.
 *
 * \todo Unfortunately Boost Spirit does not support unique pointers because of copy semantics and missing move semantics support.
 */
typedef std::unique_ptr<Mdlx> MdlxType;
typedef std::unique_ptr<Model> ModelType;
typedef std::unique_ptr<Version> VersionType;

template <typename Iterator, typename Skipper = CommentSkipper<Iterator> >
struct MdlGrammar : qi::grammar<Iterator, MdlxType(), qi::locals<std::string>, Skipper>
{
	MdlGrammar();

	qi::rule<Iterator, MdlxType(), qi::locals<std::string>, Skipper> mdl;
	qi::rule<Iterator, ModelType(), Skipper> model;
	qi::rule<Iterator, VersionType(), Skipper> version;

	MdlxType result;
};

/**
 * Might throw an boost::spirit::expectation_failure exception!
 */
template <typename Iterator>
bool parse(Iterator first, Iterator last, Mdlx &mdlx);

template <typename Iterator>
struct MdlGenerator : karma::grammar<Iterator, MdlxType()>
{
	MdlGenerator();

	karma::rule<Iterator, MdlxType()> mdl;
	karma::rule<Iterator, ModelType()> model;
	karma::rule<Iterator, VersionType()> version;
};

}

}

}

#endif
