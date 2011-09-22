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
#include "mpq.hpp"
#include "mpqfile.hpp"

namespace wc3lib
{

namespace mpq
{

Sector::Sector(class MpqFile *mpqFile) : m_mpqFile(mpqFile), m_sectorIndex(0), m_sectorOffset(0), m_sectorSize(0), m_compression(Sector::Compression::Uncompressed)
{
}

Sector::~Sector()
{
}

std::streamsize Sector::readData(istream &istream, short pkgCompressionType, int waveCompressionLevel) throw (class Exception)
{
	const std::streamoff end = wc3lib::endPosition(istream) - istream.tellg();
	const uint32 size = end < this->mpqFile()->mpq()->sectorSize() ? boost::numeric_cast<uint32>(end) : this->mpqFile()->mpq()->sectorSize();
	boost::scoped_array<byte> buffer(new byte[size]);
	std::streamsize bytes = 0;
	wc3lib::read(istream, buffer[0], bytes, size);

	return readData(buffer.get(), size);
}

std::streamsize Sector::readData(const byte *buffer, const uint32 bufferSize, short pkgCompressionType, int waveCompressionLevel) throw  (class Exception)
{
	uint32 size = 0;
	boost::scoped_array<byte> data;

	// Imploded sectors are the raw compressed data following compression with the implode algorithm (these sectors can only be in imploded files).
	if (this->mpqFile()->isImploded())
	{
		if (this->compression() & Sector::Compression::Imploded)
		{
			char *newData = 0;
			compressPklib(newData, *reinterpret_cast<int*>(&size), (char*const)buffer, boost::numeric_cast<int>(bufferSize), pkgCompressionType, 0);
			data.reset(newData);
		}
	}
	// Compressed sectors (only found in compressed - not imploded - files) are compressed with one or more compression algorithms.
	else if (this->mpqFile()->isCompressed())
	{
		// TODO check compression order
		if (this->compression() & Sector::Compression::ImaAdpcmMono) // IMA ADPCM mono
		{
			int outLength = boost::numeric_cast<int>(mpqFile()->mpq()->sectorSize());
			boost::scoped_array<byte> out(new byte[outLength]); // NOTE do always allocate enough memory.

			if (compressWaveMono((short int* const)buffer, boost::numeric_cast<int>(bufferSize), (unsigned char*)out.get(), outLength, waveCompressionLevel) == 0)
				throw Exception(_("Wave mono decompression error."));

			if (outLength < mpqFile()->mpq()->sectorSize())
			{
				data.reset(new byte[outLength]);
				memcpy(data.get(), out.get(), outLength);
			}
			else
				data.swap(out);

			size = outLength;
		}

		if (this->compression() & Sector::Compression::ImaAdpcmStereo) // IMA ADPCM stereo
		{
			int outLength = boost::numeric_cast<int>(mpqFile()->mpq()->sectorSize());
			boost::scoped_array<byte> out(new byte[outLength]); // NOTE do always allocate enough memory.

			if (compressWaveStereo((short int* const)buffer, boost::numeric_cast<int>(bufferSize), (unsigned char*)out.get(), outLength, waveCompressionLevel) == 0)
				throw Exception(_("Wave stereo decompression error."));

			if (outLength < mpqFile()->mpq()->sectorSize())
			{
				data.reset(new byte[outLength]);
				memcpy(data.get(), out.get(), outLength);
			}
			else
				data.swap(out);

			size = outLength;
		}

		if (this->compression() & Sector::Compression::Huffman) // Huffman encoded
		{
			int outLength = boost::numeric_cast<int>(mpqFile()->mpq()->sectorSize());
			boost::scoped_array<byte> out(new byte[outLength]); // NOTE do always allocate enough memory.
			int compression = defaultHuffmanCompressionType;
			int state = compressHuffman(out.get(), &outLength, (char*)buffer, boost::numeric_cast<unsigned>(bufferSize), &compression, 0);

			if (state != 0)
				throw Exception(boost::format(_("Huffman error %1%.")) % state);

			if (outLength < mpqFile()->mpq()->sectorSize())
			{
				data.reset(new byte[outLength]);
				memcpy(data.get(), out.get(), outLength);
			}
			else
				data.swap(out);

			size = outLength;
		}

		if (this->compression() & Sector::Compression::Deflated) // Deflated (see ZLib)
		{
			iarraystream istream(buffer, bufferSize);
			//arraystream ostream;
			std::basic_stringstream<byte> ostream; // TODO unbuffered stream doesnt work

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
			//ostream >> data.get();
		}

		// Imploded sectors are the raw compressed data following compression with the implode algorithm (these sectors can only be in imploded files).
		// NOTE but in this situation it's compressed not imploded file!!!
		if (this->compression() & Sector::Compression::Imploded)
		{
			std::cerr << _("Warning: Imploded sector in not imploded file!") << std::endl;

			int outLength = boost::numeric_cast<int>(mpqFile()->mpq()->sectorSize());
			boost::scoped_array<byte> out(new byte[outLength]); // NOTE do always allocate enough memory.

			compressPklib(out.get(), outLength, (char*const)buffer, boost::numeric_cast<int>(bufferSize), pkgCompressionType, 0);

			if (outLength < mpqFile()->mpq()->sectorSize())
			{
				data.reset(new byte[outLength]);
				memcpy(data.get(), out.get(), outLength);
			}
			else
				data.swap(out);
		}

		if (this->compression() & Sector::Compression::Bzip2Compressed) // BZip2 compressed (see BZip2)
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
			//ostream >> data.get();
		}
	}

	if (size > this->mpqFile()->mpq()->sectorSize())
		throw Exception(boost::format(_("Size %1% is too big. %2% is maximum sector size for archive.")) % size % mpqFile()->mpq()->sectorSize());


	// store compression byte as well before encrypting all data
	// TODO maybe we could increase speed when starting encryption at the second index and using the default compression type byte without recopying all memory
	if (this->mpqFile()->isCompressed())
	{
		boost::scoped_array<byte> newData(new byte[size + 1]);
		newData[0] = compression();
		memcpy(&newData[1], data.get(), size);
		data.swap(newData);
	}

	/*
	If the file is encrypted, each sector (after compression/implosion, if applicable) is encrypted with the file's key.
	Each sector is encrypted using the key + the 0-based index of the sector in the file.
	NOTE compression type byte (if existing) is encrypted as well!
	*/
	if (this->mpqFile()->isEncrypted())
		EncryptData(Mpq::cryptTable(), (void*)data.get(), size, this->sectorKey());

	boost::interprocess::file_lock fileLock(this->mpqFile()->mpq()->path().string().c_str());

	if (!fileLock.try_lock())
		throw Exception(boost::format("Unable to lock MPQ archive of file \"%1%\".") % this->mpqFile()->path());

	ofstream ofstream(this->mpqFile()->mpq()->path(), std::ios_base::out | std::ios_base::binary);

	if (!ofstream)
		throw Exception(boost::format(_("Unable to open file \"%1%\".")) % this->mpqFile()->mpq()->path().string());

	ofstream.seekp(this->mpqFile()->mpq()->startPosition());
	ofstream.seekp(boost::numeric_cast<std::streamoff>(this->sectorOffset()), std::ios::cur);

	if (this->mpqFile()->mpq()->format() == Mpq::Format::Mpq1)
		ofstream.seekp(boost::numeric_cast<std::streamoff>(this->mpqFile()->hash()->block()->blockOffset()), std::ios::cur);
	else
		ofstream.seekp(boost::numeric_cast<std::streamoff>(this->mpqFile()->hash()->block()->largeOffset()), std::ios::cur);

	// NOTE as well, this byte is encrypted with the sector data, if applicable.
	std::streamsize bytes = 0;
	wc3lib::write(ofstream, data[0], bytes, size);
	this->m_sectorSize = size;
	ofstream.close();
	fileLock.unlock();

	return bytes;
}

// TODO sector offset seems to be wrong (when reading compression types in MpqFile::read there is other values than here - here the whole sector data including compression type is decrypted!)
std::streamsize Sector::writeData(ostream &ostream) const throw (class Exception)
{
	boost::interprocess::file_lock fileLock(this->mpqFile()->mpq()->path().string().c_str());

	if (!fileLock.try_lock())
		throw Exception(boost::format("Unable to lock MPQ archive of file \"%1%\" when trying to read sector data.") % this->mpqFile()->path());

	ifstream ifstream(this->mpqFile()->mpq()->path(), std::ios_base::in | std::ios_base::binary);

	if (!ifstream)
		throw Exception(boost::format(_("Sector: Unable to open file \"%1%\".")) % this->mpqFile()->mpq()->path().string());

	ifstream.seekg(this->mpqFile()->mpq()->startPosition());
	ifstream.seekg(boost::numeric_cast<std::streamoff>(this->sectorOffset()), std::ios::cur);

	if (this->mpqFile()->mpq()->format() == Mpq::Format::Mpq1)
		ifstream.seekg(boost::numeric_cast<std::streamoff>(this->mpqFile()->hash()->block()->blockOffset()), std::ios::cur);
	else
		ifstream.seekg(boost::numeric_cast<std::streamoff>(this->mpqFile()->hash()->block()->largeOffset()), std::ios::cur);

	// NOTE as well, this byte is encrypted with the sector data, if applicable.
	uint32 dataSize = this->sectorSize();
	boost::scoped_array<byte> data(new byte[dataSize]);
	std::streamsize bytes = 0;
	wc3lib::read(ifstream, data[0], bytes, dataSize);
	ifstream.close();
	fileLock.unlock();

	/*
	If the file is encrypted, each sector (after compression/implosion, if applicable) is encrypted with the file's key.
	Each sector is encrypted using the key + the 0-based index of the sector in the file.
	NOTE compression type byte (if existing) is encrypted as well!
	*/
	if (this->mpqFile()->isEncrypted())
		DecryptData(Mpq::cryptTable(), (void*)data.get(), dataSize, this->sectorKey());

	// Individual sectors in a compressed or imploded file may be stored uncompressed; this occurs if and only if the file data the sector contains could not be compressed by the algorithm(s) used (if the compressed sector size was greater than or equal to the size of the file data), and is indicated by the sector's size in SectorOffsetTable being equal to the size of the file data in the sector (which may be calculated from the FileSize).
	// NOTE This byte counts towards the total sector size, meaning that the sector will be stored uncompressed if the data cannot be compressed by at least two bytes
	if ((!this->mpqFile()->isCompressed() && this->sectorSize() < this->mpqFile()->mpq()->sectorSize()) || this->sectorSize() <= this->mpqFile()->mpq()->sectorSize() - 2) // compression/implosion succeded
	{
		// Imploded sectors are the raw compressed data following compression with the implode algorithm (these sectors can only be in imploded files).
		if (this->mpqFile()->isImploded())
		{
			if (this->compression() & Sector::Compression::Imploded)
			{
				int outLength = boost::numeric_cast<int>(mpqFile()->mpq()->sectorSize());
				boost::scoped_array<byte> buffer(new byte[outLength]); // NOTE do always allocate enough memory.
				decompressPklib(buffer.get(), outLength, reinterpret_cast<char* const>(data.get()), boost::numeric_cast<int>(dataSize));

				if (outLength < mpqFile()->mpq()->sectorSize())
				{
					data.reset(new byte[outLength]);
					memcpy(data.get(), buffer.get(), outLength);
				}
				else
					data.swap(buffer);

				dataSize = outLength;
			}
		}
		// Compressed sectors (only found in compressed - not imploded - files) are compressed with one or more compression algorithms.
		else if (this->mpqFile()->isCompressed())
		{
			const_cast<Sector*>(this)->setCompression(data[0]); // first byte contains compression

			// NOTE the following decompression statements do skip this value (starting at buffer index 1)
			byte *realData = &data[1];
			const uint32 realDataSize = dataSize - 1;

			if (this->compression() & Sector::Compression::Bzip2Compressed) // BZip2 compressed (see BZip2)
			{
				iarraystream istream(realData, realDataSize);
				//arraystream ostream;
				std::basic_stringstream<byte> ostream; // TODO unbuffered stream doesnt work
				std::streamsize size = 0;

				try
				{
					size = decompressBzip2(istream, ostream, mpqFile()->mpq()->sectorSize());
				}
				catch (boost::iostreams::bzip2_error &error)
				{

					throw Exception(bzip2Error(error.error()));
				}

				data.reset(new byte[size]);
				dataSize = size;
				ostream.read(data.get(), dataSize);
				//ostream >> data.get();
			}

			// Imploded sectors are the raw compressed data following compression with the implode algorithm (these sectors can only be in imploded files).
			// NOTE but in this situation it's compressed not imploded file!!!
			if (this->compression() & Sector::Compression::Imploded)
			{
				std::cerr << _("Warning: Imploded sector in not imploded file!") << std::endl;
				int outLength = boost::numeric_cast<int>(mpqFile()->mpq()->sectorSize());
				boost::scoped_array<byte> buffer(new byte[outLength]); // NOTE do always allocate enough memory.
				decompressPklib(buffer.get(), outLength, reinterpret_cast<char* const>(realData), boost::numeric_cast<int>(realDataSize));

				if (outLength < mpqFile()->mpq()->sectorSize())
				{
					data.reset(new byte[outLength]);
					memcpy(data.get(), buffer.get(), outLength);
				}
				else
					data.swap(buffer);

				dataSize = outLength;
			}

			if (this->compression() & Sector::Compression::Deflated) // Deflated (see ZLib)
			{
				iarraystream istream(realData, realDataSize);
				//arraystream ostream;
				std::basic_stringstream<byte> ostream; // TODO unbuffered stream doesnt work
				std::streamsize size = 0;

				try
				{
					size = decompressZlib(istream, ostream, mpqFile()->mpq()->sectorSize());
				}
				catch (boost::iostreams::zlib_error &error)
				{
					throw Exception(zlibError(error.error()));
				}

				data.reset(new byte[size]);
				dataSize = size;
				ostream.read(data.get(), dataSize);
				//ostream >> data.get();
			}

			if (this->compression() & Sector::Compression::Huffman) // Huffman encoded
			{
				int outLength = boost::numeric_cast<int>(mpqFile()->mpq()->sectorSize());
				boost::scoped_array<byte> buffer(new byte[outLength]); // NOTE do always allocate enough memory.
				int state = decompressHuffman(buffer.get(), &outLength, (char*)realData, boost::numeric_cast<int>(realDataSize));
				//int state = huffman_decode_memory((const unsigned char*)realData, boost::numeric_cast<unsigned int>(realDataSize), &out, &outLength);

				if (state != 0)
					throw Exception(boost::format(_("Huffman error %1%.")) % state);

				if (outLength < mpqFile()->mpq()->sectorSize())
				{
					data.reset(new byte[outLength]);
					memcpy(data.get(), buffer.get(), outLength);
				}
				else
					data.swap(buffer);

				dataSize = outLength;
			}

			if (this->compression() & Sector::Compression::ImaAdpcmStereo) // IMA ADPCM stereo
			{
				int outLength = boost::numeric_cast<int>(mpqFile()->mpq()->sectorSize());
				boost::scoped_array<byte> buffer(new byte[outLength]); // NOTE do always allocate enough memory.

				if (decompressWaveStereo((unsigned char*const)realData, boost::numeric_cast<int>(realDataSize), (unsigned char*)buffer.get(), outLength) == 0)
					throw Exception(_("Wave stereo decompression error."));

				if (outLength < mpqFile()->mpq()->sectorSize())
				{
					data.reset(new byte[outLength]);
					memcpy(data.get(), buffer.get(), outLength);
				}
				else
					data.swap(buffer);

				dataSize = outLength;
			}

			if (this->compression() & Sector::Compression::ImaAdpcmMono) // IMA ADPCM mono
			{
				int outLength = boost::numeric_cast<int>(mpqFile()->mpq()->sectorSize());
				boost::scoped_array<byte> buffer(new byte[outLength]); // NOTE do always allocate enough memory.

				if (decompressWaveMono((unsigned char*const)realData, boost::numeric_cast<int>(realDataSize), (unsigned char*)buffer.get(), outLength) == 0)
					throw Exception(_("Wave mono decompression error."));

				if (outLength < mpqFile()->mpq()->sectorSize())
				{
					data.reset(new byte[outLength]);
					memcpy(data.get(), buffer.get(), outLength);
				}
				else
					data.swap(buffer);

				dataSize = outLength;
			}
		}
	}
	// skip compression types byte if data could not be compressed properly
	else if (this->mpqFile()->isCompressed())
	{
		// TEST
		std::cerr << "Sector could not be compressed properly." << std::endl;

		ostream.write(&data[1], dataSize - 1);

		return bytes;
	}

	ostream.write(data.get(), dataSize);

	return bytes;
}

void Sector::setCompression(byte value)
{
	this->m_compression = (BOOST_SCOPED_ENUM(Compression))(value);
}

uint32 Sector::sectorKey() const
{
	return this->mpqFile()->fileKey() + this->sectorIndex();
}

}

}
