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

#include "platform.hpp"
#include "mdlxanimatedproperties.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Layer".
 */
class Layer
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

		/**
		 * Use \ref BasicMdlxAlphas<long32>::setAlpha() to assign the texture ID.
		 */
		typedef MdlxAnimatedProperties<1, long32> TextureIds;
		typedef TextureIds::Property TextureId;

		static string filterMode(FilterMode filterMode);

		Layer();

		void setFilterMode(FilterMode filterMode);
		FilterMode filterMode() const;
		void setShading(Shading shading);
		Shading shading() const;
		void setTextureId(long32 textureId);
		long32 textureId() const;
		void setTVertexAnimationId(long32 tvertexAnimationId);
		long32 tvertexAnimationId() const;
		void setCoordinatesId(long32 coordinatesId);
		long32 coordinatesId() const;
		void setAlpha(float32 alpha);
		float32 alpha() const;
		void setAlphas(const Alphas &alphas);
		const Alphas& alphas() const;
		void setTextureIds(const TextureIds &textureIds);
		const TextureIds& textureIds() const;

	protected:
		FilterMode m_filterMode;
		Shading m_shading;
		long32 m_textureId;
		long32 m_tvertexAnimationId; // 0xFFFFFFFF if none
		long32 m_coordinatesId;
		float32 m_alpha; //(0(transparent)->1(opaque))
		Alphas m_alphas; //(KMTA)
		TextureIds m_textureIds; //(KMTF) // state is long not float
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

inline void Layer::setFilterMode(Layer::FilterMode filterMode)
{
	this->m_filterMode = filterMode;
}

inline Layer::FilterMode Layer::filterMode() const
{
	return this->m_filterMode;
}

inline void Layer::setShading(Layer::Shading shading)
{
	this->m_shading = shading;
}

inline Layer::Shading Layer::shading() const
{
	return this->m_shading;
}

inline void Layer::setTextureId(long32 textureId)
{
	this->m_textureId = textureId;
}

inline long32 Layer::textureId() const
{
	return this->m_textureId;
}

inline void Layer::setTVertexAnimationId(long32 tvertexAnimationId)
{
	this->m_tvertexAnimationId = tvertexAnimationId;
}

inline long32 Layer::tvertexAnimationId() const
{
	return this->m_tvertexAnimationId;
}

inline void Layer::setCoordinatesId(long32 coordinatesId)
{
	this->m_coordinatesId = coordinatesId;
}

inline long32 Layer::coordinatesId() const
{
	return this->m_coordinatesId;
}

inline void Layer::setAlpha(float32 alpha)
{
	this->m_alpha = alpha;
}

inline float32 Layer::alpha() const
{
	return this->m_alpha;
}

inline void Layer::setAlphas(const Alphas &alphas)
{
	this->m_alphas = alphas;
}

inline const Alphas& Layer::alphas() const
{
	return this->m_alphas;
}

inline void Layer::setTextureIds(const Layer::TextureIds &textureIds)
{
	this->m_textureIds = textureIds;
}

inline const Layer::TextureIds& Layer::textureIds() const
{
	return this->m_textureIds;
}

}

}

#endif
