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

#include "texturepatch.hpp"
#include "texturepatches.hpp"
#include "texturevertices.hpp"

namespace wc3lib
{

namespace mdlx
{

TexturePatch::TexturePatch(class TexturePatches *texturePatches) : m_texturePatches(texturePatches), m_textureVertices(new TextureVertices(this->texturePatches()->geoset()))
{
}

TexturePatch::~TexturePatch()
{
	delete this->m_textureVertices;
}

std::streamsize TexturePatch::readMdl(istream &istream)
{
	return this->m_textureVertices->readMdl(istream);
}

std::streamsize TexturePatch::writeMdl(ostream &ostream) const
{
	return this->m_textureVertices->writeMdx(ostream);
}

std::streamsize TexturePatch::readMdx(istream &istream)
{
	return this->m_textureVertices->readMdx(istream);
}

std::streamsize TexturePatch::writeMdx(ostream &ostream) const
{
	return this->m_textureVertices->writeMdx(ostream);
}

}

}
