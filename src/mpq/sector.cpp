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

std::streamsize Sector::writeData(ostream &ostream) const throw (class Exception)
{
	boost::interprocess::file_lock fileLock(this->mpqFile()->mpq()->path().string().c_str());
	
	if (!fileLock.try_lock())
		throw Exception(boost::format("Unable to lock MPQ archive of file \"%1%\" when trying to read sector data.") % this->mpqFile()->path());
	
	ifstream ifstream(this->mpqFile()->mpq()->path(), std::ios_base::in | std::ios_base::binary);

	if (!ifstream)
		throw Exception(boost::str(boost::format(_("Sector: Unable to open file \"%1%\".")) % this->mpqFile()->mpq()->path().string()));

	std::streampos position = this->mpqFile()->mpq()->startPosition() + boost::numeric_cast<std::streampos>(this->sectorOffset());

	if (this->mpqFile()->mpq()->format() == Mpq::Format::Mpq1)
		position += this->mpqFile()->hash()->block()->blockOffset();
	else
		position += this->mpqFile()->hash()->block()->largeOffset();

//	std::cout << "Sector position " << position << std::endl;
	uint32 dataSize = this->sectorSize();

	// skip compression flags byte
	if (this->mpqFile()->isCompressed())
	{
		position += 1;
		dataSize -= 1;
	}

	ifstream.seekg(position);
	boost::scoped_array<byte> data(new byte[dataSize]);
	std::cout << "Sector index: " << this->sectorIndex() << std::endl;
	std::cout << "Sector offset: " << this->sectorOffset() << std::endl;
	std::cout << "Sector real position " << position << " and sector size " << dataSize << std::endl;
	std::cout << "Sector mask: " << std::hex << (int)this->compression() << std::dec << std::endl;

	std::streamsize bytes = 0;

	wc3lib::read(ifstream, data[0], bytes, dataSize);
	ifstream.close();
	fileLock.unlock();

	/*
	If the file is encrypted, each sector (after compression/implosion, if applicable) is encrypted with the file's key.
	Each sector is encrypted using the key + the 0-based index of the sector in the file.
	*/
	if (this->mpqFile()->isEncrypted())
	{
		DecryptData(Mpq::cryptTable(), (void*)data.get(), dataSize, this->sectorKey());
		std::cout << "Is encrypted!" << std::endl;
	}

	//boost::iostreams::filtering_istreambuf filteringStreamBuffer;

	// Imploded sectors are the raw compressed data following compression with the implode algorithm (these sectors can only be in imploded files).
	if (this->mpqFile()->isImploded())
	{
		if (this->compression() & Sector::Compression::Imploded)
		{
			char *newData = reinterpret_cast<char*>(data.get());
			boost::numeric_cast<int>(dataSize);
			int *newDataSize = (int*)&dataSize;
			decompressPklib(newData, *newDataSize, newData, *newDataSize);
			std::cout << "Is imploded!" << std::endl;
		}

		//throw Exception(_("Sector: Imploded files are not supported yet."));
	}
	// Compressed sectors (only found in compressed - not imploded - files) are compressed with one or more compression algorithms.
	else if (this->mpqFile()->isCompressed())
	{
		//boost::iostreams::zlib::huffman_only
		//boost::iostreams::zlib::deflated
		//struct boost::iostreams::zlib::zlib_params zlibParameters();
		//zlibParameters.strategy = boost::iostreams::zlib::huffman_only;
		//boost::iostreams::zlib_decompressor zlibDecompressor(zlibParameters);

		if (this->compression() & Sector::Compression::Bzip2Compressed) // BZip2 compressed (see BZip2)
		{
			//filteringStreamBuffer.push(boost::iostreams::bzip2_decompressor());

			std::cout << "Decompress bzip2." << std::endl;
			istringstream istream;
			istream.rdbuf()->pubsetbuf(data.get(), dataSize); // fill stream with old data
			ostringstream ostream;
			std::streamsize size = 0;
			
			try
			{
				size = decompressBzip2(istream, ostream);
			}
			catch (boost::iostreams::bzip2_error &error)
			{
				
				throw Exception(bzip2Error(error.error()));
			}
			
			bytes += size;
			data.reset(new byte[size]);
			dataSize = size;
			ostream.rdbuf()->sgetn(data.get(), size); // fill new data
			/*
			char *newData = (char*)data.get();
			boost::numeric_cast<unsigned int>(dataSize); // check size
			unsigned int *newDataSize = (unsigned int*)dataSize;
			int state = BZ2_bzBuffToBuffDecompress(newData, newDataSize, newData, *newDataSize, 0, 1);

			if (state != BZ_OK)
				//throw Exception(boost::format(_("Sector: Bzip2 error %1%.")) % state);
			*/
		}

		if (this->compression() & Sector::Compression::Deflated) // Deflated (see ZLib)
		{
			std::cout << "Decompress zlib. with data size " << dataSize << std::endl;
			istringstream istream;
			istream.rdbuf()->pubsetbuf(data.get(), dataSize); // fill stream with old data
			ostringstream ostream;
			std::streamsize size = 0;
			
			try
			{
				// TODO throws data stream exception on (listfile) of war3.mpq and war3x.mpq.
				size = decompressZlib(istream, ostream);
			}
			catch (boost::iostreams::zlib_error &error)
			{
				throw Exception(zlibError(error.error()));
			}
			
			bytes += size;
			data.reset(new byte[size]);
			dataSize = size;
			ostream.rdbuf()->sgetn(data.get(), size); // fill new data
		}

		if (this->compression() & Sector::Compression::Huffman) // Huffman encoded
		{
			std::cout << "Decompress huffman. Data " << data << std::endl << " and data size " << dataSize << std::endl;
			byte *newData = data.get();
			//filteringStreamBuffer.push(boost::iostreams::filter:)
			int state = huffman_decode_memory((unsigned char*)data.get(), boost::numeric_cast<unsigned>(dataSize), (unsigned char**)(&newData), (unsigned*)(&dataSize));
			std::cout << "data address " << data << " and data size " << dataSize << std::endl;

			if (state != 0)
				throw Exception(boost::str(boost::format(_("Sector: Huffman error %1%.")) % state));
		}

		if (this->compression() & Sector::Compression::ImaAdpcmStereo) // IMA ADPCM stereo
		{
			std::cout << "decompress wave stereo." << std::endl;
			unsigned char *unsignedCharData = reinterpret_cast<unsigned char*>(data.get());
			std::cout << "previous data size " << dataSize << std::endl;
			decompressWaveStereo(unsignedCharData, boost::numeric_cast<int>(dataSize), unsignedCharData, reinterpret_cast<int&>(dataSize));
			std::cout << "current data size " << dataSize << std::endl;
		}

		if (this->compression() & Sector::Compression::ImaAdpcmMono) // IMA ADPCM mono
		{
			std::cout << "decompress wave mono." << std::endl;
			unsigned char *unsignedCharData = reinterpret_cast<unsigned char*>(data.get());
			std::cout << "previous data size " << dataSize << std::endl;
			decompressWaveMono(unsignedCharData, boost::numeric_cast<int>(dataSize), unsignedCharData, reinterpret_cast<int&>(dataSize));
			std::cout << "current data size " << dataSize << std::endl;
		}
	}

	//filteringStreamBuffer.push(ifstream);
	//boost::iostreams::copy(filteringStreamBuffer, ostream);
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
