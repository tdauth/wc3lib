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
#include <boost/fusion/adapted/array.hpp>
#include <boost/fusion/include/array.hpp>
#include <boost/fusion/adapted/adt/adapt_adt.hpp>
#include <boost/fusion/include/adapt_adt.hpp>

#include "../platform.hpp"
#include "../i18n.hpp"

// prevent using them as container
// http://stackoverflow.com/questions/27114891/boost-spirit-qi-storing-into-stdvector-using-repeat-leads-to-ambiguous-class-t?noredirect=1#comment42823629_27114891
namespace boost
{

namespace spirit
{

namespace traits
{

template <>
struct is_container<wc3lib::mdlx::client::VertexInteger1d> : mpl::false_
{
};


template <>
struct is_container<wc3lib::mdlx::client::VertexReal1d> : mpl::false_
{
};

template <>
struct is_container<wc3lib::mdlx::client::VertexReal2d> : mpl::false_
{
};

template <>
struct is_container<wc3lib::mdlx::client::VertexReal3d> : mpl::false_
{
};

template <>
struct is_container<wc3lib::mdlx::client::VertexReal4d> : mpl::false_
{
};

}

}

}

namespace wc3lib
{

namespace mdlx
{

namespace client
{

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
		| eol
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
std::unique_ptr<T> assignPointer(Arguments... parameters)
{
	return std::unique_ptr<T>(new T(parameters...));
}

template<typename T>
typename std::unique_ptr<T>::pointer pointerValue(std::unique_ptr<T> &ptr)
{
	return ptr.release();
}
// resetPointer

template<typename T>
void resetPointer(std::unique_ptr<T> &target, std::unique_ptr<T> &source)
{
	target.reset(source.release());
}

template<typename T, typename... Arguments>
void newPointer(std::unique_ptr<T> &target, Arguments... parameters)
{
	target.reset(new T(parameters...));
}
/**
 * @}
 */

inline void applyBounds(Bounds &bounds, Bounds &other)
{
	bounds.setBoundsRadius(other.boundsRadius());
	bounds.setMinimumExtent(other.minimumExtent());
	bounds.setMaximumExtent(other.maximumExtent());
}

template <typename Iterator, typename Skipper >
MdlGrammar<Iterator, Skipper>::MdlGrammar() : MdlGrammar<Iterator, Skipper>::base_type(mdl, "mdl")
{
	using qi::eps;
	using qi::int_parser;
	using qi::real_parser;
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

	integer_literal %=
		lexeme[
			qi::int_parser<long32>()
		]
	;

	byte_integer_literal %=
		lexeme[
			qi::int_parser<uint8>()
		]
	;

	short_integer_literal %=
		lexeme[
			qi::int_parser<short16>()
		]
	;

	real_literal %=
		lexeme[
			qi::real_parser<float32>()
		]
	;

	string_literal %=
		lexeme[
			lit("\"")
			>>
			*(
				(
					unicode::char_ - unicode::char_('\"') - unicode::char_('\\')
				)
				| (
					lit('\\')[push_back(_val, '\\')] >> unicode::char_
				)
			)
			>> lit("\"")
		]
	;

	vertex_integer_1d %=
		integer_literal[at_c<0>(_val) = _1]
	;

	vertex_real_1d %=
		real_literal[at_c<0>(_val) = _1]
	;

	vertex_real_2d =
		lit('{')
		>> real_literal[at_c<0>(_val) = _1]
		>> lit(',')
		>> real_literal[at_c<1>(_val) = _1]
		>> lit('}')
	;

	vertex_real_3d =
		lit('{')
		>> real_literal[at_c<0>(_val) = _1]
		>> lit(',')
		>> real_literal[at_c<1>(_val) = _1]
		>> lit(',')
		>> real_literal[at_c<2>(_val) = _1]
		>> lit('}')
	;

	vertex_real_4d =
		lit('{')
		>> real_literal[at_c<0>(_val) = _1]
		>> lit(',')
		>> real_literal[at_c<1>(_val) = _1]
		>> lit(',')
		>> real_literal[at_c<2>(_val) = _1]
		>> lit(',')
		>> real_literal[at_c<3>(_val) = _1]
		>> lit('}')
	;

	line_type %=
		(
			lit("DontInterp")[_val = LineType::DontInterpolate]
			| lit("Linear")[_val = LineType::Linear]
			| lit("Hermite")[_val = LineType::Hermite]
			| lit("Bezier")[_val = LineType::Bezier]
		)
		>> lit(',')
	;

	// GlobalSeqId is only shown if its value is not 0xFFFFFFFF;
	global_sequence_id %=
		eps[_val = 0xFFFFFFFF]
		>> -(
			lit("GlobalSeqId")
			>> integer_literal[_val = _1]
			>> lit(',')
		)
	;

	// And InTan and OutTan values are only listed if the LineType is Hermite or Bezier.
	animated_1d_float_value =
		integer_literal[at_c<0>(_val) = _1]
		>> lit(':')
		>> vertex_real_1d[at_c<1>(_val) = _1]
		>> lit(',')
		>> -(
			lit("InTan")
			>> vertex_real_1d[at_c<2>(_val) = _1]
			>> lit(',')
			>> lit("OutTan")
			>> vertex_real_1d[at_c<3>(_val) = _1]
			>> lit(',')
		)
	;

	// The four values listed above GlobalSeqId are the LineTypes for the animation;
	// GlobalSeqId is only shown if its value is not 0xFFFFFFFF;
	animated_1d_float_values =
		integer_literal[_a = _1]
		>> lit('{')
		>> line_type[at_c<0>(_val) = _1]
		>> global_sequence_id[at_c<1>(_val) = _1]
		>> repeat(_a)[
			animated_1d_float_value
		][at_c<2>(_val) = _1]
		>> lit('}')
	;

	animated_3d_float_value =
		integer_literal[at_c<0>(_val) = _1]
		>> lit(':')
		>> vertex_real_3d[at_c<1>(_val) = _1]
		>> lit(',')
		>> -(
			lit("InTan")
			>> vertex_real_3d[at_c<2>(_val) = _1]
			>> lit(',')
			>> lit("OutTan")
			>> vertex_real_3d[at_c<3>(_val) = _1]
			>> lit(',')
		)
	;

	animated_3d_float_values =
		integer_literal[_a = _1]
		>> lit('{')
		>> line_type[at_c<0>(_val) = _1]
		>> global_sequence_id[at_c<1>(_val) = _1]
		>> repeat(_a)[
			animated_3d_float_value
		][at_c<2>(_val) = _1]
		>> lit('}')
	;

	alpha %=
		lit("static")
		>> lit("Alpha")
		>> real_literal
		>> lit(',')
	;

	animated_alphas %=
		lit("Alpha")
		>> animated_1d_float_values
	;

	visibility %=
		lit("static")
		>> lit("Visibility")
		>> real_literal
		>> lit(',')
	;

	animated_visibilities %=
		lit("Visibility")
		>> animated_1d_float_values
	;

	intensity %=
		lit("static")
		>> lit("Intensity")
		>> real_literal
		>> lit(',')
	;

	animated_intensities %=
		lit("Intensity")
		>> animated_1d_float_values
	;

	color %=
		lit("static")
		>> lit("Color")
		>> vertex_real_3d
		>> lit(',')
	;

	animated_colors %=
		lit("Color")
		>> animated_3d_float_values
	;

	amb_intensity %=
		lit("static")
		>> lit("AmbIntensity")
		>> real_literal
		>> lit(',')
	;

	animated_amb_intensities %=
		lit("AmbIntensity")
		>> animated_1d_float_values
	;

	amb_color %=
		lit("static")
		>> lit("AmbColor")
		>> vertex_real_3d
		>> lit(',')
	;

	animated_amb_colors %=
		lit("AmbColor")
		>> animated_3d_float_values
	;

	animated_translation =
		integer_literal[at_c<0>(_val) = _1]
		>> lit(':')
		>> vertex_real_3d[at_c<1>(_val) = _1]
		>> lit(',')
		>> -(
			lit("InTan")
			>> vertex_real_3d[at_c<2>(_val) = _1]
			>> lit(',')
			>> lit("OutTan")
			>> vertex_real_3d[at_c<3>(_val) = _1]
			>> lit(',')
		)
	;

	animated_translations =
		lit("Translation")
		>> integer_literal[_a = _1]
		>> lit('{')
		>> line_type[at_c<0>(_val) = _1]
		>> global_sequence_id[at_c<1>(_val) = _1]
		>> repeat(_a)[
			animated_translation
		][at_c<2>(_val) = _1]
		>> lit('}')
	;

	animated_rotation =
		integer_literal[at_c<0>(_val) = _1]
		>> lit(':')
		>> vertex_real_4d[at_c<1>(_val) = _1]
		>> lit(',')
		>> -(
			lit("InTan")
			>> vertex_real_4d[at_c<2>(_val) = _1]
			>> lit(',')
			>> lit("OutTan")
			>> vertex_real_4d[at_c<3>(_val) = _1]
			>> lit(',')
		)
	;

	animated_rotations =
		lit("Rotation")
		>> integer_literal[_a = _1]
		>> lit('{')
		>> line_type[at_c<0>(_val) = _1]
		>> global_sequence_id[at_c<1>(_val) = _1]
		>> repeat(_a)[
			animated_rotation
		][at_c<2>(_val) = _1]
		>> lit('}')
	;

	animated_scaling =
		animated_translation
	;

	animated_scalings =
		lit("Scaling")
		>> integer_literal[_a = _1]
		>> lit('{')
		>> line_type[at_c<0>(_val) = _1]
		>> global_sequence_id[at_c<1>(_val) = _1]
		>> repeat(_a)[
			animated_scaling
		][at_c<2>(_val) = _1]
		>> lit('}')
	;

	// TODO Boost Spirits needs move semantics support to support unique pointers
	version %=
		lit("Version")
		>> lit('{')
			>> lit("FormatVersion")
			>> integer_literal
			>> lit(',')
		>> lit('}')
	;

	model =
		lit("Model")
		>> string_literal[at_c<0>(_val) = _1]
		>> lit('{')
			>> -(
				lit("NumGeosets")
				>> integer_literal[_r1 = _1]
				>> lit(',')
			)
			>> -(
				lit("NumGeosetAnims")
				>> integer_literal[_r2 = _1]
				>> lit(',')
			)
			>> lit("BlendTime") >> integer_literal[at_c<1>(_val) = _1]
			>> lit(',')
			>> bounds[at_c<2>(_val) = _1]
		>> lit('}')
	;

	// NonLooping is a flag.
	// MoveSpeed, Rarity, MinimumExtent, MaximumExtent and BoundsRadius only appear when their values are not 0.0.
	// Maximum length of name is 80 characters (0x50 bytes)
	sequence =
		lit("Anim")
		>> string_literal[at_c<0>(_val) = _1]
		>> lit('{')
			>> lit("Interval")
			>> lit('{')
				>> integer_literal[at_c<1>(_val) = _1]
				>> lit(',')
				>> integer_literal[at_c<2>(_val) = _1]
			>> lit('}')
			>> lit(',')
			>> -(
				lit("NonLooping")
				>> lit(',')
				)[at_c<4>(_val) = 1]
			>> -(
				lit("MoveSpeed")
				>> real_literal[at_c<3>(_val) = 1]
				>> lit(',')
			)
			>> -(
				lit("Rarity")
				>> real_literal[at_c<5>(_val) = 1]
				>> lit(',')
			)
			>> bounds[bind(applyBounds, _val, _1)] // TODO set bounds
		>> lit('}')
	;

	sequences =
		lit("Sequences")
		>> integer_literal[_a = _1][_r1 = _1]
		>> lit('{')
		>> repeat(_a)[
			sequence[phoenix::push_back(_val, _1)]
		]
		>> lit('}')
	;

	global_sequences %=
		lit("GlobalSequences")
		>> integer_literal[_a = _1]
		>> lit('{')
		>> repeat(_a)[
			lit("Duration")
			>> integer_literal[phoenix::push_back(_val, _1)]
			>> lit(',')
		]
		>> lit('}')
	;

	replaceable_id %=
		lit("0")[_val = ReplaceableId::None]
		| lit("1")[_val = ReplaceableId::TeamColor]
		| lit("2")[_val = ReplaceableId::TeamGlow]
		| lit("11")[_val = ReplaceableId::Cliff]
		| lit("31")[_val = ReplaceableId::LordaeronTree]
		| lit("32")[_val = ReplaceableId::AshenvaleTree]
		| lit("33")[_val = ReplaceableId::BarrensTree]
		| lit("34")[_val = ReplaceableId::NorthrendTree]
		| lit("35")[_val = ReplaceableId::MushroomTree]
	;

	wrapping_both %=
		lit("WrapWidth")[_val = mdlx::Texture::Wrapping::Both]
		>> lit(',')
		>> lit("WrapHeight")
		>> lit(',')
	;

	wrapping_one %=
		(
			lit("WrapWidth")
			>> lit(',')
		)[_val = mdlx::Texture::Wrapping::WrapWidth]
		| (
			lit("WrapHeight")
			>> lit(',')
		)[_val = mdlx::Texture::Wrapping::WrapHeight]
	;

	wrapping %=
		// both flags
		wrapping_both
		// one flag
		| wrapping_one
	;

	// Maximum length of path is 256 characters (0x100 bytes)
	// ReplaceableId only appears when its value is not 0
	// WrapWidth and WrapHeight are flags with values 1 and 2 respectively such that if the wrapping flag is 3, both appear. If it is 0, neither.
	texture =
		lit("Bitmap")
		>> lit('{')
		>> lit("Image")
		>> string_literal[at_c<1>(_val) = _1]
		>> lit(',')
		>> -(
			lit("ReplaceableId")
			>> replaceable_id[at_c<0>(_val) = _1]
			>> lit(',')
		)
		>> -wrapping[at_c<2>(_val) = _1]
		>> lit('}')
	;

	textures =
		lit("Textures")
		>> integer_literal[_a = _1]
		>> lit('{')
		>> repeat(_a)[
			texture[phoenix::push_back(_val, _1)]
		]
		>> lit('}')
	;

	filter_mode %=
		lit("None")[_val = Layer::FilterMode::None]
		| lit("Transparent")[_val = Layer::FilterMode::Transparent]
		| lit("Blend")[_val = Layer::FilterMode::Blend]
		| lit("Additive")[_val = Layer::FilterMode::Additive]
		| lit("AddAlpha")[_val = Layer::FilterMode::AddAlpha]
		| lit("Modulate")[_val = Layer::FilterMode::Modulate]
		| lit("Modulate2x")[_val = Layer::FilterMode::Modulate2x]
	;

	texture_id %=
		lit("static")
		>> lit("TextureID")
		>> integer_literal
		>> lit(',')
	;

	animated_texture_id =
		integer_literal[at_c<0>(_val) = _1]
		>> lit(':')
		>> vertex_integer_1d[at_c<1>(_val) = _1]
		>> lit(',')
		>> -(
			lit("InTan")
			>> vertex_integer_1d[at_c<2>(_val) = _1]
			>> lit(',')
			>> lit("OutTan")
			>> vertex_integer_1d[at_c<3>(_val) = _1]
			>> lit(',')
		)
	;

	animated_texture_ids =
		lit("TextureID")
		>> integer_literal[_a = _1]
		>> lit('{')
		>> line_type[at_c<0>(_val) = _1]
		>> global_sequence_id[at_c<1>(_val) = _1]
		>> repeat(_a)[
			animated_texture_id
		][at_c<2>(_val) = _1]
		>> lit('}')
	;

	// Unshaded, SphereEnvMap, TwoSided, Unfogged, NoDepthTest and NoDepthSet are flags.
	// TVertexAnimId only appear when its value is not 0xFFFFFFFF.
	// If CoordId for any of the Layers in a Material is nonzero, CoordId appears
	//  in all Layers else not at all.
	layer =
		lit("Layer")
		>> lit('{')
		>> lit("FilterMode")
		>> filter_mode[at_c<0>(_val) = _1]
		>> lit(',')
		>> -(
			lit("Unshaded")
			>> lit(',')
		)
		>> -(
			lit("SphereEnvMap")
			>> lit(',')
		)
		>> -(
			lit("TwoSided")
			>> lit(',')
		)
		>> -(
			lit("Unfogged")
			>> lit(',')
		)
		>> -(
			lit("NoDepthTest")
			>> lit(',')
		)
		>> -(
			lit("NoDepthSet")
			>> lit(',')
		)
		>> -(
			texture_id[at_c<2>(_val) = _1]
			| animated_texture_ids[at_c<7>(_val) = _1]
		)
		>> -(
			lit("TVertexAnimId")
			>> integer_literal[at_c<3>(_val) = _1]
			>> lit(',')
		)
		>> -(
			lit("CoordId")
			>> integer_literal[at_c<4>(_val) = _1]
			>> lit(',')
		)
		>> -(
			alpha[at_c<5>(_val) = _1]
			| animated_alphas[at_c<6>(_val) = _1]
		)
		>> lit('}')
	;

	layers %=
		*layer
	;

	material =
		lit("Material")
		>> lit('{')
		// TODO set render mode
		>> -(
			lit("ConstantColor")
			>> lit(',')
		)
		>> -(
			lit("SortPrimsFarZ")
			>> lit(',')
		)
		>> -(
			lit("FullResolution")
			>> lit(',')
		)
		>> -(
			lit("PriorityPlane")
			>> integer_literal[at_c<0>(_val) = _1]
			>> lit(',')
		)
		>> layers[at_c<2>(_val) = _1]
		>> lit('}')
	;

	materials =
		lit("Materials")
		>> integer_literal[_a = _1]
		>> lit('{')
		>> repeat(_a)[
			material
		][_val = _1]
		>> lit('}')
	;

	// Translation, Rotation and Scaling are all optional.
	// InTan and OutTan only appear when Hermite or Bezier.
	// GlobalSeqId only appears when its value is not 0xFFFFFFFF.
	texture_animation =
		lit("TVertexAnim")
		>> lit('{')
		>> -animated_translations[at_c<0>(_val) = _1]
		>> -animated_rotations[at_c<1>(_val) = _1]
		>> -animated_scalings[at_c<2>(_val) = _1]
		>> lit('}')
	;

	texture_animations =
		lit("TextureAnims")
		>> integer_literal[_a = _1]
		>> lit('{')
		>> repeat(_a)[
			texture_animation
		][_val = _1]
		>> lit('}')
	;

	vertices %=
		lit("Vertices")
		>> qi::omit[
			integer_literal[_a = _1]
		]
		>> lit('{')
		>> repeat(_a)[
			vertex_real_3d
			>> lit(',')
		]//[_val = _1]
		>> lit('}')
	;

	normals =
		lit("Normals")
		>> integer_literal[_a = _1]
		>> lit('{')
		>> repeat(_a)[
			vertex_real_3d
			>> lit(',')
		][_val = _1]
		>> lit('}')
	;

	texture_vertices =
		lit("TVertices")
		>> integer_literal[_a = _1]
		>> lit('{')
		>> repeat(_a)[
			vertex_real_2d
			>> lit(',')
		][_val = _1]
		>> lit('}')
	;

	group_vertices =
		lit("VertexGroup")
		>> lit('{')
		>> (+(
			byte_integer_literal
			>> lit(',')
		))[_val = _1]
		>> lit('}')
	;

	// TODO check number of values by passing them as argument.
	faces_group =
		lit("Triangles")[at_c<0>(_val) = Faces::Type::Triangles]
		>> lit('{')
		>> lit('{')
		>> (
			+(
				short_integer_literal
				>> (
					lit(',')
					| &lit('}')
				)
			)
		)[at_c<1>(_val) = _1]
		>> lit('}')
		>> lit(',')
		>> lit('}')
	;

	faces =
		lit("Faces")
		>> integer_literal[_a = _1]
		>> integer_literal[_b = _1]
		>> lit('{')
		>> repeat(_a)[
			faces_group
		][_val = _1]
		>> lit('}')
	;

	matrix =
		lit("Matrices")
		>> lit('{')
		// TODO is it possible to calcualte this number. Probably not!
		>> (
			+(
				integer_literal
				>> (
					lit(',')
					| &lit('}')
				)
			)
		)[at_c<0>(_val) = _1]
		>> lit('}')
	;

	groups =
		lit("Groups")
		>> integer_literal[_a = _1]
		>> integer_literal[_b = _1]
		>> lit('{')
		>> repeat(_a)[
			matrix[push_back(_val, _1)]
			>> lit(',')
		]//[_val = _1]
		>> lit('}')
	;

	geoset =
		lit("Geoset")
		>> lit('{')
		>> vertices[at_c<0>(_val) = _1]
		// Normals may not appear if it has 0 entries.
		>> -normals[at_c<1>(_val) = _1]
		>> texture_vertices[at_c<10>(_val) = _1]
		>> group_vertices[at_c<3>(_val) = _1]
		>> faces[at_c<2>(_val) = _1]
		>> groups
		>> bounds
		// There are the same number of Anim chunks as their are Sequences in the model.
		>> -repeat(_r1)[
			lit("Anim")
			>> lit('{')
			>> bounds
			>> lit('}')
		][at_c<8>(_val) = _1] // seems to be optional although different specification. For example "Orc_Exp.mdl" which has been converted with MdlxConv Version Mar  3 2007 has no anim chunks in one Geoset.
		>> lit("MaterialID")
		>> integer_literal[at_c<5>(_val) = _1]
		>> lit(',')
		>> lit("SelectionGroup")
		>> integer_literal[at_c<6>(_val) = _1]
		>> lit(',')
		// Unselectable is a flag which only appears if its value is 4.
		>> -(
			lit("Unselectable")[at_c<7>(_val) = Geoset::Selectable::None]
			>> lit(',')
		)
		>> lit('}')
	;

	geoset_animation =
		lit("GeosetAnim")
		>> lit('{')
		// DropShadow is a flag which only appears when its value is 1 or 3 and
		>> -lit("DropShadow")[at_c<1>(_val) = GeosetAnimation::ColorAnimation::DropShadow]
		>> (
			alpha[at_c<0>(_val) = _1]
			| animated_alphas[at_c<4>(_val) = _1]
		)
		// Color is only shown when this flag is greater than 1.
		>> -(
			color[at_c<2>(_val) = _1]
			| animated_colors[at_c<5>(_val) = _1]
		)
		>> lit("GeosetId")
		>> integer_literal[at_c<3>(_val) = _1]
		>> lit(',')
		>> lit('}')
	;

	object_id %=
		lit("ObjectId")
		>> integer_literal
		>> lit(',')
	;

	parent %=
		lit("Parent")
		>> integer_literal
		>> lit(',')
	;

	// Everything from BillboardLockZ to DontInherit { ... } is a flag.
	// It may be that only one value may be in a DontInherit block at a time.
	node_type %=
		-(lit("BillboardedLockZ")[_val = _val | Node::Type::BillboardedLockZ] >> lit(','))
		>> -(lit("BillboardedLockY")[_val = _val | Node::Type::BillboardedLockY] >> lit(','))
		>> -(lit("BillboardedLockX")[_val = _val | Node::Type::BillboardedLockX] >> lit(','))
		>> -(lit("Billboarded")[_val = _val | Node::Type::Billboarded] >> lit(','))
		>> -(lit("CameraAnchored")[_val = _val | Node::Type::CameraAnchored] >> lit(','))
		>> -(
			lit("DontInherit")
			>> lit('{')
			>> (
				-lit("Rotation")[_val = _val | Node::Type::DontInheritRotation]
				-lit("Translation")[_val = _val | Node::Type::DontInheritTranslation]
				-lit("Scaling")[_val = _val | Node::Type::DontInheritScaling]
			)
			>> lit('}')
			>> lit(',')
		)
	;

	bone =
		lit("Bone")
		>> string_literal[at_c<0>(_val) = _1]
		>> lit('{')
		>> -object_id[at_c<1>(_val) = _1] // ObjectId may be omitted if the object is the only one in the MDL.
		>> -parent[at_c<2>(_val) = _1] // Parent only appears when its value is not 0xFFFFFFFF.
		>> node_type[at_c<3>(_val) = _1]
		>> lit("GeosetId")
		>> (
			integer_literal[at_c<4>(_val) = _1]
			| lit("Multiple")[at_c<4>(_val) = -1]
		)
		>> lit(',')
		>> lit("GeosetAnimId")
		>> (
			integer_literal[at_c<5>(_val) = _1]
			| lit("None")[at_c<5>(_val) = -1]
		)
		>> lit(',')
		>> -animated_translations[at_c<6>(_val) = _1]
		>> -animated_rotations[at_c<7>(_val) = _1]
		>> -animated_scalings[at_c<8>(_val) = _1]
		>> -animated_visibilities[at_c<9>(_val) = _1]
		>> lit('}')
	;

	light_type %=
		lit("Omnidirectional")[_val == Light::LightType::Omnidirectional]
		| lit("Directional")[_val == Light::LightType::Directional]
		| lit("Ambient")[_val == Light::LightType::Ambient]
	;

	light =
		lit("Light")
		>> string_literal[at_c<0>(_val) = _1]
		>> lit('{')
		>> -object_id[at_c<1>(_val) = _1] // ObjectId may be omitted if the object is the only one in the MDL.
		>> -parent[at_c<2>(_val) = _1] // Parent only appears when its value is not 0xFFFFFFFF.
		>> node_type[at_c<3>(_val) = _1]
		>> light_type[at_c<4>(_val) = _1]
		>> lit(',')
		>> lit("static")
		>> lit("AttenuationStart")
		>> real_literal[at_c<5>(_val) = _1]
		>> lit(',')
		>> lit("static")
		>> lit("AttenuationEnd")
		>> real_literal[at_c<6>(_val) = _1]
		>> lit(',')
		>> -(
			intensity[at_c<7>(_val) = _1]
			| animated_intensities[at_c<8>(_val) = _1]
		)
		>> -(
			color[at_c<9>(_val) = _1]
			| animated_colors[at_c<10>(_val) = _1]
		)
		>> -(
			amb_intensity[at_c<11>(_val) = _1]
			| animated_amb_intensities[at_c<12>(_val) = _1]
		)
		/*
		 * TODO this particular block leads to syntax error
		>> -(
			amb_color[at_c<13>(_val) = _1]
			| animated_amb_colors[at_c<14>(_val) = _1]
		)
		*/
		//>> -animated_visibilities[at_c<15>(_val) = _1]
		//>> -animated_translations[at_c<16>(_val) = _1]
		//>> -animated_rotations[at_c<17>(_val) = _1]
		//>> -animated_scalings[at_c<18>(_val) = _1]
		>> lit('}')
	;

	mdl =
		/*
		 * Initialize counts with 0
		 */
		eps[_a = 0][_b = 0][_c = 0]
		>> -version[at_c<0>(_val) = _1]
		>> -model(_a, _b)[at_c<1>(_val) = _1]
		>> -sequences(_c)[at_c<2>(_val) = _1]
		>> -global_sequences[at_c<3>(_val) = _1]
		>> -textures[at_c<4>(_val) = _1]
		>> -materials[at_c<5>(_val) = _1]
		>> -texture_animations[at_c<6>(_val) = _1]
		// Since the number is not always present just try it.
		>> (*geoset(_c))[at_c<7>(_val) = _1]
		// Since the number is not always present just try it.
		>> (*geoset_animation)[at_c<8>(_val) = _1]
		>> (*bone)[at_c<9>(_val) = _1]
		>> (*light)[at_c<10>(_val) = _1]
	;

	// MinimumExtent, MaximumExtent, and BoundsRadius are left out if their
	// values are 0.0.
	bounds =
		-(
			lit("MinimumExtent")
			>> vertex_real_3d[at_c<0>(_val) = _1]
			>> lit(',')
		)
		>>
		-(
			lit("MaximumExtent")
			>> vertex_real_3d[at_c<1>(_val) = _1]
			>> lit(',')
		)
		>>
		-(
			lit("BoundsRadius")
			>> real_literal[at_c<2>(_val) = _1]
			>> lit(',')
		)
	;

	/*
	 * put all rule names here:
	 */
	integer_literal.name("integer_literal");
	byte_integer_literal.name("byte_literal");
	short_integer_literal.name("short_literal");
	real_literal.name("real_literal");
	string_literal.name("string_literal");

	vertex_integer_1d.name("vertex_integer_1d");
	vertex_real_1d.name("vertex_integer_1d");
	vertex_real_2d.name("vertex_real_2d");
	vertex_real_3d.name("vertex_real_3d");

	line_type.name("line_type");
	global_sequence_id.name("global_sequence_id");

	animated_alphas.name("animated_alphas");
	mdl.name("mdl");
	model.name("model");
	version.name("version");
	geoset_animation.name("geoset_animation");
	sequence.name("sequence");
	sequences.name("sequences");
	global_sequences.name("global_sequences");
	replaceable_id.name("replaceable_id");
	wrapping_both.name("wrapping_both");
	wrapping_one.name("wrapping_one");
	wrapping.name("wrapping");
	texture.name("texture");
	textures.name("textures");
	filter_mode.name("filter_mode");
	texture_id.name("texture_id");
	animated_texture_id.name("animated_texture_id");
	animated_texture_ids.name("animated_texture_ids");
	layer.name("layer");
	layers.name("layers");
	material.name("material");
	materials.name("materials");
	texture_animation.name("texture_animation");
	texture_animations.name("texture_animations");

	vertices.name("vertices");
	normals.name("normals");
	texture_vertices.name("texture_vertices");
	group_vertices.name("group_vertices");
	faces_group.name("faces_group");
	faces.name("faces");
	matrix.name("matrix");
	groups.name("groups");
	geoset.name("geoset");
	bone.name("bone");
	light.name("light");

	bounds.name("bounds");

	animated_1d_float_value.name("animated_1d_float_value");
	animated_1d_float_values.name("animated_1d_float_values");
	animated_3d_float_value.name("animated_3d_float_value");
	animated_3d_float_values.name("animated_3d_float_values");
	alpha.name("alpha");
	animated_alphas.name("animated_alphas");
	visibility.name("visibility");
	animated_visibilities.name("animated_visibilities");
	intensity.name("intensity");
	animated_intensities.name("animated_intensities");
	color.name("color");
	animated_colors.name("animated_colors");
	amb_intensity.name("amb_intensity");
	animated_amb_intensities.name("animated_amb_intensities");
	amb_color.name("amb_color");
	animated_amb_colors.name("animated_amb_colors");
	animated_translation.name("animated_translation");
	animated_translations.name("animated_translations");
	animated_rotation.name("animated_rotation");
	animated_rotations.name("animated_rotations");
	animated_scaling.name("animated_scaling");
	animated_scalings.name("animated_scalings");

	BOOST_SPIRIT_DEBUG_NODES(
		(integer_literal)
		(byte_integer_literal)
		(short_integer_literal)
		(real_literal)
		(string_literal)
		(vertex_integer_1d)
		(vertex_real_1d)
		(vertex_real_2d)
		(vertex_real_3d)
		(line_type)
		(global_sequence_id)
		(animated_alphas)
		(mdl)
		(model)
		(version)
		(geoset_animation)
		(sequence)
		(sequences)
		(global_sequences)
		(replaceable_id)
		(wrapping_both)
		(wrapping_one)
		(wrapping)
		(texture)
		(textures)
		(filter_mode)
		(texture_id)
		(animated_texture_id)
		(animated_texture_ids)
		(layer)
		(layers)
		(material)
		(materials)
		(texture_animation)
		(texture_animations)


		(vertices)
		(normals)
		(texture_vertices)
		(group_vertices)
		(faces_group)
		(faces)
		(matrix)
		(groups)
		(geoset)
		(bone)
		(light)

		(bounds)
		(animated_1d_float_value)
		(animated_1d_float_values)
		(animated_3d_float_value)
		(animated_3d_float_values)
		(alpha)
		(animated_alphas)
		(visibility)
		(animated_visibilities)
		(intensity)
		(animated_intensities)
		(color)
		(animated_colors)
		(amb_intensity)
		(animated_amb_intensities)
		(amb_color)
		(animated_amb_colors)
		(animated_translation)
		(animated_translations)
		(animated_rotation)
		(animated_rotations)
		(animated_scaling)
		(animated_scalings)
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
	using ascii::char_;
	using ascii::string;
	using karma::right_align;
	using karma::eol;
	using namespace boost::spirit;

	using phoenix::at_c;
	using phoenix::val;
	using phoenix::push_back;
	using phoenix::ref;
	using phoenix::new_;

	/*
	integer_literal %=
		karma::int_generator<long32>()
	;

	real_literal %=
		karma::real_generator<float32>()
	;

	string_literal %=
		lit("\"")
		<<
		*(
			unicode::char_ - char_('\"') - char_('\\')
			| lit('\\')[push_back(_val, '\\')] << unicode::char_
		)
		<< lit("\"")
	;

	vertexData =
		real_literal[at_c<0>(_val) = _1]
		<< lit(',') << real_literal[at_c<1>(_val) = _1]
		<< lit(',') << real_literal[at_c<2>(_val) = _1]
	;

	mdl =
		version
	;

	version %=
		lit("Version")
		<< lit('{')
		<< eol
		<< right_align(8)[
			lit("FormatVersion")
			<< karma::int_generator<long32>()[at_c<0>(_val) = _1]
			<< eol
		]
		<< lit('}')
	;

	model %=
		lit("Model")
		<< string[at_c<0>(_val) = _1] // model name
		<< lit('{')
		<< eol
		<< right_align(8)[
			lit("BlendTime")
			<< karma::int_generator<long32>()[at_c<2>(_val) = _1]
			<< eol
		]
		<< lit('}')
	;

	bounds %=
		lit("MinimumExtent") << lit('{')
			<< vertexData[at_c<0>(_val) = _1]
		<< lit('}') << lit(',')

		<< lit("MaximumExtent")
			<< vertexData[at_c<1>(_val) = _1]
		<< lit('}') >> lit(',')

		<< lit("BoundsRadius")
			<< real_literal[at_c<2>(_val) = _1]
		<< lit(',')
	;
	*/

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
	wc3lib::mdlx::client::VertexInteger1d,
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj[0], obj[0] = val)
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::client::VertexReal1d,
	(wc3lib::float32, wc3lib::float32, obj[0], obj[0] = val)
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::client::VertexReal2d,
	(wc3lib::float32, wc3lib::float32, obj[0], obj[0] = val)
	(wc3lib::float32, wc3lib::float32, obj[1], obj[1] = val)
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::VertexData,
	(wc3lib::float32, wc3lib::float32, obj[0], obj[0] = val)
	(wc3lib::float32, wc3lib::float32, obj[1], obj[1] = val)
	(wc3lib::float32, wc3lib::float32, obj[2], obj[2] = val)
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::client::VertexReal4d,
	(wc3lib::float32, wc3lib::float32, obj[0], obj[0] = val)
	(wc3lib::float32, wc3lib::float32, obj[1], obj[1] = val)
	(wc3lib::float32, wc3lib::float32, obj[2], obj[2] = val)
	(wc3lib::float32, wc3lib::float32, obj[3], obj[3] = val)
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Alpha,
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.frame(), obj.setFrame(val))
	(const wc3lib::mdlx::Alpha::Values&, const wc3lib::mdlx::Alpha::Values&, obj.values(), obj.setValues(val))
	(const wc3lib::mdlx::Alpha::Values&, const wc3lib::mdlx::Alpha::Values&, obj.inTan(), obj.setInTan(val))
	(const wc3lib::mdlx::Alpha::Values&, const wc3lib::mdlx::Alpha::Values&, obj.outTan(), obj.setOutTan(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Alphas,
	(wc3lib::mdlx::LineType, wc3lib::mdlx::LineType, obj.lineType(), obj.setLineType(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.globalSequenceId(), obj.setGlobalSequenceId(val))
	(const wc3lib::mdlx::Alphas::Properties&, const wc3lib::mdlx::Alphas::Properties&, obj.properties(), obj.setProperties(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Rotation,
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.frame(), obj.setFrame(val))
	(const wc3lib::mdlx::Rotation::Values&, const wc3lib::mdlx::Rotation::Values&, obj.values(), obj.setValues(val))
	(const wc3lib::mdlx::Rotation::Values&, const wc3lib::mdlx::Rotation::Values&, obj.inTan(), obj.setInTan(val))
	(const wc3lib::mdlx::Rotation::Values&, const wc3lib::mdlx::Rotation::Values&, obj.outTan(), obj.setOutTan(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Rotations,
	(wc3lib::mdlx::LineType, wc3lib::mdlx::LineType, obj.lineType(), obj.setLineType(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.globalSequenceId(), obj.setGlobalSequenceId(val))
	(const wc3lib::mdlx::Rotations::Properties&, const wc3lib::mdlx::Rotations::Properties&, obj.properties(), obj.setProperties(val))
)

/*
 * Defines for Translations and Colors as well!
 */
BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Scaling,
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.frame(), obj.setFrame(val))
	(const wc3lib::mdlx::Scaling::Values&, const wc3lib::mdlx::Scaling::Values&, obj.values(), obj.setValues(val))
	(const wc3lib::mdlx::Scaling::Values&, const wc3lib::mdlx::Scaling::Values&, obj.inTan(), obj.setInTan(val))
	(const wc3lib::mdlx::Scaling::Values&, const wc3lib::mdlx::Scaling::Values&, obj.outTan(), obj.setOutTan(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Scalings,
	(wc3lib::mdlx::LineType, wc3lib::mdlx::LineType, obj.lineType(), obj.setLineType(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.globalSequenceId(), obj.setGlobalSequenceId(val))
	(const wc3lib::mdlx::Scalings::Properties&, const wc3lib::mdlx::Scalings::Properties&, obj.properties(), obj.setProperties(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Mdlx,
	(wc3lib::int32, wc3lib::int32, obj.modelVersion(), obj.setModelVersion(val))
	(const wc3lib::mdlx::Model&, const wc3lib::mdlx::Model&, obj.model(), obj.setModel(val))
	(const wc3lib::mdlx::Mdlx::Sequences&, const wc3lib::mdlx::Mdlx::Sequences&, obj.sequences(), obj.setSequence(val))
	(const wc3lib::mdlx::Mdlx::GlobalSequences&, const wc3lib::mdlx::Mdlx::GlobalSequences&, obj.globalSequences(), obj.setGlobalSequences(val))
	(const wc3lib::mdlx::Mdlx::Textures&, const wc3lib::mdlx::Mdlx::Textures&, obj.textures(), obj.setTextures(val))
	(const wc3lib::mdlx::Mdlx::Materials&, const wc3lib::mdlx::Mdlx::Materials&, obj.materials(), obj.setMaterials(val))
	(const wc3lib::mdlx::Mdlx::TextureAnimations&, const wc3lib::mdlx::Mdlx::TextureAnimations&, obj.textureAnimations(), obj.setTextureAnimations(val))
	(const wc3lib::mdlx::Mdlx::Geosets&, const wc3lib::mdlx::Mdlx::Geosets&, obj.geosets(), obj.setGeosets(val))
	(const wc3lib::mdlx::Mdlx::GeosetAnimations&, const wc3lib::mdlx::Mdlx::GeosetAnimations&, obj.geosetAnimations(), obj.setGeosetAnimations(val))
	(const wc3lib::mdlx::Mdlx::Bones&, const wc3lib::mdlx::Mdlx::Bones&, obj.bones(), obj.setBones(val))
	(const wc3lib::mdlx::Mdlx::Lights&, const wc3lib::mdlx::Mdlx::Lights&, obj.lights(), obj.setLights(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Model,
	(const wc3lib::byte*, const wc3lib::byte*, obj.name(), obj.setName(val))
	//(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.unknown(), obj.setUnknown(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.blendTime(), obj.setBlendTime(val))
	(const wc3lib::mdlx::Bounds&, const wc3lib::mdlx::Bounds&, obj.bounds(), obj.setBounds(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Bounds,
	(const wc3lib::mdlx::VertexData&, const wc3lib::mdlx::VertexData&, obj.minimumExtent(), obj.setMinimumExtent(val))
	(const wc3lib::mdlx::VertexData&, const wc3lib::mdlx::VertexData&, obj.maximumExtent(), obj.setMaximumExtent(val))
	(wc3lib::float32, wc3lib::float32, obj.boundsRadius(), obj.setBoundsRadius(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::GeosetAnimation,
	(wc3lib::float32, wc3lib::float32, obj.staticAlpha(), obj.setStaticAlpha(val))
	(wc3lib::mdlx::GeosetAnimation::ColorAnimation, wc3lib::mdlx::GeosetAnimation::ColorAnimation, obj.colorAnimation(), obj.setColorAnimation(val))
	(const wc3lib::mdlx::VertexData&, const wc3lib::mdlx::VertexData&, obj.color(), obj.setColor(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.geosetId(), obj.setGeosetId(val))
	(const wc3lib::mdlx::Alphas&, const wc3lib::mdlx::Alphas&, obj.alphas(), obj.setAlphas(val))
	(const wc3lib::mdlx::Scalings&, const wc3lib::mdlx::Scalings&, obj.colors(), obj.setColors(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Sequence,
	(const wc3lib::byte*, const wc3lib::string&, obj.name(), obj.setName(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.intervalStart(), obj.setIntervalStart(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.intervalEnd(), obj.setIntervalEnd(val))
	(wc3lib::float32, wc3lib::float32, obj.moveSpeed(), obj.setMoveSpeed(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.noLooping(), obj.setNoLooping(val))
	(wc3lib::float32, wc3lib::float32, obj.rarity(), obj.setRarity(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.syncPoint(), obj.setSyncPoint(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Texture,
	(wc3lib::mdlx::ReplaceableId, wc3lib::mdlx::ReplaceableId, obj.replaceableId(), obj.setReplaceableId(val))
	(const wc3lib::byte*, const wc3lib::string&, obj.texturePath(), obj.setTexturePath(val))
	(wc3lib::mdlx::Texture::Wrapping, wc3lib::mdlx::Texture::Wrapping, obj.wrapping(), obj.setWrapping(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Layer::TextureId,
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.frame(), obj.setFrame(val))
	(const wc3lib::mdlx::Layer::TextureId::Values&, const wc3lib::mdlx::Layer::TextureId::Values&, obj.values(), obj.setValues(val))
	(const wc3lib::mdlx::Layer::TextureId::Values&, const wc3lib::mdlx::Layer::TextureId::Values&, obj.inTan(), obj.setInTan(val))
	(const wc3lib::mdlx::Layer::TextureId::Values&, const wc3lib::mdlx::Layer::TextureId::Values&, obj.outTan(), obj.setOutTan(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Layer::TextureIds,
	(wc3lib::mdlx::LineType, wc3lib::mdlx::LineType, obj.lineType(), obj.setLineType(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.globalSequenceId(), obj.setGlobalSequenceId(val))
	(const wc3lib::mdlx::Layer::TextureIds::Properties&, const wc3lib::mdlx::Layer::TextureIds::Properties&, obj.properties(), obj.setProperties(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Layer,
	(wc3lib::mdlx::Layer::FilterMode, wc3lib::mdlx::Layer::FilterMode, obj.filterMode(), obj.setFilterMode(val))
	(wc3lib::mdlx::Layer::Shading, wc3lib::mdlx::Layer::Shading, obj.shading(), obj.setShading(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.textureId(), obj.setTextureId(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.tvertexAnimationId(), obj.setTVertexAnimationId(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.coordinatesId(), obj.setCoordinatesId(val))
	(wc3lib::float32, wc3lib::float32, obj.alpha(), obj.setAlpha(val))
	(const wc3lib::mdlx::Alphas&, const wc3lib::mdlx::Alphas&, obj.alphas(), obj.setAlphas(val))
	(const wc3lib::mdlx::Layer::TextureIds&, const wc3lib::mdlx::Layer::TextureIds&, obj.textureIds(), obj.setTextureIds(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Material,
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.priorityPlane(), obj.setPriorityPlane(val))
	(wc3lib::mdlx::Material::RenderMode, wc3lib::mdlx::Material::RenderMode, obj.renderMode(), obj.setRenderMode(val))
	(const wc3lib::mdlx::Material::Layers &, const wc3lib::mdlx::Material::Layers &, obj.layers(), obj.setLayers(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::TextureAnimation,
	(const wc3lib::mdlx::Translations&, const wc3lib::mdlx::Translations&, obj.translations(), obj.setTranslations(val))
	(const wc3lib::mdlx::Rotations&, const wc3lib::mdlx::Rotations&, obj.rotations(), obj.setRotations(val))
	(const wc3lib::mdlx::Scalings&, const wc3lib::mdlx::Scalings&, obj.scalings(), obj.setScalings(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Faces,
	(wc3lib::mdlx::Faces::Type, wc3lib::mdlx::Faces::Type, obj.type(), obj.setType(val))
	(const wc3lib::mdlx::Faces::Vertices&, const wc3lib::mdlx::Faces::Vertices&, obj.vertices(), obj.setVertices(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Matrix,
	(const wc3lib::mdlx::Matrix::Values&, const wc3lib::mdlx::Matrix::Values&, obj.values(), obj.setValues(val))
)


BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Geoset,
	(const wc3lib::mdlx::Geoset::Vertices&, const wc3lib::mdlx::Geoset::Vertices&, obj.vertices(), obj.setVertices(val))
	(const wc3lib::mdlx::Geoset::Normals&, const wc3lib::mdlx::Geoset::Normals&, obj.normals(), obj.setNormals(val))
	(const wc3lib::mdlx::Geoset::Faces&, const wc3lib::mdlx::Geoset::Faces&, obj.faces(), obj.setFaces(val))
	(const wc3lib::mdlx::Geoset::GroupVertices&, const wc3lib::mdlx::Geoset::GroupVertices&, obj.groupVertices(), obj.setGroupVertices(val))
	(const wc3lib::mdlx::Geoset::Matrices&, const wc3lib::mdlx::Geoset::Matrices&, obj.matrices(), obj.setMatrices(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.materialId(), obj.setMaterialId(val))
	(wc3lib::mdlx::long32, wc3lib::mdlx::long32, obj.selectionGroup(), obj.setSelectionGroup(val))
	(wc3lib::mdlx::Geoset::Selectable, wc3lib::mdlx::Geoset::Selectable, obj.selectable(), obj.setSelectable(val))
	(const wc3lib::mdlx::Geoset::Ganimations&, const wc3lib::mdlx::Geoset::Ganimations&, obj.ganimations(), obj.setGanimations(val))
	(const wc3lib::mdlx::Geoset::TexturePatches&, const wc3lib::mdlx::Geoset::TexturePatches&, obj.texturePatches(), obj.setTexturePatches(val))
	(const wc3lib::mdlx::Geoset::TextureVertices&, const wc3lib::mdlx::Geoset::TextureVertices&, obj.textureVertices(), obj.setTextureVertices(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Bone,
	// node
	(const wc3lib::byte*, const wc3lib::string&, obj.name(), obj.setName(val))
	(const wc3lib::mdlx::long32, const wc3lib::mdlx::long32, obj.objectId(), obj.setObjectId(val))
	(const wc3lib::mdlx::long32, const wc3lib::mdlx::long32, obj.parentId(), obj.setParentId(val))
	(wc3lib::mdlx::Node::Type, wc3lib::mdlx::Node::Type, obj.type(), obj.setType(val))
	// bone
	(const wc3lib::mdlx::long32, const wc3lib::mdlx::long32, obj.geosetId(), obj.setGeosetId(val))
	(const wc3lib::mdlx::long32, const wc3lib::mdlx::long32, obj.geosetAnimationId(), obj.setGeosetAnimationId(val))
	// node
	(const wc3lib::mdlx::Translations&, const wc3lib::mdlx::Translations&, obj.translations(), obj.setTranslations(val))
	(const wc3lib::mdlx::Rotations&, const wc3lib::mdlx::Rotations&, obj.rotations(), obj.setRotations(val))
	(const wc3lib::mdlx::Scalings&, const wc3lib::mdlx::Scalings&, obj.scalings(), obj.setScalings(val))
	// object
	(const wc3lib::mdlx::Alphas&, const wc3lib::mdlx::Alphas&, obj.visibilities(), obj.setVisibilties(val))
)

BOOST_FUSION_ADAPT_ADT(
	wc3lib::mdlx::Light,
	// node
	(const wc3lib::byte*, const wc3lib::string&, obj.name(), obj.setName(val)) // 0
	(const wc3lib::mdlx::long32, const wc3lib::mdlx::long32, obj.objectId(), obj.setObjectId(val)) // 1
	(const wc3lib::mdlx::long32, const wc3lib::mdlx::long32, obj.parentId(), obj.setParentId(val)) // 2
	(wc3lib::mdlx::Node::Type, wc3lib::mdlx::Node::Type, obj.type(), obj.setType(val)) // 3
	// light
	(wc3lib::mdlx::Light::LightType, wc3lib::mdlx::Light::LightType, obj.lightType(), obj.setLightType(val)) // 4
	(wc3lib::float32, wc3lib::float32, obj.attenuationStart(), obj.setAttenuationStart(val)) // 5
	(wc3lib::float32, wc3lib::float32, obj.attenuationEnd(), obj.setAttenuationEnd(val)) // 6
	(wc3lib::float32, wc3lib::float32, obj.intensity(), obj.setIntensity(val)) // 7
	(const wc3lib::mdlx::Alphas&, const wc3lib::mdlx::Alphas&, obj.intensities(), obj.setIntensities(val)) // 8
	(const wc3lib::mdlx::VertexData&, const wc3lib::mdlx::VertexData&, obj.color(), obj.setColor(val)) // 9
	(const wc3lib::mdlx::Scalings&, const wc3lib::mdlx::Scalings&, obj.colors(), obj.setColors(val)) // 10
	(wc3lib::float32, wc3lib::float32, obj.ambientIntensity(), obj.setAmbientIntensity(val)) // 11
	(const wc3lib::mdlx::Alphas&, const wc3lib::mdlx::Alphas&, obj.ambientIntensities(), obj.setAmbientIntensities(val)) // 12
	(const wc3lib::mdlx::VertexData&, const wc3lib::mdlx::VertexData&, obj.ambientColor(), obj.setAmbientColor(val)) // 13
	(const wc3lib::mdlx::Scalings&, const wc3lib::mdlx::Scalings&, obj.ambientColors(), obj.setAmbientColors(val)) // 14
	// object
	(const wc3lib::mdlx::Alphas&, const wc3lib::mdlx::Alphas&, obj.visibilities(), obj.setVisibilties(val)) // 15
	// node
	(const wc3lib::mdlx::Translations&, const wc3lib::mdlx::Translations&, obj.translations(), obj.setTranslations(val)) // 16
	(const wc3lib::mdlx::Rotations&, const wc3lib::mdlx::Rotations&, obj.rotations(), obj.setRotations(val)) // 17
	(const wc3lib::mdlx::Scalings&, const wc3lib::mdlx::Scalings&, obj.scalings(), obj.setScalings(val)) // 18
)

#endif
