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

#ifndef WC3LIB_MDLX_TEXTUREANIMATION_HPP
#define WC3LIB_MDLX_TEXTUREANIMATION_HPP

#include "mdlxanimatedproperties.hpp"

namespace wc3lib
{

namespace mdlx
{

class TextureAnimation : public Format
{
	public:
		TextureAnimation();
		
		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

		bool hasTranslations() const;
		void setTranslations(const Translations &translations);
		const Translations& translations() const;
		void setRotations(const Rotations &rotations);
		const Rotations& rotations() const;
		void setScalings(const Scalings &scalings);
		const Scalings& scalings() const;

	protected:
		bool m_hasTranslations = true;
		Translations m_translations; //(KTAT) // Might be optional
		Rotations m_rotations; //(KTAR)
		Scalings m_scalings; //(KTAS)
};

inline bool TextureAnimation::hasTranslations() const
{
	return this->m_hasTranslations;
}

inline void TextureAnimation::setTranslations(const Translations &translations)
{
	this->m_translations = translations;
}

inline const Translations& TextureAnimation::translations() const
{
	return this->m_translations;
}

inline void TextureAnimation::setRotations(const Rotations &rotations)
{
	this->m_rotations = rotations;
}

inline const Rotations& TextureAnimation::rotations() const
{
	return this->m_rotations;
}

inline void TextureAnimation::setScalings(const Scalings &scalings)
{
	this->m_scalings = scalings;
}

inline const Scalings& TextureAnimation::scalings() const
{
	return this->m_scalings;
}

}

}

#endif
