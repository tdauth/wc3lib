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

#include "algorithm.hpp"
#include "sector.hpp"
#include "archive.hpp"
#include "file.hpp"

namespace wc3lib
{

namespace mpq
{

namespace
{

inline const char* bzip2Error(int error)
{
	if (error == boost::iostreams::bzip2::data_error)
	{
		return _("Data error: Compressed data stream is corrupted.");
	}

	if (error == boost::iostreams::bzip2::data_error_magic)
	{
		return _("Data error magic: Compressed data stream does not begin with the 'magic' sequence 'B' 'Z' 'h'.");
	}

	if (error == boost::iostreams::bzip2::config_error)
	{
		return _("Config error: libbzip2 has been improperly configured for the current platform.");
	}

	return "";
}

inline const char* zlibError(int error)
{
	if (error == boost::iostreams::zlib::stream_error)
	{
		return _("Stream error: Compressed data stream or parameter configuration is corrupted.");
	}

	if (error == boost::iostreams::zlib::version_error)
	{
		return _("Version error: Incompatible versions.");
	}

	if (error == boost::iostreams::zlib::data_error)
	{
		return _("Data error: Compressed data stream is corrupted.");
	}

	if (error == boost::iostreams::zlib::buf_error)
	{
		return _("Buff error: Internal error.");
	}

	return "";
}

}

Sector::Sector(Archive *archive, Block *block, const string &fileName, uint32 index, uint32 offset, uint32 size, uint32 uncompressedSize)
: m_archive(archive)
, m_block(block)
, m_fileName(fileName)
, m_sectorIndex(index)
, m_sectorOffset(offset)
, m_sectorSize(size)
, m_uncompressedSize(uncompressedSize)
, m_compression(Sector::Compression::Uncompressed)
{
}

Sector::Sector(Archive *archive, Block *block, const string &fileName, uint32 index, uint32 offset, uint32 size, Sector::Compression compression)
: m_archive(archive)
, m_block(block)
, m_fileName(fileName)
, m_sectorIndex(index)
, m_sectorOffset(offset)
, m_sectorSize(size)
, m_uncompressedSize(0)
, m_compression(compression)
{
}

Sector::~Sector()
{
}

std::streamsize Sector::compress(istream &istream, int waveCompressionLevel)
{
	const std::streamoff end = wc3lib::endPosition(istream) - istream.tellg();
	const uint32 size = end < this->archive()->sectorSize() ? boost::numeric_cast<uint32>(end) : this->archive()->sectorSize();
	boost::scoped_array<byte> buffer(new byte[size]);
	std::streamsize bytes = 0;
	wc3lib::read(istream, buffer[0], bytes, size);

	return compress(buffer.get(), size, waveCompressionLevel);
}

std::streamsize Sector::compress(const byte *buffer, const uint32 bufferSize, int waveCompressionLevel)
{
	ofstream ofstream(archive()->path(), std::ios_base::out | std::ios_base::binary);

	if (!ofstream)
	{
		throw Exception(boost::format(_("Unable to open file %1%.")) % archive()->path());
	}

	this->seekp(ofstream);

	// NOTE as well, this byte is encrypted with the sector data, if applicable.
	return compress(buffer, bufferSize, ofstream, waveCompressionLevel);
}

std::streamsize Sector::decompress(ostream &ostream) const
{
	const uint32 dataSize = this->sectorSize();

	if (dataSize == 0)
	{
		return 0;
	}

	ifstream ifstream(archive()->path(), std::ios_base::in | std::ios_base::binary);

	if (!ifstream)
	{
		throw Exception(boost::format(_("Unable to open file %1%.")) % archive()->path());
	}

	seekg(ifstream);

	return decompress(ifstream, ostream);
}

std::streamsize Sector::decompress(istream &istream, ostream &ostream) const
{
	const uint32 dataSize = this->sectorSize();

	if (dataSize == 0)
	{
		return 0;
	}

	// TODO unnecessary copying into array! Directly decompress data from stream!
	boost::scoped_array<byte> data(new byte[dataSize]);
	std::streamsize bytes = 0;
	wc3lib::read(istream, data[0], bytes, dataSize);

	decompress(data.get(), dataSize, ostream);

	return bytes;
}

bool Sector::compressionSucceded() const
{
	/*
	 * Individual sectors in a compressed or imploded file may be stored uncompressed; this occurs if and only if the file data the sector contains could not be compressed by the algorithm(s) used (if the compressed sector size was greater than or equal to the size of the file data), and is indicated by the sector's size in SectorOffsetTable being equal to the size of the file data in the sector (which may be calculated from the FileSize).
	 */
	return this->sectorSize() < this->uncompressedSize();
}

void Sector::setCompression(Compression value)
{
	this->m_compression = value;
}

uint32 Sector::sectorKey() const
{

	return this->block()->fileKey(this->fileName()) + this->sectorIndex();
}

void Sector::seekp(ostream &ostream) const
{
	ostream.seekp(archive()->startPosition());
	ostream.seekp(boost::numeric_cast<std::streamoff>(this->sectorOffset()), std::ios::cur);

	if (archive()->format() == Archive::Format::Mpq1)
	{
		ostream.seekp(boost::numeric_cast<std::streamoff>(this->block()->blockOffset()), std::ios::cur);
	}
	else
	{
		ostream.seekp(boost::numeric_cast<std::streamoff>(this->block()->largeOffset()), std::ios::cur);
	}
}

void Sector::seekg(istream &istream) const
{
	istream.seekg(this->archive()->startPosition());

	seekgFromArchiveStart(istream);
}

void Sector::seekgFromArchiveStart(istream &istream) const
{
	if (this->archive()->format() == Archive::Format::Mpq1)
	{
		istream.seekg(boost::numeric_cast<std::streamoff>(this->block()->blockOffset()), std::ios::cur);
	}
	else
	{
		istream.seekg(boost::numeric_cast<std::streamoff>(this->block()->largeOffset()), std::ios::cur);
	}

	seekgFromBlockStart(istream);
}

void Sector::seekgFromBlockStart(istream &istream) const
{
	istream.seekg(boost::numeric_cast<std::streamoff>(this->sectorOffset()), std::ios::cur);
}

#ifdef DEBUG
namespace
{
	void sizeCheck(uint32 dataSize, const Sector &sector, const std::string &compression)
	{
		// decompression failed
		if (dataSize <= sector.sectorSize()) // smaller size results because of the compression byte
		{
			std::cerr << boost::format(_("%1%: Sector %2% with size %3% has same or smaller size after decompression: %4%. Compression \"%5%\".")) % "Sector" % sector.sectorIndex() % sector.sectorSize() % dataSize % compression << std::endl;
		}
	}
}
#endif

byte* Sector::compress(const byte *buffer, uint32 bufferSize, Block::Flags flags, Compression compression, uint32 &size, int waveCompressionLevel)
{
	boost::scoped_array<byte> data;

	// Imploded sectors are the raw compressed data following compression with the implode algorithm (these sectors can only be in imploded files).
	if (flags & Block::Flags::IsImploded)
	{
		if (compression & Sector::Compression::Imploded)
		{
			char *newData = 0;
			compressPklib(newData, *reinterpret_cast<int*>(&size), (char*const)buffer, boost::numeric_cast<int>(bufferSize), 0, 0);
			data.reset(newData);
		}
	}
	// Compressed sectors (only found in compressed - not imploded - files) are compressed with one or more compression algorithms.
	else if (flags & Block::Flags::IsCompressed)
	{
		// TODO check compression order
		if (compression & Sector::Compression::ImaAdpcmMono) // IMA ADPCM mono
		{
			int outLength = boost::numeric_cast<int>(bufferSize);
			boost::scoped_array<byte> out(new byte[outLength]); // NOTE do always allocate enough memory.

			if (compressWaveMono((short int* const)buffer, boost::numeric_cast<int>(bufferSize), (unsigned char*)out.get(), outLength, waveCompressionLevel) == 0)
			{
				throw Exception(_("Wave mono decompression error."));
			}

			if (outLength < bufferSize)
			{
				data.reset(new byte[outLength]);
				memcpy(data.get(), out.get(), outLength);
			}
			else
			{
				data.swap(out);
			}

			size = outLength;
		}

		if (compression & Sector::Compression::ImaAdpcmStereo) // IMA ADPCM stereo
		{
			int outLength = boost::numeric_cast<int>(bufferSize);
			boost::scoped_array<byte> out(new byte[outLength]); // NOTE do always allocate enough memory.

			if (compressWaveStereo((short int* const)buffer, boost::numeric_cast<int>(bufferSize), (unsigned char*)out.get(), outLength, waveCompressionLevel) == 0)
			{
				throw Exception(_("Wave stereo decompression error."));
			}

			if (outLength < bufferSize)
			{
				data.reset(new byte[outLength]);
				memcpy(data.get(), out.get(), outLength);
			}
			else
			{
				data.swap(out);
			}

			size = outLength;
		}

		if (compression & Sector::Compression::Huffman) // Huffman encoded
		{
			int outLength = boost::numeric_cast<int>(bufferSize);
			boost::scoped_array<byte> out(new byte[outLength]); // NOTE do always allocate enough memory.
			int compression = defaultHuffmanCompressionType;
			compressHuffman(out.get(), &outLength, (char*)buffer, boost::numeric_cast<unsigned>(bufferSize), &compression, 0);

			if (outLength < bufferSize)
			{
				data.reset(new byte[outLength]);
				memcpy(data.get(), out.get(), outLength);
			}
			else
			{
				data.swap(out);
			}

			size = outLength;
		}

		if (compression & Sector::Compression::Deflated) // Deflated (see ZLib)
		{
			iarraystream istream(buffer, bufferSize);
			//arraystream ostream;
			std::basic_stringstream<byte> ostream; // TODO unbuffered stream doesnt work.

			try
			{
				size = compressZlib(istream, ostream);
			}
			catch (boost::iostreams::zlib_error &error)
			{
				throw Exception(zlibError(error.error()));
			}

			data.reset(new byte[size]);
			ostream.read(data.get(), size);
		}

		// Imploded sectors are the raw compressed data following compression with the implode algorithm (these sectors can only be in imploded files).
		// NOTE but in this situation it's compressed not imploded file!!!
		if (compression & Sector::Compression::Imploded)
		{
			std::cerr << _("Warning: Imploded sector in not imploded file!") << std::endl;

			int outLength = boost::numeric_cast<int>(bufferSize);
			boost::scoped_array<byte> out(new byte[outLength]); // NOTE do always allocate enough memory.

			compressPklib(out.get(), outLength, (char*const)buffer, boost::numeric_cast<int>(bufferSize), 0, 0);

			if (outLength < bufferSize)
			{
				data.reset(new byte[outLength]);
				memcpy(data.get(), out.get(), outLength);
			}
			else
			{
				data.swap(out);
			}
		}

		if (compression & Sector::Compression::Bzip2Compressed) // BZip2 compressed (see BZip2)
		{
			iarraystream istream(buffer, bufferSize);
			//arraystream ostream;
			std::basic_stringstream<byte> ostream; // TODO unbuffered stream doesnt work

			try
			{
				size = compressBzip2(istream, ostream);
			}
			catch (boost::iostreams::bzip2_error &error)
			{

				throw Exception(bzip2Error(error.error()));
			}

			data.reset(new byte[size]);
			ostream.read(data.get(), size);
		}
	}

	 // TODO reset ownership and directly return data!
	byte *result = new byte[size];
	memcpy(result, data.get(), size);

	return result;
}

std::streamsize Sector::writeCompressed(const byte *compressedBuffer, uint32 compressedBufferSize, ostream &out)
{
	boost::scoped_array<byte> buffer;
	uint32 bufferSize = 0;

	// store compression byte as well before encrypting all data
	// TODO maybe we could increase speed when starting encryption at the second index and using the default compression type byte without recopying all memory
	if (this->block()->flags() & Block::Flags::IsCompressed)
	{
		bufferSize = compressedBufferSize + 1;
		buffer.reset(new byte[bufferSize]);
		buffer[0] = static_cast<byte>(compression());
		memcpy(&buffer[1], compressedBuffer, compressedBufferSize);

	}
	else
	{
		bufferSize = compressedBufferSize;
		buffer.reset(new byte[bufferSize]);
		memcpy(buffer.get(), compressedBuffer, compressedBufferSize);
	}

	/*
	If the file is encrypted, each sector (after compression/implosion, if applicable) is encrypted with the file's key.
	Each sector is encrypted using the key + the 0-based index of the sector in the file.
	NOTE compression type byte (if existing) is encrypted as well!
	*/
	if (this->block()->flags() & Block::Flags::IsEncrypted)
	{
		EncryptData(Archive::cryptTable(), (void*)buffer.get(), bufferSize, this->sectorKey());
	}

	std::streamsize bytes = 0;
	wc3lib::write(out, buffer[0], bytes, bufferSize);

	return bytes;
}

std::streamsize Sector::compress(const byte *buffer, uint32 bufferSize, ostream &ostream, int waveCompressionLevel)
{
	uint32 size = 0;
	boost::scoped_array<byte> data(Sector::compress(buffer, bufferSize, this->block()->flags(), this->compression(), size, waveCompressionLevel));

	const std::streamsize bytes = this->writeCompressed(buffer, bufferSize, ostream);

	this->m_sectorSize = size;
	this->m_uncompressedSize = bufferSize;

	return bytes;
}


void Sector::decompress(byte *data, uint32 dataSize, ostream &ostream) const
{
	/*
	If the file is encrypted, each sector (after compression/implosion, if applicable) is encrypted with the file's key.
	Each sector is encrypted using the key + the 0-based index of the sector in the file.
	NOTE compression type byte (if existing) is encrypted as well!
	*/
	if (this->block()->flags() & Block::Flags::IsEncrypted)
	{
		DecryptData(Archive::cryptTable(), reinterpret_cast<void*>(data), dataSize, this->sectorKey());
	}

	// NOTE This byte counts towards the total sector size, meaning that the sector will be stored uncompressed if the data cannot be compressed by at least two bytes
	if (compressionSucceded())
	{
		/*
		 * Now decompress in the correct order
		 */
		int outLength = boost::numeric_cast<int>(archive()->sectorSize());
		// NOTE do always allocate enough memory.
		boost::scoped_array<byte> buffer(new byte[outLength]);

		// Imploded sectors are the raw compressed data following compression with the implode algorithm (these sectors can only be in imploded files).
		if (this->block()->flags() & Block::Flags::IsImploded)
		{
			if (this->compression() & Sector::Compression::Imploded)
			{
				decompressPklib(buffer.get(), outLength, reinterpret_cast<char* const>(data), boost::numeric_cast<int>(dataSize));

#ifdef DEBUG
				sizeCheck(outLength, *this, "Imploded:Imploded");
#endif
			}
		}
		// Compressed sectors (only found in compressed - not imploded - files) are compressed with one or more compression algorithms.
		else if (this->block()->flags() & Block::Flags::IsCompressed)
		{
			const_cast<Sector*>(this)->setCompression(static_cast<Sector::Compression>(data[0])); // first byte contains compression

			// NOTE the following decompression statements do skip this value (starting at buffer index 1)
			byte *realData = &data[1];
			const uint32 realDataSize = dataSize - 1;

			if (this->compression() & Sector::Compression::Bzip2Compressed) // BZip2 compressed (see BZip2)
			{
				/*
				 * NOTE Does not copy the stream. Only provides a source.
				 */
				iarraystream istream(realData, realDataSize);
				// TODO unbuffered streams are not supported.
				stringstream ostream;
				std::streamsize size = 0;

				try
				{
					size = decompressBzip2(istream, ostream, archive()->sectorSize());
				}
				catch (boost::iostreams::bzip2_error &error)
				{
					throw Exception(bzip2Error(error.error()));
				}

#ifdef DEBUG
				sizeCheck(size, *this, "Bzip2");
#endif

				outLength = boost::numeric_cast<int>(size);
				ostream.read(buffer.get(), size);
			}

			// Imploded sectors are the raw compressed data following compression with the implode algorithm (these sectors can only be in imploded files).
			// NOTE but in this situation it's compressed not imploded file!!!
			if (this->compression() & Sector::Compression::Imploded)
			{
				std::cerr << boost::format(_("%1%: Sector %2% is imploded but file is not.")) % archive()->path() % sectorIndex() << std::endl;
				decompressPklib(buffer.get(), outLength, reinterpret_cast<char* const>(realData), boost::numeric_cast<int>(realDataSize));

#ifdef DEBUG
				sizeCheck(outLength, *this, "Imploded");
#endif
			}

			if (this->compression() & Sector::Compression::Deflated) // Deflated (see ZLib)
			{
				/*
				 * NOTE Does not copy the stream. Only provides a source.
				 */
				iarraystream istream(realData, realDataSize);
				// TODO unbuffered streams are not supported.
				stringstream ostream;
				std::streamsize size = 0;

				try
				{
					size = decompressZlib(istream, ostream, archive()->sectorSize());
				}
				catch (boost::iostreams::zlib_error &error)
				{
					throw Exception(zlibError(error.error()));
				}

#ifdef DEBUG
				sizeCheck(size, *this, "Deflated");
#endif

				outLength = boost::numeric_cast<int>(size);
				ostream.read(buffer.get(), size);
			}

			if (this->compression() & Sector::Compression::Huffman) // Huffman encoded
			{
				const int state = decompressHuffman(buffer.get(), &outLength, (char*)realData, boost::numeric_cast<int>(realDataSize));
				//int state = huffman_decode_memory((const unsigned char*)realData, boost::numeric_cast<unsigned int>(realDataSize), &out, &outLength);

				if (state != 1)
				{
					throw Exception(boost::format(_("Huffman error %1%.")) % state);
				}

#ifdef DEBUG
				sizeCheck(outLength, *this, "Huffman");
#endif
			}

			if (this->compression() & Sector::Compression::ImaAdpcmStereo) // IMA ADPCM stereo
			{
				if (decompressWaveStereo((unsigned char*const)realData, boost::numeric_cast<int>(realDataSize), (unsigned char*)buffer.get(), outLength) == 0)
				{
					throw Exception(_("Wave stereo decompression error."));
				}

#ifdef DEBUG
				sizeCheck(outLength, *this, "Stereo");
#endif
			}

			if (this->compression() & Sector::Compression::ImaAdpcmMono) // IMA ADPCM mono
			{
				if (decompressWaveMono((unsigned char*const)realData, boost::numeric_cast<int>(realDataSize), (unsigned char*)buffer.get(), outLength) == 0)
				{
					throw Exception(_("Wave mono decompression error."));
				}

#ifdef DEBUG
				sizeCheck(outLength, *this, "Mono");
#endif
			}
		}

		/*
		 * Now write the decompressed buffer into the output stream.
		 */
		ostream.write(buffer.get(), outLength);
	}
	else
	{
		// If data could not be compressed properly there is no compression byte since it can always be determined if compression succeeded by checking the byte counts.
		if ((this->block()->flags() & Block::Flags::IsCompressed) || (this->block()->flags() & Block::Flags::IsImploded))
		{
			std::cerr << boost::format(_("%1%: Sector %2% with compressed size %3% and uncompressed size %4% could not be compressed properly. Archive sector size (uncompressed) is %5%.")) % "Sector" % sectorIndex() % this->sectorSize() % this->uncompressedSize() % archive()->sectorSize() << std::endl;
		}

		/*
		 * Now write the original data into the output stream which could not be compressed properly.
		 */
		ostream.write(data, dataSize);
	}
}

}

}
