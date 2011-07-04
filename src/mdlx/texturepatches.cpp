/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
 *   tamino@cdauth.de                                                      *
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

#include "texturepatches.hpp"
#include "geoset.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

TexturePatches::TexturePatches(class Geoset *geoset) : MdxBlock("UVAS", ""), m_geoset(geoset)
{
}

TexturePatches::~TexturePatches()
{
}

std::streamsize TexturePatches::readMdl(InputStream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize TexturePatches::writeMdl(OutputStream &ostream) const throw (class Exception)
{
	return 0;
}

std::streamsize TexturePatches::readMdx(InputStream &istream) throw (class Exception)
{
	std::streamsize size = MdxBlock::readMdx(istream);

	if (size == 0)
		return 0;

	wc3lib::read(istream, this->m_number, size);
	// According to Mago's specification it's followed by nothing (works correctly)!

	/*

	if (ntvrts <= 0)
	{
		char message[50];
		sprintf(message, _("Texture Patches: 0 byte texture patches.\n"));

		throw Exception(message);
	}

	bytes += istream.gcount();
	std::cout << "Before " << ntvrts << " texture patches." << std::endl;

	for ( ; ntvrts > 0; --ntvrts)
	{
		class TexturePatch *texturePatch = new TexturePatch(this);
		bytes += texturePatch->readMdx(istream);
		this->m_texturePatches.push_back(texturePatch);
	}

	std::cout << "After texture patches." << std::endl;
	*/

	return size;
}

std::streamsize TexturePatches::writeMdx(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = MdxBlock::writeMdx(ostream);

	if (size == 0)
		return 0;

	wc3lib::write(ostream, this->number(), size);

	return size;
}

}

}
