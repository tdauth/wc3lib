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

#ifndef WC3LIB_MDLX_TEXTURE_HPP
#define WC3LIB_MDLX_TEXTURE_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Bitmap".
 */
class Texture : public Format
{
	public:
		enum class Wrapping : long32
		{
			None = 0,
			WrapWidth = 1,
			WrapHeight = 2,
			Both = 3
		};

		static const std::size_t texturePathSize = 260;

		Texture();

		void setReplaceableId(ReplaceableId replaceableId);
		ReplaceableId replaceableId() const;
		void setTexturePath(const byte texturePath[texturePathSize]);
		void setTexturePath(const string &texturePath);
		/**
		 * \return Returns ASCII texture path with length \ref texturePathSize.
		 */
		const byte* texturePath() const;
		void setWrapping(Wrapping wrapping);
		Wrapping wrapping() const;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

	protected:
		ReplaceableId m_replaceableId;
		byte m_texturePath[texturePathSize]; //(0x100 bytes)
		Wrapping m_wrapping; //(1:WrapWidth;2:WrapHeight;3:Both)
};

inline void Texture::setReplaceableId(ReplaceableId replaceableId)
{
	this->m_replaceableId = replaceableId;
}

inline ReplaceableId Texture::replaceableId() const
{
	return this->m_replaceableId;
}

inline void Texture::setTexturePath(const byte texturePath[Texture::texturePathSize])
{
	memcpy(this->m_texturePath, texturePath, Texture::texturePathSize);
}

inline void Texture::setTexturePath(const string &texturePath)
{
	if (texturePath.size() + 1 > texturePathSize)
	{
		throw std::out_of_range("");
	}

	memcpy(this->m_texturePath, texturePath.c_str(), texturePath.size() + 1);
}

inline const char* Texture::texturePath() const
{
	return this->m_texturePath;
}

inline void Texture::setWrapping(Texture::Wrapping wrapping)
{
	this->m_wrapping = wrapping;
}

inline Texture::Wrapping Texture::wrapping() const
{
	return this->m_wrapping;
}

}

}

#endif
