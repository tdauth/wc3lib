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

#include "textureids.hpp"
#include "textureid.hpp"

namespace wc3lib
{

namespace mdlx
{

TextureIds::TextureIds(class Layer *layer) : MdxBlock("KMTF", "TextureID"), m_layer(layer)
{
}

TextureIds::~TextureIds()
{
	BOOST_FOREACH(TextureId *textureId, m_textureIds)
		delete textureId;
}

std::streamsize TextureIds::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize TextureIds::writeMdl(ostream &ostream) const throw (class Exception)
{
	return 0;
}

std::streamsize TextureIds::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = MdxBlock::readMdx(istream);

	if (size == 0)
		return 0;

	long32 number;
	wc3lib::read(istream, number, size);
	wc3lib::read(istream, *reinterpret_cast<long32*>(&this->m_lineType), size);
	wc3lib::read(istream, this->m_globalSequenceId, size);

	for ( ; number > 0; --number)
	{
		class TextureId *textureId = new TextureId(this);
		size += textureId->readMdx(istream);
		this->m_textureIds.push_back(textureId);
	}

	return size;
}

std::streamsize TextureIds::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = MdxBlock::writeMdx(ostream);
	long32 number = boost::numeric_cast<long32>(this->m_textureIds.size());
	wc3lib::write(ostream, *reinterpret_cast<const long32*>(&number), size);
	wc3lib::write(ostream, *reinterpret_cast<const long32*>(&this->m_lineType), size);
	wc3lib::write(ostream, this->m_globalSequenceId, size);

	BOOST_FOREACH(const class TextureId *textureId, this->m_textureIds)
		size += textureId->writeMdx(ostream);

	return size;
}

}

}
