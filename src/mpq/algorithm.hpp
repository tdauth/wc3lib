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

#ifndef WC3LIB_MPQ_ALGORITHM_HPP
#define WC3LIB_MPQ_ALGORITHM_HPP

#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/zlib.hpp>

/**
 * \file
 * Includes all possible compression algorithm used in MPQ archives.
 */

//#include <huffman/huff.h>
#include <huffman/huffman.h> // C implementation, libhuffman
//#include <zlib.h> // system library
#include <pklib/pklib.h>
//#include <bzlib.h> // system library
#include <wave/wave.h>
#include <md5-cc/md5.hh>

#include "platform.hpp"
#include "../i18n.hpp"
#include "../exception.hpp"

namespace wc3lib
{

namespace mpq
{

inline const char* pkglibError(int error)
{
	switch (error)
	{
		case CMP_INVALID_DICTSIZE:
			return _("Invalid dictionary size.");

		case CMP_INVALID_MODE:
			return _("Invalid mode.");

		case CMP_BAD_DATA:
			return _("Bad data.");

		case CMP_ABORT:
			return _("Abort.");
	}

	return "";
}

/**
 * These values need to be like they are since they're used directly in function \ref HashString() for hash value calculation.
 */
enum class HashType : uint32
{
	TableOffset = 0x000,
	NameA = 0x100,
	NameB = 0x200,
	FileKey = 0x300
};

const std::size_t cryptTableSize = 0x500;
/// The encryption and hashing functions use a number table in their procedures. This table must be initialized before the functions are called the first time.
void InitializeCryptTable(uint32 dwCryptTable[cryptTableSize]);
void EncryptData(const uint32 dwCryptTable[cryptTableSize], void *lpbyBuffer, uint32 dwLength, uint32 dwKey);
void DecryptData(const uint32 dwCryptTable[cryptTableSize], void *lpbyBuffer, uint32 dwLength, uint32 dwKey);

/// Based on code from StormLib.
uint32 HashString(const uint32 dwCryptTable[cryptTableSize], const char *lpszString, HashType hashType);

/**
 * \throw Exception Throws an exception if an error occurs on compression.
 */
void compressPklib(char *outBuffer, int &outLength, char* const inBuffer, int inLength, int * /* pCmpType */, int /* compressionLevel */) ;
/**
 * \throw Exception Throws an exception if an error occurs on decompression.
 */
void decompressPklib(char *outBuffer, int &outLength, char* const inBuffer, int inLength);
/**
 * \sa compressWaveMono, decompressWaveMono, compressWaveStereo, decompressWaveStereo
 * Wrapper of StormLib functions.
 * \param inBuffer Buffer which is read from.
 * \param inBufferLength Buffer length which should be set to amount of bytes which should be read from buffer.
 * \param outBuffer Buffer which is read into.
 * \param outBuffer Buffer length which should have the initial size of buffer \p outBuffer. If the buffer is empty this value should be 0.
 * \return Returns witten bytes.
 */
int compressWaveMono(short* const inBuffer, int inBufferLength, unsigned char *outBuffer, int &outBufferLength, int compressionLevel);
int decompressWaveMono(unsigned char* const inBuffer, int inBufferLength, unsigned char *outBuffer, int &outBufferLength);
int compressWaveStereo(short* const inBuffer, int inBufferLength, unsigned char *outBuffer, int &outBufferLength, int compressionLevel);
int decompressWaveStereo(unsigned char* const inBuffer, int inBufferLength, unsigned char *outBuffer, int &outBufferLength);

/**
 * \throws boost::iostreams::bzip2_error Throws an exception if a compression error occurs.
 */
std::streamsize compressBzip2(istream &istream, ostream &ostream);
/**
 * \param bufferSize To optimize the decompression you can set this value to the MPQ archive's sector size. If unsure use boost::iostreams::default_device_buffer_size.
 * \throws boost::iostreams::bzip2_error Throws an exception if a decompression error occurs.
 */
std::streamsize decompressBzip2(istream &istream, ostream &ostream, int bufferSize);
/**
 * Compresses all data from \p istream into \p ostream.
 * \throws boost::iostreams::zlib_error Throws an exception if a compression error occurs.
 */
std::streamsize compressZlib(istream &istream, ostream &ostream);
/**
 * \param bufferSize To optimize the decompression you can set this value to the MPQ archive's sector size. If unsure use boost::iostreams::default_device_buffer_size.
 *  \throws boost::iostreams::zlib_error Throws an exception if a decompression error occurs.
 */
std::streamsize decompressZlib(istream &istream, ostream &ostream, int bufferSize);

void compressHuffman(char *pbOutBuffer, int * pdwOutLength, char *pbInBuffer, int dwInLength, int *pCmpType, int /* nCmpLevel */);
int decompressHuffman(char *pbOutBuffer, int *pdwOutLength, char *pbInBuffer, int /* dwInLength */);

MD5Checksum md5(const byte *buffer, std::size_t bufferSize);

}

}

#endif
