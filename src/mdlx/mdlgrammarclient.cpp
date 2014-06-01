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

#ifndef WC3LIB_MDLX_MDLGRAMMARCLIENT_CPP
#define WC3LIB_MDLX_MDLGRAMMARCLIENT_CPP

#include <vector>

#include "mdlgrammarclient.hpp"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/adt/adapt_adt.hpp>
#include <boost/fusion/include/adapt_adt.hpp>

#include "../platform.hpp"
#include "../i18n.hpp"

namespace wc3lib
{

namespace mdlx
{

namespace client {

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace karma = boost::spirit::karma;
namespace ascii = boost::spirit::ascii;
/**
 * The Unicode namespace can be used for UTF-8 string and comments parsing in JASS.
 */
namespace unicode = boost::spirit::qi::unicode;

/*
 * Doesn't consume eols since value pairs are separated linewise which therefore can be specified easier in the rules
 */
template<typename Iterator>
CommentSkipper<Iterator>::CommentSkipper() : CommentSkipper<Iterator>::base_type(skip, "comments and blanks")
{
	using qi::lit;
	using ascii::char_;
	using ascii::blank;
	using qi::eol;
	using qi::eoi;
	using qi::eps;

	/*
	 * Comments may use UTF-8 characters.
	 */
	comment %=
		lit("//") >> *(unicode::char_ - eol)
	;

	emptyline %=
		+blank >> -comment // blanks only optionally followed by a comment
		| comment // one comment only
	;

	moreemptylines %=
		(eol >> -emptyline >> &eol)
	;

	emptylines %=
		// do not consume the eol of the last empty line because it is the eol between all skipped empty lines and the first one
		+moreemptylines
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

/**
 * @{
 */
template<typename T, typename... Arguments>
std::unique_ptr<T> assignPointer(Arguments... parameters) {
	return std::unique_ptr<T>(new T(parameters...));
}

template<typename T>
typename std::unique_ptr<T>::pointer pointerValue(std::unique_ptr<T> &ptr) {
	return ptr.release();
}
/**
 * @}
 */

template <typename Iterator, typename Skipper >
MdlGrammar<Iterator, Skipper>::MdlGrammar() : MdlGrammar<Iterator, Skipper>::base_type(mdl, "mdl"), result(new Mdlx())
{
	using qi::eps;
	using qi::int_parser;
	using qi::double_;
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

	//using phoenix::construct;
	using phoenix::val;
	using phoenix::at_c;
	using phoenix::push_back;
	using phoenix::ref;
	using phoenix::new_;

	/*
	mdl =
		eps[_val = result.release()]
		-version[at_c<0>(_val) = phoenix::bind(&pointerValueVersion, qi::_r1)]
	;
	*/

	// TODO Boost Spirits needs move semantics support to support unique pointers
	/*
	version =
		lit("Version")[_val = phoenix::bind(&assignPointerVers, result.get())]
		>> lit('{')
			>> lit("FormatVersion")
			>> qi::int_parser<long32>()[at_c<0>(_val) = _1]
		>> lit('}')
	;
	*/


	/*
	 * put all rule names here:
	 */
	mdl.name("mdl");
	version.name("version");

	BOOST_SPIRIT_DEBUG_NODES(
		(mdl)
		(version)
	);
}

template <typename Iterator>
bool parse(Iterator first, Iterator last, Mdlx &mdlx)
{
	MdlGrammar<Iterator> grammar;
	CommentSkipper<Iterator> commentSkipper;
	bool r = boost::spirit::qi::phrase_parse(
			first,
			last,
			grammar,
			commentSkipper,
			mdlx // store result into the passed ast
		);

	if (first != last) // fail if we did not get a full match
	{
		return false;
	}

	return r;
}

template <typename Iterator >
MdlGenerator<Iterator>::MdlGenerator() : MdlGenerator<Iterator>::base_type(mdl, "mdl")
{
	using karma::eps;
	using karma::lit;
	using karma::_val;
	using ascii::string;
	using karma::right_align;
	using karma::eol;

	using phoenix::at_c;

	mdl =
		version
	;

	version %=
		lit("Version")
		<< lit('{')
		<< eol
		<< right_align(8)[
			lit("FormatVersion")
			<< karma::int_generator<long32>()[at_c<0>(_val)]
			<< eol
		]
		<< lit('}')
	;

	model %=
		lit("Model")
		<< string[at_c<0>(_val)] // model name
		<< lit('{')
		<< eol
		<< right_align(8)[
			lit("BlendTime")
			<< karma::int_generator<long32>()[at_c<2>(_val)]
			<< eol
		]
		<< lit('}')
	;

	/*
	 * put all rule names here:
	 */
	mdl.name("mdl");
	version.name("version");
	model.name("model");

	BOOST_SPIRIT_DEBUG_NODES(
		(mdl)
		(version)
		(model)
	);
}

}

}

}

/*
 * All parsed structures have to be defined manually for Fusion.
 * Only add attributes which should be parsed!
 */
BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::client::MdlxType,
	(wc3lib::mdlx::Version*, wc3lib::mdlx::Version*, obj->modelVersion(), obj->setModelVersion(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::client::ModelType,
	(const wc3lib::byte*, const wc3lib::byte*, obj->name(), obj->setName(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj->unknown(), obj->setUnknown(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj->blendTime(), obj->setBlendTime(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::client::VersionType,
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj->modelVersion(), obj->setModelVersion(val))
)

#endif
