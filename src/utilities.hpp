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

#ifndef WC3LIB_UTILITIES_HPP
#define WC3LIB_UTILITIES_HPP

/**
 * \file
 * \brief Provides utility functions mostly for I/O stream operations.
 *
 * \ingroup io
 */

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/cstdint.hpp>

#include "config.h"
#include "exception.hpp"
#include "i18n.hpp"

namespace wc3lib
{

namespace
{

template<typename T>
inline std::string hexValue(T value)
{
	std::ostringstream sstream;
	sstream << std::hex << value;

	return sstream.str();
}

/**
 * Converts the state of an I/O stream into a readable message.
 * Useful for debugging and error reporting.
 *
 * \param state The state which is converted into a readable message.
 *
 * \return Returns a readable message which contains all state names of \p state.
 *
 * \ingroup io
 */
inline std::string iostateMessage(const std::ios_base::iostate &state)
{
	std::ostringstream sstream;

	if (state & std::ios_base::badbit)
		sstream << "bad";

	if (state & std::ios_base::eofbit)
		sstream << "eof";

	if (state & std::ios_base::failbit)
		sstream << "fail";

	if (state & std::ios_base::goodbit)
		sstream << "good";

	return sstream.str();
}

/**
 * Checks an I/O stream for its state. If an error occured it throws an \ref Exception.
 * Useful for checking streams during read processes to get immediate feedback via Exception Handling.
 *
 * \param stream I/O stream which is checked.
 *
 * \throws Exception This exception is thrown with a readable message which contains the state of the I/O stream.
 *
 * \ingroup io
 */
template<typename _CharT>
inline void checkStream(std::basic_ios<_CharT> &stream) throw (class Exception)
{
	if (!stream)
	{
		throw Exception(boost::str(boost::format(_("Stream error.\nExceptions \"%1%\".\nRD state: \"%2%\".")) % iostateMessage(stream.exceptions()) % iostateMessage(stream.rdstate())).c_str());
	}
}

}

/**
 * Common input stream read function mainly required for binary format implementations.
 * Reads converted value from input stream istream and increases sizeCounter by read size.
 * Supports different types of input streams (with various char types).
 * Size of read value can be specified, otherwise size of type T in user-defined char type _CharT is used.
 * \param istream Input stream which is read from.
 * \param value Reference of value which is filled by function. Can be uninitialised when calling this function (assignment operator is used).
 * \param sizeCounter Counter of read chars which is increased by function. @note Do not forget to initialise before calling this function since += operator is used.
 * \param size Size of value which is filled. Default value is size of type T in user-defined char type _CharT. Consider that real size is taken and not sizeof(T) since it only returns size in C++ language implementation char type.
 * \return Returns input stream istream for further treatment.
 *
 * \ingroup io
 */
template<typename T, typename _CharT>
inline std::basic_istream<_CharT>& read(std::basic_istream<_CharT> &istream, T &value, std::streamsize &sizeCounter, std::streamsize size = sizeof(T) * sizeof(_CharT)) throw (class Exception)
{
	istream.read(reinterpret_cast<_CharT*>(&value), size);

	checkStream(istream);

	if (istream.gcount() != size)
	{
		throw Exception(boost::str(boost::format(_("Input stream read size %1% is not equal to expected size %2%.")) % istream.gcount() % size).c_str());
	}

	sizeCounter += istream.gcount();

	return istream;
}

/**
 * \brief Reads a C-like string which is terminated by the character \p terminatingChar from input stream \p istream into \p value.
 *
 * Usually you should use type "char" but other types may also be supported by stream.
 *
 * \param istream The input stream which is read from.
 * \param value This value is filled with the value of the string.
 * \param sizeCounter This counter is increased by the number of bytes which are read from the stream.
 * \param terminatingChar Customizable terminating char. The terminating character is not included into the resulting \p value.
 * \param maxLength This value restricts the length of the string to avoid buffer errors. If the value is longer an exception is thrown.
 *
 * \return Returns \p istream which is read from.
 *
 * \throws Exception Throws an exception if the string is too long or there is any other stream errors.
 *
 * \ingroup io
 */
template<typename _CharT>
inline std::basic_istream<_CharT>& readString(std::basic_istream<_CharT> &istream, std::basic_string<_CharT> &value, std::streamsize &sizeCounter, _CharT terminatingChar = '\0', const std::size_t maxLength = 1024) throw (class Exception)
{
	if (maxLength == 0)
	{
		throw Exception(_("Maximum length is 0."));
	}

	/*
	 * Use a string stream to buffer the input and build up
	 * the string step by step in the most efficient way.
	 * TODO The buffer can be limited to the size maxLength as we won't go any further.
	 */
	std::basic_stringstream<_CharT> sstream(std::ios::out | std::ios::binary);
	std::size_t length = 0;
	_CharT character = ' ';
	bool finish = false;

	do
	{
		/*
		 * Check the stream before reading from it.
		 * Errors would lead to an exception, so the user gets immediate feedback if
		 * the stream is corrupted etc.
		 */
		checkStream(istream);
		istream.get(character);

		if (character != terminatingChar)
		{
			// never write formatted characters which would lead to errors for numbers etc.
			sstream.put(character);
			checkStream(sstream);

			++length;

			/*
			* Restrict to avoid buffer errors.
			*/
			if (length == maxLength)
			{
				throw Exception(boost::format(_("0 terminated string from stream is longer that maximum length %1%.")) % maxLength);
			}
		}
		else
		{
			finish = true;
		}
	}
	while (!finish);

	/*
	 * Assign from buffered string stream.
	 */
	value = sstream.str();
	sizeCounter += value.size() + 1; // + 0 terminating character

	return istream;
}

/**
 *  Parses next token (tokens are separated by characters in \p delimiters) in input stream \p istream and writes it into \p value.
 * \return Returns reference to input stream \p istream.
 * \sa read, readCString, readString
 *
 * \ingroup io
 */
template<typename T, typename _ByteT> /// \todo typename _CharT = char for C++0x
inline std::basic_istream<_ByteT>& parse(std::basic_istream<_ByteT> &istream, T &value, std::streamsize &sizeCounter, const std::basic_string<char> &delimiters = " \t\n\r")
{
	std::streampos position;
	char character;

	// skip white-spaces
	do
	{
		position = istream.tellg();
		wc3lib::read(istream, character, sizeCounter);
	}
	while (delimiters.find(character) != std::string::npos);

	istream.seekg(position);

	// get length of non-whitespace string
	position = istream.tellg();
	std::size_t length = 0;

	do
	{
		istream.get(character);

		if (delimiters.find(character) == std::string::npos)
			++length;
		else
			break;
	}
	while (true);

	istream.seekg(position);

	std::basic_string<char> stringValue;
	readString(istream, stringValue, sizeCounter, length);

	// convert string into requested type
	//std::basic_ostringstream<char> ostringstream(stringValue);
	//ostringstream >> value;
	value = boost::lexical_cast<T>(stringValue);

	return istream;
}

/**
 * \brief Gets the last position of an input stream.
 *
 * \param istream The input stream from which the last position is got from.
 *
 * \return Returns the last position of the input stream \p istream.
 *
 * \ingroup io
 */
template<typename _CharT>
inline std::streampos endPosition(std::basic_istream<_CharT> &istream)
{
	std::streampos pos = istream.tellg();
	istream.seekg(0, std::ios_base::end);
	std::streampos end = istream.tellg();
	istream.seekg(pos);

	return end;
}

/**
 * \brief Checks if the an input stream has reached its end.
 *
 * Can be used if you want to check if anything is left to read in an input stream.
 *
 * \param istream Input stream which is checked for its position matching its end.
 *
 * \return Returns true if the input stream is at its end position. Otherwise it returns false.
 *
 * \ingroup io
 */
template<typename _CharT>
inline bool eof(std::basic_istream<_CharT> &istream)
{
	//return istream.rdstate() & std::ios_base::eofbit;
	return istream.tellg() == endPosition(istream);
}

/**
 * \brief Reads a '\n' terminated string from an input stream.
 *
 * Reads a '\n' terminated string from input stream \p istream into \p value while
 * increasing \p sizeCounter by the number of bytes read.
 *
 * \return Returns the input stream \p istream which is read from.
 *
 * \ingroup io
 */
template<typename _CharT>
inline std::basic_istream<_CharT>& readLine(std::basic_istream<_CharT> &istream, std::string &value, std::streamsize &sizeCounter)
{
	readString(istream, value, sizeCounter, '\n');

	return istream;
}

/**
 * \ingroup io
 */
template<typename T, typename _CharT>
inline std::basic_ostream<_CharT>& write(std::basic_ostream<_CharT> &ostream, const T &value, std::streamsize &sizeCounter, std::streamsize size = sizeof(T) * sizeof(_CharT))
{
	ostream.write(reinterpret_cast<const _CharT*>(&value), size);

	checkStream(ostream);

	sizeCounter += size; /// @todo Why isn't there any .pcount, throw exception if it is not written completely

	return ostream;
}

/**
 * Specialization for pointers where you should specify the exact size.
 *
 * \ingroup io
 */
template<typename T, typename _CharT>
inline std::basic_ostream<_CharT>& write(std::basic_ostream<_CharT> &ostream, const T *value, std::streamsize &sizeCounter, std::streamsize size) //  = sizeof(T) * sizeof(_CharT)
{
	ostream.write(reinterpret_cast<const _CharT*>(value), size);

	checkStream(ostream);

	sizeCounter += size; /// \todo Why isn't there any .pcount, throw exception if it is not written completely

	return ostream;
}

/**
 * Specialization for pointers where you should specify the exact size.
 *
 * \ingroup io
 */
template<typename T, typename _CharT>
inline std::basic_ostream<_CharT>& write(std::basic_ostream<_CharT> &ostream, T *value, std::streamsize &sizeCounter, std::streamsize size) //  = sizeof(T) * sizeof(_CharT)
{
	ostream.write(reinterpret_cast<const _CharT*>(value), size);

	checkStream(ostream);

	sizeCounter += size; /// \todo Why isn't there any .pcount, throw exception if it is not written completely

	return ostream;
}

/**
* \brief Writes C string of value "value" into output (with 0 terminating char if size is 0).
*
* \param ostream The output stream which is written into.
* \param value The value which is written into the output stream \p ostream.
* \param sizeCounter This counter is increased automatically by the number of written bytes.
* \param size If size is 0 it will stop writing when reached 0-terminating char.
*
* \return Returns the output stream in which is written (\p ostream).
*
* \ingroup io
*/
template<typename _CharT>
inline std::basic_ostream<_CharT>& writeCString(std::basic_ostream<_CharT> &ostream, const _CharT *value, std::streamsize &sizeCounter, std::streamsize size = 0)
{
	return write(ostream, value, sizeCounter, size == 0 ? strlen(value) + 1 : size);
}

/**
 * \ingroup io
 */
template<typename _CharT>
inline std::basic_ostream<_CharT>& writeString(std::basic_ostream<_CharT> &ostream, const std::basic_string<_CharT> &value, std::streamsize &sizeCounter, std::streamsize size = 0)
{
	return writeCString(ostream, value.data(), sizeCounter, size);
}

/**
 * \ingroup io
 */
template<typename _CharT>
inline std::basic_ostream<_CharT>& writeLine(std::basic_ostream<_CharT> &ostream, const _CharT *value, std::streamsize &sizeCounter, std::streamsize size = 0)
{
	_CharT newValue[strlen(value) + 2];
	memcpy(newValue, value, strlen(value));
	newValue[strlen(value)] = '\n';
	newValue[strlen(value) + 1] = '\0';

	return writeCString(ostream, newValue, sizeCounter, size);
}

/**
 * \ingroup io
 */
template<typename _CharT>
inline std::basic_ostream<_CharT>& writeLine(std::basic_ostream<_CharT> &ostream, const std::basic_string<_CharT> &value, std::streamsize &sizeCounter, std::streamsize size = 0)
{
	const std::basic_string<_CharT> newValue = value + '\n';

	return writeCString(ostream, newValue.data(), sizeCounter, size);
}

/**
 * Writes byte count at a previous position and jumps back to the current position.
 * Required by binary formats which need to store byte counts (inclusive and exclusive) after writing all data into output stream.
 * \param ostream Output stream which byte count is written into.
 * \param byteCount Byte count which is written into output stream at position "position".
 * \param position Position in output stream where byte count is written into.
 * \param sizeCounter Counter of written output size which is increased by written size of byte count. \note Do not forget to initialise this value since increment assignment operator (+=) is used.
 * \param inclusive If this value is true size of value "byteCount" will by added automatically to byte count. Therefore it writes inclusive byte count (useful for the MDLX format for instance).
 * \return Returns output stream "ostream" for any further treatment.
 * \sa skipByteCount
 *
 * \ingroup io
 */
template<typename T, typename _CharT>
inline std::basic_ostream<_CharT>& writeByteCount(std::basic_ostream<_CharT> &ostream, const T &byteCount, std::streampos position, std::streamsize &sizeCounter, bool inclusive = false)
{
	const std::streampos backPosition = ostream.tellp();
	ostream.seekp(position);
	const T realByteCount = (inclusive ? (byteCount + sizeof(byteCount) * sizeof(_CharT)) : byteCount); // inclusive means size of byte count as well
	write<T, _CharT>(ostream, realByteCount, sizeCounter);
	ostream.seekp(backPosition); // jump back to the end or somewhere else

	return ostream;
}

/**
 * Some binary formats require chunk byte counts which can only be detected during write process after all chunk data (without the byte count itself) has been written into the output stream.
 * This function helps you to easily skip the byte counts position.
 * \sa writeByteCount
 *
 * \ingroup io
 */
template<typename T, typename _CharT>
inline std::basic_ostream<_CharT>& skipByteCount(std::basic_ostream<_CharT> &ostream, std::streampos &position)
{
	position = ostream.tellp();
	ostream.seekp(sizeof(T) * sizeof(_CharT), std::ios_base::cur);

	return ostream;
}

/**
 * Formats size \p size to string with proper binary size unit.
 * Supported units are:
 * <ul>
 * <li>Gi</li>
 * <li>Mi</li>
 * <li>Ki</li>
 * <li>B</li>
 * </ul>
 *
 * \ingroup io
 */
template<typename T>
std::string sizeStringBinary(T size)
{
	typedef uint64_t SizeType;
	static const std::size_t tableSize = 4;
	static const SizeType table[tableSize] =
	{
		(SizeType)pow(1024, 3), // TODO compile time power
		(SizeType)pow(1024, 2), // TODO compile time power
		1024,
		0
	};
	static const char *identifierTable[tableSize] =
	{
		_("Gi"),
		_("Mi"),
		_("Ki"),
		_("B")
	};

	std::string unit = "";
	SizeType remainder = 0;

	for (std::size_t i = 0; i < tableSize; ++i)
	{
		if (size >= table[i])
		{
			if (table[i] > 0)
			{
				remainder = boost::numeric_cast<SizeType>(size) % table[i];
				size /= boost::numeric_cast<T>(table[i]);
			}
			else
				remainder = 0;

			unit = identifierTable[i];

			break;
		}
	}

	std::ostringstream sstream;
	sstream << size;

	if (remainder != 0)
		sstream << _(".") << remainder;

	sstream << ' ' << unit;

	return sstream.str();
}

/**
 * Formats size \p size to string with proper decimal size unit.
 * Supported units are:
 * <ul>
 * <li>G</li>
 * <li>M</li>
 * <li>k</li>
 * <li>B</li>
 * </ul>
 *
 * \ingroup io
 */
template<typename T>
std::string sizeStringDecimal(T size)
{
	typedef uint64_t SizeType;
	static const std::size_t tableSize = 4;
	static const SizeType table[tableSize] =
	{
		(SizeType)pow(1000, 3), // TODO compile time power
		(SizeType)pow(1000, 2), // TODO compile time power
		1000,
		0
	};
	static const char *identifierTable[tableSize] =
	{
		_("G"),
		_("M"),
		_("k"),
		_("B")
	};

	std::string unit = "";
	SizeType remainder = 0;

	for (std::size_t i = 0; i < tableSize; ++i)
	{
		if (size >= table[i])
		{
			if (table[i] > 0)
			{
				remainder = boost::numeric_cast<SizeType>(size) % table[i];
				size /= boost::numeric_cast<T>(table[i]);
			}
			else
				remainder = 0;

			unit = identifierTable[i];

			break;
		}
	}

	std::ostringstream sstream;
	sstream << size;

	if (remainder != 0)
		sstream << _(".") << remainder;

	sstream << ' ' << unit;

	return sstream.str();
}

/**
 * \ingroup io
 */
inline std::string boolString(bool value)
{
	if (value)
		return _("Yes");

	return _("No");
}

/**
 * \brief Waits for the user input in terminal to confirm or reject something.
 *
 * Blocks until the user has entered the valid string to confirm or reject something.
 *
 * \ingroup io
 */
inline bool expectInput()
{
	/*
	 * Do not clear the buffer with an endl.
	 * The user input should start directly ofter the : character.
	 */
	std::cout << _("Please confirm (by typing \"y\") or reject (by typing \"n\"):");

	std::string input;
	std::cin >> input;

	while (input != _("y") && input != _("n"))
	{
		std::cerr << _("Wrong input. Expecting new input (y/n):") << std::endl;
		std::cin >> input;
	}

	if (input == _("y"))
	{
		return true;
	}

	return false;
}

}

#endif
