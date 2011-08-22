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

std::streamsize Sector::readData(istream &istream) throw (class Exception)
{
	/// \todo Change sector size!!!
	return 0;
}

std::streamsize Sector::readData(const byte *buffer, const std::size_t bufferSize) throw  (class Exception)
{
	return 0;
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
	ifstream.seekg(boost::numeric_cast<std::streampos>(this->sectorOffset()), std::ios::cur);

	if (this->mpqFile()->mpq()->format() == Mpq::Format::Mpq1)
		ifstream.seekg(boost::numeric_cast<std::streampos>(this->mpqFile()->hash()->block()->blockOffset()), std::ios::cur);
	else
		ifstream.seekg(boost::numeric_cast<std::streampos>(this->mpqFile()->hash()->block()->largeOffset()), std::ios::cur);

	uint32 dataSize = this->sectorSize();

	// WARNING as well, this byte is encrypted with the sector data, if applicable.
	// skip compression flags byte
	/*if (this->mpqFile()->isCompressed())
	{
		ifstream.seekg(1, std::ios::cur);
		dataSize -= 1;
	}*/

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
				char *newData = reinterpret_cast<char*>(data.get());
				boost::numeric_cast<int>(dataSize);
				int *newDataSize = (int*)&dataSize;
				decompressPklib(newData, *newDataSize, newData, *newDataSize);
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
				arraystream ostream;
				std::streamsize size = 0;
				
				try
				{
					size = decompressBzip2(istream, ostream);
				}
				catch (boost::iostreams::bzip2_error &error)
				{
					
					throw Exception(bzip2Error(error.error()));
				}
				
				data.reset(new byte[size]);
				dataSize = size;
				ostream >> data.get();
			}
			
			// Imploded sectors are the raw compressed data following compression with the implode algorithm (these sectors can only be in imploded files).
			// NOTE but in this situation it's compressed not imploded file!!!
			if (this->compression() & Sector::Compression::Imploded)
			{
				std::cerr << _("Warning: Imploded sector in not imploded file!") << std::endl;
				boost::scoped_array<byte> newData(new byte[realDataSize]);
				int newDataSize = realDataSize; // NOTE we need to define the buffer size for PKlib decompression!!!
				decompressPklib(newData.get(), newDataSize, (char*)realData, boost::numeric_cast<int>(realDataSize));
				
				data.swap(newData);
				dataSize = newDataSize;
			}

			if (this->compression() & Sector::Compression::Deflated) // Deflated (see ZLib)
			{
				iarraystream istream(realData, realDataSize);
				arraystream ostream;
				std::streamsize size = 0;
				
				try
				{
					size = decompressZlib(istream, ostream);
				}
				catch (boost::iostreams::zlib_error &error)
				{
					throw Exception(zlibError(error.error()));
				}
				
				data.reset(new byte[size]);
				dataSize = size;
				ostream >> data.get();
			}

			if (this->compression() & Sector::Compression::Huffman) // Huffman encoded
			{
				unsigned char *newData = 0;
				unsigned newDataSize = 0;
				int state = huffman_decode_memory((unsigned char*)realData, boost::numeric_cast<unsigned>(realDataSize), (unsigned char**)(&newData), (unsigned*)(&newDataSize));

				if (state != 0)
					throw Exception(boost::format(_("Huffman error %1%.")) % state);
				
				data.reset((byte*)newData);
				dataSize = newDataSize;
			}

			if (this->compression() & Sector::Compression::ImaAdpcmStereo) // IMA ADPCM stereo
			{
				std::cout << "decompress wave stereo." << std::endl;
				unsigned char *newData = 0;
				int newDataSize = 0;
				
				if (decompressWaveStereo((unsigned char*)realData, boost::numeric_cast<int>(realDataSize), newData, newDataSize) == 0)
					throw Exception(_("Wave stereo decompression error."));
				
				data.reset((byte*)newData);
				dataSize = newDataSize;
			}

			if (this->compression() & Sector::Compression::ImaAdpcmMono) // IMA ADPCM mono
			{
				std::cout << "decompress wave mono." << std::endl;
				unsigned char *newData = 0;
				int newDataSize = 0;
				
				if (decompressWaveMono((unsigned char*)realData, boost::numeric_cast<int>(realDataSize), newData, newDataSize) == 0)
					throw Exception(_("Wave mono decompression error."));
				
				data.reset((byte*)newData);
				dataSize = newDataSize;
			}
		}
	}
	// skip compression types byte if data could not be compressed properly
	else if (this->mpqFile()->isCompressed())
	{
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
