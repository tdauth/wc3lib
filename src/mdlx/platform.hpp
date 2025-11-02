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

#ifndef WC3LIB_MDLX_PLATFORM_HPP
#define WC3LIB_MDLX_PLATFORM_HPP

#include <iomanip>
#include <cuchar>

#include <boost/foreach.hpp>

#include "../platform.hpp"
#include "../vertex.hpp"
#include "../utilities.hpp"
#include "../format.hpp"

namespace wc3lib
{

// TODO C++11
/*
extern template class BasicFormat<byte>; // increase compile time
extern template class BasicVertex<float32, 2>;
extern template class BasicVertex<float32, 3>;
extern template class BasicVertex<float32, 4>;
*/

namespace mdlx
{

// have to be signed because of PriorityPlane -1 and other examples of negative values
typedef int16 short16; /// @todo undefined length?!
typedef int32 long32;

const long32 noneId = 0xFFFFFFFF;

const std::size_t MDX_TAG_SIZE = 4;

inline std::string readableMdxTag(const char8_t tag[MDX_TAG_SIZE])
{
	char8_t r[MDX_TAG_SIZE + 1];
	std::memcpy(&r, tag, sizeof(char8_t) * MDX_TAG_SIZE);
	r[MDX_TAG_SIZE] = 0;

	return std::string(reinterpret_cast<const char*>(r));
}

/**
 * \brief Each chunk starts with a header that consists of the chunk tag and the chunk size in bytes, not including the size of the header itself.
 */
struct MdxHeader
{
	char8_t tag[MDX_TAG_SIZE];
	uint32 size;

	void setTag(const char8_t *t)
	{
		assert(std::u8string(t).size() == MDX_TAG_SIZE);
		std::memcpy(&tag, t, sizeof(char8_t) * MDX_TAG_SIZE);
	}

	std::string readableTag() const
	{
		return readableMdxTag(tag);
	}
};

inline bool isMdxTag(const char8_t tag[MDX_TAG_SIZE], const char8_t *expected)
{
	return std::memcmp(tag, expected, MDX_TAG_SIZE) == 0;
}

inline void expectMdxTag(Format::InputStream &istream, const char8_t tag[MDX_TAG_SIZE], const char8_t *expected)
{
	if (!isMdxTag(tag, expected))
    {
		throw std::runtime_error(std::string("Expected MDX tag ") + reinterpret_cast<const char*>(expected) + " at stream position " + std::to_string(istream.tellg()));
	}
}

inline MdxHeader readMdxHeader(Format::InputStream &istream, std::streamsize &sizeCounter, const char8_t *expectedTag)
{
	MdxHeader header;
	wc3lib::read(istream, header, sizeCounter);
	expectMdxTag(istream, header.tag, expectedTag);

	return header;
}

inline MdxHeader writeMdxHeader(Format::OutputStream &ostream, std::streamsize &sizeCounter, const char8_t *expectedTag, uint32 headerSize)
{
	MdxHeader header;
	header.setTag(expectedTag);
	header.size = headerSize;
	wc3lib::write(ostream, header, sizeCounter);

	return header;
}

inline void skipMdxInclusiveEmptyBytes(Format::InputStream &istream, const long32 &inclusiveSize, const std::streamsize &size)
{
	const std::streamsize emptyBytes = inclusiveSize - size;

	if (emptyBytes > 0)
    {
		std::cout << "Skipping " << emptyBytes << " empty bytes." << std::endl;
		istream.seekg(emptyBytes, std::ios_base::cur);
	}
}

inline void skipMdxHeaderEmptyBytes(Format::InputStream &istream, const MdxHeader &header, const std::streamsize &size)
{
	const std::streamsize emptyBytes = header.size - size;

    if (emptyBytes > 0)
    {
        istream.seekg(emptyBytes, std::ios_base::cur);
    }
}

inline void readMdxTag(Format::InputStream &istream, const char8_t *expectedTag, std::streamsize &size)
{
	char8_t tag[MDX_TAG_SIZE];
	wc3lib::read(istream, tag, size, MDX_TAG_SIZE * sizeof(char8_t));
	expectMdxTag(istream, tag, expectedTag);
}

inline void writeMdxTag(Format::OutputStream &ostream, const char8_t *tag, std::streamsize &size)
{
	wc3lib::write(ostream, tag, size, sizeof(char8_t) * MDX_TAG_SIZE);
}

inline Format::OutputStream::pos_type skipMdxHeader(Format::OutputStream &ostream)
{
	auto p = ostream.tellp();
	ostream.seekp(sizeof(MdxHeader), std::ios_base::cur);

	return p;
}

/**
 * \defgroup animations MDX and MDL animations
 * The 3D formats MDX and MDL support different types of animations to apply transformations on nodes at specific time frames.
 *
 */

/**
 * MDLX format supports interpolation for scalings, translations and rotations.
 * If interpolation is not used value should be DontInterpolate.
 * If line type is Hermite or Bezier additional interpolation data is used (see structures).
 * Line type is stored as long32 in MDX files.
 * \sa InterpolationData InterpolationRotationData
 * \ingroup animations
 */
enum class LineType : long32
{
	DontInterpolate = 0,
	Linear = 1,
	Hermite = 2,
	Bezier = 3
};

/**
 * \todo Finish enum.
 * \sa Texture, ParticleEmitter2
 */
enum class ReplaceableId : long32
{
	None = 0,
	TeamColor = 1,
	TeamGlow = 2,
	Cliff = 11,
	LordaeronTree = 31,
	AshenvaleTree = 32,
	BarrensTree = 33,
	NorthrendTree = 34,
	MushroomTree = 35
};

typedef Vertex3d<float32> VertexData;

struct InterpolationData //: public Format
{
	InterpolationData(const VertexData &inTan, const VertexData &outTan) : inTan(inTan), outTan(outTan)
	{
	}

	VertexData inTan;
	VertexData outTan;
};

typedef Quaternion<float32> QuaternionData;

struct InterpolationRotationData// : public Format
{
	InterpolationRotationData(const QuaternionData &inTan, const QuaternionData &outTan) : inTan(inTan), outTan(outTan)
	{
	}

	QuaternionData inTan;
	QuaternionData outTan;
};

}

}

#endif
