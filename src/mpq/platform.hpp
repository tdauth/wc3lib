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

#ifndef WC3LIB_MPQ_PLATFORM_HPP
#define WC3LIB_MPQ_PLATFORM_HPP

#include <ctime>

#include "../platform.hpp"

namespace wc3lib
{

namespace mpq
{

typedef int32 CRC32;

struct MD5Checksum
{
	byte checksum[16]; // 128 bit

	bool operator==(const MD5Checksum &other) const
	{
		return memcmp(checksum, other.checksum, 16) == 0;
	}

	string toString() const
	{
		char *text = new char[17];
		text[16] = 0;
		memcpy(text, this->checksum, 16);

		const string result = text;
		delete[] text;

		return result;
	}
};

inline std::ostream& operator<<(std::ostream &out, const MD5Checksum &checksum)
{
	out << checksum.toString();

	return out;
}

/**
 * \brief The header structure of the MPQ 1 format.
 */
struct Header
{
	byte magic[4];
	uint32 headerSize;
	uint32 archiveSize;
	uint16 formatVersion;
	uint16 sectorSizeShift; // DEPRECATED int8
	uint32 hashTableOffset;
	uint32 blockTableOffset;
	uint32 hashTableEntries;
	uint32 blockTableEntries;
};

struct ExtendedHeader /// MPQ format 2
{
	uint64 extendedBlockTableOffset;
	uint16 hashTableOffsetHigh;
	uint16 blockTableOffsetHigh;
};

struct HashTableEntry
{
	int32 filePathHashA;
	int32 filePathHashB;
	uint16 locale;
	uint16 platform; // DEPRECATED int8
	uint32 fileBlockIndex; /// Reserved values are \ref Hash::blockIndexDeleted and \ref Hash::blockIndexEmpty.
};

struct BlockTableEntry
{
	uint32 blockOffset;
	uint32 blockSize;
	uint32 fileSize;
	uint32 flags;
};

struct ExtendedBlockTableEntry
{
	uint16 extendedBlockOffset;
};

struct ExtendedAttributesHeader
{
	uint32 version;
	uint32 attributesPresent;
};

/**
 * Windows-like file time. Redefined for compatibility.
 * \note Should be a POD/serializable type!
 */
struct FILETIME : public boost::operators<FILETIME>
{
	static const uint64 EPOCH_OFFSET = 116444736000000000ULL; // Number of 100 ns units between 01/01/1601 and 01/01/1970

	uint32_t lowDateTime;
	uint32_t highDateTime;

	FILETIME() : lowDateTime(0), highDateTime(0)
	{
	}

	FILETIME(uint32_t low, uint32_t high) : lowDateTime(low), highDateTime(high)
	{
	}

	FILETIME(uint64 time) : lowDateTime((uint32_t)time), highDateTime((uint32_t)(time >> 32))
	{
	}

	FILETIME(const time_t &time)
	{
		uint64 nTime = (uint64)time;
		nTime *= 10000000ULL;
		nTime += EPOCH_OFFSET;
		lowDateTime = (uint32_t)nTime;
		highDateTime = (uint32_t)(nTime >> 32);
	}

	FILETIME(const FILETIME &other) : lowDateTime(other.lowDateTime), highDateTime(other.highDateTime)
	{
	}

	bool toTime(time_t &time) const
	{
		// The FILETIME represents a 64-bit integer: the number of 100 ns units since January 1, 1601
		uint64 nTime = ((uint64)this->highDateTime << 32) + this->lowDateTime;

		if (nTime < EPOCH_OFFSET)
		{
			return false;
		}

		nTime -= EPOCH_OFFSET;	// Convert the time base from 01/01/1601 to 01/01/1970
		nTime /= 10000000ULL;	// Convert 100 ns to sec

		time = (time_t)nTime;

		// Test for overflow (FILETIME is 64 bits, time_t is 32 bits)
		if ((nTime - (uint64)time) > 0)
		{
			return false;
		}

		return true;
	}

	bool operator==(const FILETIME &other) const
	{
		return this->lowDateTime == other.lowDateTime && this->highDateTime == other.highDateTime;
	}

	bool operator<(const FILETIME &other) const
	{
		return this->highDateTime < other.highDateTime || (this->highDateTime == other.highDateTime && this->lowDateTime < other.lowDateTime);
	}

	operator time_t() const
	{
		time_t result;

		toTime(result);

		return result;
	}

	operator uint64() const
	{
		return ((uint64)this->highDateTime << 32) + this->lowDateTime;
	}
};

}

}

#endif