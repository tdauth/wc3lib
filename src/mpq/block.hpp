/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MPQ_BLOCK_HPP
#define WC3LIB_MPQ_BLOCK_HPP

#include <boost/detail/scoped_enum_emulation.hpp>

#include "../format.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * Each file in an MPQ archive has exactly one corresponding block which holds all of its content.
 * Blocks are divided into sectors (\ref Sector).
 * There is some properties which are equal for all contained sectors which can be accessed via \ref flags()
 * The MPQ archive's block table is limited to a size of 2^32 (\ref uint32) - 2 since the last two values are reserved by \ref Hash::blockIndexDeleted and \ref Hash::blockIndexEmpty.
 * Use \ref Mpq::maxBlockId to get the last valid block id.
 * \sa Sector
 * \sa Hash
 * \sa MpqFile
 */
class Block : public Format, private boost::noncopyable
{
	public:
		BOOST_SCOPED_ENUM_START(Flags) /// \todo C++11 : uint32
		{
			None = 0x0,
			IsFile = 0x80000000,
			IsSingleUnit = 0x01000000,
			UsesEncryptionKey = 0x00020000, /// Synonym for "BLOCK_OFFSET_ADJUSTED_KEY" which means that the file's key depends on its block offset and file size. \sa fileKey()
			IsEncrypted = 0x00010000, /// In encrypted files each file sector (\ref Sector) has to be decrypted by its custom sector key which is the sum of its corresponding file key and its index starting at 0. \sa fileKey(), MpqFile::fileKey(), Sector::sectorKey()
			IsCompressed = 0x00000200,
			IsImploded = 0x00000100
		};
		BOOST_SCOPED_ENUM_END

		/**
		 * \sa MpqFile::name()
		 */
		static uint32 fileKey(const string &name, const BlockTableEntry &blockTableEntry);

		Block(class Mpq *mpq, uint32 index);
		virtual ~Block();

		std::streamsize read(istream &istream) throw (class Exception);
		std::streamsize write(ostream &ostream) const throw (class Exception);
		virtual uint32_t version() const { return 0; }

		bool empty() const;
		bool unused() const;
		/**
		 * Combines values from \ref blockOffset() and \ref extendedBlockOffset() and returns the result.
		 * \note This is only required when \ref Mpq::Format::MPQ2 or higher is used.
		 */
		uint64 largeOffset() const;

		class Mpq* mpq() const;
		uint32 index() const;
		void setBlockOffset(uint32 blockOffset);
		/**
		 * Offset from the archive's beginning (including header) where the block's data starts.
		 * Since MPQ2 format extended offsets are support which allow larger archives.
		 * \sa extendedBlockOffset()
		 * \sa largeOffset()
		 */
		uint32 blockOffset() const;
		void setExtendedBlockOffset(uint32 extendedBlockOffset);
		/**
		 * \ref Mpq::Format::MPQ2 supports extended offsets by adding a \ref uint16 value which contains the \ref uint64's more significant bits.
		 * Use \ref largeOffset() to get the whole offset combination.
		 * \sa largeOffset()
		 * \sa blockOffset()
		 */
		uint16 extendedBlockOffset() const;
		void setBlockSize(uint32 blockSize);
		/**
		 * \return Returns the actual required block size in bytes which might be smaller than \ref fileSize() if file is compressed.
		 */
		uint32 blockSize() const;
		void setFileSize(uint32 fileSize);
		/**
		 * \return Returns the actual uncompressed file size in bytes which might be bigger than \ref blockSize() if file is compressed.
		 */
		uint32 fileSize() const;
		void setFlags(BOOST_SCOPED_ENUM(Flags) flags);
		BOOST_SCOPED_ENUM(Flags) flags() const;
		class MpqFile* file() const;
		// extended attributes
		/**
		 * \return Returns corresponding \ref CRC32 checksum stored in "(attributes)" file of the archive.
		 * \throw Attributes::Exception Is thrown if CRC32s aren't stored in "(attributes)" file.
		 */
		CRC32 crc32() const;
		/**
		 * \return Returns corresponding \ref FILETIME time stamp stored in "(attributes)" file of the archive.
		 * \throw Attributes::Exception Is thrown if time stamps aren't stored in "(attributes)" file.
		 */
		const struct FILETIME& fileTime() const;
		/**
		 * \return Returns corresponding converted \ref FILETIME time stamp stored in "(attributes)" file of the archive.
		 * \throw Attributes::Exception Is thrown if time stamps aren't stored in "(attributes)" file.
		 */
		bool fileTime(time_t &time);
		/**
		 * \return Returns corresponding \ref MD5 checksum stored in "(attributes)" file of the archive.
		 * \throw Attributes::Exception Is thrown if MD5s aren't stored in "(attributes)" file.
		 */
		MD5 md5() const;

		/**
		 * \sa MpqFile::name()
		 */
		uint32 fileKey(const string &fileName) const;

	protected:
		friend class Mpq;
		friend class Hash;

		class Mpq *m_mpq;
		uint32 m_index;
		uint32 m_blockOffset;
		uint16 m_extendedBlockOffset;
		uint32 m_blockSize;
		uint32 m_fileSize;
		BOOST_SCOPED_ENUM(Flags) m_flags;
		class MpqFile *m_file;
};

inline bool Block::empty() const
{
	return this->m_blockOffset > 0 && this->m_blockSize > 0 && this->m_fileSize == 0 && this->m_flags == Block::Flags::None;
}

inline bool Block::unused() const
{
	return this->m_blockSize == 0 && this->m_fileSize == 0 && this->m_flags == Block::Flags::None;
}

inline uint64 Block::largeOffset() const
{
	return ((uint64)this->m_extendedBlockOffset << 32) + (uint64)this->m_blockOffset;
}

inline class Mpq* Block::mpq() const
{
	return this->m_mpq;
}

inline uint32 Block::index() const
{
	return m_index;
}

inline void Block::setBlockOffset(uint32 blockOffset)
{
	this->m_blockOffset = blockOffset;
}

inline uint32 Block::blockOffset() const
{
	return this->m_blockOffset;
}

inline void Block::setExtendedBlockOffset(uint32 extendedBlockOffset)
{
	this->m_extendedBlockOffset = extendedBlockOffset;
}

inline uint16 Block::extendedBlockOffset() const
{
	return this->m_extendedBlockOffset;
}

inline void Block::setBlockSize(uint32 blockSize)
{
	this->m_blockSize = blockSize;
}

inline uint32 Block::blockSize() const
{
	return this->m_blockSize;
}

inline void Block::setFileSize(uint32 fileSize)
{
	this->m_fileSize = fileSize;
}

inline uint32 Block::fileSize() const
{
	return this->m_fileSize;
}

inline void Block::setFlags(BOOST_SCOPED_ENUM(Flags) flags)
{
	this->m_flags = flags;
}

inline BOOST_SCOPED_ENUM(Block::Flags) Block::flags() const
{
	return this->m_flags;
}

inline MpqFile *Block::file() const
{
	return this->m_file;
}

}

}

#endif
