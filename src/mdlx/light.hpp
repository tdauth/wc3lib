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

#include <boost/cast.hpp>

#include "object.hpp"
#include "groupmdxblockmember.hpp"
#include "lights.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Light".
 */
class Light : public Object, public GroupMdxBlockMember
{
	public:
		enum class Type : long32
		{
			Omnidirectional = 0,
			Directional = 1,
			Ambient = 2
		};

		Light(class Lights *lights);
		virtual ~Light();

		class Lights* lights() const;
		Type type() const;
		float32 attenuationStart() const;
		float32 attenuationEnd() const;
		VertexData& color();
		const VertexData& color() const;
		float32 intensity() const;
		VertexData& ambientColor();
		const VertexData& ambientColor() const;
		float32 ambientIntensity() const;
		class Intensities* intensities() const;
		class LightAmbientVisibilities* visibilities() const;
		class LightAmbientColors* colors() const;
		class AmbientColors* ambientColors() const;
		class AmbientIntensities* ambientIntensities() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		Type m_type;
		float32 m_attenuationStart, m_attenuationEnd;
		VertexData m_color;
		float32 m_intensity;
		VertexData m_ambientColor;
		float32 m_ambientIntensity;
		class Intensities *m_intensities; //(KLAI)
		class LightAmbientVisibilities *m_visibilities; //(KLAV)
		class LightAmbientColors *m_colors; //(KLAC)
		class AmbientColors *m_ambientColors; //(KLBC)
		class AmbientIntensities *m_ambientIntensities; //(KLBI)
};

inline class Lights* Light::lights() const
{
	return boost::polymorphic_cast<class Lights*>(this->parent());
}

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

inline class Intensities* Light::intensities() const
{
	return this->m_intensities;
}

inline class LightAmbientVisibilities* Light::visibilities() const
{
	return this->m_visibilities;
}

inline class LightAmbientColors* Light::colors() const
{
	return this->m_colors;
}

inline class AmbientColors* Light::ambientColors() const
{
	return this->m_ambientColors;
}

inline class AmbientIntensities* Light::ambientIntensities() const
{
	return this->m_ambientIntensities;
}

}

}

#endif
