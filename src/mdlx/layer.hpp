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
		enum class FilterMode : long32
		{
			None = 0,
			Transparent = 1,
			Blend = 2,
			Additive = 3,
			AddAlpha = 4,
			Modulate = 5,
			Modulate2x = 6
		};

		enum class Shading : long32
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

		static string filterMode(FilterMode filterMode);

		Layer(class Layers *layers);
		virtual ~Layer();

		class Layers* layers() const;
		FilterMode filterMode() const;
		Shading shading() const;
		long32 textureId() const;
		long32 tvertexAnimationId() const;
		long32 coordinatesId() const;
		float32 alpha() const;
		class MaterialAlphas* alphas() const;
		class TextureIds* textureIds() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		FilterMode m_filterMode;
		Shading m_shading;
		long32 m_textureId;
		long32 m_tvertexAnimationId; // 0xFFFFFFFF if none
		long32 m_coordinatesId;
		float32 m_alpha; //(0(transparent)->1(opaque))
		class MaterialAlphas *m_alphas; //(KMTA)
		class TextureIds *m_textureIds; //(KMTF) // state is long not float
};

inline constexpr bool operator&(Layer::FilterMode x, Layer::FilterMode y)
{
	return static_cast<bool>(static_cast<long32>(x) & static_cast<long32>(y));
}

inline constexpr bool operator&(Layer::Shading x, Layer::Shading y)
{
	return static_cast<bool>(static_cast<long32>(x) & static_cast<long32>(y));
}

inline string Layer::filterMode(FilterMode filterMode)
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

inline Layer::FilterMode Layer::filterMode() const
{
	return this->m_filterMode;
}

inline Layer::Shading Layer::shading() const
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
