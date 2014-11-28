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

#ifndef WC3LIB_MDLX_GEOSETANIMATION_HPP
#define WC3LIB_MDLX_GEOSETANIMATION_HPP

#include "platform.hpp"
#include "mdlxanimatedproperties.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "GeosetAnim".
 */
class GeosetAnimation
{
	public:
		enum class ColorAnimation : long32
		{
			None = 0,
			DropShadow = 1,
			Color = 2,
			Both = 3
		};

		GeosetAnimation();

		void setStaticAlpha(float32 alpha);
		float32 staticAlpha() const;
		void setColorAnimation(ColorAnimation colorAnimation);
		ColorAnimation colorAnimation() const;
		void setColor(const VertexData &color);
		const VertexData& color() const;
		void setGeosetId(long32 geosetId);
		long32 geosetId() const;
		void setAlphas(const Alphas &alphas);
		const Alphas& alphas() const;
		void setColors(const Scalings &colors);
		const Scalings& colors() const;

	protected:
		float32 m_staticAlpha; //(1.0:use KGAO)
		ColorAnimation m_colorAnimation; //(0:none;1:DropShadow;2:Color;3:Both)
		VertexData m_color; //(default:1)
		long32 m_geosetId;
		Alphas m_alphas; //(KGAO)
		Scalings m_colors; //(KGAC)
};

inline void GeosetAnimation::setStaticAlpha(float32 alpha)
{
	this->m_staticAlpha = alpha;
}

inline float32 GeosetAnimation::staticAlpha() const
{
	return this->m_staticAlpha;
}

inline void GeosetAnimation::setColorAnimation(GeosetAnimation::ColorAnimation colorAnimation)
{
	this->m_colorAnimation = colorAnimation;
}

inline GeosetAnimation::ColorAnimation GeosetAnimation::colorAnimation() const
{
	return this->m_colorAnimation;
}

inline void GeosetAnimation::setColor(const VertexData &color)
{
	this->m_color = color;
}

inline const VertexData& GeosetAnimation::color() const
{
	return this->m_color;
}

inline void GeosetAnimation::setGeosetId(long32 geosetId)
{
	this->m_geosetId = geosetId;
}

inline long32 GeosetAnimation::geosetId() const
{
	return this->m_geosetId;
}

inline void GeosetAnimation::setAlphas(const Alphas &alphas)
{
	this->m_alphas = alphas;
}

inline const Alphas& GeosetAnimation::alphas() const
{
	return this->m_alphas;
}

inline void GeosetAnimation::setColors(const Scalings &colors)
{
	this->m_colors = colors;
}

inline const Scalings& GeosetAnimation::colors() const
{
	return this->m_colors;
}

}

}

#endif
