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

#include "platform.hpp"
#include "algorithm.hpp"
#include "hash.hpp"
#include "block.hpp"
#include "mpqfile.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * \brief This class allows users to read, write and modify MPQ archives. MPQ (Mo'PaQ, short for Mike O'Brien Pack) is an archiving file format used in several of Blizzard Entertainment's games.
 * Here's a simple example how to use this class:
 * \code
 * #include <wc3lib/mpq.hpp>
 * #include <boost/scoped_ptr.hpp>
 *
 * using namespace wc3lib::mpq;
 *
 * int main()
 * {
 * 	boost::scoped_ptr<Mpq> mpq(new Mpq());
 *
 * 	// Using exception handling to catch errors.
 * 	try
 * 	{
 * 		mpq->open("war3x.mpq"); // Opens an existing MPQ archive.
 * 	}
 * 	catch (Exception &exception)
 * 	{
 * 		std::cerr << "Unable to open archive: " << exception << std::endl;
 *
 * 		return EXIT_FAILURE;
 * 	}
 *
 *
 * 	const class MpqFile *mpqFile = mpq->findFile("UI/MiscData.txt"); // Default parameters search for files with neutral locale and default platform.
 *
 * 	if (mpqFile == 0)
 * 		return EXIT_FAILURE;
 *
 * 	std::cout << "MiscData.txt:\n" << mpqFile << std::endl; // Operator overloading allows you to get an MPQ file's content via << operator.
 * 	mpq->close(); // This function will be called automatically when variable mpq is being deleted.
 *
 *  	return EXIT_SUCCESS;
 * }
 * \endcode
 * There are three further classes which are related to this one and can be used by the user.
 * In fact, mostly user does only need to know \ref MpqFile which allows him constant access
 * to files contained by the MPQ archive.
 * Use \ref Mpq::addFile to add a new file which will return 0 (if an error occurs) or the new MpqFile
 * instance which refers to the newly created file.
 * Blocks (\ref Block), hashes (\ref Hash) and files (\ref MpqFile) are stored via smart pointers from Boost C++ Libraries (\ref boost::shared_ptr) for automatic deletion when freeing an MPQ object. Furthermore, they are stored under specific conditions and indices (especially files using Boost Multiindex library).
 */
class Mpq : public mpq::Format, private boost::noncopyable
{
	public:
		typedef uint64 LargeSizeType;

		BOOST_SCOPED_ENUM_START(Format)
		{
			Mpq1, /// Original format (Starcraft, Warcraft 3, Warcraft 3 The Frozen Throne, World of Warcraft)
			Mpq2 /// Burning Crusade, large files (size can be larger than 2^32 -> up to 2^64)
		};
		BOOST_SCOPED_ENUM_END

		BOOST_SCOPED_ENUM_START(ExtendedAttributes)
		{
			None = 0x0,
			FileCrc32s = 0x00000001,
			FileTimeStamps = 0x00000002,
			FileMd5s = 0x00000004
		};
		BOOST_SCOPED_ENUM_END

		typedef boost::shared_ptr<Block> BlockPtr;
		/**
		 * Blocks are mapped by their position in block table starting with 0. Maximum key value is \ref Mpq::blocks().size() - 1.
		 * Consider that block indices \ref Hash::blockIndexDeleted and \ref Hash::blockIndexEmpty.
		 */
		typedef boost::bimap<uint32, BlockPtr> Blocks;
		typedef boost::shared_ptr<Hash> HashPtr;
		/**
		 * Hashes are mapped by their position in hash table starting with 0. Maximum key value is \ref Mpq::hashes().size() - 1 (tag \ref uint32, index 0).
		 * Additionally hashes are stored hashed by their own data since they're already hash values (tag \ref HashData, index 1).
		 */
		typedef boost::multi_index_container<HashPtr,
		boost::multi_index::indexed_by<
		// ordered by its corresponding hash table index (like blocks)
		boost::multi_index::ordered_unique<boost::multi_index::tag<uint32>, boost::multi_index::const_mem_fun<Hash, uint32, &Hash::index> >,
		// hashed by their own value
		boost::multi_index::hashed_non_unique<boost::multi_index::tag<HashData>, boost::multi_index::const_mem_fun<Hash, const HashData&, &Hash::hashData> >
		>
		>
		Hashes;

		typedef boost::shared_ptr<MpqFile> FilePtr;
		/**
		 * Files are stored under various conditions:
		 * <ul>
		 * <li>as sequence (comparable to \ref std::list, index 0)</li>
		 * <li>unique ordered and indicated by their corresponding block (tag \ref Block, index 1)</li>
		 * <li>non unique and unordered (hashed) by their corresponding hash (tag \ref Hash, index 2)</li>
		 * <li>non unique and unordered (hashed) by their corresponding file key (tag \ref uint32, index 3)</li>
		 * </ul>
		 */
		typedef boost::multi_index_container<FilePtr,
		boost::multi_index::indexed_by<
		// first index is usual sequenced index like std::lists
		boost::multi_index::sequenced<>,
		// blocks are unique indices since each file must have its own block!
		boost::multi_index::ordered_unique<boost::multi_index::tag<Block>, boost::multi_index::const_mem_fun<MpqFile, Block*, &MpqFile::block> >,
		// hashed by corresponding hash value
		boost::multi_index::hashed_non_unique<boost::multi_index::tag<Hash>, boost::multi_index::const_mem_fun<MpqFile, Hash*, &MpqFile::hash> >,
		// hashed by corresponding block file key
		boost::multi_index::hashed_non_unique<boost::multi_index::tag<uint32>, boost::multi_index::const_mem_fun<MpqFile, uint32, &MpqFile::fileKey> >
		>
		> Files;

		static const byte identifier[4];
		static const int16 formatVersion1Identifier;
		static const int16 formatVersion2Identifier;
		static const int32 extendedAttributesVersion;

		/**
		 * 0xFFFFFFFE and 0xFFFFFFFF are reserved for deleted and empty blocks (by hashes).
		 */
		static const uint32 maxBlockId;
		static const uint32 maxHashId;
		static const std::size_t strongDigitalSignatureSize = 256;

		/**
		 * \return Returns static crypt table with size of \ref cryptTableSize.
		 */
		static const uint32* cryptTable();
		static bool hasStrongDigitalSignature(istream &istream);
		static std::streamsize strongDigitalSignature(istream &istream, char signature[Mpq::strongDigitalSignatureSize]) throw (class Exception);

		Mpq();
		virtual ~Mpq();

		/**
		 * Creates a new MPQ archive at file path \p path of format \p format starting at position \p startPosition in corresponding file and using extended attributes \p extendedAttributes and sector size \p sectorSize.
		 * \param overwriteExisting If this value is true existing any file at file path \p path will be overwritten before creating file/archive. Otherwise an exception will be thrown if there already is some file at the given path.
		 * \param hasStrongDigitalSignature If this value is true file strong digital signature will be generated automatically on archive creation (\ref Mpq::strongDigitalSignature).
		 * \param containsListfileFile If this value is true file "(listfile)" for all necessary file paths will be created on file/archive creation.
		 * \param containsSignatureFile If this value is true file "(signature)" for weak digital signature will be created on file/archive creation.
		 * \return Returns size of all created file data in bytes.
		 */
		std::streamsize create(const boost::filesystem::path &path, bool overwriteExisting = false, std::streampos startPosition = 0, BOOST_SCOPED_ENUM(Format) format = Format::Mpq1, BOOST_SCOPED_ENUM(ExtendedAttributes) extendedAttributes = ExtendedAttributes::None, int32 sectorSize = 4096, bool hasStrongDigitalSignature = false, bool containsListfileFile = false, bool containsSignatureFile = false) throw (class Exception);
		/**
		 * \param listfileEntries Instead of relying on an internal "(listfile)" file of the archive you can pass your own list of all files. If empty this list is ignored.
		 */
		std::streamsize open(const boost::filesystem::path &path, const MpqFile::ListfileEntries &listfileEntries = MpqFile::ListfileEntries()) throw (class Exception);
		void close();

		/**
		 * \param listfileEntries Instead of relying on an internal "(listfile)" file of the archive you can pass your own list of all files. If empty this list is ignored.
		 * \return Returns MPQ's size in bytes.
		 */
		virtual std::streamsize read(InputStream &stream, const MpqFile::ListfileEntries &listfileEntries) throw (class Exception);
		virtual std::streamsize read(InputStream &stream) throw (class Exception)
		{
			return read(stream, MpqFile::ListfileEntries());
		}
		/**
		 * Writes the whole MPQ archive into output stream \p ostream. Note that you don't have to call this function each time you want to save your changed data of the opened MPQ archive.
		* If you change some data of the opened MPQ archive it's written directly into the corresponding file (the whole archive is not loaded into memory!).
		 * \return Returns MPQ's size in bytes.
		 */
		virtual std::streamsize write(OutputStream &stream) const throw (class Exception);
		virtual uint32_t version() const;

		bool check() const;
		bool fix() const;
		/**
		* \return Returns true if the archive contains a "(listfile)" file.
		*/
		bool containsListfileFile() const;
		/**
		 * Creates a "(listfile)" file if there isn't already one. All file entries which do have corresponding file paths will be added to this file.
		 * \return Returns the file instance of the created file. If it does already exist the instance to the current file will be returned (note that it won't be refreshed by this method!).
		 * \sa Mpq::containsListfileFile, Mpq::listfileFile
		 */
		const class MpqFile* createListfileFile() throw (class Exception);
		const class MpqFile* listfileFile() const;
		/**
		 * \return Returns true if the archive contains a "(attributes)" file. This can also be checked by checking if \ref Mpq::extendedAttributes() does not return \ref Mpq::ExtendedAttributes::None.
		 */
		bool containsAttributesFile() const;
		/**
		 * Creates an "(attributes)" file if there isn't already one. The extended attributes of all block entries will be added to the file.
		 * \return Returns the file instance of the created file. If it does already exist the instance to the current file will be returned (note that it won't be refreshed by this method!).
		 * \sa Mpq::containsAttributesFile, Mpq::attributesFile
		 */
		const class MpqFile* createAttributesFile() throw (class Exception);
		const class MpqFile* attributesFile() const;

		bool containsSignatureFile() const;
		const class MpqFile* signatureFile() const;

		/**
		 * Note that the MPQ archive has no information about file paths if there is no "(listfile)" file. This function is the best way to get your required file.
		 * \return Returns the corresponding \ref MpqFile instance of the searched file. If no file was found it returns 0.
		 * \throw Exception Throws an exception if updating sector table fails which is necessary if file is encrypted and path is updated the first time.
		 */
		const class MpqFile* findFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale = MpqFile::Locale::Neutral, BOOST_SCOPED_ENUM(MpqFile::Platform) platform = MpqFile::Platform::Default) const  throw (Exception);
		/**
		 * Path of MPQ file \p mpqFile should be set if you use this method.
		 * Does not search for the instance of \p mpqFile. Only uses its hash data!
		 * \return Returns 0 if no file was found.
		 */
		const class MpqFile* findFile(const class MpqFile &mpqFile) const throw (Exception);

		/**
		 * Adds a new file to the MPQ archive with path \p path, locale \p locale and platform \p platform.
		 * \param istream This input stream is used for reading the initial file data.
		 * \todo Replace reservedSpace by size of istream?
		 */
		class MpqFile* addFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale, BOOST_SCOPED_ENUM(MpqFile::Platform) platform, const istream *istream = 0, bool overwriteExisting = false, int32 reservedSpace = 0) throw (class Exception);
		/**
		 * Path of MPQ file \p mpqFile should be set if you use this method.
		 * Does not add \p mpqFile. Only uses its meta data (beside you set \p addData to true)!
		 * \param addData If this value is true contained data of the file will be added to the new one.
		 */
		class MpqFile* addFile(const class MpqFile &mpqFile, bool addData = true, bool overwriteExisting = false) throw (class Exception);
		bool removeFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale = MpqFile::Locale::Neutral, BOOST_SCOPED_ENUM(MpqFile::Platform) platform = MpqFile::Platform::Default);
		/**
		 * Path of MPQ file \p mpqFile should be set if you use this method.
		 * \param mpqFile An MPQ file is searched which has the same hash value as \p mpqFile.
		 * \return Returns true if an MPQ file was found and deleted successfully.
		 */
		bool removeFile(const class MpqFile &mpqFile);

		class Hash* findHash(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale = MpqFile::Locale::Neutral, BOOST_SCOPED_ENUM(MpqFile::Platform) platform = MpqFile::Platform::Default);
		class Hash* findHash(const Hash &hash);
		class MpqFile* findFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale = MpqFile::Locale::Neutral, BOOST_SCOPED_ENUM(MpqFile::Platform) platform = MpqFile::Platform::Default) throw (Exception);
		class MpqFile* findFile(const class MpqFile &mpqFile) throw (Exception);

		/**
		 * This function is reserved for internal uses since it gets a non-constant parameter.
		 * \param mpqFile This file has to be contained by the MPQ archive and is completely deleted if function returns true (also its memory!). Note that the parameter has to be a reference of an MPQ file pointer and is set to 0.
		 * \return Returns true if the MPQ file was deleted successfully.
		 */
		bool removeFile(class MpqFile *&mpqFile);

		class MpqFile* listfileFile();
		class MpqFile* attributesFile();
		class MpqFile* signatureFile();
		class MpqFile* refreshAttributesFile();
		class MpqFile* refreshSignatureFile();

		/**
		 * \return Returns the size of the whole MPQ archive file.
		 */
		std::size_t size() const;
		const boost::filesystem::path& path() const;
		/**
		 * \return Returns the start position of the MPQ archive in its file.
		 */
		std::streampos startPosition() const;
		BOOST_SCOPED_ENUM(Format) format() const;
		BOOST_SCOPED_ENUM(ExtendedAttributes) extendedAttributes() const;
		/**
		 * Usually the sector size has type int16 and is computed by using formula:
		 * pow(2, sectorSizeShift) * 512. Instead of computing it every time we save
		 * the computed result and use int32.
		 * \sa Mpq::sectorSizeShift
		 */
		uint32 sectorSize() const;
		/**
		 * \return Computes the original header sector size shift value by using formula:
		 * sqrt(sectorSize / 512)
		 */
		uint16 sectorSizeShift() const;
		bool hasStrongDigitalSignature() const;
		/**
		 * \return Returns archive's strong digital signature with size of \ref Mpq::stringDigitalSignatureSize.
		 */
		const char* strongDigitalSignature() const;
		/**
		 * When \ref Mpq::open() or \ref Mpq::create() is called archive is opened automatically until destructor or \ref Mpq::close() is called.
		 * For interprocess communication a file lock object is created which can be used via \ref Mpq::fileLock().
		 */
		bool isOpen() const;
		const boost::interprocess::file_lock& fileLock();
		const Blocks& blocks() const;
		const Hashes& hashes() const;
		const Files& files() const;

		/**
		 * \return Returns the sum of all block sizes.
		 * \warning \ref T can be too small to hold all block's sizes since each block's size is defined as \ref int32 value.
		 * \sa Block::blockSize
		 */
		LargeSizeType entireBlockSize() const;
		/**
		 * \return Returns the sum of all sizes of all used blocks.
		 * \warning \ref T can be too small to hold all sizes of all used blocks since each block's size is defined as \ref int32 value.
		 * \sa Block::blockSize, Block::empty
		 */
		LargeSizeType entireUsedBlockSize() const;
		/**
		 * \return Returns the sum of all sizes of all empty blocks.
		 * \warning \ref T can be too small to hold all sizes of all empty blocks since each block's size is defined as \ref int32 value.
		 * \sa Block::blockSize, Block::empty
		 */
		LargeSizeType entireEmptyBlockSize() const;
		/**
		 * \return Returns the sum of all file sizes (uncompressed).
		 * \warning \ref T can be too small to hold all file sizes since each file's size is defined as \ref int32 value.
		 * * \sa Block::fileSize
		 */
		LargeSizeType entireFileSize() const;

		/**
		 * \return Returns true if archive is not opened.
		 */
		bool operator!() const;
		/**
		 * Copies data of file \p mpqFile and adds it to the MPQ archive.
		 * Does not overwrite existing files.
		 * \return Returns *this.
		 */
		class Mpq& operator<<(const class MpqFile &mpqFile) throw (class Exception);
		/**
		 * Copies data of all files of MPQ archive \p mpq to the MPQ archive.
		 * Does not overwrite existing files.
		 * \return Returns *this.
		 */
		class Mpq& operator<<(const class Mpq &mpq) throw (class Exception);
		/**
		 * Copies data of alle files of the MPQ archive to MPQ archive \p mpq.
		 * \return Returns *this.
		 */
		class Mpq& operator>>(class Mpq &mpq) throw (class Exception);

	protected:
		friend class MpqFile;
		friend class Hash;
		friend class Block;

		Mpq(const Mpq &mpq);

		/**
		 * Overwrite these member functions to return custom type-based objects if you want to extend their functionality.
		 */
		virtual class MpqFile* newFile(class Hash *hash) throw ();
		virtual class Hash* newHash() throw ();
		virtual class Block* newBlock() throw ();

		/**
		 * Does not check if archive is open.
		 * \sa Mpq::close
		 */
		void clear();

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
		 * \return Returns the block with the biggest offset.
		 */
		class Block* lastOffsetBlock() const;
		class Hash* firstEmptyHash() const;
		class Hash* firstDeletedHash() const;
		/**
		 * \return Returns the next block offset as large unsigned integer. Next block offset means the offset of the next added block (last block offset + last block size / header size).
		 */
		uint64 nextBlockOffset() const;
		/**
		 * Same as function \ref Mpq::nextBlockOffset but divides large offset value into \p blockOffset and \p extendedBlockOffset.
		 */
		void nextBlockOffsets(uint32 &blockOffset, uint16 &extendedBlockOffset) const;

		/// This value is required for getting the first block offset (relative to the beginning of archive).
		static const std::size_t headerSize;

		std::size_t m_size;
		boost::filesystem::path m_path;
		std::streampos m_startPosition;
		BOOST_SCOPED_ENUM(Format) m_format;
		BOOST_SCOPED_ENUM(ExtendedAttributes) m_extendedAttributes;
		uint32 m_sectorSize;
		boost::scoped_array<char> m_strongDigitalSignature;
		bool m_isOpen;
		boost::interprocess::file_lock m_fileLock;
		Blocks m_blocks;
		Hashes m_hashes;
		Files m_files;
};

inline bool Mpq::hasStrongDigitalSignature(istream &istream)
{
	static const uint32 identifier = 'NGIS';
	uint32 data;
	std::streamsize size = 0;
	const std::streampos position = istream.tellg();
	wc3lib::read(istream, data, size);
	bool result = size == sizeof(identifier) && memcmp(&identifier, &data, sizeof(identifier)) == 0;
	istream.seekg(position); // jump back

	return result;
}

inline std::streamsize Mpq::strongDigitalSignature(istream &istream, char signature[Mpq::strongDigitalSignatureSize]) throw (class Exception)
{
	boost::scoped_ptr<StrongDigitalSignature> sig(new StrongDigitalSignature);
	std::streamsize size = 0;
	wc3lib::read(istream, *sig.get(), size);
	memcpy(signature, sig->signature, Mpq::strongDigitalSignatureSize);

	return size;
}

inline ostream& operator<<(ostream &stream, const class Mpq &mpq) throw (class Exception)
{
	mpq.write(stream);

	return stream;
}

inline istream& operator>>(istream &stream, class Mpq &mpq) throw (class Exception)
{
	mpq.read(stream);

	return stream;
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
	if (this->extendedAttributes() != Mpq::ExtendedAttributes::None && !this->containsAttributesFile())
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

	std::list<std::basic_string<byte> > entries;

	BOOST_FOREACH(const FilePtr mpqFile, this->m_files.get<0>())
	{
		if (!mpqFile->path().empty() && mpqFile->path().string() != "(attributes)") /// @todo Exclude directories and file (signature)?
			entries.push_back(reinterpret_cast<const byte*>(mpqFile->path().string().c_str()));
	}

	entries.sort(); /// @todo Sort alphabetically?
	arraystream stream;

	BOOST_FOREACH(std::basic_string<byte> entry, entries)
		stream << entry << '\n';

	return this->addFile("(listfile)", MpqFile::Locale::Neutral, MpqFile::Platform::Default, &stream);
}

inline const class MpqFile* Mpq::listfileFile() const
{
	return this->findFile("(listfile)", MpqFile::Locale::Neutral, MpqFile::Platform::Default);
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
	stringstream stream;
	stream.write(reinterpret_cast<byte*>(&extendedAttributesHeader), sizeof(extendedAttributesHeader));

	if (this->extendedAttributes() & Mpq::ExtendedAttributes::FileCrc32s)
	{
		BOOST_FOREACH(Blocks::left_const_reference reference, this->m_blocks.left)
			stream.write(reinterpret_cast<const byte*>(&reference.second->m_crc32), sizeof(reference.second->m_crc32));
	}

	if (this->extendedAttributes() & Mpq::ExtendedAttributes::FileTimeStamps)
	{
		BOOST_FOREACH(Blocks::left_const_reference reference, this->m_blocks.left)
			stream.write(reinterpret_cast<const byte*>(&reference.second->m_fileTime), sizeof(reference.second->m_fileTime));
	}

	if (this->extendedAttributes() & Mpq::ExtendedAttributes::FileMd5s)
	{
		BOOST_FOREACH(Blocks::left_const_reference reference, this->m_blocks.left)
			stream.write(reinterpret_cast<const byte*>(&reference.second->m_md5), sizeof(reference.second->m_md5));
	}

	return this->addFile("(attributes)", MpqFile::Locale::Neutral, MpqFile::Platform::Default, &stream);
}

inline const class MpqFile* Mpq::attributesFile() const
{
	return this->findFile("(attributes)");
}

inline bool Mpq::containsSignatureFile() const
{
	return this->signatureFile() != 0;
}

inline const class MpqFile* Mpq::signatureFile() const
{
	return this->findFile("(signature)");
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

inline BOOST_SCOPED_ENUM(Mpq::Format) Mpq::format() const
{
	return this->m_format;
}

inline BOOST_SCOPED_ENUM(Mpq::ExtendedAttributes) Mpq::extendedAttributes() const
{
	return this->m_extendedAttributes;
}

inline uint32 Mpq::sectorSize() const
{
	return this->m_sectorSize;
}

inline uint16 Mpq::sectorSizeShift() const
{
	return sqrt(this->m_sectorSize / 512);
}

inline bool Mpq::hasStrongDigitalSignature() const
{
	return this->m_strongDigitalSignature.get() != 0;
}

inline const char* Mpq::strongDigitalSignature() const
{
	return this->m_strongDigitalSignature.get();
}

inline bool Mpq::isOpen() const
{
	return this->m_isOpen;
}

inline const boost::interprocess::file_lock& Mpq::fileLock()
{
	return m_fileLock;
}

inline const Mpq::Blocks& Mpq::blocks() const
{
	return this->m_blocks;
}

inline const Mpq::Hashes& Mpq::hashes() const
{
	return this->m_hashes;
}

inline const Mpq::Files& Mpq::files() const
{
	return this->m_files;
}

inline Mpq::LargeSizeType Mpq::entireBlockSize() const
{
	Mpq::LargeSizeType result = 0;

	BOOST_FOREACH(Blocks::left_const_reference block, this->m_blocks.left)
		result += block.second->m_blockSize;

	return result;
}

inline Mpq::LargeSizeType Mpq::entireUsedBlockSize() const
{
	Mpq::LargeSizeType result = 0;

	BOOST_FOREACH(Blocks::left_const_reference block, this->m_blocks.left)
	{
		if (!block.second->empty())
			result += block.second->blockSize();
	}

	return result;
}

inline Mpq::LargeSizeType Mpq::entireEmptyBlockSize() const
{
	Mpq::LargeSizeType result = 0;

	BOOST_FOREACH(Blocks::left_const_reference block, this->m_blocks.left)
	{
		if (block.second->empty())
			result += block.second->blockSize();
	}

	return result;
}

inline Mpq::LargeSizeType Mpq::entireFileSize() const
{
	Mpq::LargeSizeType result = 0;

	BOOST_FOREACH(Blocks::left_const_reference block, this->m_blocks.left)
	{
		// usually size should be 0 if it's not a file but anyway we should check
		if (block.second->flags() & Block::Flags::IsFile)
			result += block.second->fileSize();
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
	BOOST_FOREACH(Blocks::left_const_reference reference, this->m_blocks.left)
	{
		if (!reference.second->check())
			return false;
	}

	return true;
}

inline bool Mpq::checkHashes() const
{
	BOOST_FOREACH(const HashPtr hash, this->m_hashes)
	{
		if (!hash->check())
			return false;

		BOOST_FOREACH(const HashPtr hash2, this->m_hashes)
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
	BOOST_FOREACH(Blocks::left_const_reference value, this->m_blocks.left)
	{
		if (value.second->empty())
			return value.second.get();
	}

	return 0;
}

inline class Block* Mpq::firstUnusedBlock() const
{
	BOOST_FOREACH(Blocks::left_const_reference value, this->m_blocks.left)
	{
		if (value.second->unused())
			return value.second.get();
	}

	return 0;
}

inline class Block* Mpq::lastOffsetBlock() const
{
	uint64 offset = 0;
	class Block *result = 0;

	BOOST_FOREACH(Blocks::left_const_reference value, this->m_blocks.left)
	{
		uint64 newOffset = value.second->largeOffset();

		if (newOffset > offset)
		{
			result = value.second.get();
			offset = newOffset;
		}
	}

	return result;
}


inline class Hash* Mpq::firstEmptyHash() const
{
	BOOST_FOREACH(HashPtr hash, this->m_hashes)
	{
		if (hash->empty())
			return hash.get();
	}

	return 0;
}

inline class Hash* Mpq::firstDeletedHash() const
{
	BOOST_FOREACH(HashPtr hash, this->m_hashes)
	{
		if (hash->deleted())
			return hash.get();
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

inline void Mpq::nextBlockOffsets(uint32 &blockOffset, uint16 &extendedBlockOffset) const
{
	uint64 offset = this->nextBlockOffset();
	blockOffset = int32(offset << 16); // die hinteren Bits werden abgeschnitten???
	extendedBlockOffset = int16(offset >> 32);
}

}

}

#endif
