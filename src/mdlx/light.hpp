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
		enum class Type : long32
		{
			Omnidirectional = 0,
			Directional = 1,
			Ambient = 2
		};

		Light();

		Type type() const;
		float32 attenuationStart() const;
		float32 attenuationEnd() const;
		VertexData& color();
		const VertexData& color() const;
		float32 intensity() const;
		VertexData& ambientColor();
		const VertexData& ambientColor() const;
		float32 ambientIntensity() const;
		const Alphas& intensities() const;
		const Alphas& visibilities() const;
		const Scalings& colors() const;
		const Scalings& ambientColors() const;
		const Scalings& ambientIntensities() const;

	protected:
		Type m_type;
		float32 m_attenuationStart, m_attenuationEnd;
		VertexData m_color;
		float32 m_intensity;
		VertexData m_ambientColor;
		float32 m_ambientIntensity;
		Alphas m_intensities; //(KLAI)
		Alphas m_visibilities; //(KLAV)
		Scalings m_colors; //(KLAC)
		Scalings m_ambientColors; //(KLBC)
		Scalings m_ambientIntensities; //(KLBI)
};

inline Light::Type Light::type() const
{
	return this->m_type;
}

inline float32 Light::attenuationStart() const
{
	return this->m_attenuationStart;
}

inline float32 Light::attenuationEnd() const
{
	return this->m_attenuationEnd;
}

inline VertexData& Light::color()
{
	return this->m_color;
}

inline const VertexData& Light::color() const
{
	return this->m_color;
}

inline float32 Light::intensity() const
{
	return this->m_intensity;
}

inline VertexData& Light::ambientColor()
{
	return this->m_ambientColor;
}

inline const VertexData& Light::ambientColor() const
{
	return this->m_ambientColor;
}

inline float32 Light::ambientIntensity() const
{
	return this->m_ambientIntensity;
}

inline const Alphas& Light::intensities() const
{
	return this->m_intensities;
}

inline const Alphas& Light::visibilities() const
{
	return this->m_visibilities;
}

inline const Scalings& Light::colors() const
{
	return this->m_colors;
}

inline const Scalings& Light::ambientColors() const
{
	return this->m_ambientColors;
}

inline const Scalings& Light::ambientIntensities() const
{
	return this->m_ambientIntensities;
}

}

}

#endif
