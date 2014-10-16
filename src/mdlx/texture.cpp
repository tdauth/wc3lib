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

#include "texture.hpp"

namespace wc3lib
{

namespace mdlx
{

Texture::Texture(class Textures *textures) : GroupMdxBlockMember(textures, "Bitmap")
{
}

std::streamsize Texture::readMdl(istream &istream)
{
	return 0;
}

std::streamsize Texture::writeMdl(ostream &ostream) const
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "Bitmap");
	writeMdlValuePropertyWithQuotes(ostream, size, "Image", this->texturePath(), 1);

	if (this->replaceableId() != ReplaceableId::None)
	{
		writeMdlValueProperty(ostream, size, "ReplaceableId", static_cast<long32>(this->replaceableId()), 1);
	}

	if (this->wrapping() == Wrapping::WrapWidth || this->wrapping() == Wrapping::Both)
	{
		writeMdlProperty(ostream, size, "WrapWidth", 1);
	}

	if (this->wrapping() == Wrapping::WrapHeight || this->wrapping() == Wrapping::Both)
	{
		writeMdlProperty(ostream, size, "WrapHeight", 1);
	}

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize Texture::readMdx(istream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, reinterpret_cast<long32&>(this->m_replaceableId), size);
	wc3lib::read(istream, this->m_texturePath, size, texturePathSize);
	wc3lib::read(istream, this->m_unknown0, size);
	wc3lib::read(istream, reinterpret_cast<long32&>(this->m_wrapping), size);

	return size;
}

std::streamsize Texture::writeMdx(ostream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, static_cast<long32>(this->replaceableId()), size);
	wc3lib::write(ostream, this->texturePath(), size, texturePathSize);
	wc3lib::write(ostream, this->unknown0(), size);
	wc3lib::write(ostream, static_cast<long32>(this->wrapping()), size);

	return size;
}

}

}
