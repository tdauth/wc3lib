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

#include <vector>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "platform.hpp"
#include "block.hpp"
#include "sector.hpp"

namespace wc3lib
{

namespace mpq
{

class Hash;
class Mpq;

/**
 * \brief Provides access to a file's data which is actually a combination of its block and hash entries as well as its path which is provided by the optional "(listfile)" file and its extended attributes provided by the optional "(attributes)" file.
 *
 * \note MpqFile uses mutex locking and unlocking whenever data is being changed or read. It calls \ref boost::mutex::try_lock (immediate lock) in that case so make sure data is not locked at that moment, otherwise it throws an exception.
 *
 * \note Only class \ref Mpq can modify a file since it is responsible for holding all files.
 */
class MpqFile : private boost::noncopyable
{
	public:
		/// \todo Define all locales. <a href="http://wiki.devklog.net/index.php?title=The_MoPaQ_Archive_Format#Locales">Source</a>.
		enum class Locale : uint16
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

		/// There seems to be only one platform.
		enum class Platform : uint16
		{
			Default
		};

		/**
		 * The content of a file is divided into one or more sectors.
		 * These sectors only have to be read or written when reading or writing the file's content.
		 * Therefore we do not need a heap allocated container.
		 */
		typedef std::vector<Sector> Sectors;

		/**
		 * Reads data from input stream \p istream and hence overwrites old file data.
		 * \p compression Compression which is used for all added sectors.
		 * \return Returns size of read data.
		 * \throws Exception Usually thrown when there is not enough space in file's corresponding block. Throws an exception if file is locked as well.
		 */
		virtual std::streamsize readData(istream &istream, Sector::Compression compression = Sector::Compression::Uncompressed) throw (class Exception);
		/**
		 * Writes uncompressed file data into output stream \p ostream.
		 * \return Returns size of written data.
		 */
		virtual std::streamsize writeData(ostream &ostream) throw (class Exception);
		/**
		 * Same as \ref writeData(ostream &) but doesn't work independently since it expects to be at the correct position in archive using \p istream as input archive stream.
		 */
		virtual std::streamsize writeData(istream &istream, ostream &ostream) throw (Exception);

		/**
		 * \todo Implement removal of all data from the block which should mark hash as deleted and clear the block and should be synchronized with the archive.
		 */
		virtual void removeData();

		/**
		 * Reads the file sectors' meta data.
		 * Reads from input stream \p istream which should contain the MPQ archive and stores the result in \p sectors.
		 *
		 * \return Returns the read size in bytes.
		 *
		 * \throws Exception Throws an exception if file is locked.
		 */
		std::streamsize sectors(istream &istream, Sectors &sectors) throw (class Exception);
		/**
		 * Same as \ref read(istream &) but opens MPQ archive for reading sector table.
		 */
		std::streamsize sectors(Sectors &sectors) throw (class Exception);
		/**
		 * Writes the file sectors' meta data.
		 */
		std::streamsize writeSectors(ostream &ostream, const Sectors &sectors) const throw (class Exception);

		// hash attributes
		Locale locale() const;
		Platform platform() const;

		/**
		 * \return Returns the archive which the file does belong into.
		 */
		Mpq* mpq() const;
		/**
		 * \return Returns the corresponding hash which belongs to the file.
		 *
		 * \note The data must fit to file path \ref path().
		 */
		Hash* hash() const;
		/**
		 * \return Returns file's corresponding block (same as hash()->block()).
		 */
		Block* block() const;

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

		/**
		 * Compressed sectors (only found in compressed - not imploded - files) are compressed with one or more compression algorithms, and have the following structure:
		 * \ref byte CompressionMask : Mask of the compression types applied to this sector.
		 * byte(SectorSize - 1) SectorData : The compressed data for the sector.
		 */
		bool hasSectorOffsetTable() const;

		static uint16 localeToInt(Locale locale);
		static Locale intToLocale(uint16 value);
		static uint16 platformToInt(Platform platform);
		static Platform intToPlatform(uint16 value);

	protected:
		friend Mpq;
		friend void boost::checked_delete<>(MpqFile*);
		friend void boost::checked_delete<>(MpqFile const*);
		friend std::auto_ptr<MpqFile>;

		/**
		 * MPQ files are created by \ref Mpq only.
		 * \param path Initial path which is set without any synchronization of the corresponding hash entry.
		 */
		MpqFile(Mpq *mpq, Hash *hash, const boost::filesystem::path &path);
		virtual ~MpqFile();

		/**
		 * Sets the path of the file to \p path.
		 * Updates the hash data as well since it is always associated with the actual path entry.
		 * If no hash is associated nothing is updated at all.
		 * \todo This can only be done by the class Mpq since it is not synchronized at the moment!!!!
		 */
		void changePath(const boost::filesystem::path &path);

		/**
		 * Overwrite this member function to return custom type-based objects if you want to extend their functionality.
		 * \sa Mpq::newBlock()
		 * \sa Mpq::newHash()
		 * \sa Mpq::newFile()
		 */
		virtual Sector newSector(uint32 index, uint32 offset, uint32 size) throw ();

	private:
		Mpq *m_mpq;
		Hash *m_hash;
		boost::filesystem::path m_path;
};

inline Mpq* MpqFile::mpq() const
{
	return this->m_mpq;
}

inline Hash* MpqFile::hash() const
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
	{
		return pathString;
	}
	else if (pos == pathString.size() - 1)
	{
		return "";
	}

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

inline bool MpqFile::hasSectorOffsetTable() const
{
	return !(this->block()->flags() & Block::Flags::IsSingleUnit) && ((this->block()->flags() & Block::Flags::IsCompressed) || (this->block()->flags() & Block::Flags::IsImploded));
}

inline uint16 MpqFile::localeToInt(MpqFile::Locale locale)
{
	return static_cast<uint16>(locale);
}

inline MpqFile::Locale MpqFile::intToLocale(uint16 value)
{
	return (MpqFile::Locale)(value);
}

inline uint16 MpqFile::platformToInt(MpqFile::Platform platform)
{
	return static_cast<uint16>(platform);
}

inline MpqFile::Platform MpqFile::intToPlatform(uint16 value)
{
	return static_cast<MpqFile::Platform>(value);
}

}

}

#endif
