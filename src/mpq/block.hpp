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

#include "../format.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace mpq
{

class Archive;

/**
 * \brief Blocks are used to provide data for a file in an MPQ archive or empty or deletion regions in the archive which can be used for new blocks.
 *
 * Each file in an MPQ archive has exactly one corresponding block which holds all of its content.
 * Blocks are divided into sectors (\ref Sector).
 * There is some properties which are equal for all contained sectors which can be accessed via \ref flags()
 * The MPQ archive's block table is limited to a size of 2^32 (\ref uint32) - 2 since the last two values are reserved by \ref Hash::blockIndexDeleted and \ref Hash::blockIndexEmpty.
 * Use \ref Archive::maxBlockId to get the last valid block id.
 *
 *
 * \note All the block's data is kept private. Only the class \ref Archive can modify it since it is responsible for holding all blocks.
 *
 * \sa Sector
 * \sa Hash
 * \sa File
 */
class Block : public Format
{
	public:
		/**
		 * Each block has flags which set the type of the block.
		 * They can be accessed using \ref flags()
		 */
		enum class Flags : uint32
		{
			None = 0x0,
			IsFile = 0x80000000,
			IsSingleUnit = 0x01000000,
			UsesEncryptionKey = 0x00020000, /// Synonym for "BLOCK_OFFSET_ADJUSTED_KEY" which means that the file's key depends on its block offset and file size. \sa fileKey()
			IsEncrypted = 0x00010000, /// In encrypted files each file sector (\ref Sector) has to be decrypted by its custom sector key which is the sum of its corresponding file key and its index starting at 0. \sa fileKey(), File::fileKey(), Sector::sectorKey()
			IsCompressed = 0x00000200,
			IsImploded = 0x00000100
		};

		/**
		 * Calculates the hash key for the given file name \p fileName using block \p blockTableEntry.
		 * If the file's block is encrypted (\ref Flags::UsesEncryptionKey) it also has to be decrypted.
		 *
		 * \sa File::name()
		 * \sa Block::fileKey(const string&)
		 */
		static uint32 fileKey(const string &name, const BlockTableEntry &blockTableEntry);

		static bool hasSectorOffsetTable(Flags flags);

		/**
		 * Copy constructor.
		 */
		Block(const Block &other);

		std::streamsize write(ostream &ostream) const;

		/**
		 * Makes the block empty and clears all values.
		 * After this call \ref empty() returns true.
		 */
		void remove();

		bool empty() const;
		bool unused() const;
		/**
		 * Combines values from \ref blockOffset() and \ref extendedBlockOffset() and returns the result.
		 * \note This is only required when \ref Archive::Format::Mpq2 or higher is used.
		 */
		uint64 largeOffset() const;

		uint32 index() const;

		/**
		 * Offset from the archive's beginning (including header) where the block's data starts.
		 * Since MPQ2 format extended offsets are support which allow larger archives.
		 * \sa extendedBlockOffset()
		 * \sa largeOffset()
		 */
		uint32 blockOffset() const;

		/**
		 * \ref Archive::Format::Mpq2 supports extended offsets by adding a \ref uint16 value which contains the \ref uint64's more significant bits.
		 * Use \ref largeOffset() to get the whole offset combination.
		 * \sa largeOffset()
		 * \sa blockOffset()
		 */
		uint16 extendedBlockOffset() const;

		/**
		 * \return Returns the actual required block size in bytes which might be smaller than \ref fileSize() if file is compressed.
		 */
		uint32 blockSize() const;

		/**
		 * \return Returns the actual uncompressed file size in bytes which might be bigger than \ref blockSize() if file is compressed.
		 */
		uint32 fileSize() const;
		Flags flags() const;

		/**
		 * Calculates the hash key for the given file name \p fileName.
		 * If the file's block is encrypted (\ref Flags::UsesEncryptionKey) it also has to be decrypted.
		 *
		 * \sa File::name()
		 * \sa Block::fileKey(const string&, const BlockTableEntry&)
		 */
		uint32 fileKey(const string &fileName) const;

		/**
		 * \return Returns the corresponding block table entry which is written into the block table
		 * of an MPQ archive.
		 */
		BlockTableEntry toBlockTableEntry() const;

	protected:
		friend Archive;
		friend void boost::checked_delete<>(Block*);
		friend void boost::checked_delete<>(Block const*);
		friend std::auto_ptr<Block>;

		/**
		 * The constructor has to fill the block data which cannot be read directly from a \ref BlockTableEntry.
		 *
		 * \note The extended block offset have to be set using \ref setExtendedBlockOffset().
		 */
		Block(uint32 index);
		virtual ~Block();

		std::streamsize read(istream &istream);

		void setFileSize(uint32 fileSize);
		void setBlockSize(uint32 blockSize);
		void setExtendedBlockOffset(uint16 extendedBlockOffset);
		void setBlockOffset(uint32 blockOffset);
		void setFlags(Flags flags);

	private:
		uint32 m_index;
		uint32 m_blockOffset;
		uint16 m_extendedBlockOffset;
		uint32 m_blockSize;
		uint32 m_fileSize;
		Flags m_flags;
};

inline constexpr bool operator&(Block::Flags x, Block::Flags y)
{
	return static_cast<bool>(static_cast<uint32>(x) & static_cast<uint32>(y));
}

inline bool Block::hasSectorOffsetTable(Block::Flags flags)
{
	return !(flags & Block::Flags::IsSingleUnit) && ((flags & Block::Flags::IsCompressed) || (flags & Block::Flags::IsImploded));
}


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

inline void Block::setExtendedBlockOffset(uint16 extendedBlockOffset)
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

inline void Block::setFlags(Flags flags)
{
	this->m_flags = flags;
}

inline Block::Flags Block::flags() const
{
	return this->m_flags;
}

inline BlockTableEntry Block::toBlockTableEntry() const
{
	BlockTableEntry entry;
	entry.blockOffset = this->blockOffset();
	entry.blockSize = this->blockSize();
	entry.fileSize = this->fileSize();
	entry.flags = static_cast<int32>(this->flags());

	return entry;
}

}

}

#endif
