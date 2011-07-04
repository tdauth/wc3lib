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

#ifndef WC3LIB_MDLX_TEXTUREID_HPP
#define WC3LIB_MDLX_TEXTUREID_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace mdlx
{

/// State is long not float therefore it doesn't inherit \ref MdxAlpha.
class TextureId
{
	public:
		TextureId(class TextureIds *textureIds);
		virtual ~TextureId();

		class TextureIds* textureIds() const;
		long32 frame() const;
		long32 state() const;
		float32 inTan() const;
		float32 outTan() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		class TextureIds *m_textureIds;
		long32 m_frame;
		long32 m_state; //(0 or 1), state is long not float!!!
		//if (LineType > 1) {
		float32 m_inTan;
		float32 m_outTan;
		//}
};

inline class TextureIds* TextureId::textureIds() const
{
	return this->m_textureIds;
}

inline long32 TextureId::frame() const
{
	return this->m_frame;
}

inline long32 TextureId::state() const
{
	return this->m_state;
}

inline float32 TextureId::inTan() const
{
	return this->m_inTan;
}

inline float32 TextureId::outTan() const
{
	return this->m_outTan;
}

}

}

#endif
