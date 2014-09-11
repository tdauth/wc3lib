/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/scoped_array.hpp>

#include "algorithm.hpp" // include before #ifdef to get proper flag
#include "config.h"

#ifdef USE_ENCRYPTION
#include <crypto++/md5.h>
#else
#include <openssl/md5.h>
#endif

using namespace huffman;

namespace wc3lib
{

namespace mpq
{

void InitializeCryptTable(uint32 dwCryptTable[cryptTableSize])
{
	uint32 seed = 0x00100001;

	for (uint32 index1 = 0; index1 < 0x100; ++index1)
	{
		uint32 index2 = index1;

		for (short i = 0; i < 5; i++, index2 += 0x100)
		{
			seed  = (seed * 125 + 3) % 0x2AAAAB;
			uint32 temp1 = (seed & 0xFFFF) << 0x10;

			seed  = (seed * 125 + 3) % 0x2AAAAB;
			uint32 temp2 = (seed & 0xFFFF);

			dwCryptTable[index2] = (temp1 | temp2);
		}
	}
}

void EncryptData(const uint32 dwCryptTable[cryptTableSize], void *lpbyBuffer, uint32 dwLength, uint32 dwKey)
{
    assert(lpbyBuffer);

    uint32 *lpdwBuffer = (uint32 *)lpbyBuffer;
    uint32 seed = 0xEEEEEEEE;
    uint32 ch;

    dwLength /= sizeof(uint32);

    while(dwLength-- > 0)
    {
        seed += dwCryptTable[0x400 + (dwKey & 0xFF)];
        ch = *lpdwBuffer ^ (dwKey + seed);

        dwKey = ((~dwKey << 0x15) + 0x11111111) | (dwKey >> 0x0B);
        seed = *lpdwBuffer + seed + (seed << 5) + 3;

	*lpdwBuffer++ = ch;
    }
}

void DecryptData(const uint32 dwCryptTable[cryptTableSize], void *lpbyBuffer, uint32 dwLength, uint32 dwKey)
{
	assert(lpbyBuffer);

	uint32 *lpdwBuffer = (uint32 *)lpbyBuffer;
	uint32 seed = 0xEEEEEEEEL;
	uint32 ch;

	dwLength /= sizeof(uint32);

	while(dwLength-- > 0)
	{
		seed += dwCryptTable[0x400 + (dwKey & 0xFF)];
		ch = *lpdwBuffer ^ (dwKey + seed);

		dwKey = ((~dwKey << 0x15) + 0x11111111L) | (dwKey >> 0x0B);
		seed = ch + seed + (seed << 5) + 3;

		*lpdwBuffer++ = ch;
	}
}

/// Based on code from StormLib.
uint32 HashString(const uint32 dwCryptTable[cryptTableSize], const char *lpszString, HashType hashType)
{
    assert(lpszString);

    uint32 dwHashType = uint32(hashType);
    uint32 seed1 = 0x7FED7FEDL;
    uint32 seed2 = 0xEEEEEEEEL;
    int ch;

    while (*lpszString != 0)
    {
        ch = toupper(*lpszString++);

        seed1 = dwCryptTable[dwHashType + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
    }

    return seed1;
}

struct DataInfo
{
	char *inBuffer;		// Pointer to input data buffer
	char *inBufferEnd;	// End of the input buffer
	char *outBuffer;	// Pointer to output data buffer
	char *outBufferEnd; 	// Pointer to output data buffer
};

/**
 * Function loads data from the input buffer. Used by Pklib's "implode"
 * and "explode" function as user-defined callback.
 * \return Returns number of bytes loaded.
 * \author StormLib
 * \param buf Pointer to a buffer where to store loaded data.
 * \param size Max. number of bytes to read.
 * \param param Custom pointer, parameter of implode/explode.
 */
static unsigned int readBuffer(char *buf, unsigned int *size, void *param)
{
	struct DataInfo *info = reinterpret_cast<struct DataInfo*>(param);
	unsigned int maxAvailable = (unsigned int)(info->inBufferEnd - info->inBuffer);
	unsigned int toRead = *size;

	// Check the case when not enough data available
	if(toRead > maxAvailable)
		toRead = maxAvailable;

	// Load data and increment offsets
	memcpy(buf, info->inBuffer, toRead);
	info->inBuffer += toRead;
	assert(info->inBuffer <= info->inBufferEnd);

	 return toRead;
}

/**
 * Function for store output data. Used by Pklib's "implode" and "explode"
 * as user-defined callback.
 * \author StormLib
 * \param buf Pointer to data to be written.
 * \param size Number of bytes to write.
 * \param param Custom pointer, parameter of implode/explode
 */
static void writeBuffer(char *buf, unsigned int *size, void *param)
{
	struct DataInfo *info = reinterpret_cast<struct DataInfo*>(param);
	unsigned int maxWrite = (unsigned int)(info->outBufferEnd - info->outBuffer);
	unsigned int toWrite = *size;

	// Check the case when not enough space in the output buffer
	if(toWrite > maxWrite)
		toWrite = maxWrite;

	// Write output data and increments offsets
	memcpy(info->outBuffer, buf, toWrite);
	info->outBuffer += toWrite;
	assert(info->outBuffer <= info->outBufferEnd);
}

void compressPklib(char *outBuffer, int &outLength, char* const inBuffer, int inLength, int * /* pCmpType */, int /* compressionLevel */)
{
	struct DataInfo info; // Data information
	// Fill data information structure
	info.inBuffer  = inBuffer;
	info.inBufferEnd = inBuffer + inLength;
	info.outBuffer = outBuffer;
	info.outBufferEnd  = outBuffer + outLength;
	boost::scoped_array<char> workBuffer(new char[CMP_BUFFER_SIZE]); // Pklib's work buffer
	memset(workBuffer.get(), 0, CMP_BUFFER_SIZE);
	unsigned int dictonarySize;                             // Dictionary size
	unsigned int ctype = CMP_BINARY;                    // Compression type

	// Set the compression type and dictionary size
	 // Set the compression type and dictionary size
	if (inLength < 0x600)
	{
		dictonarySize = CMP_IMPLODE_DICT_SIZE1;
	}
	else if (0x600 <= inLength && inLength < 0xC00)
	{
		dictonarySize = CMP_IMPLODE_DICT_SIZE2;
	}
	else
	{
		dictonarySize = CMP_IMPLODE_DICT_SIZE3;
	}

	// Do the compression
	unsigned int state = implode(readBuffer, writeBuffer, workBuffer.get(), &info, &ctype, &dictonarySize);

	if (state != CMP_NO_ERROR)
	{
		throw Exception(boost::format(_("Implode error: \"%1%\".")) % pkglibError(state));
	}

	outLength = (int)(info.outBuffer - outBuffer);
}

void decompressPklib(char *outBuffer, int &outLength, char* const inBuffer, int inLength)
{
	struct DataInfo info; // Data information
	// Fill data information structure
	info.inBuffer = inBuffer;
	info.inBufferEnd = inBuffer + inLength;
	info.outBuffer = outBuffer;
	info.outBufferEnd = outBuffer + outLength;
	boost::scoped_array<char> workBuffer(new char[EXP_BUFFER_SIZE]); // Pklib's work buffer
	memset(workBuffer.get(), 0, EXP_BUFFER_SIZE);

	// Do the decompression
	unsigned int state = explode(readBuffer, writeBuffer, workBuffer.get(), &info);

	// If PKLIB is unable to decompress the data, return 0;
	if(info.outBuffer == outBuffer)
	{
		throw Exception(boost::format(_("Explode error: \"%1%\".")) % pkglibError(state));
	}

	// Give away the number of decompressed bytes
	outLength = (int)(info.outBuffer - outBuffer);
}

int compressWaveMono(short* const inBuffer, int inBufferLength, unsigned char *outBuffer, int &outBufferLength, int compressionLevel)
{
	// Prepare the compression level for the next compression
	// (After us, the Huffmann compression will be called)
	if (0 < compressionLevel && compressionLevel <= 2)
		compressionLevel = 4;
	else if (compressionLevel == 3)
		compressionLevel = 6;
	else
		compressionLevel = 5;

	if (outBuffer == 0)
		throw Exception(_("Output buffer is 0."));

	outBufferLength = CompressADPCM(outBuffer, outBufferLength, inBuffer, inBufferLength, 1, compressionLevel);

	return outBufferLength;
}

int decompressWaveMono(unsigned char* const inBuffer, int inBufferLength, unsigned char *outBuffer, int &outBufferLength)
{
	if (outBuffer == 0)
		throw Exception(_("Output buffer is 0."));

	outBufferLength = DecompressADPCM(outBuffer, outBufferLength, inBuffer, inBufferLength, 1);

	return outBufferLength;
}

int compressWaveStereo(short* const inBuffer, int inBufferLength, unsigned char *outBuffer, int &outBufferLength, int compressionLevel)
{
	// Prepare the compression type for the next compression
	// (After us, the Huffmann compression will be called)
	if(0 < compressionLevel && compressionLevel <= 2)
		compressionLevel = 4;
	else if(compressionLevel == 3)
		compressionLevel = 6;
	else
		compressionLevel = 5;

	if (outBuffer == 0)
		throw Exception(_("Output buffer is 0."));

	outBufferLength = CompressADPCM(outBuffer, outBufferLength, inBuffer, inBufferLength, 2, compressionLevel);

	return outBufferLength;
}

int decompressWaveStereo(unsigned char* const inBuffer, int inBufferLength, unsigned char *outBuffer, int &outBufferLength)
{
	if (outBuffer == 0)
		throw Exception(_("Output buffer is 0."));

	inBufferLength = DecompressADPCM(outBuffer, outBufferLength, inBuffer, inBufferLength, 2);

	return outBufferLength;
}

std::streamsize compressBzip2(istream &istream, ostream &ostream) throw (boost::iostreams::bzip2_error)
{
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	in.push(boost::iostreams::basic_bzip2_compressor<std::allocator<byte> >());
	in.push(istream);

	return boost::iostreams::copy(in, ostream);
}

std::streamsize decompressBzip2(istream &istream, ostream &ostream, int bufferSize) throw (boost::iostreams::bzip2_error)
{
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	boost::iostreams::basic_bzip2_decompressor<std::allocator<byte> > decompressor(boost::iostreams::bzip2::default_small, bufferSize);
	in.push(decompressor);
	in.push(istream);

	return boost::iostreams::copy(in, ostream);
}

std::streamsize compressZlib(istream &istream, ostream &ostream) throw (boost::iostreams::zlib_error)
{
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	in.push(boost::iostreams::basic_zlib_compressor<std::allocator<byte> >());
	in.push(istream);

	return boost::iostreams::copy(in, ostream);
}

std::streamsize decompressZlib(istream &istream, ostream &ostream, int bufferSize) throw (boost::iostreams::zlib_error)
{
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	boost::iostreams::basic_zlib_decompressor<std::allocator<byte> > decompressor(boost::iostreams::zlib_params(), bufferSize);
	//boost::iostreams::zlib_params params(boost::iostreams::zlib::default_compression, boost::iostreams::zlib::deflated, boost::iostreams::zlib::default_window_bits, boost::iostreams::zlib::default_mem_level, boost::iostreams::zlib::default_strategy, true, boost::iostreams::zlib::default_crc); // TEST omit header
	// omitting header doesn't fix it
	//in.push(boost::iostreams::basic_zlib_decompressor<std::allocator<byte> >(params, istream.rdbuf()->in_avail()));
	in.push(decompressor);
	in.push(istream);

	return boost::iostreams::copy(in, ostream);
}

void compressHuffman(
    char * pbOutBuffer,
    int * pcbOutBuffer,
    char * pbInBuffer,
    int cbInBuffer,
    int * pCmpType,
    int /* nCmpLevel */)
{
	THuffmannTree ht;                   // Huffmann tree for compression
	TOutputStream os;                   // Output stream

	// Initialize output stream
	os.pbOutBuffer = (unsigned char *)pbOutBuffer;
	os.cbOutSize   = *pcbOutBuffer;
	os.pbOutPos    = (unsigned char *)pbOutBuffer;
	os.dwBitBuff   = 0;
	os.nBits       = 0;

	// Initialize the Huffmann tree for compression
	ht.InitTree(true);

	*pcbOutBuffer = ht.DoCompression(&os, (unsigned char *)pbInBuffer, cbInBuffer, *pCmpType);

	// The following code is not necessary to run, because it has no
	// effect on the output data. It only clears the huffmann tree, but when
	// the tree is on the stack, who cares ?
	//  ht.UninitTree();
}

int decompressHuffman(char * pbOutBuffer, int * pcbOutBuffer, char * pbInBuffer, int cbInBuffer)
{
	THuffmannTree ht;
	TInputStream  is;

	// Initialize input stream
	is.pbInBufferEnd = (unsigned char *)pbInBuffer + cbInBuffer;
	is.pbInBuffer = (unsigned char *)pbInBuffer;
	is.BitBuffer  = 0;
	is.BitCount   = 0;

	// Initialize the Huffmann tree for compression
	ht.InitTree(false);
	*pcbOutBuffer = ht.DoDecompression((unsigned char *)pbOutBuffer, *pcbOutBuffer, &is);

	if(*pcbOutBuffer == 0)
		return 0;

	// The following code is not necessary to run, because it has no
	// effect on the output data. It only clears the huffmann tree, but when
	// the tree is on the stack, who cares ?
	//  ht.UninitTree();
	return 1;
}

MD5 md5(const byte *buffer, std::size_t bufferSize)
{
	MD5 md5 = 0;
#ifdef USE_ENCRYPTION
	CryptoPP::Weak1::MD5 checksum;
	checksum.CalculateDigest((unsigned char*)&md5, (unsigned char*)buffer, bufferSize);
#else
	MD5_CTX c;
	MD5_Init(&c);
	MD5_Update(&c, (const void *)buffer, bufferSize);
	MD5_Final((unsigned char*)&md5, &c);
#endif
	return md5;
}

}

}
