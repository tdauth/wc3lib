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

#include <cstring>

#include "mdlx.hpp"

namespace wc3lib
{

namespace mdlx
{

const long32 Mdlx::tftVersion = 800;
const long32 Mdlx::reforgedVersion = 901;
const long32 Mdlx::currentVersion = 800;

Mdlx::Mdlx() : m_modelVersion(currentVersion)
{
}

Mdlx::~Mdlx()
{
}

// Based on information from https://www.hiveworkshop.com/threads/mdx-specifications.240487/
std::streamsize Mdlx::read(InputStream &istream)
{
	std::streamsize size = 0;
	char8_t tag[MDX_TAG_SIZE];
	wc3lib::read(istream, tag, size);
	expectMdxTag(istream, tag, u8"MDLX");

	auto p = istream.tellg();
	istream.seekg(0, std::ios::end);
	auto end = istream.tellg();
	istream.seekg(p);

	while (istream && istream.tellg() < end)
	{
		p = istream.tellg();
		MdxHeader header;
		wc3lib::read(istream, header, size);

		std::cerr << "Found tag " << header.readableTag() << " at position " << p << std::endl;

		if (isMdxTag(header.tag, u8"VERS"))
		{
			std::cerr << "Before reading VERS with previous position " << p << std::endl;
			std::streamsize modelVersionSize = 0;
			wc3lib::read(istream, m_modelVersion, modelVersionSize);
			skipMdxHeaderEmptyBytes(istream, header, modelVersionSize);
			size += modelVersionSize;
		}
		else if (isMdxTag(header.tag, u8"MODL"))
		{
			std::cerr << "Before reading MODL with previous position " << p << std::endl;
			istream.seekg(p);
			size += m_model.read(istream);
		}
		else if (isMdxTag(header.tag, u8"SEQS"))
		{
			m_sequences.clear();

			while (header.size > 0)
			{
				Sequence sequence;
				const std::streamsize s = sequence.read(istream);
				header.size -= s;
				size += s;
				m_sequences.push_back(sequence);
			}
		}
		else if (isMdxTag(header.tag, u8"GLBS"))
		{
			m_globalSequences.clear();

			while (header.size > 0)
			{
				long32 globalSequence = 0;
				std::streamsize s = 0;
				wc3lib::read(istream, globalSequence, s);
				header.size -= s;
				size += s;
				m_globalSequences.push_back(globalSequence);
			}
		}
		else if (isMdxTag(header.tag, u8"MTLS"))
		{
			m_materials.clear();

			while (header.size > 0)
			{
				Material material;
				const std::streamsize s = material.read(istream, m_modelVersion);
				header.size -= s;
				size += s;
				m_materials.push_back(material);
			}
		}
		else if (isMdxTag(header.tag, u8"TXAN"))
		{
			m_textureAnimations.clear();

			while (header.size > 0)
			{
				TextureAnimation textureAnimation;
				const std::streamsize s = textureAnimation.read(istream);
				header.size -= s;
				size += s;
				m_textureAnimations.push_back(textureAnimation);
			}
		}
		else if (isMdxTag(header.tag, u8"GEOS"))
		{
			m_geosets.clear();

			while (header.size > 0)
			{
				Geoset geoset;
				const std::streamsize s = geoset.read(istream);
				header.size -= s;
				size += s;
				m_geosets.push_back(geoset);
			}
		}
		else if (isMdxTag(header.tag, u8"TEXS"))
		{
			m_textures.clear();

			while (header.size > 0)
			{
				Texture texture;
				const std::streamsize s = texture.read(istream);
				header.size -= s;
				size += s;
				m_textures.push_back(texture);
			}
		}
		else if (isMdxTag(header.tag, u8"SNDS"))
		{
			// Note that this is here for completeness' sake.
			// These objects were only used at some point before Warcraft 3 released.
			m_soundTracks.clear();

			while (header.size > 0)
			{
				SoundTrack soundTrack;
				const std::streamsize s = soundTrack.read(istream);
				header.size -= s;
				size += s;
				m_soundTracks.push_back(soundTrack);
			}
		}
		else if (isMdxTag(header.tag, u8"PIVT"))
		{
			m_pivotPoints.clear();

			while (header.size > 0)
			{
				VertexData pivotPoint;
				const std::streamsize s = pivotPoint.read(istream);
				header.size -= s;
				size += s;
				m_pivotPoints.push_back(pivotPoint);
			}
		}
		else
		{
			std::cerr << "Unknown tag " << header.readableTag() << " at position " << istream.tellg() << std::endl;

			// try the next byte as start
			if (istream.tellg() < end - std::ios::pos_type(1))
			{
				istream.seekg(p + std::ios:: pos_type(1));
			}
			else
			{
				std::cerr << "Reached the end " << end << " at " << istream.tellg() << std::endl;

				break;
			}
		}
	}

	return size;
}

std::streamsize Mdlx::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	writeMdxTag(ostream, u8"MDLX", size);

	// VERS
	writeMdxHeader(ostream, size, u8"VERS", sizeof(m_modelVersion));
	wc3lib::write(ostream, m_modelVersion, size);

	// MODL
	size += m_model.write(ostream);

	// SEQS
	auto p = skipMdxHeader(ostream);
	std::streamsize sequencesSize = 0;

	for (const Sequence &sequence : m_sequences)
	{
		sequencesSize += sequence.write(ostream);
	}

	auto p2 = ostream.tellp();
	ostream.seekp(p);
	writeMdxHeader(ostream, size, u8"SEQS", sequencesSize);
	ostream.seekp(p2);
	size += sequencesSize;

	// GLBS
	p = skipMdxHeader(ostream);
	std::streamsize globalSequencesSize = 0;

	for (const long32 &globalSequence : m_globalSequences)
	{
		wc3lib::write(ostream, globalSequence, globalSequencesSize);
	}

	p2 = ostream.tellp();
	ostream.seekp(p);
	writeMdxHeader(ostream, size, u8"GLBS", globalSequencesSize);
	ostream.seekp(p2);
	size += globalSequencesSize;

	// MTLS
	p = skipMdxHeader(ostream);
	std::streamsize materialsSize = 0;

	for (const Material &material : m_materials)
	{
		materialsSize += material.write(ostream, m_modelVersion);
	}

	p2 = ostream.tellp();
	ostream.seekp(p);
	writeMdxHeader(ostream, size, u8"MTLS", materialsSize);
	ostream.seekp(p2);
	size += materialsSize;
	
	// TXAN
	p = skipMdxHeader(ostream);
	std::streamsize textureAnimationsSize = 0;

	for (const TextureAnimation &textureAnimation : m_textureAnimations)
	{
		textureAnimationsSize += textureAnimation.write(ostream);
	}

	p2 = ostream.tellp();
	ostream.seekp(p);
	writeMdxHeader(ostream, size, u8"TXAN", textureAnimationsSize);
	ostream.seekp(p2);
	size += textureAnimationsSize;
	
	// GEOS
	p = skipMdxHeader(ostream);
	std::streamsize geosetsSize = 0;

	for (const Geoset &geoset : m_geosets)
	{
		geosetsSize += geoset.write(ostream);
	}

	p2 = ostream.tellp();
	ostream.seekp(p);
	writeMdxHeader(ostream, size, u8"GEOS", geosetsSize);
	ostream.seekp(p2);
	size += geosetsSize;

	// TEXS
	p = skipMdxHeader(ostream);
	std::streamsize texturesSize = 0;

	for (const Texture &texture : m_textures)
	{
		texturesSize += texture.write(ostream);
	}

	p2 = ostream.tellp();
	ostream.seekp(p);
	writeMdxHeader(ostream, size, u8"TEXS", texturesSize);
	ostream.seekp(p2);
	size += texturesSize;

	// SNDS
	p = skipMdxHeader(ostream);
	std::streamsize soundTracksSize = 0;

	for (const SoundTrack &soundTrack : m_soundTracks)
	{
		soundTracksSize += soundTrack.write(ostream);
	}

	p2 = ostream.tellp();
	ostream.seekp(p);
	writeMdxHeader(ostream, size, u8"SNDS", soundTracksSize);
	ostream.seekp(p2);
	size += soundTracksSize;

	// PIVT
	p = skipMdxHeader(ostream);
	std::streamsize pivotPointsSize = 0;

	for (const VertexData &pivotPoint : m_pivotPoints)
	{
		pivotPointsSize += pivotPoint.write(ostream);
	}

	p2 = ostream.tellp();
	ostream.seekp(p);
	writeMdxHeader(ostream, size, u8"PIVT", pivotPointsSize);
	ostream.seekp(p2);
	size += pivotPointsSize;

	return size;
}

}

}
