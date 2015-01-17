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

#ifndef WC3LIB_MDLX_LIGHT_HPP
#define WC3LIB_MDLX_LIGHT_HPP

#include "object.hpp"
#include "mdlxanimatedproperties.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Light".
 */
class Light : public Object
{
	public:
		enum class LightType : long32
		{
			Omnidirectional = 0,
			Directional = 1,
			Ambient = 2
		};

		Light();

		void setLightType(LightType type);
		LightType lightType() const;
		void setAttenuationStart(float32 attenuationStart);
		float32 attenuationStart() const;
		void setAttenuationEnd(float32 attenuationEnd);
		float32 attenuationEnd() const;
		void setColor(const VertexData &color);
		const VertexData& color() const;
		void setIntensity(float32 intensity);
		float32 intensity() const;
		void setAmbientColor(const VertexData &ambientColor);
		const VertexData& ambientColor() const;
		void setAmbientIntensity(float32 ambientIntensity);
		float32 ambientIntensity() const;
		void setIntensities(const Alphas &intensities);
		const Alphas& intensities() const;
		void setVisibilities(const Alphas &visibilities);
		const Alphas& visibilities() const;
		void setColors(const Scalings &colors);
		const Scalings& colors() const;
		void setAmbientColors(const Scalings &ambientColors);
		const Scalings& ambientColors() const;
		void setAmbientIntensities(const Alphas &ambientIntensities);
		const Alphas& ambientIntensities() const;

	protected:
		LightType m_lightType;
		float32 m_attenuationStart, m_attenuationEnd;
		VertexData m_color;
		float32 m_intensity;
		VertexData m_ambientColor;
		float32 m_ambientIntensity;
		Alphas m_intensities; //(KLAI)
		Scalings m_colors; //(KLAC)
		Scalings m_ambientColors; //(KLBC)
		Alphas m_ambientIntensities; //(KLBI)
};

inline void Light::setLightType(Light::LightType type)
{
	this->m_lightType = type;
}

inline Light::LightType Light::lightType() const
{
	return this->m_lightType;
}

inline void Light::setAttenuationStart(float32 attenuationStart)
{
	this->m_attenuationStart = attenuationStart;
}

inline float32 Light::attenuationStart() const
{
	return this->m_attenuationStart;
}

inline void Light::setAttenuationEnd(float32 attenuationEnd)
{
	this->m_attenuationEnd = attenuationEnd;
}

inline float32 Light::attenuationEnd() const
{
	return this->m_attenuationEnd;
}

inline void Light::setColor(const VertexData &color)
{
	this->m_color = color;
}

inline const VertexData& Light::color() const
{
	return this->m_color;
}

inline void Light::setIntensity(float32 intensity)
{
	this->m_intensity = intensity;
}

inline float32 Light::intensity() const
{
	return this->m_intensity;
}

inline void Light::setAmbientColor(const VertexData &ambientColor)
{
	this->m_ambientColor = ambientColor;
}

inline const VertexData& Light::ambientColor() const
{
	return this->m_ambientColor;
}

inline void Light::setAmbientIntensity(float32 ambientIntensity)
{
	this->m_ambientIntensity = ambientIntensity;
}

inline float32 Light::ambientIntensity() const
{
	return this->m_ambientIntensity;
}

inline void Light::setIntensities(const Alphas &intensities)
{
	this->m_intensities = intensities;
}

inline const Alphas& Light::intensities() const
{
	return this->m_intensities;
}

inline void Light::setVisibilities(const Alphas &visibilities)
{
	this->m_visibilities = visibilities;
}

inline const Alphas& Light::visibilities() const
{
	return this->m_visibilities;
}

inline void Light::setColors(const Scalings& colors)
{
	this->m_colors = colors;
}

inline const Scalings& Light::colors() const
{
	return this->m_colors;
}

inline void Light::setAmbientColors(const Scalings &ambientColors)
{
	this->m_ambientColors = ambientColors;
}

inline const Scalings& Light::ambientColors() const
{
	return this->m_ambientColors;
}

inline void Light::setAmbientIntensities(const Alphas &ambientIntensities)
{
	this->m_ambientIntensities = ambientIntensities;
}

inline const Alphas& Light::ambientIntensities() const
{
	return this->m_ambientIntensities;
}

}

}

#endif
