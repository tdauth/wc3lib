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
 * Provides utility functions mostly for stream operations.
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
std::string hexValue(T value)
{
	std::ostringstream sstream;
	sstream << std::hex << value;

	return sstream.str();
}

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

template<typename _CharT>
void checkStream(std::basic_ios<_CharT> &stream) throw (class Exception)
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
 */
template<typename T, typename _CharT>
inline std::basic_istream<_CharT>& read(std::basic_istream<_CharT> &istream, T &value, std::streamsize &sizeCounter, std::size_t size = sizeof(T) * sizeof(_CharT)) throw (class Exception)
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
 * Reads C string char into value "value" (with 0-terminating if size is 0).
 * Usually you should use type "char" but other types may also be supported by stream.
 * \param value This value is filled with an array of \p _CharT values. You'll have to take care about memory release!
 * \param size If this value is 0 it will stop when reaching 0-terminating char.
 * \param terminatingChar Customizable terminating char.
 */
template<typename _CharT>
inline std::basic_istream<_CharT>& readCString(std::basic_istream<_CharT> &istream, _CharT *&value, std::streamsize &sizeCounter, std::size_t size = 0, _CharT terminatingChar = '\0') throw (class Exception)
{
	// value is filled by this function
	if (value != 0)
		throw Exception(_("readCString: Value should be 0."));

	if (size == 0)
	{
		// get 0 terminating character, get name
		std::streampos position = istream.tellg();
		_CharT character;

		checkStream(istream);

		istream.get(character);

		std::size_t i = position;
		std::size_t length = 0;

		while (character != terminatingChar)
		{
			++i;
			++length;

			if (!istream)
				throw Exception(_("Input stream error."));

			istream.get(character);
		}

		istream.seekg(position);
		size = length; // assign new length
	}

	value = new _CharT[size + 1];
	read(istream, value[0], sizeCounter, size + 1); // read terminating char but replace afterwards with user-defined, if you don't read you'll always read the same string with size 1 (0-terminating char)
	value[size] = '\0';

	return istream;
}

template<typename _CharT>
inline std::basic_istream<_CharT>& readString(std::basic_istream<_CharT> &istream, std::basic_string<_CharT> &value, std::streamsize &sizeCounter, std::size_t size = 0) throw (class Exception)
{
	_CharT *cString = 0;
	readCString(istream, cString, sizeCounter, size);
	value = cString; // uses const cString and copies content
	delete[] cString;

	return istream;
}

/**
 *  Parses next token (tokens are separated by characters in \p delimiters) in input stream \p istream and writes it into \p value.
 * \return Returns reference to input stream \p istream.
 * \sa read, readCString, readString
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

template<typename _CharT>
inline std::streampos endPosition(std::basic_istream<_CharT> &istream)
{
	std::streampos pos = istream.tellg();
	istream.seekg(0, std::ios_base::end);
	std::streampos end = istream.tellg();
	istream.seekg(pos);

	return end;
}

template<typename _CharT>
inline bool eof(std::basic_istream<_CharT> &istream)
{
	//return istream.rdstate() & std::ios_base::eofbit;
	return istream.tellg() == endPosition(istream);
}

template<typename _CharT>
inline std::basic_istream<_CharT>& readLine(std::basic_istream<_CharT> &istream, std::string &value, std::streamsize &sizeCounter, std::size_t size = 0)
{
	_CharT *cString = 0;
	readCString(istream, cString, sizeCounter, size, '\n');
	value = cString; // uses const c string and copies value
	delete[] cString;

	return istream;
}

template<typename T, typename _CharT>
inline std::basic_ostream<_CharT>& write(std::basic_ostream<_CharT> &ostream, const T &value, std::streamsize &sizeCounter, std::size_t size = sizeof(T) * sizeof(_CharT))
{
	ostream.write(reinterpret_cast<const _CharT*>(&value), size);

	checkStream(ostream);

	sizeCounter += size; /// @todo Why isn't there any .pcount, throw exception if it is not written completely

	return ostream;
}

/**
 * Specialization for pointers where you should specify the exact size.
 */
template<typename T, typename _CharT>
inline std::basic_ostream<_CharT>& write(std::basic_ostream<_CharT> &ostream, const T *value, std::streamsize &sizeCounter, std::size_t size) //  = sizeof(T) * sizeof(_CharT)
{
	ostream.write(reinterpret_cast<const _CharT*>(value), size);

	checkStream(ostream);

	sizeCounter += size; /// @todo Why isn't there any .pcount, throw exception if it is not written completely

	return ostream;
}

/**
 * Specialization for pointers where you should specify the exact size.
 */
template<typename T, typename _CharT>
inline std::basic_ostream<_CharT>& write(std::basic_ostream<_CharT> &ostream, T *value, std::streamsize &sizeCounter, std::size_t size) //  = sizeof(T) * sizeof(_CharT)
{
	ostream.write(reinterpret_cast<const _CharT*>(value), size);

	checkStream(ostream);

	sizeCounter += size; /// @todo Why isn't there any .pcount, throw exception if it is not written completely

	return ostream;
}

/**
* Writes C string of value "value" into output (with 0 terminating char if size is 0).
* @param size If size is 0 it will stop writing when reached 0-terminating char.
*/
template<typename _CharT>
inline std::basic_ostream<_CharT>& writeCString(std::basic_ostream<_CharT> &ostream, const _CharT *value, std::streamsize &sizeCounter, std::size_t size = 0)
{
	return write(ostream, value, sizeCounter, size == 0 ? strlen(value) + 1 : size);
}

template<typename _CharT>
inline std::basic_ostream<_CharT>& writeString(std::basic_ostream<_CharT> &ostream, const std::basic_string<_CharT> &value, std::streamsize &sizeCounter, std::size_t size = 0)
{
	return writeCString(ostream, value.data(), sizeCounter, size);
}

template<typename _CharT>
inline std::basic_ostream<_CharT>& writeLine(std::basic_ostream<_CharT> &ostream, const _CharT *value, std::streamsize &sizeCounter, std::size_t size = 0)
{
	_CharT newValue[strlen(value) + 2];
	memcpy(newValue, value, strlen(value));
	newValue[strlen(value)] = '\n';
	newValue[strlen(value) + 1] = '\0';

	return writeCString(ostream, newValue, sizeCounter, size);
}

template<typename _CharT>
inline std::basic_ostream<_CharT>& writeLine(std::basic_ostream<_CharT> &ostream, const std::basic_string<_CharT> &value, std::streamsize &sizeCounter, std::size_t size = 0)
{
	const std::basic_string<_CharT> newValue = value + '\n';

	return writeCString(ostream, newValue.data(), sizeCounter, size);
}

/**
 * Writes byte count at a previous position. Required by binary formats which need to store byte counts (inclusive and exclusive) after writing all data into output stream.
 * \param ostream Output stream which byte count is written into.
 * \param byteCount Byte count which is written into output stream at position "position".
 * \param position Position in output stream where byte count is written into.
 * \param sizeCounter Counter of written output size which is increased by written size of byte count. \note Do not forget to initialise this value since increment assignment operator (+=) is used.
 * \param inclusive If this value is true size of value "byteCount" will by added automatically to byte count. Therefore it writes inclusive byte count (useful for the MDLX format for instance).
 * @return Returns output stream "ostream" for any further treatment.
 * \sa skipByteCount
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

inline std::string boolString(bool value)
{
	if (value)
		return _("Yes");

	return _("No");
}

inline bool expectInput()
{
	std::string input;
	std::cin >> input;

	while (input != _("y") && input != _("n"))
	{
		std::cout << _("Wrong input. Expecting new input (y/n):") << std::endl;
		std::cin >> input;
	}

	if (input == _("y"))
		return true;

	return false;
}

/*
/// \todo Only for SCHED_FIFO or SCHED_RR.
inline int threadPriorityMin()
{
#ifdef UNIX
	sched_get_priority_min()
#elifdef WIN32
#endif
}

/// \todo Only for SCHED_FIFO or SCHED_RR
inline int threadPriorityMax()
{
#ifdef UNIX
	return sched_get_priority_max();
#elifdef WIN32
	return
#endif
}

inline void setThreadPriority(boost::thread &thread, int priority) throw (class Exception)
{
#ifdef UNIX
	int retcode;
	int policy;

	const pthread_t threadId = (pthread_t)thread.native_handle();

	struct sched_param param;

	if ((retcode = pthread_getschedparam(threadId, &policy, &param)) != 0)
		throw Exception(boost::format(_("Exception: Error code %1% when getting scheduler parameters of thread %2%.")) % retcode % threadId);

	std::cout << "INHERITED: ";
	std::cout << "policy=" << ((policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                               (policy == SCHED_RR)    ? "SCHED_RR" :
                               (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                                                         "???")
              << ", priority=" << param.sched_priority << std::endl;


	//policy = SCHED_FIFO;
	param.sched_priority = priority;

	if ((retcode = pthread_setschedparam(threadId, policy, &param)) != 0)
		throw Exception(boost::format(_("Exception: Error code %1% when setting scheduler parameters for thread %2%.")) % retcode % threadId);
#elifdef WIN32
	HANDLE thread = (HANDLE)thread.native_handle();
	SetThreadPriority(thread, priority);
#endif
}
*/

}

#endif
