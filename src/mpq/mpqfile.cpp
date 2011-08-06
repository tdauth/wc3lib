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
#include "mpq.hpp"
#include "algorithm.hpp" //debug

namespace wc3lib
{

namespace mpq
{

void MpqFile::removeData() throw (class Exception)
{
	if (!this->try_lock())
		throw Exception(boost::format(_("Unable to lock MPQ file \"%1%\".")) % path());
	
	this->block()->setFileSize(0);
	
	BOOST_FOREACH(SectorPtr sector, this->m_sectors)
		sector.reset();
		
	/// \todo Clear corresponding sector table in MPQ file?
	
	this->unlock();
}
	
std::streamsize MpqFile::readData(istream &istream, BOOST_SCOPED_ENUM(Sector::Compression) compression) throw (class Exception)
{
	removeData();
	
	if (!this->try_lock())
		throw Exception(boost::format(_("Unable to lock MPQ file \"%1%\".")) % path());
	
	std::streamsize bytes = 0;
	int16 sectorSize = 0;
	
	if (!hasSectorOffsetTable())
	{
		if (this->block()->flags() & Block::Flags::IsSingleUnit)
		{
			SectorPtr sector(new Sector(this));
			this->m_sectors.get<uint32>().insert(sector);
			sector->m_compression = compression;
			sector->m_sectorIndex = 0;
			sector->m_sectorOffset = 0;
			bytes = sector->readData(istream);
		}
		else
		{
			int32 sectorIndex = 0;
			int32 sectorOffset = 0;
			
			// split into equal sized (sometimes except last one) sectors
			while (!eof(istream))
			{
				// if last one it may be smaller than MPQ's sector size
				int16 e = endPosition(istream)- istream.tellg();
				int16 sectorSize = e >= this->m_mpq->sectorSize() ? this->m_mpq->sectorSize() : e;
				boost::scoped_array<byte> buffer(new byte[sectorSize]);
				std::streamsize sizeCounter = 0;
				wc3lib::read(istream, buffer[0], sizeCounter, sectorSize);
				
				SectorPtr sector(new Sector(this));
				this->m_sectors.get<uint32>().insert(sector);
				sector->m_compression = compression;
				sector->m_sectorIndex = sectorIndex;
				sector->m_sectorOffset = sectorOffset;
				bytes += sector->readData(buffer.get(), sectorSize);
				
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
	
	this->unlock();

	return bytes;
}

std::streamsize MpqFile::appendData(istream &istream) throw (class Exception)
{
	throw Exception(_("MpqFile: appendData is not implemented yet!"));
	
	if (!this->try_lock())
		throw Exception(boost::format(_("Unable to lock MPQ file \"%1%\".")) % path());

	this->unlock();
	
	return 0;
}

std::streamsize MpqFile::sync() throw (class Exception)
{
	if (!this->try_lock())
		throw Exception(boost::format(_("Unable to lock MPQ file \"%1%\".")) % path());
	/*
	 * FIXME
	if (!this->mpq()->fileLock().is_locked())
		throw Exception(boost::format(_("Corresponding MPQ archive \"%1%\" of file \"%2%\" isn't locked.")) % mpq()->path() % path());
	*/

	ofstream ostream(mpq()->path(), std::ios_base::binary | std::ios_base::out);
	ostream.seekp(mpq()->startPosition());
	// TODO two operation or one? (for one std::streamoff must be at least uint64!!!
	ostream.seekp(boost::numeric_cast<std::streamoff>(block()->blockOffset()), std::ios_base::cur);
	ostream.seekp(boost::numeric_cast<std::streamoff>(block()->extendedBlockOffset()), std::ios_base::cur);
	//ostream.seekp(boost::numeric_cast<std::streamoff>(block()->largeOffset()), std::ios_base::cur);
	
	this->unlock();
}

std::streamsize MpqFile::writeData(ostream &ostream) const throw (class Exception)
{
	std::streamsize bytes = 0;

	std::cout << "WRITING DATA FROM FILE " << this->path().string() << " INTO STREAM!!!" << std::endl;
	std::cout << "We have " << this->m_sectors.size() << " sectors." << std::endl;

	// TEST usually exceptions should never be catched!
	BOOST_FOREACH(const SectorPtr &sector, this->sectors())
	{
		try
		{
			bytes += sector->writeData(ostream);
		}
		catch (Exception &exception)
		{
			std::cerr << "Sector error:\n" << exception.what() << std::endl;
		}
	}

	return bytes;
}

MpqFile::MpqFile(class Mpq *mpq, class Hash *hash) : m_mpq(mpq), m_hash(hash), m_path("")
{
	if (hash != 0)
		hash->m_mpqFile = this;
}

MpqFile::~MpqFile()
{
}

std::streamsize MpqFile::read(istream &istream) throw (class Exception)
{
	if (!this->try_lock())
		throw Exception(boost::format(_("Unable to lock MPQ file \"%1%\".")) % path());
	
	std::streamsize bytes = 0;
	
	// TEST
	bool TEST = false;
	HashData hashData(HashString(Mpq::cryptTable(), "(listfile)", HashType::NameA), HashString(Mpq::cryptTable(), "(listfile)", HashType::NameB), MpqFile::Locale::Neutral, MpqFile::Platform::Default);
	
	if (this->hash()->hashData() == hashData)
	{
		
		TEST = true;
		std::cout << "Is (listfile)!" << std::endl;
	}

	// This table is not present if this information can be calculated.
	// If the file is not compressed/imploded, then the size and offset of all sectors is known, based on the archive's SectorSizeShift. If the file is stored as a single unit compressed/imploded, then the SectorOffsetTable is omitted, as the single file "sector" corresponds to BlockSize and FileSize, as mentioned previously.
	if  (!hasSectorOffsetTable() || this->block()->flags() & 0x00030000) /// @todo test
	{
		//std::cout << "File is not imploded/compressed or it's a single unit " << std::endl;
		
		const int16 sectorSize = (this->block()->flags() & Block::Flags::IsSingleUnit) ? this->block()->fileSize() : this->m_mpq->sectorSize();
		// If the file is stored as a single unit (indicated in the file's Flags), there is effectively only a single sector, which contains the entire file data.
		const int32 sectors = (this->m_hash->block()->flags() & Block::Flags::IsSingleUnit) ? 1 : this->m_hash->block()->blockSize() / sectorSize;

		if (TEST)
		{
			std::cout << "File size: " << this->m_hash->block()->fileSize() << " and number of sectors to read " << sectors << " and sector size " << sectorSize << std::endl;
			std::cout << "Correct flags: " << std::hex << this->m_hash->m_block->flags() << std::dec << std::endl;
		}

		int32 newOffset = 0;
		// not necessary for single units
		const int32 lastSize = this->m_hash->block()->blockSize() % sectorSize;

		for (int32 i = 0; i < sectors; ++i)
		{
			SectorPtr sector(new Sector(this));
			sector->m_sectorIndex = i; // important for index function in sectors container
			sector->m_sectorOffset = newOffset;
			sector->m_sectorSize = sectorSize;
			this->m_sectors.insert(sector);
			//this->m_sectors.get<uint32>().insert(sector);

			newOffset += sector->sectorSize();
		}

		// the last sector may contain less than this, depending on the size of the entire file's data.
		if (!(this->m_hash->block()->flags() & Block::Flags::IsSingleUnit) && lastSize > 0)
		{
			SectorPtr sector(new Sector(this));
			sector->m_sectorIndex = this->sectors().size(); // important for index function in sectors container
			sector->m_sectorOffset = newOffset;
			sector->m_sectorSize = lastSize;
			this->m_sectors.insert(sector);
		}
		
		if (TEST)
		{
			std::cout << "actual sectors " << m_sectors.size() << std::endl;
		}

		//std::cout << "Real sector count " << this->m_sectors.size() << std::endl;

	}
	// However, the SectorOffsetTable will be present if the file is compressed/imploded and the file is not stored as a single unit, even if there is only a single sector in the file (the size of the file is less than or equal to the archive's sector size).
	// sector offset table
	else
	{
		// sector number calculation from StormLib
		// hf->nBlocks  = (hf->pBlock->dwFSize + ha->dwBlockSize - 1) / ha->dwBlockSize;
		
		const int32 sectors = (this->block()->fileSize() + this->mpq()->sectorSize() - 1) / this->mpq()->sectorSize();
		
		// StormLib specific implementation (extra data)?!
		//dwToRead = (hf->nBlocks+1) * sizeof(DWORD);
		//if(hf->pBlock->dwFlags & MPQ_FILE_HAS_EXTRA)
			//dwToRead += sizeof(DWORD);
		
		//std::cout << "Starting required debug messages:" << std::endl;
		//int32 sectors = this->m_hash->block()->fileSize() / this->m_mpq->sectorSize(); /// @todo How to get this value?
		//std::cout << "Sectors " << sectors << " and flags " << std::hex << this->m_hash->block()->flags() << std::dec << std::endl;
		//std::cout << "Block offset: " << this->m_hash->block()->blockOffset() << ", Block size: " << this->m_hash->block()->blockSize() << std::endl;
		//std::cout << "Get position: " << istream.tellg() << std::endl;
		std::cout << "File size: " << this->m_hash->block()->fileSize() << " and number of sectors to read " << sectors << std::endl;

		for (int32 i = 0; i < sectors; ++i)
		{
			int32 offset;
			wc3lib::read(istream, offset, bytes);
			//std::cout << "Offset: " << offset << std::endl;

			// TEST
			if (offset >= this->m_hash->m_block->fileSize() || offset <= 0)
			{
				std::cerr << "Invalid offset: " << offset << " with flags: " << std::hex << this->m_hash->m_block->flags() << std::dec << std::endl;
				
				return bytes;
			}

			SectorPtr sector(new Sector(this));
			sector->m_sectorIndex = i; // important for index function in sectors container
			sector->m_sectorOffset = offset;
			this->m_sectors.insert(sector);

			/*
			DEPRECATED
			if (i == 0) // sector table ends before first sector offset
			{
				sectors = sector->m_sectorOffset / sizeof(sector->m_sectorOffset) - 1;
				std::cout << "Calculating sectors: " << sectors << std::endl;
			}
			*/
		}

		//int32 size;
		//wc3lib::read(istream, size, bytes);
		//std::cout << "Real file size: " << size << std::endl;

		// TODO Is there really a last size entry in section offset table?!
		// The last entry contains the file size, making it possible to easily calculate the size of any given sector.
		
		// calculate sector size, not required but maybe useful at some point
		int32 size = this->block()->fileSize();
		
		BOOST_REVERSE_FOREACH(SectorPtr sector, this->m_sectors)
		{
			sector->m_sectorSize = size - sector->m_sectorOffset;
			//std::cout << "Sector size is " << sector->m_sectorSize << std::endl;
			size = sector->m_sectorOffset;
		}
	}

	BOOST_FOREACH(SectorPtr sector, this->m_sectors)
	{
		// Compressed sectors (only found in compressed - not imploded - files) are compressed with one or more compression algorithms.
		if (this->isCompressed())
		{
			byte compression;
			wc3lib::read(istream, compression, bytes);

			// as well, this byte is encrypted with the sector data, if applicable.
			if (this->isEncrypted())
			{
				std::cout << "Compression before " << std::hex << compression << std::dec << std::endl;
				DecryptData(Mpq::cryptTable(), (void*)&compression, uint32(sizeof(compression)), sector->sectorKey());
				std::cout << "Compression afterwards " << std::hex << compression << std::dec << std::endl;
			}

			sector->setCompression(compression);
			istream.seekg(sector->m_sectorSize - 1, std::ios_base::cur); // skip sector data
		}
		else
		{
			sector->setCompression(0);
			istream.seekg(sector->m_sectorSize, std::ios_base::cur); // skip sector data
		}
	}
	
	if (TEST)
	{
		std::cout << "actual sectors " << m_sectors.size() << std::endl;
	}
	
	this->unlock();

	return bytes;
}


std::streamsize MpqFile::write(ostream &ostream) const throw (class Exception)
{
	return 0;
}

void MpqFile::remove() throw (class Exception)
{
	if (!this->try_lock())
		throw Exception(boost::format(_("Unable to lock MPQ file \"%1%\".")) % path());
	
	this->m_hash->clear();

	/// @todo Clear file content -> usually unnecessary?
	//BOOST_FOREACH(class Sector *sector, this->m_sectors)
		//bytes += sector->clear();
	
	this->unlock();
}

bool MpqFile::rename(const std::string &newName, bool overwriteExisting) throw (class Exception)
{
	return this->move(std::string(this->m_path.directory_string()).append(newName), overwriteExisting);
}

bool MpqFile::move(const boost::filesystem::path &newPath, bool overwriteExisting) throw (class Exception)
{
	if (this->m_path == newPath)
		return false;

	if (!this->try_lock())
		throw Exception(boost::format(_("Unable to lock MPQ file \"%1%\".")) % path());
	
	class MpqFile *mpqFile = this->m_mpq->findFile(newPath, this->locale(), this->platform());

	if (mpqFile != 0)
	{
		if (!overwriteExisting)
			return false;

		mpqFile->remove();
	}

	this->m_hash->changePath(newPath);
	
	this->unlock();

	return true;
}

class MpqFile& MpqFile::operator<<(const class MpqFile &mpqFile) throw (class Exception)
{
	stringstream sstream;
	mpqFile.writeData(sstream);
	this->appendData(sstream);

	return *this;
}

class MpqFile& MpqFile::operator>>(class Mpq &mpq) throw (class Exception)
{
	mpq.addFile(*this, true, false);

	return *this;
}

class MpqFile& MpqFile::operator>>(class MpqFile &mpqFile) throw (class Exception)
{
	stringstream sstream;
	this->writeData(sstream);
	mpqFile.appendData(sstream);

	return *this;
}

}

}
