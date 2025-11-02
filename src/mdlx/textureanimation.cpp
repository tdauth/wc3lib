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

#include "textureanimation.hpp"

namespace wc3lib
{

namespace mdlx
{

TextureAnimation::TextureAnimation()
{
}

std::streamsize TextureAnimation::read(InputStream &istream)
{
	std::streamsize size = 0;
	long32 inclusiveSize = 0;
	wc3lib::read(istream, inclusiveSize, size);
	long32 remainingSize = inclusiveSize - sizeof(inclusiveSize);

	// All blocks are optional:
	m_hasTranslations = false;
	m_hasRotations = false;
	m_hasScalings = false;

	char8_t tag[MDX_TAG_SIZE];

	while (remainingSize >= MDX_TAG_SIZE) {
		auto p = istream.tellg();
		std::streamsize tagSize = 0;
		wc3lib::read(istream, tag, tagSize);
		std::cerr << "Remaining size " << remainingSize << " with tag " << readableMdxTag(tag) << std::endl;

		if (isMdxTag(tag, u8"KTAT")) {
			remainingSize -= tagSize;
			size += tagSize;

			m_hasTranslations = true;
			std::streamsize blockSize = 0;
			blockSize = m_translations.read(istream);
			remainingSize -= blockSize;
			size += blockSize;
		} else if (isMdxTag(tag, u8"KTAR")) {
			remainingSize -= tagSize;
			size += tagSize;

			m_hasRotations = true;
			std::streamsize blockSize = 0;
			blockSize = m_rotations.read(istream);
			remainingSize -= blockSize;
			size += blockSize;
		} else if (isMdxTag(tag, u8"KTAS")) {
			remainingSize -= tagSize;
			size += tagSize;

			m_hasScalings = true;
			std::streamsize blockSize = 0;
			blockSize = m_scalings.read(istream);
			remainingSize -= blockSize;
			size += blockSize;
		} else {
			istream.seekg(p);
			throw std::runtime_error("Unexpected tag " + readableMdxTag(tag));
		}
	}

	std::cerr << "Remaining size " << remainingSize << " at the end of texture animation." << std::endl;

	skipMdxInclusiveEmptyBytes(istream, inclusiveSize, size);

	return size;
}

std::streamsize TextureAnimation::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	auto p = ostream.tellp();
	
	if (hasTranslations()) {
		wc3lib::write(ostream, u8"KTAT", size);
		size += m_translations.write(ostream);
	}

	if (hasRotations()) {
		wc3lib::write(ostream, u8"KTAR", size);
		size += m_rotations.write(ostream);
	}

	if (hasScalings()) {
		wc3lib::write(ostream, u8"KTAS", size);
		size += m_scalings.write(ostream);
	}
    
	const long32 inclusiveSize = size + sizeof(long32);
	auto p2 = ostream.tellp();
	ostream.seekp(p);
	wc3lib::write(ostream, inclusiveSize, size);
	ostream.seekp(p2);
	ostream.seekp(sizeof(long32), std::ios_base::cur);

	return size;
}

}

}
