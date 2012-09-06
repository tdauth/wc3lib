/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MDLX_LAYER_HPP
#define WC3LIB_MDLX_LAYER_HPP

#include "groupmdxblockmember.hpp"
#include "layers.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Layer".
 */
class Layer : public GroupMdxBlockMember
{
	public:
		BOOST_SCOPED_ENUM_START(FilterMode) /// \todo C++11 : long32
		{
			None = 0,
			Transparent = 1,
			Blend = 2,
			Additive = 3,
			AddAlpha = 4,
			Modulate = 5,
			Modulate2x = 6
		};
		BOOST_SCOPED_ENUM_END

		BOOST_SCOPED_ENUM_START(Shading) /// \todo C++11 : long32
		{
			Unshaded = 1,
			SphereEnvironmentMap = 2,
			Unknown0 = 4,
			Unknown1 = 8,
			TwoSided = 16,
			Unfogged = 32,
			NoDepthTest = 64,
			NoDepthSet = 128
		};
		BOOST_SCOPED_ENUM_END

		static string filterMode(BOOST_SCOPED_ENUM(FilterMode) filterMode);

		Layer(class Layers *layers);
		virtual ~Layer();

		class Layers* layers() const;
		BOOST_SCOPED_ENUM(FilterMode) filterMode() const;
		BOOST_SCOPED_ENUM(Shading) shading() const;
		long32 textureId() const;
		long32 tvertexAnimationId() const;
		long32 coordinatesId() const;
		float32 alpha() const;
		class MaterialAlphas* alphas() const;
		class TextureIds* textureIds() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		BOOST_SCOPED_ENUM(FilterMode) m_filterMode;
		BOOST_SCOPED_ENUM(Shading) m_shading;
		long32 m_textureId;
		long32 m_tvertexAnimationId; // 0xFFFFFFFF if none
		long32 m_coordinatesId;
		float32 m_alpha; //(0(transparent)->1(opaque))
		class MaterialAlphas *m_alphas; //(KMTA)
		class TextureIds *m_textureIds; //(KMTF) // state is long not float
};

inline string Layer::filterMode(BOOST_SCOPED_ENUM(FilterMode) filterMode)
{
	switch (filterMode)
	{
		case FilterMode::None:
			return "None";

		case FilterMode::Transparent:
			return "Transparent";

		case FilterMode::Blend:
			return "Blend";

		case FilterMode::Additive:
			return "Additive";

		case FilterMode::AddAlpha:
			return "AddAlpha";

		case FilterMode::Modulate:
			return "Modulate";

		case FilterMode::Modulate2x:
			return "Modulate2x";
	}

	return "";
}

inline class Layers* Layer::layers() const
{
	return boost::polymorphic_cast<class Layers*>(this->parent());
}

inline BOOST_SCOPED_ENUM(Layer::FilterMode) Layer::filterMode() const
{
	return this->m_filterMode;
}

inline BOOST_SCOPED_ENUM(Layer::Shading) Layer::shading() const
{
	return this->m_shading;
}

inline long32 Layer::textureId() const
{
	return this->m_textureId;
}

inline long32 Layer::tvertexAnimationId() const
{
	return this->m_tvertexAnimationId;
}

inline long32 Layer::coordinatesId() const
{
	return this->m_coordinatesId;
}

inline float32 Layer::alpha() const
{
	return this->m_alpha;
}

inline class MaterialAlphas* Layer::alphas() const
{
	return this->m_alphas;
}

inline class TextureIds* Layer::textureIds() const
{
	return this->m_textureIds;
}

}

}

#endif
