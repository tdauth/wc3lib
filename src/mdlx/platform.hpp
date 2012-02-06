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

#include "../core.hpp"

namespace wc3lib
{

namespace mdlx
{

/// @todo Check signed and unsigned!
/**
* MDL notes:
* Floating point numbers are in IEEE scientific notation format with 6 significant
* figures. Exponent is not shown for exponents of ±4. If the number is an integral
* float, decimal point is not shown.
*/
typedef float float32;
typedef uint16_t short16; /// @todo undefined length?!
typedef uint32_t long32;
typedef char ascii;
typedef char byte;
typedef std::basic_string<ascii> string; /// Required for MDL format.
typedef std::basic_istringstream<ascii> istringstream;
typedef std::basic_ostringstream<ascii> ostringstream;
typedef std::basic_stringstream<ascii> stringstream;
typedef std::basic_istream<byte> istream;
typedef std::basic_ostream<byte> ostream;


class Format : public wc3lib::Format<byte>
{
	public:
		virtual uint32_t version() const { return 800; }
};

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

typedef Vertex<float32> VertexData;
class Vertex; // workaround, we already have a class called Vertex in MDLX module!

struct QuaternionData
{
	QuaternionData() : a(0), b(0), c(0), d(0) { }

	QuaternionData(float32 a, float32 b, float32 c, float32 d) : a(a), b(b), c(c), d(d)
	{
	}

	QuaternionData(const std::vector<float32> &values) : a(values[0]), b(values[1]), c(values[2]), d(values[3])
	{
	}

	float32 operator[](uint8_t index) const
	{
		if (index == 0)
			return a;
		else if (index == 1)
			return b;
		else if (index == 2)
			return c;

		return d;
	}

	float32 a, b, c, d;
};

struct InterpolationData
{
	InterpolationData(const std::vector<float32> &inTan, const std::vector<float32> &outTan) : inTanX(inTan[0]), inTanY(inTan[1]), inTanZ(inTan[2]), outTanX(outTan[0]), outTanY(outTan[1]), outTanZ(outTan[2])
	{
	}

	float32 inTanX, inTanY, inTanZ;
	float32 outTanX, outTanY, outTanZ;
};

struct InterpolationRotationData
{
	InterpolationRotationData(const std::vector<float32> &inTan, const std::vector<float32> &outTan) : inTanA(inTan[0]), inTanB(inTan[1]), inTanC(inTan[2]), inTanD(inTan[3]), outTanA(outTan[0]), outTanB(outTan[1]), outTanC(outTan[2]), outTanD(outTan[3])
	{
	}

	float32 inTanA, inTanB, inTanC, inTanD;
	float32 outTanA, outTanB, outTanC, outTanD;
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

		std::basic_ostringstream<ascii> osstream(value);
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

inline istream& parseMdlChar(istream &stream, const ascii &character, std::streamsize &sizeCounter, bool optional = false) throw (class Exception)
{
	std::streamsize counter = 0;
	std::streampos position = stream.tellg();
	ascii readChar;
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

			ascii commata;
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

inline ostream& writeMdlCountedBlock(ostream &stream, const string &identifier, const long32 count, std::streamsize &size)
{
	ostringstream sstream;
	sstream << identifier << " " << count << " { \n";
	return wc3lib::write(stream, sstream.str().c_str()[0], size, sstream.str().length()); // without 0-terminating character!
}

/**
 * MDL properties are usually called flags and do not necessarily appear in an MDL scope.
 * Examples are:
 * DropShadow,
 * Unselectable,
 * Billboarded,
 * ...
 */
inline ostream& writeMdlProperty(ostream &stream, const string &identifier, std::streamsize &size)
{
	string value(identifier + ", \n");
	wc3lib::write(stream, value.c_str()[0], size, value.length()); // without 0-terminating character!

	return stream;
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
inline ostream& writeMdlValueProperty(ostream &stream, const string &identifier, T value, std::streamsize &size)
{
	string newValue(identifier + boost::str(boost::format(" %1%,\n") % value));
	wc3lib::write(stream, newValue.c_str()[0], size, newValue.length()); // without 0-terminating character!

	return stream;
}

/**
 * Static MDL value properties are only written into file if property has no animated values.
 * Examples are:
 * static TextureID <long>,
 * static Alpha <float>,
 * ...
 */
template<typename T>
inline ostream& writeMdlStaticValueProperty(ostream &stream, const string &identifier, T value, std::streamsize &size)
{
	string newValue("static ");
	wc3lib::write(stream, newValue.c_str()[0], size, newValue.length()); // without 0-terminating character!

	return writeMdlValueProperty(stream, identifier, value, size);
}

/**
 * Examples are:
 * Visibility <float>
 * Translation { <float_x>, <float_y>, <float_z> }
 * Rotation { <float_a>, <float_b>, <float_c>, <float_d> }
 * Scaling { <float_x>, <float_y>, <float_z> }
 */
template<typename T>
ostream& writeMdlVectorProperty(ostream &ostream, const string &identifier, const std::vector<T> &values, std::streamsize &size)
{
	string value(boost::str(boost::format("%1% %2%") % identifier % (values.size() == 1 ? "" : "{ ")));
	std::size_t i = 0;

	BOOST_FOREACH(const T &v, values)
	{
		value = boost::str(boost::format("%1%%2%") % v % (i == values.size() - 1 ? " " : ", "));
		++i;
	}

	if (values.size() == 1)
		value += "\n";
	else
		value += "}\n";

	wc3lib::write(ostream, value.c_str()[0], size, value.length()); // without 0-terminating character!

	return ostream;
}

/**
 * Static MDL vector properties are vector properties (usually required for 3 dimensional space and BGR color values) which are only written into file property has no animated values.
 * Examples are:
 * static Color { <float_b>, <float_g>, <float_r> },
 */
template<typename T>
inline ostream& writeMdlStaticVectorProperty(ostream &stream, const string &identifier, const std::vector<T> &values, std::streamsize &size)
{
	const string value("static ");
	wc3lib::write(stream, value.c_str()[0], size, value.length()); // without 0-terminating character!

	return writeMdlVectorProperty(stream, identifier, values, size);
}

inline ostream& writeMdlBlockConclusion(ostream &stream, std::streamsize &size)
{
	const string value("}\n");

	return wc3lib::write(stream, value.c_str()[0], size, value.length()); // without 0-terminating character!
}

}

}

#endif
