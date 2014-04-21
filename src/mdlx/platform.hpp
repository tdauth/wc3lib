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

#include <boost/detail/scoped_enum_emulation.hpp>
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

/**
 * MDLX format supports interpolation for scalings, translations and rotations.
 * If interpolation is not used value should be DontInterpolate.
 * If line type is Hermite or Bezier additional interpolation data is used (see structures).
 * Line type is stored as long32 in MDX files.
 * \sa InterpolationData, InterpolationRotationData
 */
BOOST_SCOPED_ENUM_START(LineType) /// \todo C++11 : long32
{
	DontInterpolate = 0,
	Linear = 1,
	Hermite = 2,
	Bezier = 3
};
BOOST_SCOPED_ENUM_END

/**
 * \todo Finish enum.
 * \sa Texture, ParticleEmitter2
 */
BOOST_SCOPED_ENUM_START(ReplaceableId) /// \todo C++11 : long32
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
BOOST_SCOPED_ENUM_END

typedef wc3lib::Vertex VertexData;
typedef wc3lib::BasicVertex<float32, 2> TextureVertexData;
class Vertex; // workaround, we already have a class called Vertex in MDLX module!

struct QuaternionData : public BasicVertex<float32, 4>
{
	QuaternionData(const Base& base)
	{
		*this = base;
	}
};

struct InterpolationData //: public Format
{
	InterpolationData(const VertexData &inTan, const VertexData &outTan) : inTan(inTan), outTan(outTan)
	{
	}

	VertexData inTan;
	VertexData outTan;
};

struct InterpolationRotationData// : public Format
{
	InterpolationRotationData(const QuaternionData &inTan, const QuaternionData &outTan) : inTan(inTan), outTan(outTan)
	{
	}

	QuaternionData inTan;
	QuaternionData outTan;
};

inline ostream& writeStringStream(ostream &stream, ostringstream &sstream, std::streamsize &size)
{
	return wc3lib::write(stream, sstream.str().c_str()[0], size, sstream.str().length()); // without 0-terminating character!
}

inline ostringstream& setupMdlFormat(ostringstream &stream)
{
	stream << std::fixed << std::setprecision(6);

	return stream;
}

inline ostringstream& writeMdlDepth(ostringstream &stream, std::size_t depth)
{
	for (std::size_t i = 0; i < depth; ++i)
		stream << "\t";

	return stream;
}

inline ostream& writeMdlBlock(ostream &stream, std::streamsize &size, const string &keyword, const string &identifier = "", std::size_t depth = 0, bool forceIdentifier = false)
{
	ostringstream sstream;
	setupMdlFormat(sstream);
	writeMdlDepth(sstream, depth);
	sstream << keyword << ' ';

	if (!identifier.empty() || forceIdentifier)
	{
		sstream << '\"' << identifier << '\"' << ' ';
	}

	sstream << "{\n";

	return writeStringStream(stream, sstream, size);
}

inline ostream& writeMdlCountedBlock(ostream &stream, std::streamsize &size, const string &keyword, const long32 count, std::size_t depth = 0)
{
	ostringstream sstream;
	setupMdlFormat(sstream);
	writeMdlDepth(sstream, depth);
	sstream << keyword << " " << count << " {\n";

	return writeStringStream(stream, sstream, size);
}

/**
 * Examples are:
 * Groups <count1> <count2>
 * Faces <count1> <count2>
 */
inline ostream& writeMdlCountedBlockDouble(ostream &stream, std::streamsize &size, const string &keyword, const long32 count1, const long32 count2, std::size_t depth = 0)
{
	ostringstream sstream;
	setupMdlFormat(sstream);
	writeMdlDepth(sstream, depth);
	sstream << keyword << ' ' << count1 << ' ' << count2 << " {\n";

	return writeStringStream(stream, sstream, size);
}

/**
 * MDL properties are usually called flags and do not necessarily appear in an MDL scope.
 * Examples are:
 * DropShadow,
 * Unselectable,
 * Billboarded,
 * ...
 */
inline ostream& writeMdlProperty(ostream &stream, std::streamsize &size, const string &identifier, std::size_t depth = 0)
{
	ostringstream sstream;
	setupMdlFormat(sstream);
	writeMdlDepth(sstream, depth);
	sstream << identifier << ",\n";

	return writeStringStream(stream, sstream, size);
}

/**
 * MDL value properties need exactly one corresponding value.
 * Examples are:
 * MaterialID <long>,
 * SelectionGroup <long>,
 * ReplaceableId <long>,
 * ...
 */
template<typename T>
inline ostream& writeMdlValueProperty(ostream &stream, std::streamsize &size, const string &identifier, T value, std::size_t depth = 0, const string &prefix = "")
{
	ostringstream sstream;
	setupMdlFormat(sstream);
	writeMdlDepth(sstream, depth);

	if (!identifier.empty())
		sstream << prefix << identifier << ' ';

	sstream << value << ",\n";

	return writeStringStream(stream, sstream, size);
}

/**
 * Properties like Image "path" need quotes around their string values.
 */
template<typename T>
inline ostream& writeMdlValuePropertyWithQuotes(ostream &stream, std::streamsize &size, const string &identifier, T value, std::size_t depth = 0, const string &prefix = "")
{
	ostringstream sstream;
	setupMdlFormat(sstream);
	writeMdlDepth(sstream, depth);

	if (!identifier.empty())
		sstream << prefix << identifier << ' ';

	sstream << " \"" << value << "\",\n";

	return writeStringStream(stream, sstream, size);
}

/**
 * Static MDL value properties are only written into file if property has no animated values.
 * Examples are:
 * static TextureID <long>,
 * static Alpha <float>,
 * ...
 */
template<typename T>
inline ostream& writeMdlStaticValueProperty(ostream &stream, std::streamsize &size, const string &identifier, T value, std::size_t depth = 0)
{
	return writeMdlValueProperty(stream, size, identifier, value, depth, "static ");
}

template<typename T>
inline ostream& writeMdlStaticValuePropertyWithQuotes(ostream &stream, std::streamsize &size, const string &identifier, T value, std::size_t depth = 0)
{
	return writeMdlValuePropertyWithQuotes(stream, size, identifier, value, depth, "static ");
}

/**
 * Examples are:
 * Visibility <float>
 * Translation { <float_x>, <float_y>, <float_z> }
 * Rotation { <float_a>, <float_b>, <float_c>, <float_d> }
 * Scaling { <float_x>, <float_y>, <float_z> }
 * or for pivot points
 * { <float_x>, <float_y>, <float_z> }
 */
template<typename VertexType> //  = BasicVertex<T, N>
ostream& writeMdlVectorProperty(ostream &stream, std::streamsize &size, const string &identifier, const VertexType &values, std::size_t depth = 0, const string &prefix = "")
{
	ostringstream sstream;
	setupMdlFormat(sstream);
	writeMdlDepth(sstream, depth);

	if (!identifier.empty())
		sstream << prefix << identifier << ' ';

	if (values.size() > 1)
		sstream << "{ ";

	std::size_t i = 0;

	BOOST_FOREACH(typename VertexType::const_reference v, values)
	{
		sstream << v;

		if (i != values.size() - 1)
			sstream << ", ";
		else if (values.size() > 1)
			sstream << " ";


		++i;
	}

	if (values.size() == 1)
		sstream << ",\n";
	else
		sstream << "},\n";

	return writeStringStream(stream, sstream, size);
}

/**
 * Static MDL vector properties are vector properties (usually required for 3 dimensional space and BGR color values) which are only written into file property has no animated values.
 * Examples are:
 * static Color { <float_b>, <float_g>, <float_r> },
 */
template<typename VertexType> //  = BasicVertex<T, N>
inline ostream& writeMdlStaticVectorProperty(ostream &stream, std::streamsize &size, const string &identifier, const VertexType &values, std::size_t depth = 0)
{
	return writeMdlVectorProperty(stream, size, identifier, values, depth, "static ");
}

/**
 * This function is required for values with non-static size which is calculated at runtime.
 * Examples are:
 * Visibility <float>
 * Matrices { <long>, <long>, <long> },
 */
template<typename ValueType> //  = BasicVertex<T, N>
ostream& writeMdlVectorProperty(ostream &stream, std::streamsize &size, const string &identifier, const std::vector<ValueType> &values, std::size_t depth = 0, const string &prefix = "", bool forceBrackets = false)
{
	ostringstream sstream;
	setupMdlFormat(sstream);
	writeMdlDepth(sstream, depth);

	if (!identifier.empty())
		sstream << prefix << identifier << ' ';

	if (values.size() > 1 || forceBrackets)
		sstream << "{ ";

	std::size_t i = 0;

	BOOST_FOREACH(typename std::vector<ValueType>::const_reference v, values)
	{
		sstream << v;

		if (i != values.size() - 1)
			sstream << ", ";
		else if (values.size() > 1)
			sstream << " ";

		++i;
	}

	if (values.size() == 1 && !forceBrackets)
	{
		sstream << ",\n";
	}
	else
	{
		if (values.size() == 1)
			sstream << ' ';

		sstream << "},\n";
	}

	return writeStringStream(stream, sstream, size);
}

inline ostream& writeMdlBlockConclusion(ostream &stream, std::streamsize &size, std::size_t depth = 0)
{
	ostringstream sstream;
	setupMdlFormat(sstream);
	writeMdlDepth(sstream, depth);
	sstream << "}\n";

	return writeStringStream(stream, sstream, size);
}

inline ostream& writeMdlPropertyBlockConclusion(ostream &stream, std::streamsize &size, std::size_t depth = 0)
{
	ostringstream sstream;
	setupMdlFormat(sstream);
	writeMdlDepth(sstream, depth);
	sstream << "},\n";

	return writeStringStream(stream, sstream, size);
}

}

}

#endif
