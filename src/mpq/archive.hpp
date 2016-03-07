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

#ifndef WC3LIB_MPQ_MPQ_HPP
#define WC3LIB_MPQ_MPQ_HPP

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/cast.hpp>
#include <boost/foreach.hpp>

/*
 * Get definitions first.
 */
#include "platform.hpp"

#include "algorithm.hpp"
#include "hash.hpp"
#include "block.hpp"
#include "listfile.hpp"
#include "attributes.hpp"
#include "signature.hpp"

namespace wc3lib
{

namespace mpq
{

class File;

/**
 * \brief This class allows users to read, write and modify MPQ archives. MPQ (Mo'PaQ, short for Mike O'Brien Pack) is an archiving file format used in several of Blizzard Entertainment's games.
 *
 * Use \ref Archive::open() or \ref Archive::create() to open an existing or create a new MPQ archive on the filesystem.
 *
 * For operations on the archive there is several functions with different variations:
 *
 * Use \ref Archive::removeFile() to remove a file from archive.
 *
 * Use \ref Archive::findFile() to find a file in the archive.
 *
 * Special files can be accessed via the following functions:
 * <ul>
 * <li> \ref listfileFile() - accesses file "(listfile)" with filepath entries for all contained files </li>
 * <li> \ref signatureFile() - accesses file "(signature)" with a digital signature of the archive </li>
 * <li> \ref attributesFile() - accesses file "(attributes)" with extended attributes like timestamps and checksums of contained files </li>
 * </ul>
 *
 * \note Synchronization is not implemented in any way since Boost IPC file locks are only "advisory locks". Therefore you should never call operations concurrently.
 *
 */
class Archive : public Format, private boost::noncopyable
{
	public:
		enum class Format : uint16
		{
			Mpq1, /// Original format (Starcraft, Warcraft 3, Warcraft 3 The Frozen Throne, World of Warcraft)
			Mpq2 /// Burning Crusade, large files (size can be larger than 2^32 -> up to 2^64)
		};

		/**
		 * Blocks are indexed by their position in block table starting with 0.
		 * Maximum index value is \ref Archive::blocks().size() - 1.
		 * Consider that block indices \ref Hash::blockIndexDeleted and \ref Hash::blockIndexEmpty are reserved for special purpose.
		 */
		typedef boost::ptr_vector<Block> Blocks;

		/**
		 * Hashes are stored using their hash values (\ref HashData) for fast
		 * access on files by their file paths.
		 * Each Hash refers to one file.
		 *
		 * \note Since hashes can be empty or deleted a multimap is required.
		 */
		typedef boost::ptr_unordered_multimap<HashData, Hash> Hashes;

		/**
		 * Array with size of \ref strongDigitalSignatureSize.
		 * \note Doesn't include any header data.
		 */
		typedef boost::scoped_array<char> StrongDigitalSignature;

		static const byte identifier[4];
		static const int16 formatVersion1Identifier;
		static const int16 formatVersion2Identifier;

		/**
		 * 0xFFFFFFFE and 0xFFFFFFFF are reserved for deleted and empty blocks (by hashes).
		 */
		static const uint32 maxBlockId;
		static const uint32 maxHashId;
		/**
		 * \note Doesn't include any header data.
		 */
		static const std::size_t strongDigitalSignatureSize = 256;

		/**
		 * \return Returns static crypt table with size of \ref cryptTableSize.
		 */
		static const uint32* cryptTable();
		static bool hasStrongDigitalSignature(istream &istream);
		static std::streamsize strongDigitalSignature(istream &istream, StrongDigitalSignature &signature);

		/**
		 * Creates a new instance for an MPQ archive.
		 * Use \ref create() or \ref open() to create or open a real archive file.
		 */
		Archive();
		virtual ~Archive();

		/**
		 * Creates a new MPQ archive at file path \p path of format \p format starting at position \p startPosition in corresponding file and using sector size \p sectorSize.
		 * \param overwriteExisting If this value is true existing any file at file path \p path will be overwritten before creating file/archive. Otherwise an exception will be thrown if there already is some file at the given path.
		 * \return Returns size of all created file data in bytes.
		 */
		std::streamsize create(const boost::filesystem::path &path, bool overwriteExisting = false, std::streampos startPosition = 0, Format format = Format::Mpq1, uint32 sectorSize = 4096);
		/**
		 * Opens an MPQ file on the local file system at \p path.
		 * This reads the block and hash tables.
		 */
		std::streamsize open(const boost::filesystem::path &path);
		/**
		 * Closes the MPQ archive which clears all hashes and blocks.
		 */
		void close();

		/**
		 * \return Returns MPQ's size in bytes.
		 */
		virtual std::streamsize read(InputStream &stream) override;
		/**
		 * Writes the whole MPQ archive into output stream \p ostream. Note that you don't have to call this function each time you want to save your changed data of the opened MPQ archive.
		* If you change some data of the opened MPQ archive it's written directly into the corresponding file (the whole archive is not loaded into memory!).
		 * \return Returns MPQ's size in bytes.
		 */
		virtual std::streamsize write(OutputStream &stream) const override;
		virtual uint32_t version() const override;

		/**
		* \return Returns true if the archive contains a "(listfile)" file.
		*/
		bool containsListfileFile() const;
		Listfile listfileFile();

		/**
		 * \return Returns true if the archive contains a "(attributes)" file.
		 */
		bool containsAttributesFile() const;
		/**
		 * \note Accesses the archive for reading the header data.
		 * \todo Remove reading header data. Class \ref Attributes should do it itself and read it from the archive on request!
		 */
		Attributes attributesFile();

		/**
		 * The weak digital signature is a digital signature using Microsoft CryptoAPI. It is an implimentation
		 * of the RSASSA-PKCS1-v1_5 digital signature protocol, using the MD5 hashing algorithm and a 512-bit (weak)
		 * RSA key (for more information about this protocol, see the RSA Labs PKCS1 specification).
		 * \sa hasStrongDigitalSignature()
		 */
		bool containsSignatureFile() const;
		Signature signatureFile();

		/**
		 * Path of MPQ file \p mpqFile should be set if you use this method.
		 * \param mpqFile An MPQ file is searched which has the same hash value as \p mpqFile.
		 * \return Returns true if an MPQ file was found and deleted successfully.
		 */
		bool removeFile(const File &mpqFile);
		/**
		 * Adds a new file to the archive.
		 * \return Returns the newly added file.
		 */
		File addFile(const boost::filesystem::path &filePath, istream &istream, Sector::Compression compression = Sector::Compression::Uncompressed);

		/**
		 * Searches for hash table entry using \p hashData.
		 * This function returns used hash entries as well as deleted and empty ones.
		 * \return Returns 0 if no hash entry was found.
		 * \ingroup search
		 *
		 * @{
		 */
		Hash* findHash(const HashData &hashData);
		const Hash* findHash(const HashData &hashData) const;
		/**
		 * @}
		 */
		/**
		 * Searches for hash table entry by generating an \ref HashData instance using \p path, \p locale and \p platform.
		 * This function returns used hash entries as well as deleted and empty ones.
		 * \return Returns an 0 if no hash entry was found.
		 *
		 * \ingroup search
		 *
		 * @{
		 */
		Hash* findHash(const boost::filesystem::path &path, File::Locale locale = File::Locale::Neutral, File::Platform platform = File::Platform::Default);
		const Hash* findHash(const boost::filesystem::path &path, File::Locale locale = File::Locale::Neutral, File::Platform platform = File::Platform::Default) const;
		/**
		 * @}
		 */

		/**
		 * Searches for file using \p hashData.
		 * \return Returns an invalid file if no file was found.
		 * \ingroup search
		 */
		File findFile(const HashData &hashData);
		/**
		 * Searches for file by generating an \ref HashData instance using \p path, \p locale and \p platform.
		 * \return Returns an invalid file if no file was found.
		 *
		 * \ingroup search
		 */
		File findFile(const boost::filesystem::path &path, File::Locale locale = File::Locale::Neutral, File::Platform platform = File::Platform::Default);

		/**
		 * \return Returns the size of the whole MPQ archive file.
		 */
		std::size_t size() const;
		const boost::filesystem::path& path() const;
		/**
		 * \return Returns the start position of the MPQ archive in its file.
		 */
		std::streampos startPosition() const;
		Format format() const;
		/**
		 * Usually the sector size has type uint16 and is computed by using formula:
		 * pow(2, sectorSizeShift) * 512. Instead of computing it every time we save
		 * the computed result and use uint32.
		 * \sa Archive::sectorSizeShift
		 */
		uint32 sectorSize() const;
		/**
		 * \return Computes the original header sector size shift value by using formula:
		 * sqrt(sectorSize / 512)
		 */
		uint16 sectorSizeShift() const;
		/**
		 * The strong digital signature uses a simple proprietary implementation of RSA signing, using the SHA-1 hashing algorithm and a 2048-bit (strong) RSA key.
		 * \sa containsSignatureFile()
		 */
		bool hasStrongDigitalSignature() const;
		/**
		 * \return Returns archive's strong digital signature with size of \ref strongDigitalSignatureSize.
		 */
		const StrongDigitalSignature& strongDigitalSignature() const;

		/**
		 * When \ref open() or \ref create() is called archive is opened automatically until destructor or \ref close() is called.
		 */
		bool isOpen() const;

		/**
		 * \return Returns all blocks from the block table.
		 *
		 * @{
		 */
		Blocks& blocks();
		const Blocks& blocks() const;
		/**
		 * @}
		 */

		/**
		 * \return Returns all hashes from the hash table.
		 *
		 * @{
		 */
		Hashes& hashes();
		const Hashes& hashes() const;
		/**
		 * @}
		 */

		/**
		 * \return Returns true if archive is not opened.
		 */
		bool operator!() const;

		/**
		 * Basic IO operations for header structures with offsets and format information.
		 *
		 * @{
		 */
		/**
		 * Reads the header structure of an MPQ archive from input stream \p in and stores the position of the header into \p startPosition.
		 * \return Returns true if the header was found. Otherwise it returns false.
		 */
		bool readHeader(istream &in, Header &header, uint64 &startPosition, std::streamsize &size);
		bool readBlockTable(istream &in, uint32 entries, std::streamsize &size);
		bool readExtendedBlockTable(istream &in, std::streamsize &size);
		bool readHashTable(istream &in, uint32 entries, std::streamsize &size);
		bool writeHeader(ostream &out, std::streamsize &size) const;
		bool writeBlockTable(ostream &out, std::streamsize &size) const;
		bool writeExtendedBlockTable(ostream &out, std::streamsize &size) const;
		bool writeHashTable(ostream &out, std::streamsize &size) const;
		/**
		 * @}
		 */

	protected:
		/**
		 * Does not check if archive is open.
		 * \sa Archive::close
		 */
		void clear();

		/**
		 * Empty space entries should have BlockOffset and BlockSize nonzero, and FileSize and Flags zero.
		 */
		Block* firstEmptyBlock();
		/**
		 * Unused block table entries should have BlockSize, FileSize, and Flags zero.
		 */
		Block* firstUnusedBlock();
		/**
		 * \return Returns the block with the biggest offset.
		 */
		Block* lastOffsetBlock();
		Hash* firstEmptyHash();
		Hash* firstDeletedHash();
		/**
		 * \return Returns the next block offset as large unsigned integer. Next block offset means the offset of the next added block (last block offset + last block size / header size).
		 */
		uint64 nextBlockOffset();
		/**
		 * Same as function \ref Archive::nextBlockOffset but divides large offset value into \p blockOffset and \p extendedBlockOffset.
		 */
		void nextBlockOffsets(uint32 &blockOffset, uint16 &extendedBlockOffset);

		/// This value is required for getting the first block offset (relative to the beginning of archive).
		static const std::size_t headerSize;

		std::size_t m_size;
		boost::filesystem::path m_path;
		uint64 m_startPosition;
		uint64 m_blockTableOffset; /// Relative to the start position.
		uint64 m_extendedBlockTableOffset; /// Relative to the start position.
		uint64 m_hashTableOffset; /// Relative to the start position.
		uint64 m_strongDigitalSignaturePosition; // position of strong digital signature in archive starting at its header ('NGIS')!
		Format m_format;
		uint32 m_sectorSize;
		StrongDigitalSignature m_strongDigitalSignature;
		bool m_isOpen;
		Blocks m_blocks;
		Hashes m_hashes;
};

inline bool Archive::hasStrongDigitalSignature(istream &istream)
{
	static const uint32 identifier[4] = { 'N', 'G', 'I', 'S' };
	uint32 data;
	std::streamsize size = 0;
	const std::streampos position = istream.tellg();
	wc3lib::read(istream, data, size);
	bool result = size == sizeof(identifier) && memcmp(&identifier, &data, sizeof(identifier)) == 0;
	istream.seekg(position); // jump back

	return result;
}

inline std::streamsize Archive::strongDigitalSignature(istream &istream, StrongDigitalSignature &signature)
{
	istream.seekg(sizeof(uint32), std::ios::cur); // skip header
	StrongDigitalSignature sig(new char[strongDigitalSignatureSize]);
	std::streamsize size = 0;
	wc3lib::read(istream, sig[0], size, strongDigitalSignatureSize);
	signature.swap(sig);

	return size;
}

inline bool Archive::containsListfileFile() const
{
	return this->findHash("(listfile)") != 0;
}

inline bool Archive::containsAttributesFile() const
{
	return this->findHash("(attributes)") != 0;
}

inline bool Archive::containsSignatureFile() const
{
	return this->findHash("(signature)") != 0;
}

inline std::size_t Archive::size() const
{
	return this->m_size;
}

inline const boost::filesystem::path& Archive::path() const
{
	return this->m_path;
}

inline std::streampos Archive::startPosition() const
{
	return this->m_startPosition;
}

inline Archive::Format Archive::format() const
{
	return this->m_format;
}

inline uint32 Archive::sectorSize() const
{
	return this->m_sectorSize;
}

inline uint16 Archive::sectorSizeShift() const
{
	return sqrt(this->m_sectorSize / 512);
}

inline bool Archive::hasStrongDigitalSignature() const
{
	return this->m_strongDigitalSignature.get() != 0;
}

inline const Archive::StrongDigitalSignature& Archive::strongDigitalSignature() const
{
	return this->m_strongDigitalSignature;
}

inline bool Archive::isOpen() const
{
	return this->m_isOpen;
}

inline Archive::Blocks& Archive::blocks()
{
	return this->m_blocks;
}

inline const Archive::Blocks& Archive::blocks() const
{
	return this->m_blocks;
}

inline Archive::Hashes& Archive::hashes()
{
	return this->m_hashes;
}

inline const Archive::Hashes& Archive::hashes() const
{
	return this->m_hashes;
}

inline Block* Archive::firstEmptyBlock()
{
	BOOST_FOREACH(Block &block, this->blocks())
	{
		if (block.empty())
		{
			return &block;
		}
	}

	return 0;
}

inline Block* Archive::firstUnusedBlock()
{
	BOOST_FOREACH(Block &block, this->blocks())
	{
		if (block.unused())
		{
			return &block;
		}
	}

	return 0;
}

inline Block* Archive::lastOffsetBlock()
{
	uint64 offset = 0;
	class Block *result = 0;

	BOOST_FOREACH(Block &block, this->blocks())
	{
		const uint64 newOffset = block.largeOffset();

		if (newOffset > offset)
		{
			result = &block;
			offset = newOffset;
		}
	}

	return result;
}


inline Hash* Archive::firstEmptyHash()
{
	BOOST_FOREACH(Hashes::value_type pair, this->hashes())
	{
		if (pair.second->empty())
		{
			return pair.second;
		}
	}

	return 0;
}

inline Hash* Archive::firstDeletedHash()
{
	BOOST_FOREACH(Hashes::value_type pair, this->hashes())
	{
		if (pair.second->deleted())
		{
			return pair.second;
		}
	}

	return 0;
}

inline uint64 Archive::nextBlockOffset()
{
	Block *block = this->lastOffsetBlock();

	if (block == 0)
	{
		return Archive::headerSize;
	}

	return block->largeOffset() + block->blockSize();
}

inline void Archive::nextBlockOffsets(uint32 &blockOffset, uint16 &extendedBlockOffset)
{
	uint64 offset = this->nextBlockOffset();
	blockOffset = int32(offset << 16); // die hinteren Bits werden abgeschnitten???
	extendedBlockOffset = int16(offset >> 32);
}

}

}

#endif
