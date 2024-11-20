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

#include "layer.hpp"
#include "mdlx.hpp"

namespace wc3lib
{

namespace mdlx
{

Layer::Layer() : m_filterMode(FilterMode::None), m_shading(Shading::Unshaded), m_textureId(0), m_tvertexAnimationId(0xFFFFFFFF), m_coordinatesId(0), m_alpha(0.0)
{
}

std::streamsize Layer::read(InputStream &istream, long32 version)
{
	std::streamsize size = 0;
	long32 inclusiveSize = 0;
	wc3lib::read(istream, inclusiveSize, size);
	wc3lib::read(istream, m_filterMode, size);
	wc3lib::read(istream, m_shading, size);
	wc3lib::read(istream, m_textureId, size);
	wc3lib::read(istream, m_tvertexAnimationId, size);
	wc3lib::read(istream, m_coordinatesId, size);
	wc3lib::read(istream, m_alpha, size);

	if (version > 800)
	{
		wc3lib::read(istream, m_emissiveGain, size);
		size += m_fresnelColor.read(istream);
		wc3lib::read(istream, m_fresnelOpacity, size);
		wc3lib::read(istream, m_fresnelTeamColor, size);
	}

	auto p = istream.tellg();
	char8_t tag[MDX_TAG_SIZE];
	std::streamsize headerSize = 0;
	std::cerr << "Reading header at " << istream.tellg() << std::endl;
	wc3lib::read(istream, tag, headerSize);
	bool foundOptional = false;

	// seems optional
	if (isMdxTag(tag, u8"KMTF"))
	{
		foundOptional = true;
		size += headerSize;
		size += m_textureIds.read(istream);
		std::cerr << "Reading header 2 at " << istream.tellg() << std::endl;
		wc3lib::read(istream, tag, headerSize);
	}

	// seems optional
	if (isMdxTag(tag, u8"KMTA"))
	{
		foundOptional = true;
		size += headerSize;
		std::cerr << "Reading alphas at " << istream.tellg() << std::endl;
		size += m_alphas.read(istream);
	}

	if (!foundOptional)
	{
		std::cerr << "Moving back to layer position " << p << std::endl;
		istream.seekg(p); // move back to get the header again
	}

	if (version > 800)
	{
		readMdxTag(istream, u8"KMTE", size);
		wc3lib::read(istream, m_kmteEmissiveGain, size);
	}

	if (version > 900)
	{
		readMdxTag(istream, u8"KFC3", size);
		wc3lib::read(istream, m_kfcaFresnelAlpha, size);

		readMdxTag(istream, u8"KFCA", size);
		wc3lib::read(istream, m_kfcaFresnelAlpha, size);

		readMdxTag(istream, u8"KFTC", size);
		wc3lib::read(istream, m_kftcFresnelTeamColor, size);
	}

	skipMdxInclusiveEmptyBytes(istream, inclusiveSize, size);

	return size;
}

std::streamsize Layer::write(OutputStream &ostream, long32 version) const
{
	std::streamsize size = 0;
	auto p = ostream.tellp();
	ostream.seekp(sizeof(long32), std::ios_base::cur);

	wc3lib::write(ostream, m_filterMode, size);
	wc3lib::write(ostream, m_shading, size);
	wc3lib::write(ostream, m_textureId, size);
	wc3lib::write(ostream, m_tvertexAnimationId, size);
	wc3lib::write(ostream, m_coordinatesId, size);
	wc3lib::write(ostream, m_alpha, size);

	if (version > 800)
	{
		wc3lib::write(ostream, m_emissiveGain, size);
		size += m_fresnelColor.write(ostream);
		wc3lib::write(ostream, m_fresnelOpacity, size);
		wc3lib::write(ostream, m_fresnelTeamColor, size);
	}

	if (m_textureIds.properties().size() > 0)
	{
		writeMdxTag(ostream, u8"KMTF", size);
		size += m_textureIds.write(ostream);
	}

	if (m_alphas.properties().size() > 0)
	{
		writeMdxTag(ostream, u8"KMTA", size);
		size += m_alphas.write(ostream);
	}

	if (version > 800)
	{
		writeMdxTag(ostream, u8"KMTE", size);
		wc3lib::write(ostream, m_kmteEmissiveGain, size);
	}

	if (version > 900)
	{
		writeMdxTag(ostream, u8"KFC3", size);
		wc3lib::write(ostream, m_kfcaFresnelAlpha, size);

		writeMdxTag(ostream, u8"KFCA", size);
		wc3lib::write(ostream, m_kfcaFresnelAlpha, size);

		writeMdxTag(ostream, u8"KFTC", size);
		wc3lib::write(ostream, m_kftcFresnelTeamColor, size);
	}

	const long32 inclusiveSize = size + sizeof(long32);
	auto p2 = ostream.tellp();
	ostream.seekp(p);
	wc3lib::write(ostream, inclusiveSize, size);
	ostream.seekp(p2);

	return size;
}

std::streamsize Layer::read(InputStream &istream)
{
	return read(istream, Mdlx::currentVersion);
}

std::streamsize Layer::write(OutputStream &ostream) const
{
	return write(ostream, Mdlx::currentVersion);
}

}

}
