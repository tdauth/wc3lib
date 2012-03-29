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

#ifndef WC3LIB_MPQ_MPQFILE_HPP
#define WC3LIB_MPQ_MPQFILE_HPP

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/algorithm/string/find.hpp>

#include "platform.hpp"
#include "block.hpp"
#include "sector.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * \brief Abstract class for mpq file access. Combines hash and block table data of file.
 * MpqFile uses mutex locking and unlocking whenever data is being changed or read. It calls \ref boost::mutex::try_lock (immediate lock) in that case so make sure data is not locked at that moment, otherwise it throws an exception.
 */
class MpqFile : private boost::noncopyable
{
	public:
		/// \todo Define all locales. <a href="http://wiki.devklog.net/index.php?title=The_MoPaQ_Archive_Format#Locales">Source</a>.
		BOOST_SCOPED_ENUM_START(Locale) /// \todo C++11 : uint16
		{
			Neutral = 0x0,
			Chinese = 0x404,
			Czech = 0x405,
			German = 0x407,
			English = 0x409,
			Spanish = 0x40a,
			French = 0x40c,
			Italian = 0x410,
			Japanese = 0x411,
			Korean = 0x412,
			Dutch = 0x413,
			Polish = 0x415,
			Portuguese = 0x416,
			Russsian = 0x419,
			EnglishUK = 0x809
		};
		BOOST_SCOPED_ENUM_END

		/// There seems to be only one platform.
		BOOST_SCOPED_ENUM_START(Platform) /// \todo C++11 : uint16
		{
			Default
		};
		BOOST_SCOPED_ENUM_END

		typedef boost::shared_ptr<Sector> SectorPtr;
		typedef
		boost::multi_index_container<SectorPtr,
		boost::multi_index::indexed_by<
		// ordered by its corresponding sector table index
		boost::multi_index::ordered_unique<boost::multi_index::tag<uint32>, boost::multi_index::const_mem_fun<Sector, uint32, &Sector::sectorIndex> >
		>
		>
		Sectors;

		/**
		 * Removes all data from file.
		 * Clears corresponding block file size.
		 * Deletes all corresponding data sectors.
		 * \throws Exception Throws an exception if file is locked.
		 */
		virtual void removeData();
		/**
		 * Reads data from input stream \p istream and hence overwrites old file data.
		 * \p compression Compression which is used for all added sectors.
		 * \return Returns size of read data.
		 * \throws Exception Usually thrown when there is not enough space in file's corresponding block. Throws an exception if file is locked as well.
		 */
		virtual std::streamsize readData(istream &istream, BOOST_SCOPED_ENUM(Sector::Compression) compression = Sector::Compression::Uncompressed) throw (class Exception);
		/**
		 * Reads data from stream \p istream and appends it to the already existing file data.
		 * \return Returns the number of read bytes.
		 * \throws Exception Throws an exception if file is locked.
		 */
		virtual std::streamsize appendData(istream &istream) throw (class Exception);
		/**
		 * Writes uncompressed file data into output stream \p ostream.
		 * \return Returns size of written data.
		 */
		virtual std::streamsize writeData(ostream &ostream) const throw (class Exception);
		/**
		 * Same as \ref writeData(ostream &) but doesn't work independently since it expects to be at the correct position in archive using \p istream as input archive stream.
		 */
		virtual std::streamsize writeData(istream &istream, ostream &ostream) const throw (Exception);

		// hash attributes
		BOOST_SCOPED_ENUM(Locale) locale() const;
		BOOST_SCOPED_ENUM(Platform) platform() const;

		class Mpq* mpq() const;
		class Hash* hash() const;
		/**
		 * \return Returns file's corresponding block (same as hash()->block()).
		 */
		class Block* block() const;
		/**
		 * \return Returns the file path. Note that MPQ archives without list file don't have any information about the file paths.
		 * \note The path has usually a Windows format. For platform safety use \ref nativePath().
		 * \sa name()
		 * \sa nativePath()
		 */
		const boost::filesystem::path& path() const;
		/**
		 * \sa path()
		 * \sa toNativePath()
		 */
		std::string name() const;
		/**
		 * \sa Listfile::toNativePath()
		 */
		boost::filesystem::path nativePath() const;

		// block attributes
		/**
		 * \return Returns the file's key calculated by its \ref name().
		 * \sa Block::fileKey()
		 */
		uint32 fileKey() const;
		/**
		 * \return Returns uncompressed file size in bytes.
		 * \sa Block::fileSize()
		 */
		uint32 size() const;
		/**
		 * \return Returns compressed file size in bytes.
		 * \sa Block::blockSize()
		 */
		uint32 compressedSize() const;
		bool isFile() const;
		bool isEncrypted() const;
		bool isCompressed() const;
		bool isImploded() const;

		// extended attributes
		/**
		 * \return Returns true if checksums are correct or there isn't any stored checksums.
		 * \sa Attributes::check()
		 */
		bool check() const;
		CRC32 crc32() const;
		const struct FILETIME& fileTime() const;
		bool fileTime(time_t &time) const;
		MD5 md5() const;

		const Sectors& sectors() const;
		/**
		 * If \ref Mpq::storeSectors() is false this returns all found sectors anyway since it reads them directly from the MPQ archive.
		 * Useful for getting detailed file information.
		 */
		Sectors realSectors() const throw (Exception);

		/**
		 * Compressed sectors (only found in compressed - not imploded - files) are compressed with one or more compression algorithms, and have the following structure:
		 * \ref byte CompressionMask : Mask of the compression types applied to this sector.
		 * byte(SectorSize - 1) SectorData : The compressed data for the sector.
		 */
		bool hasSectorOffsetTable() const;

		static uint16 localeToInt(BOOST_SCOPED_ENUM(Locale) locale);
		static BOOST_SCOPED_ENUM(Locale) intToLocale(uint16 value);
		static uint16 platformToInt(BOOST_SCOPED_ENUM(Platform) platform);
		static BOOST_SCOPED_ENUM(Platform) intToPlatform(uint16 value);

		/**
		* Appends data of file \p mpqFile.
		*/
		class MpqFile& operator<<(const class MpqFile &mpqFile) throw (class Exception);
		/**
		* Adds a copy of the file to MPQ archive \p mpq.
		* @note Does not overwrite existing files.
		*/
		class MpqFile& operator>>(class Mpq &mpq) throw (class Exception);
		/**
		* Appends data of the file to file \p mpqFile.
		*/
		class MpqFile& operator>>(class MpqFile &mpqFile) throw (class Exception);

	protected:
		friend class Mpq;

		/**
		 * Overwrite this member function to return custom type-based objects if you want to extend their functionality.
		 * \sa Mpq::newBlock()
		 */
		virtual class Sector* newSector(uint32 index, uint32 offset, uint32 size) throw ();

		/**
		* MPQ files are created by @class Mpq only.
		*/
		MpqFile(class Mpq *mpq, class Hash *hash);

		template<class T>
		friend void boost::checked_delete(T*); // for destruction by shared ptr
		virtual ~MpqFile();

		/**
		 * Reads the file sectors' meta data.
		 * \throws Exception Throws an exception if file is locked.
		 */
		std::streamsize read(istream &istream) throw (class Exception);
		/**
		 * Same as \ref read(istream &) but opens MPQ archive for reading sector table.
		 */
		std::streamsize read() throw (class Exception);
		/**
		 * Writes the file sectors' meta data.
		 */
		std::streamsize write(ostream &ostream) const throw (class Exception);

		/**
		 * Remove file (clears file hash and block data which frees the file's used space).
		 */
		void remove() throw (class Exception);
		/**
		 * \return Returns false if the file could not be renamed or if it does already have to specified name.
		 */
		bool rename(const std::string &newName, bool overwriteExisting = false) throw (class Exception);
		bool move(const boost::filesystem::path &newPath, bool overwriteExisting = false) throw (class Exception);

		class Mpq *m_mpq;
		class Hash *m_hash;
		boost::filesystem::path m_path;
		Sectors m_sectors;
		boost::mutex m_mutex; // for MPQ archive
};

inline class Mpq* MpqFile::mpq() const
{
	return this->m_mpq;
}

inline class Hash* MpqFile::hash() const
{
	return this->m_hash;
}

inline const boost::filesystem::path& MpqFile::path() const
{
	return this->m_path;
}

inline std::string MpqFile::name() const
{
	const std::string pathString = path().string();
	const std::string::size_type pos = pathString.find_last_of("\\");

	if (pos == std::string::npos)
		return pathString;
	else if (pos == pathString.size() - 1)
		return "";

	return pathString.substr(pos + 1);
}

inline boost::filesystem::path MpqFile::nativePath() const
{
#ifdef UNIX
	string pathString = path().string();
	// (listfile) entries usually have Windows path format
	boost::algorithm::replace_all(pathString, "\\", "/");

	return boost::filesystem::path(pathString);
#else
	return path();
#endif
}

inline uint32 MpqFile::fileKey() const
{
	return this->block()->fileKey(this->name());
}

inline uint32 MpqFile::size() const
{
	return this->block()->fileSize();
}

/// @todo FIXME (return compressed and not real size)
inline uint32 MpqFile::compressedSize() const
{
	return this->block()->blockSize();
}

inline bool MpqFile::isFile() const
{
	return this->block()->flags() & Block::Flags::IsFile;
}

inline bool MpqFile::isEncrypted() const
{
	return this->block()->flags() & Block::Flags::IsEncrypted;
}

inline bool MpqFile::isCompressed() const
{
	return this->block()->flags() & Block::Flags::IsCompressed;
}

inline bool MpqFile::isImploded() const
{
	return this->block()->flags() & Block::Flags::IsImploded;
}

inline CRC32 MpqFile::crc32() const
{
	return this->block()->crc32();
}

inline const struct FILETIME& MpqFile::fileTime() const
{
	return this->block()->fileTime();
}

inline bool MpqFile::fileTime(time_t &time) const
{
	return this->block()->fileTime(time);
}

inline MD5 MpqFile::md5() const
{
	return this->block()->md5();
}

inline const MpqFile::Sectors& MpqFile::sectors() const
{
	return this->m_sectors;
}

inline bool MpqFile::hasSectorOffsetTable() const
{
	return !(this->block()->flags() & Block::Flags::IsSingleUnit) && ((this->block()->flags() & Block::Flags::IsCompressed) || (this->block()->flags() & Block::Flags::IsImploded));
}

inline uint16 MpqFile::localeToInt(BOOST_SCOPED_ENUM(MpqFile::Locale) locale)
{
	return (uint16)(locale); // TODO boost::numeric_cast<
}

inline BOOST_SCOPED_ENUM(MpqFile::Locale) MpqFile::intToLocale(uint16 value)
{
	return BOOST_SCOPED_ENUM(MpqFile::Locale)(value);
}

inline uint16 MpqFile::platformToInt(BOOST_SCOPED_ENUM(MpqFile::Platform) platform)
{
	return (uint16)(platform); // TODO boost::numeric_cast<
}

inline BOOST_SCOPED_ENUM(MpqFile::Platform) MpqFile::intToPlatform(uint16 value)
{
	return BOOST_SCOPED_ENUM(MpqFile::Platform)(value);
}

/**
 * Appends data from input stream \p istream to file \p mpqFile.
 */
inline istream& operator>>(istream &istream, class MpqFile &mpqFile) throw (class Exception)
{
	mpqFile.appendData(istream);

	return istream;
}

/**
 * Writes data of the file \p mpqFile into output stream \p ostream.
 */
inline ostream& operator<<(ostream &ostream, const class MpqFile &mpqFile) throw (class Exception)
{
	mpqFile.writeData(ostream);

	return ostream;
}

}

}

#endif
