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

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/cast.hpp>
#include <boost/foreach.hpp>

#include <crypto++/sha.h>
#include <crypto++/rsa.h>

#include "platform.hpp"
#include "algorithm.hpp"
#include "hash.hpp"
#include "block.hpp"
#include "mpqfile.hpp"
#include "listfile.hpp"
#include "attributes.hpp"
#include "signature.hpp"

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
class Mpq : public Format, private boost::noncopyable
{
	public:
		typedef uint64 LargeSizeType;

		BOOST_SCOPED_ENUM_START(Format)
		{
			Mpq1, /// Original format (Starcraft, Warcraft 3, Warcraft 3 The Frozen Throne, World of Warcraft)
			Mpq2 /// Burning Crusade, large files (size can be larger than 2^32 -> up to 2^64)
		};
		BOOST_SCOPED_ENUM_END

		typedef boost::shared_ptr<Block> BlockPtr;
		typedef boost::shared_ptr<const Block> BlockPtrConst;
		/**
		 * Blocks are mapped by their position in block table starting with 0. Maximum key value is \ref Mpq::blocks().size() - 1 (tag \ref uint32, index 0).
		 * Consider that block indices \ref Hash::blockIndexDeleted and \ref Hash::blockIndexEmpty.
		 */
		typedef boost::multi_index_container<BlockPtr,
		boost::multi_index::indexed_by<
		// ordered by its corresponding hash table index (like blocks)
		boost::multi_index::ordered_unique<boost::multi_index::tag<uint32>, boost::multi_index::const_mem_fun<Block, uint32, &Block::index> >
		>
		>
		Blocks;
		typedef boost::shared_ptr<Hash> HashPtr;
		typedef boost::shared_ptr<const Hash> HashPtrConst;
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
		typedef boost::shared_ptr<const MpqFile> FilePtrConst;
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

		/**
		 * Array with size of \ref CryptoPP::SHA1::DIGESTSIZE.
		 */
		typedef boost::scoped_array<unsigned char> SHA1Digest;
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
		static const bool defaultStoreSectors = true;

		/**
		 * \return Returns static crypt table with size of \ref cryptTableSize.
		 */
		static const uint32* cryptTable();
		static bool hasStrongDigitalSignature(istream &istream);
		static std::streamsize strongDigitalSignature(istream &istream, StrongDigitalSignature &signature) throw (class Exception);

		Mpq();
		virtual ~Mpq();

		/**
		 * Creates a new MPQ archive at file path \p path of format \p format starting at position \p startPosition in corresponding file and using extended attributes \p extendedAttributes and sector size \p sectorSize.
		 * \param overwriteExisting If this value is true existing any file at file path \p path will be overwritten before creating file/archive. Otherwise an exception will be thrown if there already is some file at the given path.
		 * \return Returns size of all created file data in bytes.
		 */
		std::streamsize create(const boost::filesystem::path &path, bool overwriteExisting = false, std::streampos startPosition = 0, BOOST_SCOPED_ENUM(Format) format = Format::Mpq1, uint32 sectorSize = 4096) throw (class Exception);
		/**
		 * \param listfileEntries Instead of relying on an internal "(listfile)" file of the archive you can pass your own list of all files (internal file will be ignored!). If it's empty this list is ignored.
		 */
		std::streamsize open(const boost::filesystem::path &path, const Listfile::Entries &listfileEntries = Listfile::Entries()) throw (class Exception);
		void close();

		/**
		 * \param listfileEntries Instead of relying on an internal "(listfile)" file of the archive you can pass your own list of all files (internal file will be ignored!). If it's empty this list is ignored.
		 * \return Returns MPQ's size in bytes.
		 */
		virtual std::streamsize read(InputStream &stream, const Listfile::Entries &listfileEntries) throw (class Exception);
		virtual std::streamsize read(InputStream &stream) throw (class Exception)
		{
			return read(stream, Listfile::Entries());
		}
		/**
		 * Writes the whole MPQ archive into output stream \p ostream. Note that you don't have to call this function each time you want to save your changed data of the opened MPQ archive.
		* If you change some data of the opened MPQ archive it's written directly into the corresponding file (the whole archive is not loaded into memory!).
		 * \return Returns MPQ's size in bytes.
		 */
		virtual std::streamsize write(OutputStream &stream) const throw (class Exception);
		virtual uint32_t version() const;

		/**
		* \return Returns true if the archive contains a "(listfile)" file.
		*/
		bool containsListfileFile() const;
		/**
		 * Creates a "(listfile)" file if there isn't already one. All file entries which do have corresponding file paths will be added to this file.
		 * \return Returns the file instance of the created file. If it does already exist the instance to the current file will be returned (note that it won't be refreshed by this member functio!).
		 * \sa Mpq::containsListfileFile(), Mpq::listfileFile()
		 */
		const class Listfile* createListfileFile();
		const class Listfile* listfileFile() const;
		/**
		 * \return Returns true if the archive contains a "(attributes)" file.
		 */
		bool containsAttributesFile() const;
		/**
		 * Creates an "(attributes)" file if there isn't already one. The extended attributes of all block entries will be added to the file.
		 * \param extendedAttributes These attributes are stored for each file.
		 * \return Returns the file instance of the created file. If it does already exist the instance to the current file will be returned (note that it won't be refreshed by this member function!).
		 * \sa Mpq::containsAttributesFile(), Mpq::attributesFile()
		 */
		const class Attributes* createAttributesFile(BOOST_SCOPED_ENUM(Attributes::ExtendedAttributes) extendedAttributes);
		const class Attributes* attributesFile() const;

		/**
		 * The weak digital signature is a digital signature using Microsoft CryptoAPI. It is an implimentation
		 * of the RSASSA-PKCS1-v1_5 digital signature protocol, using the MD5 hashing algorithm and a 512-bit (weak)
		 * RSA key (for more information about this protocol, see the RSA Labs PKCS1 specification).
		 * \sa hasStrongDigitalSignature()
		 */
		bool containsSignatureFile() const;
		const class Signature* signatureFile() const;

		/**
		 * Note that the MPQ archive has no information about file paths if there is no "(listfile)" file. This function is the best way to get your required file.
		 * \return Returns the corresponding \ref MpqFile instance of the searched file. If no file was found it returns 0.
		 * \throw Exception Throws an exception if updating sector table fails which is necessary if file is encrypted and path is updated the first time.
		 */
		FilePtrConst findFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale = MpqFile::Locale::Neutral, BOOST_SCOPED_ENUM(MpqFile::Platform) platform = MpqFile::Platform::Default) const  throw (Exception);
		/**
		 * Path of MPQ file \p mpqFile should be set if you use this method.
		 * Does not search for the instance of \p mpqFile. Only uses its hash data!
		 * \return Returns 0 if no file was found.
		 */
		FilePtrConst findFile(const class MpqFile &mpqFile) const throw (Exception);

		/**
		 * Adds a new file to the MPQ archive with path \p path, locale \p locale and platform \p platform.
		 * \param istream This input stream is used for reading the initial file data.
		 * \todo Replace reservedSpace by size of istream?
		 */
		FilePtr addFile(const boost::filesystem::path &path, const byte *buffer, std::size_t bufferSize, bool overwriteExisting = false, BOOST_SCOPED_ENUM(MpqFile::Locale) locale = MpqFile::Locale::Neutral, BOOST_SCOPED_ENUM(MpqFile::Platform) platform = MpqFile::Platform::Default) throw (class Exception);
		/**
		 * Path of MPQ file \p mpqFile should be set if you use this method.
		 * Does not add \p mpqFile. Only uses its meta data (beside you set \p addData to true)!
		 * \param addData If this value is true contained data of the file will be added to the new one.
		 */
		FilePtr addFile(const class MpqFile &mpqFile, bool addData = true, bool overwriteExisting = false) throw (class Exception);
		bool removeFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale = MpqFile::Locale::Neutral, BOOST_SCOPED_ENUM(MpqFile::Platform) platform = MpqFile::Platform::Default);
		/**
		 * Path of MPQ file \p mpqFile should be set if you use this method.
		 * \param mpqFile An MPQ file is searched which has the same hash value as \p mpqFile.
		 * \return Returns true if an MPQ file was found and deleted successfully.
		 */
		bool removeFile(const class MpqFile &mpqFile);

		HashPtr findHash(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale = MpqFile::Locale::Neutral, BOOST_SCOPED_ENUM(MpqFile::Platform) platform = MpqFile::Platform::Default);
		HashPtr findHash(const MpqFile &file);
		FilePtr findFile(const boost::filesystem::path &path, BOOST_SCOPED_ENUM(MpqFile::Locale) locale = MpqFile::Locale::Neutral, BOOST_SCOPED_ENUM(MpqFile::Platform) platform = MpqFile::Platform::Default) throw (Exception);
		FilePtr findFile(const class MpqFile &mpqFile) throw (Exception);

		/**
		 * This function is reserved for internal uses since it gets a non-constant parameter.
		 * \param mpqFile This file has to be contained by the MPQ archive and is completely deleted if function returns true (also its memory!). Note that the parameter has to be a reference of an MPQ file pointer and is set to 0.
		 * \return Returns true if the MPQ file was deleted successfully.
		 */
		bool removeFile(class MpqFile *&mpqFile);

		class Listfile* listfileFile();
		class Attributes* attributesFile();
		class Signature* signatureFile();

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
		/**
		 * Usually the sector size has type uint16 and is computed by using formula:
		 * pow(2, sectorSizeShift) * 512. Instead of computing it every time we save
		 * the computed result and use uint32.
		 * \sa Mpq::sectorSizeShift
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
		 * \return Returns archive's strong digital signature with size of \ref Mpq::strongDigitalSignatureSize.
		 */
		const StrongDigitalSignature& strongDigitalSignature() const;
		/**
		 * \return Returns true if there is no kind of signature or if stored signatures are correct.
		 * \sa sign(), checkStrong(), strongDigitalSignature(), signatureFile()
		 */
		bool check(const CryptoPP::RSA::PrivateKey &strongPrivateKey, const CryptoPP::RSA::PrivateKey &weakPrivateKey) const;
		/**
		 * Updates all existing signatures.
		 * \sa check(), signStrong(), strongDigitalSignature(), signatureFile()
		 */
		void sign(const CryptoPP::RSA::PublicKey &strongPublicKey, const CryptoPP::RSA::PublicKey &weakPublicKey);
		/**
		 * \param digest Digest is reset by a newly generated SHA1 digest with size of \ref CryptoPP::SHA1::DIGESTSIZE.
		 */
		void digest(SHA1Digest &digest) const;
		/**
		 * \param digest Digest is reset by a newly generated SHA1 digest with size of \ref CryptoPP::SHA1::DIGESTSIZE.
		 * \throw Exception Throws an exception if there is no strong digital signature.
		 */
		void storedDigest(SHA1Digest &digest, const CryptoPP::RSA::PrivateKey &privateKey) const;
		bool checkStrong(const CryptoPP::RSA::PrivateKey &privateKey) const;
		/**
		 * \param signature Signature is reset by a newly created strong signature with size of \ref strongDigitalSignatureSize which contains a 2048-bit (strong) RSA key encrypted SHA1 digest.
		 * \sa digest()
		 */
		void signStrong(StrongDigitalSignature &signature, const CryptoPP::RSA::PublicKey &publicKey) const;
		/**
		 * Explicity signs the archive with a strong signature.
		 * \return Returns the number of written bytes which usually should be \ref strongDigitalSignatureSize + signature header size.
		 * \sa sign()
		 */
		std::streamsize signStrong(const CryptoPP::RSA::PublicKey &publicKey);
		/**
		 * When \ref Mpq::open() or \ref Mpq::create() is called archive is opened automatically until destructor or \ref Mpq::close() is called.
		 * For interprocess communication a file lock object is created which can be used via \ref Mpq::fileLock().
		 */
		bool isOpen() const;
		/**
		 * \param storeSectors If this value becomes true all sector tables are loaded immediately in this function.
		 * \note It is recommended to set this value to false when only extracting many files e. g. since it heavily reduces memory usage but slows down sector meta data access at later time (e. g. when reading file data again). Besides you should consider that you won't have access to the sector information via member functions of class \ref MpqFile at all when setting this value to false.
		 */
		void setStoreSectors(bool storeSectors) throw (Exception);
		/**
		 * \return If this function returns true, all sector meta data (offset, size etc.) is stored in heap which consumes much memory but increases sector data access performance. Sector data is loaded for all found non-encrypted files when opening the archive and their paths are not known. For encrypted files it is loaded when \ref findHash() detects the file's corresponding path as their sector table can only be read when knowing the file's path.
		 * \note Default value for newly created archives is \ref defaultStoreSectors.
		 */
		bool storeSectors() const;
		const boost::interprocess::file_lock& fileLock();
		Blocks& blocks();
		const Blocks& blocks() const;
		Hashes& hashes();
		const Hashes& hashes() const;
		Files& files();
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
		virtual class Hash* newHash(uint32 index) throw ();
		virtual class Block* newBlock(uint32 index) throw ();

		/**
		 * Does not check if archive is open.
		 * \sa Mpq::close
		 */
		void clear();

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
		std::streampos m_strongDigitalSignaturePosition; // position of strong digital signature in archive starting at its header ('NGIS')!
		BOOST_SCOPED_ENUM(Format) m_format;
		uint32 m_sectorSize;
		StrongDigitalSignature m_strongDigitalSignature;
		bool m_isOpen;
		bool m_storeSectors;
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

inline std::streamsize Mpq::strongDigitalSignature(istream &istream, StrongDigitalSignature &signature) throw (class Exception)
{
	istream.seekg(sizeof(uint32), std::ios::cur); // skip header
	StrongDigitalSignature sig(new char[strongDigitalSignatureSize]);
	std::streamsize size = 0;
	wc3lib::read(istream, sig[0], size, strongDigitalSignatureSize);
	signature.swap(sig);

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

inline bool Mpq::containsListfileFile() const
{
	return this->listfileFile() != 0;
}

inline const class Listfile* Mpq::createListfileFile()
{
	if (this->containsListfileFile())
		return this->listfileFile();

	Hash *hash = firstEmptyHash();
	hash->hashData().setLocale(MpqFile::Locale::Neutral);
	hash->hashData().setPlatform(MpqFile::Platform::Default);
	hash->changePath("(listfile)");
	// TODO get empty block etc. add file ...
	FilePtr file(new Listfile(this, hash));
	this->m_files.get<0>().push_back(file);
	boost::polymorphic_cast<Listfile*>(file.get())->refresh();
	boost::polymorphic_cast<Listfile*>(file.get())->writeData();

	return boost::polymorphic_cast<Listfile*>(file.get());
}

inline const class Listfile* Mpq::listfileFile() const
{
	return const_cast<Mpq*>(this)->listfileFile();
}


inline bool Mpq::containsAttributesFile() const
{
	return this->attributesFile() != 0;
}

inline const class Attributes* Mpq::createAttributesFile(BOOST_SCOPED_ENUM(Attributes::ExtendedAttributes) extendedAttributes)
{
	if (this->containsAttributesFile())
		return attributesFile();

	Hash *hash = firstEmptyHash();
	hash->hashData().setLocale(MpqFile::Locale::Neutral);
	hash->hashData().setPlatform(MpqFile::Platform::Default);
	hash->changePath("(attributes)");
	// TODO get empty block etc. add file ...
	FilePtr file(new Attributes(this, hash, extendedAttributes));
	this->m_files.get<0>().push_back(file);
	boost::polymorphic_cast<Attributes*>(file.get())->refresh();
	boost::polymorphic_cast<Attributes*>(file.get())->writeData();

	return boost::polymorphic_cast<Attributes*>(file.get());
}

inline const class Attributes* Mpq::attributesFile() const
{
	return const_cast<Mpq*>(this)->attributesFile();
}

inline bool Mpq::containsSignatureFile() const
{
	return this->signatureFile() != 0;
}

inline const class Signature* Mpq::signatureFile() const
{
	return const_cast<Mpq*>(this)->signatureFile();
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

inline const Mpq::StrongDigitalSignature& Mpq::strongDigitalSignature() const
{
	return this->m_strongDigitalSignature;
}

inline bool Mpq::isOpen() const
{
	return this->m_isOpen;
}

inline void Mpq::setStoreSectors(bool storeSectors) throw (Exception)
{
	m_storeSectors = storeSectors;

	BOOST_FOREACH(const FilePtr &ptr, files())
		ptr->read(); // read sector table
}

inline bool Mpq::storeSectors() const
{
	return this->m_storeSectors;
}

inline const boost::interprocess::file_lock& Mpq::fileLock()
{
	return m_fileLock;
}

inline Mpq::Blocks& Mpq::blocks()
{
	return this->m_blocks;
}

inline const Mpq::Blocks& Mpq::blocks() const
{
	return this->m_blocks;
}

inline Mpq::Hashes& Mpq::hashes()
{
	return this->m_hashes;
}

inline const Mpq::Hashes& Mpq::hashes() const
{
	return this->m_hashes;
}

inline Mpq::Files& Mpq::files()
{
	return this->m_files;
}

inline const Mpq::Files& Mpq::files() const
{
	return this->m_files;
}

inline Mpq::LargeSizeType Mpq::entireBlockSize() const
{
	Mpq::LargeSizeType result = 0;

	BOOST_FOREACH(const BlockPtr &block, this->blocks())
		result += block->blockSize();

	return result;
}

inline Mpq::LargeSizeType Mpq::entireUsedBlockSize() const
{
	Mpq::LargeSizeType result = 0;

	BOOST_FOREACH(const BlockPtr &block, this->blocks())
	{
		if (!block->empty())
			result += block->blockSize();
	}

	return result;
}

inline Mpq::LargeSizeType Mpq::entireEmptyBlockSize() const
{
	Mpq::LargeSizeType result = 0;

	BOOST_FOREACH(const BlockPtr &block, this->blocks())
	{
		if (block->empty())
			result += block->blockSize();
	}

	return result;
}

inline Mpq::LargeSizeType Mpq::entireFileSize() const
{
	Mpq::LargeSizeType result = 0;

	BOOST_FOREACH(const BlockPtr &block, this->blocks())
	{
		// usually size should be 0 if it's not a file but anyway we should check
		if (block->flags() & Block::Flags::IsFile)
			result += block->fileSize();
	}

	return result;
}

inline class Listfile* Mpq::listfileFile()
{
	FilePtr file = this->findFile("(listfile)");

	if (file.get() == 0)
		return 0;

	return boost::polymorphic_cast<class Listfile*>(file.get());
}

inline class Attributes* Mpq::attributesFile()
{
	FilePtr file = this->findFile("(attributes)");

	if (file.get() == 0)
		return 0;

	return boost::polymorphic_cast<class Attributes*>(file.get());
}

inline class Signature* Mpq::signatureFile()
{
	FilePtr file = this->findFile("(signature)");

	if (file.get() == 0)
		return 0;

	return boost::polymorphic_cast<class Signature*>(file.get());
}

inline class Block* Mpq::firstEmptyBlock() const
{
	BOOST_FOREACH(const BlockPtr &block, this->blocks())
	{
		if (block->empty())
			return block.get();
	}

	return 0;
}

inline class Block* Mpq::firstUnusedBlock() const
{
	BOOST_FOREACH(const BlockPtr &block, this->blocks())
	{
		if (block->unused())
			return block.get();
	}

	return 0;
}

inline class Block* Mpq::lastOffsetBlock() const
{
	uint64 offset = 0;
	class Block *result = 0;

	BOOST_FOREACH(const BlockPtr &block, this->blocks())
	{
		const uint64 newOffset = block->largeOffset();

		if (newOffset > offset)
		{
			result = block.get();
			offset = newOffset;
		}
	}

	return result;
}


inline class Hash* Mpq::firstEmptyHash() const
{
	BOOST_FOREACH(const HashPtr &hash, this->hashes())
	{
		if (hash->empty())
			return hash.get();
	}

	return 0;
}

inline class Hash* Mpq::firstDeletedHash() const
{
	BOOST_FOREACH(const HashPtr &hash, this->hashes())
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
