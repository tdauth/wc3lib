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

#include <boost/scoped_array.hpp>

#include "platform.hpp"
#include "block.hpp"

namespace wc3lib
{

namespace mpq
{

class Archive;
class File;

/**
 * \brief Blocks are divided into one or several sectors.
 *
 * Each \ref Block instance consists of one or several sectors which can have different compression types (\ref Compression).
 * Compressed sectors do usually have the size of \ref Archive::sectorSize() (except the last one which might be smaller).
 * \note Actually there is no read and write member functions since Sectors are created by \ref File instances when data is being read or written.
 */
class Sector // FIXME : private boost::noncopyable
{
	public:
		/**
		 * The content of a file is divided into one or more sectors.
		 * These sectors only have to be read or written when reading or writing the file's content.
		 * Therefore we do not need a heap allocated container.
		 */
		typedef std::vector<Sector> Sectors;

		// TODO get best values
		static const int defaultWaveCompressionLevel = 3;
		static const int defaultHuffmanCompressionType = 0;

		/**
		 * For compressed files (\ref mpq::File::isCompressed()) or imploded files (\ref mpq::File::isImploded()) a compression type might be set
		 * for each sector but only if its compression succeded (\ref compressionSucceded()).
		 *
		 * The different decompression algorithms are provided in \ref algorithm.hpp.
		 *
		 * Use \ref compression() to get a sectors compression type.
		 */
		enum class Compression : uint8
		{
			Uncompressed = 0,
			ImaAdpcmMono = 0x40, /// IMA ADPCM mono
			ImaAdpcmStereo = 0x80, /// IMA ADPCM stereo
			Huffman = 0x01, /// Huffman encoded
			Deflated = 0x02, /// Deflated (see ZLib)
			Imploded = 0x08, /// Imploded (see PKWare Data Compression Library)
			Bzip2Compressed = 0x10, /// BZip2 compressed (see BZip2)

			Sparse = 0x20, /// <a href="http://www.zezula.net/en/mpq/stormlib/sfileaddfileex.html">Source</a>.
			Lzma = 0x12 /// <a href="http://www.zezula.net/en/mpq/stormlib/sfileaddfileex.html">Source</a>.
		};

		/**
		 * This constructor is used when the sector is read from a specific file.
		 */
		Sector(Archive *archive, Block *block, const string &fileName, uint32 index, uint32 offset, uint32 size, uint32 uncompressedSize);

		/**
		 * This constructor is used when a sector is newly created for writing a specific file.
		 */
		Sector(Archive *archive, Block *block, const string &fileName, uint32 index, uint32 offset, uint32 size, Compression compression);

		virtual ~Sector();

		/**
		 * Same as \ref readData(const byte*, const uint32, int) but detects buffer and buffer size automatically by simply using \ref Archive::sectorSize() of the corresponding MPQ archive or less (if input stream hasn't that much data).
		 */
		std::streamsize compress(istream &istream, int waveCompressionLevel = defaultWaveCompressionLevel);
		/**
		 * Compresses and encrypts data from \p buffer of size \p bufferSize if necessary and writes it into the corresponding MPQ archive at the sector's place.
		 * \note Compressed data has to be less than or equal to \ref Archive::sectorSize() of the corresponding MPQ archive. Otherwise it throws an exception.
		 * \return Returns size of bytes which has been written into the corresponding MPQ archive.
		 */
		std::streamsize compress(const byte *buffer, const uint32 bufferSize, int waveCompressionLevel = defaultWaveCompressionLevel);
		/**
		 * Writes sector data into output stream \p ostream.
		 * \return Returns size of written data.
		 */
		std::streamsize decompress(ostream &ostream) const;
		/**
		 * Same as \ref Sector::writeData(wc3lib::ostream &) but doesn't work independently since it expects to be at the correct position in archive using \p istream as input archive stream.
		 */
		std::streamsize decompress(istream &istream, ostream &ostream) const;

		/**
		 * Seeks the put position of the given output stream to the offset of the sector.
		 */
		void seekp(ostream &ostream) const;

		/**
		 * Jumps to the sector's position in input stream \p istream.
		 * This means: archive offset + block offset + sector offset.
		 * \note Doesn't jump relative to the stream's current position.
		 *
		 * \sa seekgFromArchiveStart() seekgFromBlockStart()
		 */
		void seekg(istream &istream) const;
		/**
		 * Expects to be at the archive's start position in stream and jumps from the current position to the sector's offset in input stream \p istream.
		 */
		void seekgFromArchiveStart(istream &istream) const;
		void seekgFromBlockStart(istream &istream) const;

		Archive* archive() const;
		Block* block() const;
		const string& fileName() const;

		/**
		 * \return Returns the index of the sector starting with 0.
		 */
		uint32 sectorIndex() const;
		/**
		 * \return Returns the offset of the sector relative to the block offset.
		 */
		uint32 sectorOffset() const;
		/**
		 * \return Returns the compressed size of the sector.
		 */
		uint32 sectorSize() const;
		/**
		 * \return Returns the uncompressed size of the sector.
		 */
		uint32 uncompressedSize() const;
		/**
		 * The compression mode determines how the data must be compressed/decompressed.
		 *
		 * \return Returns the compression mode of the sector.
		 */
		Compression compression() const;

		/**
		 * Individual sectors in a compressed or imploded file may be stored uncompressed; this occurs if and only if the file data the sector contains could not be compressed by the algorithm(s) used (if the compressed sector size was greater than or equal to the size of the file data), and is indicated by the sector's size in SectorOffsetTable being equal to the size of the file data in the sector (which may be calculated from the FileSize).
		 * \return Returns true if sector could be compressed properly.
		 */
		bool compressionSucceded() const;

		/**
		 * Writes data from \p buffer with size \p bufferSize compressed into an output array which is returned.
		 * \note The returned array is allocated on the heap and has to be deleted!
		 */
		static byte* compress(const byte *buffer, uint32 bufferSize, Block::Flags flags, Compression compression, uint32 &size, int waveCompressionLevel = defaultWaveCompressionLevel);
		/**
		 * Writes the file sectors' meta data into output stream \p ostream.
		 * This requires some information about the file data already since for encrypted files this information is used to encrypt the sector table.
		 * Besides the total compressed size of the file data is present in the last entry of the sector table.
		 * The sector table is omitted if the file is a single unit and compressed or imploded or neither compressed or imploded.
		 * \param sectors The sectors to be written.
		 * \param flags The flags of the block corresponding to the file.
		 * \param uncompressedFileSize The uncompressed size of the file data.
		 * \param compressedFileSize The compressed size of the file data.
		 * \param blockOffset The offset of the corresponding block.
		 * \param fileName The name of the file.
		 * \return Returns the number of written bytes to the output stream.
		 * \sa File::sectors()
		 */
		static std::streamsize writeSectors(ostream &ostream, const Sectors &sectors, Block::Flags flags, uint32 compressedFileSize, uint32 uncompressedFileSize, uint32 blockOffset, const string &fileName);

	protected:
		friend Archive;
		friend File;

		void setCompression(Compression value);
		/**
		 * The sectors key is calculated by using the file key of the file with the name \p name and adding the sector's index.
		 * \return Returns the sector key.
		 */
		uint32 sectorKey() const;

		/**
		 * Writes the compressed buffer for the sector into the output stream \p out.
		 * If it is compressed the compression byte will be prepended.
		 * If the block is encrypted, the sector will be encrypted.
		 *
		 * \return Returns the number of written bytes.
		 */
		std::streamsize writeCompressed(const byte *compressedBuffer, uint32 compressedBufferSize, ostream &out);

		/**
		 * Compresses data from an input buffer into an output stream and updates the sector's compressed and uncompressed size.
		 * \param buffer Input buffer.
		 * \param bufferSize Input buffer size.
		 * \param ostream Output stream.
		 *
		 * \return Returns the number of compressed bytes.
		 */
		std::streamsize compress(const byte *buffer, uint32 bufferSize, ostream &ostream, int waveCompressionLevel = defaultWaveCompressionLevel);

		/**
		 * Decompresses data from an input buffer into an output stream.
		 *
		 * For internal usage.
		 */
		void decompress(byte *data, uint32 dataSize, ostream &ostream) const;

		/**
		 * The corresponding archive of the sector which is required to determine the sector size.
		 */
		Archive *m_archive;
		/**
		 * The corresponding block the sector does belong to which contains offset, size and flag information.
		 */
		Block *m_block;
		string m_fileName; // the file name is used for the file key calculation
		uint32 m_sectorIndex;
		uint32 m_sectorOffset;
		uint32 m_sectorSize; // not required, added by wc3lib, should be the compressed size!
		uint32 m_uncompressedSize;
		Compression m_compression;
};

inline constexpr bool operator&(Sector::Compression x, Sector::Compression y)
{
	return static_cast<bool>(static_cast<byte>(x) & static_cast<byte>(y));
}

inline Archive* Sector::archive() const
{
	return this->m_archive;
}

inline Block* Sector::block() const
{
	return this->m_block;
}

inline const string& Sector::fileName() const
{
	return this->m_fileName;
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

inline uint32 Sector::uncompressedSize() const
{
	return this->m_uncompressedSize;
}

inline Sector::Compression Sector::compression() const
{
	return this->m_compression;
}

}

}

#endif
