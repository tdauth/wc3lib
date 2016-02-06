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
#include "bounds.hpp"
#include "geosetanimation.hpp"
#include "mdlxanimatedproperties.hpp"

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
struct CommentSkipper : public qi::grammar<Iterator>
{
	CommentSkipper();

	qi::rule<Iterator> skip;
	qi::rule<Iterator> emptyline;
	qi::rule<Iterator> moreemptylines;
	qi::rule<Iterator> emptylines;
	qi::rule<Iterator> comment;
};

typedef BasicVertex<long32, 1> VertexInteger1d;
typedef BasicVertex<float32, 1> VertexReal1d;
typedef Vertex2d<float32> VertexReal2d;
typedef VertexData VertexReal3d;
typedef BasicVertex<float32, 4> VertexReal4d;

/**
 * Qi based grammar for the MDL file format.
 * \ingroup mdl
 */
template <typename Iterator, typename Skipper = CommentSkipper<Iterator> >
struct MdlGrammar : qi::grammar<Iterator, Mdlx(), qi::locals<long32, long32, long32>, Skipper>
{
	MdlGrammar();

	qi::rule<Iterator, long32(), Skipper> integer_literal;
	qi::rule<Iterator, uint8(), Skipper> byte_integer_literal;
	qi::rule<Iterator, short16(), Skipper> short_integer_literal;
	qi::rule<Iterator, float32(), Skipper> real_literal;
	qi::rule<Iterator, string(), Skipper> string_literal;

	qi::rule<Iterator, VertexInteger1d(), Skipper> vertex_integer_1d;
	qi::rule<Iterator, VertexReal1d(), Skipper> vertex_real_1d;
	qi::rule<Iterator, VertexReal2d(), Skipper> vertex_real_2d;
	qi::rule<Iterator, VertexReal3d(), Skipper> vertex_real_3d;
	qi::rule<Iterator, VertexReal4d(), Skipper> vertex_real_4d;

	qi::rule<Iterator, LineType(), Skipper> line_type;
	qi::rule<Iterator, long32(), Skipper> global_sequence_id;

	/*
	 * "a" is the number of geosets
	 * "b" is the number of geoset animations
	 * "c" is the number of sequences
	 */
	qi::rule<Iterator, Mdlx(), qi::locals<long32, long32, long32>, Skipper> mdl;
	/*
	 * All numbers are passed as attribute references and are set in the model rule.
	 * 0: Number of Geosets
	 * 1: Number of Geoset Animations
	 */
	qi::rule<Iterator, Model(long32&, long32&), Skipper> model;
	qi::rule<Iterator, long32(), Skipper> version;
	qi::rule<Iterator, Sequence(), Skipper> sequence;
	// pass the number of sequences as reference
	qi::rule<Iterator, Mdlx::Sequences(long32&), Skipper, qi::locals<long32> > sequences;
	qi::rule<Iterator, Mdlx::GlobalSequences(), Skipper, qi::locals<long32> > global_sequences;
	qi::rule<Iterator, ReplaceableId(), Skipper> replaceable_id;
	qi::rule<Iterator, Texture::Wrapping(), Skipper> wrapping_both;
	qi::rule<Iterator, Texture::Wrapping(), Skipper> wrapping_one;
	qi::rule<Iterator, Texture::Wrapping(), Skipper> wrapping;
	qi::rule<Iterator, Texture(), Skipper> texture;
	qi::rule<Iterator, Mdlx::Textures(), Skipper, qi::locals<long32> > textures;
	qi::rule<Iterator, Layer::FilterMode(), Skipper> filter_mode;
	qi::rule<Iterator, long32(), Skipper> texture_id;
	qi::rule<Iterator, Layer::TextureId(), Skipper> animated_texture_id;
	qi::rule<Iterator, Layer::TextureIds(), Skipper, qi::locals<long32> > animated_texture_ids;
	qi::rule<Iterator, Layer(), Skipper> layer;
	qi::rule<Iterator, Material::Layers(), Skipper> layers;
	qi::rule<Iterator, Material(), Skipper> material;
	qi::rule<Iterator, Mdlx::Materials(), Skipper, qi::locals<long32> > materials;
	qi::rule<Iterator, TextureAnimation(), Skipper> texture_animation;
	qi::rule<Iterator, Mdlx::TextureAnimations(), Skipper, qi::locals<long32> > texture_animations;

	qi::rule<Iterator, Geoset::Vertices(), Skipper, qi::locals<long32> > vertices;
	qi::rule<Iterator, Geoset::Normals(), Skipper, qi::locals<long32> > normals;
	qi::rule<Iterator, Geoset::TextureVertices(), Skipper, qi::locals<long32> > texture_vertices;
	qi::rule<Iterator, Geoset::GroupVertices(), Skipper> group_vertices;
	qi::rule<Iterator, Faces(), Skipper> faces_group;
	qi::rule<Iterator, Geoset::Faces(), Skipper, qi::locals<long32, long32> > faces;
	qi::rule<Iterator, Matrix(), Skipper, qi::locals<long32, long32> > matrix;
	qi::rule<Iterator, Geoset::Matrices(), Skipper, qi::locals<long32, long32> > groups;
	/*
	 * Gets passed the number of sequences in the model.
	 */
	qi::rule<Iterator, Geoset(long32), Skipper> geoset;

	qi::rule<Iterator, GeosetAnimation(), Skipper> geoset_animation;

	/*
	 * The following rules are required by all node chunks.
	 */
	qi::rule<Iterator, long32(), Skipper> object_id;
	qi::rule<Iterator, long32(), Skipper> parent;
	qi::rule<Iterator, Node::Type(), Skipper> node_type;

	qi::rule<Iterator, Bone(), Skipper> bone;
	qi::rule<Iterator, Light::LightType(), Skipper> light_type;
	qi::rule<Iterator, Light(), Skipper> light;
	qi::rule<Iterator, Mdlx::Lights(), Skipper> lights;



	qi::rule<Iterator, Bounds(), Skipper> bounds;


	qi::rule<Iterator, Alpha(), Skipper> animated_1d_float_value;
	qi::rule<Iterator, Alphas(), Skipper, qi::locals<long32> > animated_1d_float_values;

	qi::rule<Iterator, Scaling(), Skipper> animated_3d_float_value;
	qi::rule<Iterator, Scalings(), Skipper, qi::locals<long32> > animated_3d_float_values;

	qi::rule<Iterator, float32(), Skipper> alpha;
	qi::rule<Iterator, Alphas(), Skipper, qi::locals<long32> > animated_alphas;

	qi::rule<Iterator, float32(), Skipper> visibility;
	qi::rule<Iterator, Alphas(), Skipper, qi::locals<long32> > animated_visibilities;

	qi::rule<Iterator, float32(), Skipper> intensity;
	qi::rule<Iterator, Alphas(), Skipper, qi::locals<long32> > animated_intensities;

	qi::rule<Iterator, VertexData(), Skipper> color;
	qi::rule<Iterator, Scalings(), Skipper, qi::locals<long32> > animated_colors;

	qi::rule<Iterator, float32(), Skipper> amb_intensity;
	qi::rule<Iterator, Alphas(), Skipper, qi::locals<long32> > animated_amb_intensities;

	qi::rule<Iterator, VertexData(), Skipper> amb_color;
	qi::rule<Iterator, Scalings(), Skipper, qi::locals<long32> > animated_amb_colors;

	qi::rule<Iterator, Translation(), Skipper> animated_translation;
	qi::rule<Iterator, Translations(), Skipper, qi::locals<long32> > animated_translations;
	qi::rule<Iterator, Rotation(), Skipper> animated_rotation;
	qi::rule<Iterator, Rotations(), Skipper, qi::locals<long32> > animated_rotations;
	qi::rule<Iterator, Scaling(), Skipper> animated_scaling;
	qi::rule<Iterator, Scalings(), Skipper, qi::locals<long32> > animated_scalings;
};

/**
 * Might throw an boost::spirit::expectation_failure exception!
 */
template <typename Iterator>
bool parse(Iterator first, Iterator last, Mdlx &mdlx);

template <typename Iterator>
struct MdlGenerator : karma::grammar<Iterator, Mdlx()>
{
	MdlGenerator();

	karma::rule<Iterator, long32()> integer_literal;
	karma::rule<Iterator, float32()> real_literal;
	karma::rule<Iterator, string()> string_literal;
	karma::rule<Iterator, VertexData()> vertexData;

	karma::rule<Iterator, Mdlx()> mdl;
	karma::rule<Iterator, Model()> model;
	karma::rule<Iterator, long32()> version;

	karma::rule<Iterator, Bounds()> bounds;
};

}

}

}


/*
 * The specialization is required for proper debug output of Spirit.
 * It prints attributes of rules and therefore attributes of type jass_ast_node which misses a stream output operator.
 * Unfortunately we have to define it for every type because it does not detect inheritance and virtual methods.
 */
// your specialization needs to be in namespace boost::spirit::traits
// https://stackoverflow.com/questions/5286720/how-to-define-streaming-operator-for-boostspiritqi-and-stdlist-container
// https://svn.boost.org/trac/boost/ticket/9803
namespace boost
{

namespace spirit
{

namespace traits
{
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::LineType>
	{
		static void call(Out& out, wc3lib::mdlx::LineType const& val)
		{
			out << "line_type";

			switch (val)
			{
				case wc3lib::mdlx::LineType::DontInterpolate:
				{
					out << "DontInterpolate";

					break;
				}

				case wc3lib::mdlx::LineType::Linear:
				{
					out << "Linear";

					break;
				}

				case wc3lib::mdlx::LineType::Hermite:
				{
					out << "Hermite";

					break;
				}

				case wc3lib::mdlx::LineType::Bezier:
				{
					out << "Bezier";

					break;
				}
			}
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Mdlx>
	{
		static void call(Out& out, wc3lib::mdlx::Mdlx const& /* val */)
		{
			out << "mdlx";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Model>
	{
		static void call(Out& out, wc3lib::mdlx::Model const& /* val */)
		{
			out << "model";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Bounds>
	{
		static void call(Out& out, wc3lib::mdlx::Bounds const& /* val */)
		{
			out << "bounds";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::GeosetAnimation>
	{
		static void call(Out& out, wc3lib::mdlx::GeosetAnimation const& /* val */)
		{
			out << "geoset_animation";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Sequence>
	{
		static void call(Out& out, wc3lib::mdlx::Sequence const& /* val */)
		{
			out << "sequence";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::ReplaceableId>
	{
		static void call(Out& out, wc3lib::mdlx::ReplaceableId const& /* val */)
		{
			out << "replaceable_id";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Texture::Wrapping>
	{
		static void call(Out& out, wc3lib::mdlx::Texture::Wrapping const& /* val */)
		{
			out << "wrapping";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Texture>
	{
		static void call(Out& out, wc3lib::mdlx::Texture const& /* val */)
		{
			out << "texture";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Layer::FilterMode>
	{
		static void call(Out& out, wc3lib::mdlx::Layer::FilterMode const& /* val */)
		{
			out << "filter_mode";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Layer::TextureId>
	{
		static void call(Out& out, wc3lib::mdlx::Layer::TextureId const& /* val */)
		{
			out << "texture_id";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Layer::TextureIds>
	{
		static void call(Out& out, wc3lib::mdlx::Layer::TextureIds const& /* val */)
		{
			out << "texture_ids";
		}
	};


	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Layer>
	{
		static void call(Out& out, wc3lib::mdlx::Layer const& /* val */)
		{
			out << "layer";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Material>
	{
		static void call(Out& out, wc3lib::mdlx::Material const& /* val */)
		{
			out << "material";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::TextureAnimation>
	{
		static void call(Out& out, wc3lib::mdlx::TextureAnimation const& /* val */)
		{
			out << "texture_animation";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Faces>
	{
		static void call(Out& out, wc3lib::mdlx::Faces const& /* val */)
		{
			out << "faces";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Matrix>
	{
		static void call(Out& out, wc3lib::mdlx::Matrix const& /* val */)
		{
			out << "matrix";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Geoset>
	{
		static void call(Out& out, wc3lib::mdlx::Geoset const& /* val */)
		{
			out << "geoset";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Bone>
	{
		static void call(Out& out, wc3lib::mdlx::Bone const& /* val */)
		{
			out << "bone";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Light>
	{
		static void call(Out& out, wc3lib::mdlx::Light const& /* val */)
		{
			out << "light";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Alpha>
	{
		static void call(Out& out, wc3lib::mdlx::Alpha const& /* val */)
		{
			out << "alpha";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Alphas>
	{
		static void call(Out& out, wc3lib::mdlx::Alphas const& /* val */)
		{
			out << "alphas";
		}
	};

	/*
	 * Defines for Translation and Color as well.
	 */
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Scaling>
	{
		static void call(Out& out, wc3lib::mdlx::Scaling const& /* val */)
		{
			out << "scaling";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Scalings>
	{
		static void call(Out& out, wc3lib::mdlx::Scalings const& /* val */)
		{
			out << "scalings";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Rotation>
	{
		static void call(Out& out, wc3lib::mdlx::Rotation const& /* val */)
		{
			out << "rotation";
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::mdlx::Rotations>
	{
		static void call(Out& out, wc3lib::mdlx::Rotations const& /* val */)
		{
			out << "rotations";
		}
	};
}

}

}

#endif
