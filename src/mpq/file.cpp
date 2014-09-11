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

#include "file.hpp"
#include "hash.hpp"
#include "archive.hpp"

namespace wc3lib
{

namespace mpq
{

std::streamsize File::readData(istream &istream, Sector::Compression compression)
{
	throw Exception(_("Not supported yet!"));
}

std::streamsize File::writeData(ostream &ostream)
{
	ifstream ifstream(this->mpq()->path(), std::ios_base::in | std::ios_base::binary);

	if (!ifstream)
	{
		throw Exception(boost::format(_("Unable to open file %1%.")) % this->mpq()->path());
	}

	std::streamsize bytes = this->writeData(ifstream, ostream);

	return bytes;
}

std::streamsize File::writeData(istream &istream, ostream &ostream)
{
	/*
	 * Read sectors from MPQ archive file and store them.
	 */
	Sectors sectors;
	this->sectors(istream, sectors);

	if (sectors.empty() && hasSectorOffsetTable() && isEncrypted() && path().empty())
	{
		std::cerr << boost::format(_("Warning: Writing data from file with block index %1% and hash index %2% failed because we need its path to decrypt its data.")) % block()->index() % hash()->index() << std::endl;

		return 0;
	}

	std::streamsize bytes = 0;

	BOOST_FOREACH(Sectors::const_reference sector, sectors)
	{
		try
		{
			sector.seekg(istream);
			bytes += sector.writeData(istream, ostream);
		}
		catch (Exception &exception)
		{
			throw Exception(boost::format(_("Sector error (sector %1%, file %2%):\n%3%")) % sector.sectorIndex() % this->path() % exception.what());
		}
	}

	return bytes;
}

File::Locale File::locale() const
{
	return File::intToLocale(this->hash()->cHashData().locale());
}

File::Platform File::platform() const
{
	return File::intToPlatform(this->hash()->cHashData().platform());
}

Block* File::block() const
{
	return this->hash()->block();
}

File::File() : m_mpq(0), m_hash(0)
{
}

void File::close()
{
	this->m_mpq = 0;
	this->m_hash = 0;
}

File::File(Archive *mpq, Hash *hash, const boost::filesystem::path &path) : m_mpq(mpq), m_hash(hash), m_path(path)
{
}

File::~File()
{
}

File::File(const File &other) : m_mpq(other.mpq()), m_hash(other.hash()), m_path(other.path())
{
}

std::streamsize File::sectors(istream &istream, Sectors &sectors)
{
	// if we have a sector offset table and file is encrypted we first need to know its path for proper decryption!
	if (hasSectorOffsetTable() && isEncrypted() && path().empty())
	{
		return 0;
	}

	istream.seekg(mpq()->startPosition());
	istream.seekg(this->block()->blockOffset(), std::ios::cur);

	if (mpq()->format() == Archive::Format::Mpq2 && block()->extendedBlockOffset() > 0)
	{
		istream.seekg(block()->extendedBlockOffset(), std::ios::cur);
	}

	std::streamsize bytes = 0;

	// This table is not present if this information can be calculated.
	// If the file is not compressed/imploded, then the size and offset of all sectors is known, based on the archive's SectorSizeShift. If the file is stored as a single unit compressed/imploded, then the SectorOffsetTable is omitted, as the single file "sector" corresponds to BlockSize and FileSize, as mentioned previously.
	if (!this->isCompressed() && !this->isImploded())
	{
		if (this->size() != this->compressedSize())
		{
			std::cerr << boost::format(_("Although file %1% is not compressed or imploded, compressed size %2% differs from uncompressed size %3%.")) % this->path() % this->compressedSize() % this->size() << std::endl;
		}

		const uint32 sectorSize = this->mpq()->sectorSize();
		const uint32 sectorsCount = this->compressedSize() / sectorSize;
		sectors.reserve(sectorsCount + 1); // the last sector might use less size than the default one so reserve + 1

		uint32 newOffset = 0;
		// The sectors are not compressed so the size of each sector is the same in both cases.
		uint32 remainingUncompressedSize = this->size();

		for (uint32 i = 0; i < sectorsCount; ++i)
		{
			sectors.push_back(Sector(this, i, newOffset, sectorSize, sectorSize));

			newOffset += sectorSize;
			remainingUncompressedSize -= sectorSize;
		}

		// the last sector might use less size (remaining bytes)
		if (remainingUncompressedSize > 0)
		{
			sectors.push_back(Sector(this, sectors.size(), newOffset, remainingUncompressedSize, remainingUncompressedSize));
		}
	}
	// a single unit file only has one sector
	else if ((this->block()->flags() & Block::Flags::IsSingleUnit) && (this->isCompressed() || this->isImploded()))
	{
		const uint32 sectorOffset = 0;
		const uint32 compressedSectorSize = this->block()->blockSize();
		const uint32 uncompressedSectorSize = this->block()->fileSize();
		sectors.push_back(Sector(this, 0, sectorOffset, compressedSectorSize, uncompressedSectorSize));
	}
	// However, the SectorOffsetTable will be present if the file is compressed/imploded and the file is not stored as a single unit, even if there is only a single sector in the file (the size of the file is less than or equal to the archive's sector size).
	// sector offset table
	else
	{
		const uint32 sectorsCount = (this->block()->fileSize() + this->mpq()->sectorSize() - 1) / this->mpq()->sectorSize();

		// last offset contains file size
		const uint32 offsetsSize = sectorsCount + 1;
		boost::scoped_array<uint32> offsets(new uint32[offsetsSize]);
		const uint32 readSize = offsetsSize * sizeof(uint32);
		wc3lib::read(istream, offsets[0], bytes, readSize);

		// The SectorOffsetTable, if present, is encrypted using the key - 1.
		if (isEncrypted())
		{
			DecryptData(Archive::cryptTable(), offsets.get(), readSize, fileKey() - 1);
		}

		// TEST
		if (this->compressedSize() != offsets[sectorsCount])
		{
			std::cerr << "File: " << this->path() << std::endl;
			std::cerr << "We have " << sectorsCount << " sectors." << std::endl;
			std::cerr << "Read sector table block size " << offsets[sectorsCount] << " is not equal to original block size " << this->compressedSize() << std::endl;
			std::cerr << "Uncompressed file size is " << this->size() << std::endl;
			std::cerr << "First sector offset " << sectors[0].sectorOffset() << std::endl;
		}

		sectors.reserve(sectorsCount);
		// the actual compressed file size, starting from the first offset of data
		uint32 size = offsets[sectorsCount] - offsets[0];
		// The remaining uncompressed size of the file
		uint32 remainingUncompressedSize = this->size();

		if (offsets[0] > 0)
		{
			std::cerr << boost::format(_("%1% useless bytes in file %2%.")) % offsets[0] % this->path() << std::endl;
		}

		for (uint32 i = 0; i < sectorsCount; ++i)
		{
			/*
			 * The last entry contains the total compressed file
			 * size, making it possible to easily calculate the size of any given
			 * sector by simple subtraction.
			 */
			const uint32 sectorSize = offsets[i + 1] - offsets[i]; // works since last offset contains file size
			const uint32 uncompressedSectorSize = mpq()->sectorSize() > remainingUncompressedSize ? remainingUncompressedSize : this->mpq()->sectorSize(); // use remaining size of the block if it is smaller than the default expected uncompressed sector size

			sectors.push_back(Sector(this, i, offsets[i], sectorSize, uncompressedSectorSize));

			remainingUncompressedSize -= uncompressedSectorSize;
		}
	}

	return bytes;
}

void File::changePath(const boost::filesystem::path &path)
{
	this->m_path = path;

	if (this->hash() != 0)
	{
		this->hash()->changePath(path);
	}
}

std::streamsize File::sectors(Sectors &sectors)
{
	ifstream istream(mpq()->path(), std::ios::in | std::ios::binary);

	if (!istream)
	{
		throw Exception(boost::format(_("Unable to open file %1%.")) % mpq()->path());
	}

	std::streamsize result = this->sectors(istream, sectors);

	return result;
}

std::streamsize File::writeSectors(ostream &ostream, const Sectors &sectors) const
{
	return 0;
}

void File::removeData()
{
	throw Exception(_("Not implemented yet."));
}


}

}
