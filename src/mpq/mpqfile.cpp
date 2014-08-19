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

#include "mpqfile.hpp"
#include "hash.hpp"
#include "mpq.hpp"

namespace wc3lib
{

namespace mpq
{

void MpqFile::removeData()
{
	this->block()->setFileSize(0);

	/// \todo Clear corresponding sector table in MPQ file?
	// writeSectors(Sectors())
}

std::streamsize MpqFile::readData(istream &istream, Sector::Compression compression) throw (class Exception)
{
	removeData();

	std::streamsize bytes = 0;
	uint16 sectorSize = 0;
	Sectors sectors;

	if (!hasSectorOffsetTable())
	{
		if (this->block()->flags() & Block::Flags::IsSingleUnit)
		{
			sectors.reserve(1);
			Sector sector(newSector(0, 0, 0));

			sector.setCompression(compression);
			bytes = sector.readData(istream);

			sectors.push_back(sector);
		}
		else
		{
			uint32 sectorIndex = 0;
			uint32 sectorOffset = 0;

			// split into equal sized (sometimes except last one) sectors
			while (!eof(istream))
			{
				// if last one it may be smaller than MPQ's sector size
				const uint32 e = endPosition(istream)- istream.tellg();
				const uint32 sectorSize = e >= this->mpq()->sectorSize() ? this->mpq()->sectorSize() : e;
				boost::scoped_array<byte> buffer(new byte[sectorSize]);
				std::streamsize sizeCounter = 0;
				wc3lib::read(istream, buffer[0], sizeCounter, sectorSize);

				Sector sector(newSector(sectorIndex, sectorOffset, sectorSize));

				sector.setCompression(compression);
				bytes += sector.readData(buffer.get(), sectorSize);

				sectors.push_back(sector);

				++sectorIndex;
				sectorOffset += sectorSize;
			}
		}
	}
	// compressed
	else
	{
		/// \todo AHH!
	}

	// TODO write sectors into MPQ archive
	// writeSectors(sectors)

	return bytes;
}

std::streamsize MpqFile::appendData(istream &istream) throw (class Exception)
{
	throw Exception(_("MpqFile: appendData is not implemented yet!"));

	return 0;
}

std::streamsize MpqFile::writeData(ostream &ostream) throw (class Exception)
{
	boost::interprocess::file_lock fileLock(this->mpq()->path().string().c_str());

	if (!fileLock.try_lock())
		throw Exception(boost::format("Unable to lock MPQ archive of file \"%1%\" when trying to read sector data.") % this->path());

	ifstream ifstream(this->mpq()->path(), std::ios_base::in | std::ios_base::binary);

	if (!ifstream)
		throw Exception(boost::format(_("Unable to open file %1%.")) % this->mpq()->path());

	std::streamsize bytes = this->writeData(ifstream, ostream);

	return bytes;
}

std::streamsize MpqFile::writeData(istream &istream, ostream &ostream) throw (Exception)
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

MpqFile::Locale MpqFile::locale() const
{
	return MpqFile::intToLocale(this->m_hash->hashData().locale());
}

MpqFile::Platform MpqFile::platform() const
{
	return MpqFile::intToPlatform(this->m_hash->hashData().platform());
}

Block* MpqFile::block() const
{
	return this->m_hash->block();
}

MpqFile::MpqFile(class Mpq *mpq, class Hash *hash) : m_mpq(mpq), m_hash(hash)
{
}

MpqFile::~MpqFile()
{
}

Sector MpqFile::newSector(uint32 index, uint32 offset, uint32 size) throw ()
{
	return Sector(this, index, offset, size);
}

std::streamsize MpqFile::sectors(istream &istream, Sectors &sectors) throw (class Exception)
{
	// if we have a sector offset table and file is encrypted we first need to know its path for proper decryption!
	if (hasSectorOffsetTable() && isEncrypted() && path().empty())
	{
		return 0;
	}

	istream.seekg(mpq()->startPosition());
	istream.seekg(this->block()->blockOffset(), std::ios::cur);

	if (mpq()->format() == Mpq::Format::Mpq2 && block()->extendedBlockOffset() > 0)
	{
		istream.seekg(block()->extendedBlockOffset(), std::ios::cur);
	}

	std::streamsize bytes = 0;

	// This table is not present if this information can be calculated.
	// If the file is not compressed/imploded, then the size and offset of all sectors is known, based on the archive's SectorSizeShift. If the file is stored as a single unit compressed/imploded, then the SectorOffsetTable is omitted, as the single file "sector" corresponds to BlockSize and FileSize, as mentioned previously.
	if  (!hasSectorOffsetTable())
	{
		//std::cout << "File is not imploded/compressed or it's a single unit " << std::endl;

		const uint32 sectorSize = (this->block()->flags() & Block::Flags::IsSingleUnit) ? this->block()->fileSize() : this->m_mpq->sectorSize();
		// If the file is stored as a single unit (indicated in the file's Flags), there is effectively only a single sector, which contains the entire file data.
		const uint32 sectorsCount = (this->m_hash->block()->flags() & Block::Flags::IsSingleUnit) ? 1 : this->m_hash->block()->blockSize() / sectorSize;
		sectors.reserve(sectorsCount + 1);

		uint32 newOffset = 0;
		// NOTE setting index before adding to container is important for index function in sectors container

		for (uint32 i = 0; i < sectorsCount; ++i)
		{
			sectors.push_back(newSector(i, newOffset, sectorSize));

			newOffset += sectorSize;
		}

		// the last sector may contain less than this, depending on the size of the entire file's data.
		if (!(this->m_hash->block()->flags() & Block::Flags::IsSingleUnit))
		{
			// not necessary for single units
			const uint32 lastSize = this->m_hash->block()->blockSize() % sectorSize;

			if (lastSize > 0)
			{
				sectors.push_back(newSector(sectors.size(), newOffset, lastSize));
			}
		}
	}
	// However, the SectorOffsetTable will be present if the file is compressed/imploded and the file is not stored as a single unit, even if there is only a single sector in the file (the size of the file is less than or equal to the archive's sector size).
	// sector offset table
	else
	{
		// sector number calculation from StormLib
		// hf->nBlocks  = (hf->pBlock->dwFSize + ha->dwBlockSize - 1) / ha->dwBlockSize;

		const uint32 sectorsCount = (this->block()->fileSize() + this->mpq()->sectorSize() - 1) / this->mpq()->sectorSize();

		// StormLib specific implementation (extra data)?!
		//dwToRead = (hf->nBlocks+1) * sizeof(DWORD);
		//if(hf->pBlock->dwFlags & MPQ_FILE_HAS_EXTRA)
			//dwToRead += sizeof(DWORD);

		// last offset contains file size
		const uint32 offsetsSize = sectorsCount + 1;
		boost::scoped_array<uint32> offsets(new uint32[offsetsSize]);
		const uint32 readSize = offsetsSize * sizeof(uint32);
		wc3lib::read(istream, offsets[0], bytes, readSize);

		// The SectorOffsetTable, if present, is encrypted using the key - 1.
		if (isEncrypted())
		{
			DecryptData(Mpq::cryptTable(), offsets.get(), readSize, fileKey() - 1);
		}

		/*
		 * The last entry contains the total compressed file
		 * size, making it possible to easily calculate the size of any given
		 * sector by simple subtraction.
		 */
		uint32 size = offsets[sectorsCount];

		// TEST
		if (hash()->block()->blockSize() != size)
		{
			std::cerr << "File: " << this->path() << std::endl;
			std::cerr << "We have " << sectorsCount << " sectors." << std::endl;
			std::cerr << "Read sector table block size " << size << " is not equal to original block size " << hash()->block()->blockSize() << std::endl;
			std::cerr << "Uncompressed file size is " << this->size() << std::endl;
			std::cerr << "First sector offset " << sectors[0].sectorOffset() << std::endl;
			size = hash()->block()->blockSize();
		}
		//else
			//std::cout << "Everything is alright!" << std::endl;

		boost::scoped_array<uint32> sizes(new uint32[sectorsCount]);

		for (uint32 i = sectorsCount - 1; ; --i)
		{
			sizes[i] = size - offsets[i];

			if (i == 0)
			{
				break;
			}
			else
			{
				size = offsets[i];
			}
		}

		sectors.reserve(sectorsCount);

		for (uint32 i = 0; i < sectorsCount; ++i)
		{
			sectors.push_back(newSector(i, offsets[i], sizes[i]));
		}
	}

	return bytes;
}

void MpqFile::changePath(const boost::filesystem::path &path)
{
	this->m_path = path;

	if (this->hash() != 0)
	{
		this->hash()->changePath(path);
	}
}

std::streamsize MpqFile::sectors(Sectors &sectors) throw (class Exception)
{
	boost::interprocess::file_lock fileLock(mpq()->path().string().c_str());

	if (!fileLock.try_lock())
		throw Exception(boost::format(_("Warning: Couldn't lock MPQ file for refreshing sector data of file %1%.")) % path());

	ifstream istream(mpq()->path(), std::ios::in | std::ios::binary);

	if (!istream)
		throw Exception(boost::format(_("Unable to open file %1%.")) % mpq()->path());

	std::streamsize result = this->sectors(istream, sectors);
	fileLock.unlock();

	return result;
}


std::streamsize MpqFile::writeSectors(ostream &ostream, const Sectors &sectors) const throw (class Exception)
{
	return 0;
}

void MpqFile::remove() throw (class Exception)
{
	this->m_hash->removeData();

	/// @todo Clear file content -> usually unnecessary?
	//BOOST_FOREACH(class Sector *sector, this->m_sectors)
		//bytes += sector->clear();
	throw Exception(_("Not implemented yet!"));
}

bool MpqFile::rename(const std::string &newName, bool overwriteExisting) throw (class Exception)
{
	return this->move((this->path().has_parent_path() ? this->path().parent_path() / newName : newName).string(), overwriteExisting);
}

bool MpqFile::move(const boost::filesystem::path &newPath, bool overwriteExisting) throw (class Exception)
{
	if (this->m_path == newPath)
		return false;

	MpqFile *file = this->mpq()->findFile(newPath, this->locale(), this->platform());

	if (file != 0)
	{
		if (!overwriteExisting)
			return false;

		file->remove();
	}

	this->hash()->changePath(newPath);

	return true;
}

}

}
