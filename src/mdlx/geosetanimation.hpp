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

#include "mdlxproperty.hpp"
#include "groupmdxblockmember.hpp"
#include "geosetanimations.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "GeosetAnim".
 */
class GeosetAnimation : public GroupMdxBlockMember
{
	public:
		enum class ColorAnimation : long32
		{
			None = 0,
			DropShadow = 1,
			Color = 2,
			Both = 3
		};

		GeosetAnimation(class GeosetAnimations *geosetAnimations);
		virtual ~GeosetAnimation();

		class GeosetAnimations* geosetAnimations() const;
		float32 staticAlpha() const;
		ColorAnimation colorAnimation() const;
		float32 colorRed() const;
		float32 colorGreen() const;
		float32 colorBlue() const;
		long32 geosetId() const;
		class GeosetAnimationAlphas* alphas() const;
		class GeosetAnimationColors* colors() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		float32 m_staticAlpha; //(1.0:use KGAO)
		ColorAnimation m_colorAnimation; //(0:none;1:DropShadow;2:Color;3:Both)
		float32 m_colorRed, m_colorGreen, m_colorBlue; //(default:1)
		long32 m_geosetId;
		class GeosetAnimationAlphas *m_alphas; //(KGAO)
		class GeosetAnimationColors *m_colors; //(KGAC)
};

inline class GeosetAnimations* GeosetAnimation::geosetAnimations() const
{
	return boost::polymorphic_cast<class GeosetAnimations*>(this->parent());
}

inline float32 GeosetAnimation::staticAlpha() const
{
	return this->m_staticAlpha;
}

inline GeosetAnimation::ColorAnimation GeosetAnimation::colorAnimation() const
{
	return this->m_colorAnimation;
}

inline float32 GeosetAnimation::colorRed() const
{
	return this->m_colorRed;
}

inline float32 GeosetAnimation::colorGreen() const
{
	return this->m_colorGreen;
}

inline float32 GeosetAnimation::colorBlue() const
{
	return this->m_colorBlue;
}

inline long32 GeosetAnimation::geosetId() const
{
	return this->m_geosetId;
}

inline class GeosetAnimationAlphas* GeosetAnimation::alphas() const
{
	return this->m_alphas;
}

inline class GeosetAnimationColors* GeosetAnimation::colors() const
{
	return this->m_colors;
}

}

}

#endif
