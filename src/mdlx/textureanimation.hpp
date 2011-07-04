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

#include "mdlxproperty.hpp"
#include "groupmdxblockmember.hpp"
#include "textureanimations.hpp"

namespace wc3lib
{

namespace mdlx
{

class TextureAnimation : public GroupMdxBlockMember
{
	public:
		TextureAnimation(class TextureAnimations *textureAnimations);
		virtual ~TextureAnimation();

		class TextureAnimations* textureAnimations() const;
		class TextureAnimationTranslations* translations() const;
		class TextureAnimationRotations* rotations() const;
		class TextureAnimationScalings* scalings() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		class TextureAnimationTranslations *m_translations; //(KTAT) // Might be optional
		class TextureAnimationRotations *m_rotations; //(KTAR)
		class TextureAnimationScalings *m_scalings; //(KTAS)
};

inline class TextureAnimations* TextureAnimation::textureAnimations() const
{
	return boost::polymorphic_cast<class TextureAnimations*>(this->parent());
}

inline class TextureAnimationTranslations* TextureAnimation::translations() const
{
	return this->m_translations;
}

inline class TextureAnimationRotations* TextureAnimation::rotations() const
{
	return this->m_rotations;
}

inline class TextureAnimationScalings* TextureAnimation::scalings() const
{
	return this->m_scalings;
}

}

}

#endif
