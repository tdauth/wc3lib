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

#include "../config.h"
#include "../platform.hpp"

#ifdef ENCRYPTION
#include <crypto++/md5.h>
#else
#include <openssl/md5.h>
#endif

namespace wc3lib
{

namespace mpq
{

typedef int32 CRC32;

typedef int16_t MD5; // 128 bit

inline MD5 md5(const byte *buffer)
{
	MD5 md5 = 0;
#ifdef ENCRYPTION
	CryptoPP::Weak1::MD5 checksum;
	checksum.CalculateDigest((unsigned char*)&md5, (unsigned char*)buffer, sizeof(md5));
#else
	MD5_CTX c;
	MD5_Init(&c);
	MD5_Update(&c, (const void *)buffer, sizeof(md5));
	MD5_Final((unsigned char*)&md5, &c);
#endif
	return md5;
}

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
			return false;

		nTime -= EPOCH_OFFSET;	// Convert the time base from 01/01/1601 to 01/01/1970
		nTime /= 10000000ULL;	// Convert 100 ns to sec

		time = (time_t)nTime;

		// Test for overflow (FILETIME is 64 bits, time_t is 32 bits)
		if ((nTime - (uint64)time) > 0)
			return false;

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

// Known Blizzard public keys
// Created by Jean-Francois Roy using OpenSSL
static const char * blizzardWeakPublicKey =
"-----BEGIN PUBLIC KEY-----"
"MFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBAJJidwS/uILMBSO5DLGsBFknIXWWjQJe"
"2kfdfEk3G/j66w4KkhZ1V61Rt4zLaMVCYpDun7FLwRjkMDSepO1q2DcCAwEAAQ=="
"-----END PUBLIC KEY-----";

static const char * blizzardStrongPublicKey =
"-----BEGIN PUBLIC KEY-----"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsQZ+ziT2h8h+J/iMQpgd"
"tH1HaJzOBE3agjU4yMPcrixaPOZoA4t8bwfey7qczfWywocYo3pleytFF+IuD4HD"
"Fl9OXN1SFyupSgMx1EGZlgbFAomnbq9MQJyMqQtMhRAjFgg4TndS7YNb+JMSAEKp"
"kXNqY28n/EVBHD5TsMuVCL579gIenbr61dI92DDEdy790IzIG0VKWLh/KOTcTJfm"
"Ds/7HQTkGouVW+WUsfekuqNQo7ND9DBnhLjLjptxeFE2AZqYcA1ao3S9LN3GL1tW"
"lVXFIX9c7fWqaVTQlZ2oNsI/ARVApOK3grNgqvwH6YoVYVXjNJEo5sQJsPsdV/hk"
"dwIDAQAB"
"-----END PUBLIC KEY-----";

static const char * warcraftIIIMapPublicKey =
"-----BEGIN PUBLIC KEY-----"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA1BwklUUQ3UvjizOBRoF5"
"yyOVc7KD+oGOQH5i6eUk1yfs0luCC70kNucNrfqhmviywVtahRse1JtXCPrx2bd3"
"iN8Dx91fbkxjYIOGTsjYoHKTp0BbaFkJih776fcHgnFSb+7mJcDuJVvJOXxEH6w0"
"1vo6VtujCqj1arqbyoal+xtAaczF3us5cOEp45sR1zAWTn1+7omN7VWV4QqJPaDS"
"gBSESc0l1grO0i1VUSumayk7yBKIkb+LBvcG6WnYZHCi7VdLmaxER5m8oZfER66b"
"heHoiSQIZf9PAY6Guw2DT5BTc54j/AaLQAKf2qcRSgQLVo5kQaddF3rCpsXoB/74"
"6QIDAQAB"
"-----END PUBLIC KEY-----";

static const char * wowPatchPublicKey =
"-----BEGIN PUBLIC KEY-----"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwOsMV0LagAWPEtEQM6b9"
"6FHFkUyGbbyda2/Dfc9dyl21E9QvX+Yw7qKRMAKPzA2TlQQLZKvXpnKXF/YIK5xa"
"5uwg9CEHCEAYolLG4xn0FUOE0E/0PuuytI0p0ICe6rk00PifZzTr8na2wI/l/GnQ"
"bvnIVF1ck6cslATpQJ5JJVMXzoFlUABS19WESw4MXuJAS3AbMhxNWdEhVv7eO51c"
"yGjRLy9QjogZODZTY0fSEksgBqQxNCoYVJYI/sF5K2flDsGqrIp0OdJ6teJlzg1Y"
"UjYnb6bKjlidXoHEXI2TgA/mD6O3XFIt08I9s3crOCTgICq7cgX35qrZiIVWZdRv"
"TwIDAQAB"
"-----END PUBLIC KEY-----";

static const char * wowSurveyPublicKey =
"-----BEGIN PUBLIC KEY-----"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAnIt1DR6nRyyKsy2qahHe"
"MKLtacatn/KxieHcwH87wLBxKy+jZ0gycTmJ7SaTdBAEMDs/V5IPIXEtoqYnid2c"
"63TmfGDU92oc3Ph1PWUZ2PWxBhT06HYxRdbrgHw9/I29pNPi/607x+lzPORITOgU"
"BR6MR8au8HsQP4bn4vkJNgnSgojh48/XQOB/cAln7As1neP61NmVimoLR4Bwi3zt"
"zfgrZaUpyeNCUrOYJmH09YIjbBySTtXOUidoPHjFrMsCWpr6xs8xbETbs7MJFL6a"
"vcUfTT67qfIZ9RsuKfnXJTIrV0kwDSjjuNXiPTmWAehSsiHIsrUXX5RNcwsSjClr"
"nQIDAQAB"
"-----END PUBLIC KEY-----";

static const char * starcraftIIMapPublicKey =
"-----BEGIN PUBLIC KEY-----"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAmk4GT8zb+ICC25a17KZB"
"q/ygKGJ2VSO6IT5PGHJlm1KfnHBA4B6SH3xMlJ4c6eG2k7QevZv+FOhjsAHubyWq"
"2VKqWbrIFKv2ILc2RfMn8J9EDVRxvcxh6slRrVL69D0w1tfVGjMiKq2Fym5yGoRT"
"E7CRgDqbAbXP9LBsCNWHiJLwfxMGzHbk8pIl9oia5pvM7ofZamSHchxlpy6xa4GJ"
"7xKN01YCNvklTL1D7uol3wkwcHc7vrF8QwuJizuA5bSg4poEGtH62BZOYi+UL/z0"
"31YK+k9CbQyM0X0pJoJoYz1TK+Y5J7vBnXCZtfcTYQ/ZzN6UcxTa57dJaiOlCh9z"
"nQIDAQAB"
"-----END PUBLIC KEY-----";

}

}

#endif