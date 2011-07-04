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

#ifndef WC3LIB_MPQ_MPQFILE_HPP
#define WC3LIB_MPQ_MPQFILE_HPP

#include <istream>
#include <ostream>
#include <list>
#include <sstream>

#include <boost/filesystem.hpp>

#include "platform.hpp"
#include "../exception.hpp"
#include "block.hpp"
#include "hash.hpp"

namespace wc3lib
{

namespace mpq
{
	
class Mpq;
class Sector;

/**
* @brief Abstract class for mpq file access. Combines hash and block table data of file.
*/
class MpqFile
{
	public:
		/// @todo Define all locales. @link http://wiki.devklog.net/index.php?title=The_MoPaQ_Archive_Format#Locales
		enum Locale
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
		enum Platform
		{
			Default
		};

		std::streamsize readData(std::istream &istream) throw (class Exception);
		/**
		* Reads data from stream @param istream and appends it to the already existing file data.
		* @return Returns the number of read bytes.
		*/
		std::streamsize appendData(std::istream &istream) throw (class Exception);
		std::streamsize writeData(std::ostream &ostream) const throw (class Exception);

		// hash attributes
		enum Locale locale() const;
		enum Platform platform() const;

		class Mpq* mpq() const;
		class Hash* hash() const;
		/**
		* @return Returns file's corresponding block (same as hash()->block()).
		*/
		class Block* block() const;
		/**
		* @return Returns the file path. Note that MPQ archives without list file don't have any information about the file paths.
		*/
		const boost::filesystem::path& path() const;
		
		// block attributes
		uint32 fileKey() const;
		/// @return Reurns file size in bytes.
		int32 size() const;
		/// @return Returns compressed file size in bytes.
		int32 compressedSize() const;
		bool isFile() const;
		bool isEncrypted() const;
		bool isCompressed() const;
		bool isImploded() const;
		
		// extended attributes
		CRC32 crc32() const;
		void setFileTime(const time_t &time);
		const struct FILETIME& fileTime() const;
		bool fileTime(time_t &time) const;
		MD5 md5() const;
		
		const std::list<class Sector*>& sectors() const;

		/**
		* Appends data of file @param mpqFile.
		*/
		class MpqFile& operator<<(const class MpqFile &mpqFile) throw (class Exception);
		/**
		* Adds a copy of the file to MPQ archive @param mpq.
		* @note Does not overwrite existing files.
		*/
		class MpqFile& operator>>(class Mpq &mpq) throw (class Exception);
		/**
		* Appends data of the file to file @param mpqFile.
		*/
		class MpqFile& operator>>(class MpqFile &mpqFile) throw (class Exception);

	protected:
		friend class Mpq;
		
		static int16 localeToInt(enum Locale locale);
		static enum Locale intToLocale(int16 value);
		static int8 platformToInt(enum Platform platform);
		static enum Platform intToPlatform(int8 value);
		
		/**
		* MPQ files are created by @class Mpq only.
		*/
		MpqFile(class Mpq *mpq, class Hash *hash);
		~MpqFile();

		/**
		* Reads the file sector meta data.
		*/
		std::streamsize read(std::istream &istream) throw (class Exception);
		/**
		* Writes the file sector meta data.
		*/
		std::streamsize write(std::ostream &ostream) const throw (class Exception);
		
		/**
		* Remove file (clears file hash and block data which frees the file's used space).
		*/
		void remove() throw (class Exception);
		/**
		* @return Returns false if the file could not be renamed or if it does already have to specified name.
		*/
		bool rename(const std::string &newName, bool overwriteExisting = false) throw (class Exception);
		bool move(const boost::filesystem::path &newPath, bool overwriteExisting = false) throw (class Exception);
		
		class Mpq *m_mpq;
		class Hash *m_hash;
		boost::filesystem::path m_path;
		std::list<class Sector*> m_sectors;
};

inline enum MpqFile::Locale MpqFile::locale() const
{
	return MpqFile::intToLocale(this->m_hash->locale());
}

inline enum MpqFile::Platform MpqFile::platform() const
{
	return MpqFile::intToPlatform(this->m_hash->platform());
}

inline class Mpq* MpqFile::mpq() const
{
	return this->m_mpq;
}

inline class Hash* MpqFile::hash() const
{
	return this->m_hash;
}

inline class Block* MpqFile::block() const
{
	return this->m_hash->block();
}

inline const boost::filesystem::path& MpqFile::path() const
{
	return this->m_path;
}

inline uint32 MpqFile::fileKey() const
{
	return this->block()->fileKey(this->path());
}

inline int32 MpqFile::size() const
{
	return this->block()->fileSize();
}

/// @todo FIXME (return compressed and not real size)
inline int32 MpqFile::compressedSize() const
{
	return this->block()->blockSize();
}

inline bool MpqFile::isFile() const
{
	return this->block()->flags() & Block::IsFile;
}

inline bool MpqFile::isEncrypted() const
{
	return this->block()->flags() & Block::IsEncrypted;
}

inline bool MpqFile::isCompressed() const
{
	return this->block()->flags() & Block::IsCompressed;
}

inline bool MpqFile::isImploded() const
{
	return this->block()->flags() & Block::IsImploded;
}

inline CRC32 MpqFile::crc32() const
{
	return this->block()->crc32();
}

inline void MpqFile::setFileTime(const time_t &time)
{
	this->block()->setFileTime(time);
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

inline const std::list<class Sector*>& MpqFile::sectors() const
{
	return this->m_sectors;
}

inline int16 MpqFile::localeToInt(enum MpqFile::Locale locale)
{
	return int16(locale);
}

inline enum MpqFile::Locale MpqFile::intToLocale(int16 value)
{
	return MpqFile::Locale(value);
}

inline int8 MpqFile::platformToInt(enum MpqFile::Platform platform)
{
	return int8(platform);
}

inline enum MpqFile::Platform MpqFile::intToPlatform(int8 value)
{
	return MpqFile::Platform(value);
}

/**
* Appends data from input stream @param istream to file @param mpqFile.
*/
inline std::istream& operator>>(std::istream &istream, class MpqFile &mpqFile) throw (class Exception)
{
	mpqFile.appendData(istream);

	return istream;
}

/**
* Writes data of the file @param mpqFile into output stream @param ostream.
*/
inline std::ostream& operator<<(std::ostream &ostream, const class MpqFile &mpqFile) throw (class Exception)
{
	mpqFile.writeData(ostream);

	return ostream;
}

}

}

#endif
