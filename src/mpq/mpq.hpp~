/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
 *   tamino@cdauth.de                                                      *
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

#ifndef WC3LIB_MPQ_MPQ_HPP
#define WC3LIB_MPQ_MPQ_HPP

#include <cmath>

#include <istream>
#include <ostream>
#include <list>
#include <map>
#include <string>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include "platform.hpp"
#include "hash.hpp"
#include "block.hpp"
#include "mpqfile.hpp"
#include "../exception.hpp"

namespace wc3lib
{

namespace mpq
{

/**
* @brief This class allows users to read, write and modify MPQ archives. MPQ (Mo'PaQ, short for Mike O'Brien Pack) is an archiving file format used in several of Blizzard Entertainment's games.
* Here's a simple example how to use this class:
* @code
* #include <wc3lib/mpq.hpp>
*
* using namespace wc3lib::mpq;
*
* void testFunction()
* {
*	class Mpq mpq();
*
*	// Using exception handling to catch errors.
*	try
*	{
*		mpq.open("War3x.mpq"); // Opens an existing MPQ archive.
*	}
*	catch (class Exception &exception)
*	{
*		std::cerr << "Unable to open archive: " << exception << std::endl;
*	}
*
*	return EXIT_FAILURE;
* }
*
* const class MpqFile *mpqFile = mpq.findFile("UI/MiscData.txt", MpqFile::Neutral, MpqFile::Neutral);
*
* if (mpqFile == 0)
*	return EXIT_FAILURE;
*
* std::cout << "MiscData.txt:\n" << mpqFile << std::endl;
* mpq.close(); // This function will be called automatically when variable mpq is deleted.
* @endcode
* There are three further classes which are related to this one and can be used by the user.
* In fact, mostly user does only need to know @class MpqFile which allows him constant access
* to files contained by the MPQ archive.
* Use Mpq::addFile to add a new file which will return 0 (if an error occurs) or the new MpqFile
* instance which refers to the newly created file.

*/
class Mpq
{
	public:
		enum Format
		{
			Mpq1, // original format
			Mpq2 // Burning Crusade, large files
		};

		enum ExtendedAttributes
		{
			None = 0x0,
			FileCrc32s = 0x00000001,
			FileTimeStamps = 0x00000002,
			FileMd5s = 0x00000004
		};

		static const uint32* cryptTable();
		static bool hasStrongDigitalSignature(std::istream &istream);
		static std::streamsize strongDigitalSignature(std::istream &istream, char signature[256]) throw (class Exception);

		Mpq();
		~Mpq();

		std::streamsize create(const boost::filesystem::path &path, bool overwriteExisting = false, std::streampos startPosition = 0, enum Format format = Mpq1, enum ExtendedAttributes extendedAttributes = None, int32 sectorSize = 4096, bool hasStrongDigitalSignature = false, bool containsListfileFile = false, bool containsSignatureFile = false) throw (class Exception);
		std::streamsize open(const boost::filesystem::path &path, std::istream *listfileIstream = 0) throw (class Exception);
		void close();

		/**
		* @param listfilefileIstream If you want to preselect your custom listfile file, use this value (entries will be appended to the already contained listfile file if it does exist).
		* @return Returns MPQ's size in bytes.
		*/
		std::streamsize readMpq(std::istream &istream, std::istream *listfileIstream = 0) throw (class Exception);
		/**
		* Writes the whole MPQ archive into output stream @param ostream. Note that you don't have to call this function each time you want to save your changed data of the opened MPQ archive.
		* If you change some data of the opened MPQ archive it's written directly into the corresponding file (the whole archive is not loaded into memory!).
		* @return Returns MPQ's size in bytes.
		*/
		std::streamsize writeMpq(std::ostream &ostream) const throw (class Exception);

		std::streamsize openTar(const boost::filesystem::path &path, std::istream *listfileIstream = 0) throw (class Exception);
		std::streamsize readTar(std::istream &istream) throw (class Exception);
		std::streamsize writeTar(std::ostream &ostream) const throw (class Exception);

		bool check() const;
		bool fix() const;
		/**
		* @return Returns true if the archive contains a "(listfile)" file.
		*/
		bool containsListfileFile() const;
		/**
		* Creates a "(listfile)" file if there isn't already one. All file entries which do have corresponding file paths will be added to this file.
		* @return Returns the file instance of the created file. If it does already exist the instance to the current file will be returned (note that it won't be refreshed by this method!).
		* @see Mpq::containsListfileFile, Mpq::listfileFile
		*/
		const class MpqFile* createListfileFile() throw (class Exception);
		const class MpqFile* listfileFile() const;
		/**
		* @return Returns true if the archive contains a "(attributes)" file. This can also be checked by checking if @fn Mpq.extendedAttributes() does not return @enum Mpq.None.
		*/
		bool containsAttributesFile() const;
		/**
		* Creates an "(attributes)" file if there isn't already one. The extended attributes of all block entries will be added to the file.
		* @return Returns the file instance of the created file. If it does already exist the instance to the current file will be returned (note that it won't be refreshed by this method!).
		* @see Mpq::containsAttributesFile, Mpq::attributesFile
		*/
		const class MpqFile* createAttributesFile() throw (class Exception);
		const class MpqFile* attributesFile() const;

		bool containsSignatureFile() const;
		const class MpqFile* signatureFile() const;

		/**
		* @return Returns used block space of the archive.
		*/
		int64 usedSpace() const;
		/**
		* @return Returns unused block space of the archive.
		*/
		int64 unusedSpace() const;

		/**
		* Note that the MPQ archive has no information about file paths if there is no "(listfile)" file. This function is the best way to get your required file.
		* @return Returns the corresponding @class MpqFile instance of the searched file. If no file was found it returns 0.
		*/
		const class MpqFile* findFile(const boost::filesystem::path &path, enum MpqFile::Locale locale, enum MpqFile::Platform platform) const;
		/**
		* @note This version compares path @param path to all other pathes. Therefore file paths should be set!
		*/
		const class MpqFile* findFile(const boost::filesystem::path &path) const;
		/**
		* @note This version compares name @param name to all other names. Therefore file paths should be set!
		*/
		const class MpqFile* findFile(const std::string &name) const;
		/**
		* Path of MPQ file @param mpqFile should be set if you use this method.
		* Does not search for the instance of @param mpqFile. Only uses its hash data!
		* @return Returns 0 if no file was found.
		*/
		const class MpqFile* findFile(const class MpqFile &mpqFile) const;
		/**
		* Adds a new file to the MPQ archive with path @param path, locale @param locale and platform @param platform.
		* @param istream This input stream is used for reading the initial file data.
		* @todo Replace reservedSpace by size of istream?
		*/
		class MpqFile* addFile(const boost::filesystem::path &path, enum MpqFile::Locale locale, enum MpqFile::Platform platform, const std::istream *istream = 0, bool overwriteExisting = false, int32 reservedSpace = 0) throw (class Exception);
		/**
		* Path of MPQ file @param mpqFile should be set if you use this method.
		* Does not add @param mpqFile. Only uses its meta data (beside you set @param addData to true)!
		* @param addData If this value is true contained data of the file will be added to the new one.
		*/
		class MpqFile* addFile(const class MpqFile &mpqFile, bool addData = true, bool overwriteExisting = false) throw (class Exception);
		bool removeFile(const boost::filesystem::path &path, enum MpqFile::Locale locale, enum MpqFile::Platform platform);
		bool removeFile(const boost::filesystem::path &path);
		bool removeFile(const std::string &name);
		/**
		* Path of MPQ file @param mpqFile should be set if you use this method.
		* @param mpqFile An MPQ file is searched which has the same hash value as @param mpqFile.
		* @return Returns true if an MPQ file was found and deleted successfully.
		*/
		bool removeFile(const class MpqFile &mpqFile);
		/**
		* @return Returns the size of the whole MPQ archive file.
		*/
		std::size_t size() const;
		const boost::filesystem::path& path() const;
		/**
		* @return Returns the start position of the MPQ archive in its file.
		*/
		std::streampos startPosition() const;
		enum Format format() const;
		enum ExtendedAttributes extendedAttributes() const;
		/**
		* Usually the sector size has type int16 and is computed by using formula:
		* pow(2, sectorSizeShift) * 512. Instead of computing it every time we save
		* the computed result and use int32.
		* @see Mpq.sectorSizeShift
		*/
		int32 sectorSize() const;
		/**
		* @return Computes the original header sector size shift value by using formula:
		* sqrt(sectorSize / 512)
		*/
		int16 sectorSizeShift() const;
		bool hasStrongDigitalSignature() const;
		const char* strongDigitalSignature() const;
		bool isOpen() const;
		const std::list<class Block*>& blocks() const;
		/**
		* Block map is primarily required by hash entries since they get blocks by their indices.
		*/
		const std::map<int32, class Block*>& blockMap() const;
		const std::list<class Hash*>& hashes() const;
		const std::list<class MpqFile*>& files() const;

		/**
		* @return Returns the sum of all block sizes.
		*/
		int64 entireBlockSize() const;
		int64 entireUsedBlockSize() const;
		int64 entireUnusedBlockSize() const;
		/**
		* @return Returns the sum of all file sizes (uncompressed).
		*/
		int64 entireFileSize() const;

		/**
		* @return Returns true if archive is not opened.
		*/
		bool operator!() const;
		/**
		* Copies data of file @param mpqFile and adds it to the MPQ archive.
		* Does not overwrite existing files.
		* @return Returns *this.
		*/
		class Mpq& operator<<(const class MpqFile &mpqFile) throw (class Exception);
		/**
		* Copies data of all files of MPQ archive @param mpq to the MPQ archive.
		* Does not overwrite existing files.
		* @return Returns *this.
		*/
		class Mpq& operator<<(const class Mpq &mpq) throw (class Exception);
		/**
		* Copies data of alle files of the MPQ archive to MPQ archive @param mpq.
		* @return Returns *this.
		*/
		class Mpq& operator>>(class Mpq &mpq) throw (class Exception);

		static const byte identifier[4];
		static const int16 formatVersion1Identifier;
		static const int16 formatVersion2Identifier;
		static const int32 extendedAttributesVersion;

		/**
		* 0xFFFFFFFE and 0xFFFFFFFF are reserved for deleted and empty blocks (by hashes).
		*/
		static const int32 maxBlockId = 0xFFFFFFFD;
		static const int32 maxHashId = 0xFFFFFFFF;

	protected:
		friend class MpqFile;
		friend class Hash;
		friend class Block;

		Mpq(const Mpq &mpq);
		/**
		* Does not check if archive is open.
		* @see Mpq.close
		*/
		void clear();
		class Hash* findHash(const boost::filesystem::path &path, enum MpqFile::Locale locale, enum MpqFile::Platform platform);
		class Hash* findHash(const Hash &hash);
		class MpqFile* findFile(const boost::filesystem::path &path, enum MpqFile::Locale locale, enum MpqFile::Platform platform);
		class MpqFile* findFile(const boost::filesystem::path &path);
		class MpqFile* findFile(const std::string &name);
		class MpqFile* findFile(const class MpqFile &mpqFile);

		/**
		* This function is reserved for internal uses since it gets a non-constant parameter.
		* @param mpqFile This file has to be contained by the MPQ archive and is completely deleted if function returns true (also its memory!). Note that the parameter has to be a reference of an MPQ file pointer and is set to 0.
		* @return Returns true if the MPQ file was deleted successfully.
		*/
		bool removeFile(class MpqFile *&mpqFile);

		class MpqFile* listfileFile();
		class MpqFile* attributesFile();
		class MpqFile* signatureFile();
		class MpqFile* refreshAttributesFile();
		class MpqFile* refreshSignatureFile();

		/**
		* Uses input stream @param istream for reading file path entries and refreshing them by getting their instances (using their hashes).
		* @return Returns the number of added path entries.
		*/
		std::size_t readListfilePathEntries(std::istream &istream);
		bool checkBlocks() const;
		bool checkHashes() const;
		/**
		* Empty space entries should have BlockOffset and BlockSize nonzero, and FileSize and Flags zero.
		*/
		class Block* firstEmptyBlock() const;
		/**
		* Unused block table entries should have BlockSize, FileSize, and Flags zero.
		*/
		class Block* firstUnusedBlock() const;
		/**
		* @return Returns the block with the biggest offset.
		*/
		class Block* lastOffsetBlock() const;
		class Hash* firstEmptyHash() const;
		class Hash* firstDeletedHash() const;
		/**
		* @return Returns the next block offset as large unsigned integer. Next block offset means the offset of the next added block (last block offset + last block size / header size).
		*/
		uint64 nextBlockOffset() const;
		/**
		* Same as function @fn Mpq.nextBlockOffset but divides large offset value into @param blockOffset and @param extendedBlockOffset.
		*/
		void nextBlockOffsets(int32 &blockOffset, int16 &extendedBlockOffset) const;

		class Mpq& operator=(const class Mpq &mpq);

		/// This value is required for getting the first block offset (relative to the beginning of archive).
		static const std::size_t headerSize;

		std::size_t m_size;
		boost::filesystem::path m_path;
		std::streampos m_startPosition;
		enum Format m_format;
		enum ExtendedAttributes m_extendedAttributes;
		int32 m_sectorSize;
		char *m_strongDigitalSignature;
		bool m_isOpen;
		std::list<class Block*> m_blocks;
		std::map<int32, class Block*> m_blockMap;
		std::list<class Hash*> m_hashes;
		std::list<class MpqFile*> m_files;
};

/// @todo Implement.
inline bool Mpq::hasStrongDigitalSignature(std::istream &istream)
{
	return false;
}

/// @todo Implement.
inline std::streamsize Mpq::strongDigitalSignature(std::istream &istream, char signature[256]) throw (class Exception)
{
	return 0;
}

inline std::ostream& operator<<(std::ostream &ostream, const class Mpq &mpq) throw (class Exception)
{
	mpq.writeMpq(ostream);

	return ostream;
}

inline std::istream& operator>>(std::istream &istream, class Mpq &mpq) throw (class Exception)
{
	mpq.readMpq(istream);

	return istream;
}

inline bool operator>(const class Mpq &mpq1, const class Mpq &mpq2)
{
	return mpq1.size() > mpq2.size();
}

inline bool operator<(const class Mpq &mpq1, const class Mpq &mpq2)
{
	return mpq1.size() < mpq2.size();
}

inline bool operator>=(const class Mpq &mpq1, const class Mpq &mpq2)
{
	return mpq1.size() >= mpq2.size();
}

inline bool operator<=(const class Mpq &mpq1, const class Mpq &mpq2)
{
	return mpq1.size() <= mpq2.size();
}

inline bool operator==(const class Mpq &mpq1, const class Mpq &mpq2)
{
	return mpq1.size() == mpq2.size();
}

inline bool operator!=(const class Mpq &mpq1, const class Mpq &mpq2)
{
	return mpq1.size() != mpq2.size();
}

inline bool Mpq::check() const
{
	/// @todo Check format, size, extended attributes and signature.
	if (this->extendedAttributes() != Mpq::None && !this->containsAttributesFile())
		return false;

	if (!this->checkBlocks())
		return false;

	if (!this->checkHashes())
		return false;

	return true;
}

/// @todo Fix corrupted entries if possible.
inline bool Mpq::fix() const
{
	return false;
}

inline bool Mpq::containsListfileFile() const
{
	return this->listfileFile() != 0;
}

inline const class MpqFile* Mpq::createListfileFile() throw (class Exception)
{
	if (this->containsListfileFile())
		return this->listfileFile();

	std::list<std::string> entries;

	BOOST_FOREACH(const class MpqFile *mpqFile, this->m_files)
	{
		if (!mpqFile->m_path.empty() && mpqFile->m_path.string() != "(attributes)") /// @todo Exclude directories and file (signature)?
			entries.push_back(mpqFile->m_path.string());
	}

	entries.sort(); /// @todo Sort alphabetically?
	std::stringstream sstream;

	BOOST_FOREACH(std::string entry, entries)
		sstream << entry << std::endl;

	return this->addFile("(listfile)", MpqFile::Neutral, MpqFile::Default, &sstream);
}

inline const class MpqFile* Mpq::listfileFile() const
{
	return this->findFile("(listfile)", MpqFile::Neutral, MpqFile::Default);
}


inline bool Mpq::containsAttributesFile() const
{
	return this->attributesFile() != 0;
}

inline const class MpqFile* Mpq::createAttributesFile() throw (class Exception)
{
	if (this->containsAttributesFile())
		return 0;

	struct ExtendedAttributesHeader extendedAttributesHeader;
	extendedAttributesHeader.version = Mpq::extendedAttributesVersion;
	extendedAttributesHeader.attributesPresent = this->extendedAttributes();
	std::stringstream stream;
	stream.write(reinterpret_cast<char*>(&extendedAttributesHeader), sizeof(extendedAttributesHeader));

	if (this->extendedAttributes() & Mpq::FileCrc32s)
	{
		BOOST_FOREACH(const class Block *block, this->m_blocks)
			stream.write(reinterpret_cast<const char*>(&block->m_crc32), sizeof(block->m_crc32));
	}

	if (this->extendedAttributes() & Mpq::FileTimeStamps)
	{
		BOOST_FOREACH(const class Block *block, this->m_blocks)
			stream.write(reinterpret_cast<const char*>(&block->m_fileTime), sizeof(block->m_fileTime));
	}

	if (this->extendedAttributes() & Mpq::FileMd5s)
	{
		BOOST_FOREACH(const class Block *block, this->m_blocks)
			stream.write(reinterpret_cast<const char*>(&block->m_md5), sizeof(block->m_md5));
	}

	return this->addFile("(attributes)", MpqFile::Neutral, MpqFile::Default, &stream);
}

inline const class MpqFile* Mpq::attributesFile() const
{
	return this->findFile("(attributes)", MpqFile::Neutral, MpqFile::Default);
}

inline bool Mpq::containsSignatureFile() const
{
	return this->signatureFile() != 0;
}

inline int64 Mpq::usedSpace() const
{
	return this->entireUsedBlockSize();
}

inline int64 Mpq::unusedSpace() const
{
	return this->entireUnusedBlockSize();
}

inline const class MpqFile* Mpq::signatureFile() const
{
	return this->findFile("(signature)", MpqFile::Neutral, MpqFile::Default);
}

inline std::size_t Mpq::size() const
{
	return this->m_size;
}

inline const boost::filesystem::path& Mpq::path() const
{
	return this->m_path;
}

inline std::streampos Mpq::startPosition() const
{
	return this->m_startPosition;
}

inline enum Mpq::Format Mpq::format() const
{
	return this->m_format;
}

inline enum Mpq::ExtendedAttributes Mpq::extendedAttributes() const
{
	return this->m_extendedAttributes;
}

inline int32 Mpq::sectorSize() const
{
	return this->m_sectorSize;
}

inline int16 Mpq::sectorSizeShift() const
{
	return sqrt(this->m_sectorSize / 512);
}

inline bool Mpq::hasStrongDigitalSignature() const
{
	return this->m_strongDigitalSignature != 0;
}

inline const char* Mpq::strongDigitalSignature() const
{
	return this->m_strongDigitalSignature;
}

inline bool Mpq::isOpen() const
{
	return this->m_isOpen;
}

inline const std::list<class Block*>& Mpq::blocks() const
{
	return this->m_blocks;
}

inline const std::map<int32, class Block*>& Mpq::blockMap() const
{
	return this->m_blockMap;
}

inline const std::list<class Hash*>& Mpq::hashes() const
{
	return this->m_hashes;
}

inline const std::list<class MpqFile*>& Mpq::files() const
{
	return this->m_files;
}

inline int64 Mpq::entireBlockSize() const
{
	int64 result = 0;

	BOOST_FOREACH(const Block *block, this->m_blocks)
		result += block->m_blockSize;

	return result;
}

inline int64 Mpq::entireUsedBlockSize() const
{
	int64 result = 0;

	BOOST_FOREACH(const class Block *block, this->m_blocks)
	{
		if (!block->empty())
			result += block->m_blockSize;
	}

	return result;
}

inline int64 Mpq::entireUnusedBlockSize() const
{
	int64 result = 0;

	BOOST_FOREACH(const class Block *block, this->m_blocks)
	{
		if (block->empty())
			result += block->m_blockSize;
	}

	return result;
}

inline int64 Mpq::entireFileSize() const
{
	int64 result = 0;

	BOOST_FOREACH(const Block *block, this->m_blocks)
	{
		// usually size should be 0 if it's not a file but anyway we should check
		if (block->flags() & Block::IsFile)
			result += block->m_fileSize;
	}

	return result;
}

inline class MpqFile* Mpq::listfileFile()
{
	return const_cast<class MpqFile*>(const_cast<const class Mpq*>(this)->listfileFile());
}

inline class MpqFile* Mpq::attributesFile()
{
	return const_cast<class MpqFile*>(const_cast<const class Mpq*>(this)->attributesFile());
}

inline class MpqFile* Mpq::signatureFile()
{
	return const_cast<class MpqFile*>(const_cast<const class Mpq*>(this)->signatureFile());
}

inline bool Mpq::checkBlocks() const
{
	BOOST_FOREACH(const class Block *block, this->m_blocks)
	{
		if (!block->check())
			return false;
	}

	return true;
}

inline bool Mpq::checkHashes() const
{
	BOOST_FOREACH(const class Hash *hash, this->m_hashes)
	{
		if (!hash->check())
			return false;

		BOOST_FOREACH(const class Hash *hash2, this->m_hashes)
		{
			/// @todo same block, ok?
			if (hash != hash2 && !hash->empty() && !hash->deleted() && hash->block() == hash2->block())
				return false;
		}
	}

	return true;
}

inline class Block* Mpq::firstEmptyBlock() const
{
	BOOST_FOREACH(class Block *block, this->m_blocks)
	{
		if (block->empty())
			return block;
	}

	return 0;
}

inline class Block* Mpq::firstUnusedBlock() const
{
	BOOST_FOREACH(class Block *block, this->m_blocks)
	{
		if (block->unused())
			return block;
	}

	return 0;
}

inline class Block* Mpq::lastOffsetBlock() const
{
	uint64 offset = 0;
	class Block *result = 0;

	BOOST_FOREACH(class Block *block, this->m_blocks)
	{
		uint64 newOffset = block->largeOffset();

		if (newOffset > offset)
			result = block;
	}

	return result;
}


inline class Hash* Mpq::firstEmptyHash() const
{
	BOOST_FOREACH(class Hash *hash, this->m_hashes)
	{
		if (hash->empty())
			return hash;
	}

	return 0;
}

inline class Hash* Mpq::firstDeletedHash() const
{
	BOOST_FOREACH(class Hash *hash, this->m_hashes)
	{
		if (hash->deleted())
			return hash;
	}

	return 0;
}

inline uint64 Mpq::nextBlockOffset() const
{
	class Block *block = this->lastOffsetBlock();

	if (block == 0)
		return Mpq::headerSize;

	return block->largeOffset() + block->m_blockSize;
}

inline void Mpq::nextBlockOffsets(int32 &blockOffset, int16 &extendedBlockOffset) const
{
	uint64 offset = this->nextBlockOffset();
	blockOffset = int32(offset << 16); // die hinteren Bits werden abgeschnitten???
	extendedBlockOffset = int16(offset >> 32);
}

}

}

#endif
