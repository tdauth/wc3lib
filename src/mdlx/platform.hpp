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

namespace wc3lib
{

namespace mdlx
{

/// @todo Check signed and unsigned!
typedef uint16 short16; /// @todo undefined length?!
typedef uint32 long32;

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

/*
 * #ifndef WC3LIB_MDLX_FORMAT_HPP
#define WC3LIB_MDLX_FORMAT_HPP

#include <sstream>

#include <boost/algorithm/string/trim.hpp>

#include "platform.hpp"
#include "../format.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

class Format : public Format<byte>
{
	template<typename T>
	static InputStream readMdl(InputStream &istream, T &value, std::streamsize &sizeCounter) throw (class Exception)
	{
		string value;
		bool isInComment = false;

		do
		{
			readString(istream, stringValue, sizeCounter);

			if (isInComment && value != "\n")
				continue;

			if (value == "//")
				isInComment = true;
		}
		while (value == ' ' || value == '\t' || value == ',' || value == '{' || value == '}' || value == "static" || isInComment);

		boost::trim_if(value, boost::is_any_of("\""));

		std::basic_ostringstream<byte> osstream(value);
		osstream >> value;

		return istream;
	}
};

}

}
*/

inline istream& parseMdlKeyword(istream &stream, const string &keyword, std::streamsize &sizeCounter, bool optional = false) throw (class Exception)
{
	string identifier;
	std::streamsize counter = 0;
	std::streampos position = stream.tellg();
	parse(stream, identifier, counter);

	if (identifier != keyword)
	{
		if (optional)
		{
			stream.seekg(position);

			return stream;
		}
		else
			throw Exception(boost::format(_("MDL Expression \"%1%\" does not match keyword \"%2%\".")) % identifier % keyword);
	}

	sizeCounter += counter;

	return stream;
}

inline istream& parseMdlString(istream &stream, string &out, std::streamsize &sizeCounter, bool optional = false) throw (class Exception)
{
	string identifier;
	std::streamsize counter = 0;
	std::streampos position = stream.tellg();
	parse(stream, identifier, counter);

	if (identifier.size() < 2 || identifier[0] != '"' || identifier[identifier.size() - 1] != '"')
	{
		if (optional)
		{
			stream.seekg(position);

			return stream;
		}
		else
			throw Exception(boost::format(_("MDL String \"%1%\" has not the correct format.")) % identifier);
	}

	out = identifier;
	sizeCounter += counter;

	return stream;
}

inline istream& parseMdlChar(istream &stream, const byte &character, std::streamsize &sizeCounter, bool optional = false) throw (class Exception)
{
	std::streamsize counter = 0;
	std::streampos position = stream.tellg();
	byte readChar;
	parse(stream, readChar, counter);

	if (readChar != character)
	{
		if (optional)
		{
			stream.seekg(position);

			return stream;
		}
		else
			throw Exception(boost::format(_("\'%1%\' is not equal to required character \'%2%\'.")) % readChar % character);
	}

	sizeCounter += counter;

	return stream;
}

inline istream& parseMdlNamedBlock(istream &stream, const string &blockName, string &name, std::streamsize &sizeCounter, bool optional = false) throw (class Exception)
{
	std::streamsize size = sizeCounter;
	parseMdlKeyword(stream, blockName, sizeCounter, optional);

	if (sizeCounter == size) // optional, not found
		return stream;

	parseMdlString(stream, name, sizeCounter);
	parseMdlChar(stream, '{', sizeCounter);

	return stream;
}

struct MdlxPropertyInformation
{
	BOOST_SCOPED_ENUM_START(Type)
	{
		None, /// Property does not expect any value.
		Byte,
		Float,
		Short,
		Long,
		String
	};
	BOOST_SCOPED_ENUM_END

	const string &identifier;
	const bool isStatic;
	const BOOST_SCOPED_ENUM(Type) &type;
	void *reference; /// Store corresponding pointer to value here which will be changed when property is found.
	bool found;

};

/**
 * Searches for MDL properties in input stream \p stream from list \p properties.
 * If property is being found it's member \ref MdlxPropertyInformation::found is being changed to true.
 */
inline istream& parseMdlProperty(istream &stream, std::list<struct MdlxPropertyInformation> &properties, std::streamsize &sizeCounter) throw (class Exception)
{
	string identifier;
	std::streamsize counter = 0;
	std::streampos position = stream.tellg();
	parse(stream, identifier, counter);

	BOOST_FOREACH(MdlxPropertyInformation &information, properties)
	{
		if (information.identifier == identifier || (information.isStatic && identifier == "static"))
		{
			if (information.isStatic)
			{
				string newIdentifier;
				std::streampos newPosition = stream.tellg();
				std::streamsize newCounter = 0;
				parse(stream, newIdentifier, newCounter);

				// reset
				if (information.identifier != newIdentifier)
				{
					stream.seekg(newPosition);

					continue;
				}
				else
					counter += newCounter;
			}
			else if (information.found)
				throw Exception(boost::format(_("Doubled property \"%1%\".")) % information.identifier);

			// property requires value
			if (information.type != MdlxPropertyInformation::Type::None)
			{
				if (information.reference == 0)
					throw Exception(boost::format(_("Reference is 0.")));

				parse(stream, identifier, counter);
				//std::basic_ostringstream<byte> ostringstream(identifier);
				try
				{
					// "byte", "float", "long" or "string"
					switch (information.type)
					{
						case MdlxPropertyInformation::Type::Byte:
							*static_cast<byte*>(information.reference) = boost::lexical_cast<byte>(identifier);

							break;
						case MdlxPropertyInformation::Type::Float:
							*static_cast<float32*>(information.reference) = boost::lexical_cast<float32>(identifier);

							break;

						case MdlxPropertyInformation::Type::Short:
							*static_cast<short16*>(information.reference) = boost::lexical_cast<short16>(identifier);

							break;

						case MdlxPropertyInformation::Type::Long:
							*static_cast<long32*>(information.reference) = boost::lexical_cast<long32>(identifier);

							break;

						case MdlxPropertyInformation::Type::String:
							*static_cast<string*>(information.reference) = boost::lexical_cast<string>(identifier);

							break;
					}
				}
				catch (boost::bad_lexical_cast &exception)
				{
					throw Exception(boost::format(_("MDLX property value cast exception: \"%1%\".")) % exception.what());
				}
			}

			byte commata;
			parse(stream, commata, counter);

			if (commata != ',')
				throw Exception(boost::format(_("%1% is no commata.")) % commata);

			information.found = true;
			sizeCounter += counter;

			return stream;
		}
	}

	// no value has been found
	stream.seekg(position);

	return stream;
}

inline ostream& writeStringStream(ostream &stream, ostringstream &sstream, std::streamsize &size)
{
	return wc3lib::write(stream, sstream.str().c_str()[0], size, sstream.str().length()); // without 0-terminating character!
}

inline ostringstream& writeMdlDepth(ostringstream &stream, std::size_t depth)
{
	for (std::size_t i = 0; i < depth; ++i)
		stream << "\t";

	return stream;
}

inline ostream& writeMdlBlock(ostream &stream, std::streamsize &size, const string &keyword, const string &identifier = "", std::size_t depth = 0)
{
	ostringstream sstream;
	writeMdlDepth(sstream, depth);
	sstream << keyword << ' ';

	if (!identifier.empty())
	{
		sstream << '\"' << identifier << '\"' << ' ';
	}

	sstream << "{\n";

	return writeStringStream(stream, sstream, size);
}

inline ostream& writeMdlCountedBlock(ostream &stream, std::streamsize &size, const string &keyword, const long32 count, std::size_t depth = 0)
{
	ostringstream sstream;
	writeMdlDepth(sstream, depth);
	sstream << keyword << " " << count << " {\n";

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
	writeMdlDepth(sstream, depth);
	sstream << identifier << ",\n";

	return writeStringStream(stream, sstream, size);
}

/**
 * MDL value properties need exactly one corresponding value.
 * Examples are:
 * MaterialID <long>,
 * SelectionGroup <long>,
 * Image <string_path>,
 * ReplaceableId <long>,
 * ...
 */
template<typename T>
inline ostream& writeMdlValueProperty(ostream &stream, std::streamsize &size, const string &identifier, T value, std::size_t depth = 0)
{
	ostringstream sstream;
	writeMdlDepth(sstream, depth);
	sstream << identifier << ' ' << value << ",\n";

	return writeStringStream(stream, sstream, size);
}

template<typename T>
inline ostream& writeMdlValuePropertyWithQuotes(ostream &stream, std::streamsize &size, const string &identifier, T value, std::size_t depth = 0)
{
	ostringstream sstream;
	writeMdlDepth(sstream, depth);
	sstream << identifier << " \"" << value << "\",\n";

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
	ostringstream sstream;
	writeMdlDepth(sstream, depth);
	sstream << "static " << identifier << ' ' << value << ",\n";

	return writeStringStream(stream, sstream, size);
}

template<typename T>
inline ostream& writeMdlStaticValuePropertyWithQuotes(ostream &stream, std::streamsize &size, const string &identifier, T value, std::size_t depth = 0)
{
	ostringstream sstream;
	writeMdlDepth(sstream, depth);
	sstream << "static " << identifier << " \"" << value << "\",\n";

	return writeStringStream(stream, sstream, size);
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
ostream& writeMdlVectorProperty(ostream &stream, std::streamsize &size, const string &identifier, const VertexType &values, std::size_t depth = 0, const string prefix = "")
{
	ostringstream sstream;
	writeMdlDepth(sstream, depth);

	if (!identifier.empty())
		sstream << prefix << identifier << ' ';

	if (values.size() > 1)
		sstream << "{ ";

	std::size_t i = 0;

	BOOST_FOREACH(typename VertexType::const_reference v, values)
	{
		sstream << v;

		if (i == values.size() - 1)
			sstream << " ";
		else
			sstream << ", ";

		++i;
	}

	if (values.size() == 1)
		sstream << "\n";
	else
		sstream << "}\n";

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

inline ostream& writeMdlBlockConclusion(ostream &stream, std::streamsize &size, std::size_t depth = 0)
{
	ostringstream sstream;
	writeMdlDepth(sstream, depth);
	sstream << "}\n";

	return writeStringStream(stream, sstream, size);
}

}

}

#endif
