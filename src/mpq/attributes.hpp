/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MPQ_ATTRIBUTES_HPP
#define WC3LIB_MPQ_ATTRIBUTES_HPP

#include <boost/bimap.hpp>

#include "mpqfile.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * <a href="http://wiki.devklog.net/index.php?title=The_MoPaQ_Archive_Format#Extended_Attributes">Source</a>
 * The extended attributes are optional file attributes for files in the block table. These attributes were added at times after the MoPaQ format was already finalized, and it is not necessary for every archive to have all (or any) of the extended attributes. If an archive contains a given attribute, there will be an instance of that attribute for every block in the block table, although the attribute will be meaningless if the block is not a file. The order of the attributes for blocks correspond to the order of the blocks in the block table, and are of the same number. The attributes are stored in parallel arrays in the "(attributes)" file (default language and platform), in the archive. The attributes corresponding to this file need not be valid (and logically cannot be). Unlike all the other structures in the MoPaQ format, entries in the extended attributes are NOT guaranteed to be aligned. Also note that in some archives, malicious zeroing of the attributes has been observed, perhaps with the intent of
breaking archive viewers.
 * \todo Create common class for file based formats which do have latest file version and file name (like \ref map::FileFormat) since this class does have these attributes.
 * \todo MD5 implementation requires library Crypto++ but should only use Boost/not require an extra dependency.
 * \sa Listfile
 * \sa Signature
 */
class Attributes : public MpqFile
{
	public:
		BOOST_SCOPED_ENUM_START(ExtendedAttributes) /// \todo C++11 : uint32
		{
			None = 0x0,
			FileCrc32s = 0x00000001,
			FileTimeStamps = 0x00000002,
			FileMd5s = 0x00000004
		};
		BOOST_SCOPED_ENUM_END

		class Exception : public wc3lib::Exception
		{
			public:
				Exception(BOOST_SCOPED_ENUM(ExtendedAttributes) extendedAttributes, const std::string &what = "") throw ();
				Exception(BOOST_SCOPED_ENUM(ExtendedAttributes) extendedAttributes, const boost::format &what) throw ();

				BOOST_SCOPED_ENUM(ExtendedAttributes) extendedAttributes() const;

			private:
				BOOST_SCOPED_ENUM(ExtendedAttributes) m_extendedAttributes;
		};

		static const int32 latestVersion = 100;

		typedef std::vector<CRC32> Crc32s;
		typedef std::vector<FILETIME> FileTimes;
		typedef std::vector<MD5> Md5s;

		const Crc32s& crc32s() const;
		const FileTimes& fileTimes() const;
		const Md5s& md5s() const;

		virtual void removeData();

		/**
		 * Refreshs extended file attributes of file \p mpqFile.
		 * \note Attributes are refreshed in heap/this structure only. For synchronizing data in the corresponding MPQ archive call \ref writeData() afterwards.
		 */
		void refreshFile(const MpqFile *mpqFile);
		/**
		 * Refreshes extended file attributes of all files in archive.
		 * \note Attributes are refreshed in heap/this structure only. For synchronizing data in the corresponding MPQ archive call \ref writeData() afterwards.
		 */
		void refresh();

		/**
		 * Reads data from corresponding file "(attributes)" of the archive and stores attributes into containers.
		 */
		std::streamsize readData();
		/**
		 * Writes data into corresponding file "(attributes)" of the archive.
		 */
		std::streamsize writeData();

		/**
		 * Checks file data via used checksum methods (\ref Mpq::ExtendedAttributes::FileCrc32s and \ref Mpq::ExtendedAttributes::FileMd5s).
		 * If none is used check's result is always positive.
		 * \return Returns true if checksums are correct or there isn't any checksum method.
		 */
		bool check(const MpqFile *mpqFile) const;
		/**
		 * Checks all files of the archive via used checksum methods (\ref Mpq::ExtendedAttributes::FileCrc32s and \ref Mpq::ExtendedAttributes::FileMd5s).
		 * If none is used check's result is always positive.
		 * \return Returns true if checksums are correct or there isn't any checksum method.
		 */
		bool check() const;

		/**
		 * \ref latestVersion contains the latest format version.
		 */
		virtual uint32_t version() const;
		BOOST_SCOPED_ENUM(ExtendedAttributes) extendedAttributes() const;
		bool hasChecksums() const;

		CRC32 crc32(const MpqFile *mpqFile) const;
		const FILETIME& fileTime(const MpqFile *mpqFile) const;
		MD5 md5(const MpqFile *mpqFile) const;

		/**
		 * \throw Exception Is thrown if CRC32s aren't stored in "(attributes)"
		 */
		CRC32 crc32(const Block *block) const;
		/**
		 * \throw Exception Is thrown if time stamps aren't stored in "(attributes)"
		 */
		const FILETIME& fileTime(const Block *block) const;
		/**
		 * \throw Exception Is thrown if MD5s aren't stored in "(attributes)"
		 */
		MD5 md5(const Block *block) const;

		virtual const char* fileName() const;

	protected:
		friend class Mpq;

		Attributes(Mpq *mpq, Hash *hash, BOOST_SCOPED_ENUM(ExtendedAttributes) extendedAttributes, uint32 version = latestVersion);

		void setVersion(uint32 version);
		void setExtendedAttributes(BOOST_SCOPED_ENUM(ExtendedAttributes) extendedAttributes);
		void setCrc32(const Block *block, CRC32 crc32);
		void setFileTime(const Block *block, const FILETIME &fileTime);
		void setMd5(const Block *block, MD5 md5);

		Crc32s& crc32s();
		FileTimes& fileTimes();
		Md5s& md5s();

	private:
		uint32 m_version;
		BOOST_SCOPED_ENUM(ExtendedAttributes) m_extendedAttributes;
		Crc32s m_crc32s;
		FileTimes m_fileTimes;
		Md5s m_md5s;
};

inline const Attributes::Crc32s& Attributes::crc32s() const
{
	return this->m_crc32s;
}

inline const Attributes::FileTimes& Attributes::fileTimes() const
{
	return this->m_fileTimes;
}

inline const Attributes::Md5s& Attributes::md5s() const
{
	return this->m_md5s;
}

inline Attributes::Exception::Exception(BOOST_SCOPED_ENUM(Attributes::ExtendedAttributes) extendedAttributes, const std::string &what) throw () : m_extendedAttributes(extendedAttributes), wc3lib::Exception(what)
{
}

inline Attributes::Exception::Exception(BOOST_SCOPED_ENUM(Attributes::ExtendedAttributes) extendedAttributes, const boost::format &what) throw () : m_extendedAttributes(extendedAttributes), wc3lib::Exception(what)
{
}

inline BOOST_SCOPED_ENUM(Attributes::ExtendedAttributes) Attributes::Exception::extendedAttributes() const
{
	return this->m_extendedAttributes;
}

inline bool Attributes::hasChecksums() const
{
	return extendedAttributes() & ExtendedAttributes::FileCrc32s || extendedAttributes() & ExtendedAttributes::FileMd5s;
}

inline uint32_t Attributes::version() const
{
	return this->m_version;
}

inline BOOST_SCOPED_ENUM(Attributes::ExtendedAttributes) Attributes::extendedAttributes() const
{
	return m_extendedAttributes;
}

inline CRC32 Attributes::crc32(const MpqFile *mpqFile) const
{
	return crc32(mpqFile->block());
}

inline const FILETIME& Attributes::fileTime(const MpqFile *mpqFile) const
{
	return fileTime(mpqFile->block());
}

inline MD5 Attributes::md5(const MpqFile *mpqFile) const
{
	return md5(mpqFile->block());
}

inline CRC32 Attributes::crc32(const Block *block) const
{
	if (!(extendedAttributes() & ExtendedAttributes::FileCrc32s))
		throw Exception(ExtendedAttributes::FileCrc32s);

	return crc32s()[block->index()];
}


inline const FILETIME& Attributes::fileTime(const Block *block) const
{
	if (!(extendedAttributes() & ExtendedAttributes::FileTimeStamps))
		throw Exception(ExtendedAttributes::FileTimeStamps);

	return fileTimes()[block->index()];
}


inline MD5 Attributes::md5(const Block *block) const
{
	if (!(extendedAttributes() & ExtendedAttributes::FileMd5s))
		throw Exception(ExtendedAttributes::FileMd5s);

	return md5s()[block->index()];
}

inline const char* Attributes::fileName() const
{
	return "(attributes)";
}

inline void Attributes::setVersion(uint32 version)
{
	this->m_version = version;
}

inline void Attributes::setExtendedAttributes(BOOST_SCOPED_ENUM(Attributes::ExtendedAttributes) extendedAttributes)
{
	this->m_extendedAttributes = extendedAttributes;
}

inline void Attributes::setCrc32(const Block *block, CRC32 crc32)
{
	this->crc32s()[block->index()] = crc32;
}

inline void Attributes::setFileTime(const Block *block, const FILETIME &fileTime)
{
	this->fileTimes()[block->index()] = fileTime;
}

inline void Attributes::setMd5(const Block* block, MD5 md5)
{
	this->md5s()[block->index()] = md5;
}

inline Attributes::Crc32s& Attributes::crc32s()
{
	return this->m_crc32s;
}

inline Attributes::FileTimes& Attributes::fileTimes()
{
	return this->m_fileTimes;
}

inline Attributes::Md5s& Attributes::md5s()
{
	return this->m_md5s;
}


}

}

#endif
