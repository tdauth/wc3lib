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

#ifndef WC3LIB_MPQ_SECTOR_HPP
#define WC3LIB_MPQ_SECTOR_HPP

#include <boost/detail/scoped_enum_emulation.hpp>
#include <boost/scoped_array.hpp>

#include "platform.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * \brief Blocks are divided into one or several sectors.
 * 
 * Each \ref Block instance consists of one or several sectors which can have different compression types (\ref Compression).
 * Compressed sectors do usually have the size of \ref Mpq::sectorSize() (except the last one which might be smaller).
 * \note Actually there is no read and write member functions since Sectors are created by \ref MpqFile instances when data is being read or written.
 */
class Sector // FIXME : private boost::noncopyable
{
	public:
		// TODO get best values
		static const int defaultWaveCompressionLevel = 3;
		static const int defaultHuffmanCompressionType = 0;

		BOOST_SCOPED_ENUM_START(Compression) /// \todo C++11 : byte
		{
			Uncompressed = 0,
			ImaAdpcmMono = 0x40, // IMA ADPCM mono
			ImaAdpcmStereo = 0x80, // IMA ADPCM stereo
			Huffman = 0x01, // Huffman encoded
			Deflated = 0x02, // Deflated (see ZLib)
			Imploded = 0x08, // Imploded (see PKWare Data Compression Library)
			Bzip2Compressed = 0x10, // BZip2 compressed (see BZip2)

			Sparse = 0x20, /// <a href="http://www.zezula.net/en/mpq/stormlib/sfileaddfileex.html">Source</a>.
			Lzma = 0x12 /// <a href="http://www.zezula.net/en/mpq/stormlib/sfileaddfileex.html">Source</a>.
		};
		BOOST_SCOPED_ENUM_END

		Sector(class MpqFile *mpqFile, uint32 index, uint32 offset, uint32 size);
		virtual ~Sector();

		/**
		 * Sames as \ref readData(const byte*, uint32) but detects buffer and buffer size automatically by simply using \ref Mpq::sectorSize() of the corresponding MPQ archive or less (if input stream hasn't that much data).
		 */
		std::streamsize readData(istream &istream, int waveCompressionLevel = defaultWaveCompressionLevel) throw (class Exception);
		/**
		 * Compresses and encrypts data from \p buffer of size \p bufferSize if necessary and writes it into the corresponding MPQ archive at the sector's place.
		 * \note Compressed data has to be less than or equal to \ref Mpq::sectorSize() of the corresponding MPQ archive. Otherwise it throws an exception.
		 * \return Returns size of bytes which has been written into the corresponding MPQ archive.
		 */
		std::streamsize readData(const byte *buffer, const uint32 bufferSize, int waveCompressionLevel = defaultWaveCompressionLevel) throw (class Exception);
		/**
		 * Writes sector data into output stream \p ostream.
		 * \return Returns size of written data.
		 */
		std::streamsize writeData(ostream &ostream) const throw (class Exception);
		/**
		 * Same as \ref writeData(ostream &) but doesn't work independently since it expects to be at the correct position in archive using \p istream as input archive stream.
		 */
		std::streamsize writeData(istream &istream, ostream &ostream) const throw (Exception);

		/**
		 * Jumps to the sector's position in input stream \p istream.
		 * \note Doesn't jump relative to the stream's current position.
		 */
		void seekg(istream &istream) const;
		/**
		 * Expects to be at the archive's start position in stream and jumps from the current position to the sector's offset in input stream \p istream.
		 */
		void seekgFromArchiveStart(istream &istream) const;
		void seekgFromBlockStart(istream &istream) const;

		class MpqFile* mpqFile() const;
		uint32 sectorIndex() const;
		uint32 sectorOffset() const;
		uint32 sectorSize() const;
		BOOST_SCOPED_ENUM(Compression) compression() const;

		/**
		 * Individual sectors in a compressed or imploded file may be stored uncompressed; this occurs if and only if the file data the sector contains could not be compressed by the algorithm(s) used (if the compressed sector size was greater than or equal to the size of the file data), and is indicated by the sector's size in SectorOffsetTable being equal to the size of the file data in the sector (which may be calculated from the FileSize).
		 * \return Returns true if sector could be compressed properly.
		 */
		bool compressionSucceded() const;

	protected:
		friend class Mpq;
		friend class MpqFile;

		void setCompression(byte value);
		uint32 sectorKey() const;

		/**
		 * For internal usage.
		 */
		void decompressData(boost::scoped_array<byte> &data, uint32 dataSize, ostream &ostream) const throw (Exception);

		class MpqFile *m_mpqFile;
		uint32 m_sectorIndex;
		uint32 m_sectorOffset;
		uint32 m_sectorSize; // not required, added by wc3lib, should be the compressed size!
		BOOST_SCOPED_ENUM(Compression) m_compression;
};

inline class MpqFile* Sector::mpqFile() const
{
	return this->m_mpqFile;
}

inline uint32 Sector::sectorIndex() const
{
	return this->m_sectorIndex;
}

inline uint32 Sector::sectorOffset() const
{
	return this->m_sectorOffset;
}

inline uint32 Sector::sectorSize() const
{
	return this->m_sectorSize;
}

inline BOOST_SCOPED_ENUM(Sector::Compression) Sector::compression() const
{
	return this->m_compression;
}

}

}

#endif
