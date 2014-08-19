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

#include <vector>

#include "mpqfile.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * \brief Class for MPQ's "(attributes)" file which stores extended file attributes such as time stamps and check sums.
 *
 * This class provides useful functions for accessing a file's corresponding attributes as well as refreshing/changing it.
 * Use \ref attributes() to get the stored extendend attributes from the file.
 * Use \ref writeAttributes() to update the extended attributes.
 *
 * <a href="http://wiki.devklog.net/index.php?title=The_MoPaQ_Archive_Format#Extended_Attributes">Source</a>
 * The extended attributes are optional file attributes for files in the block table. These attributes were added at times after the MoPaQ format was already finalized, and it is not necessary for every archive to have all (or any) of the extended attributes. If an archive contains a given attribute, there will be an instance of that attribute for every block in the block table, although the attribute will be meaningless if the block is not a file. The order of the attributes for blocks correspond to the order of the blocks in the block table, and are of the same number. The attributes are stored in parallel arrays in the "(attributes)" file (default language and platform), in the archive. The attributes corresponding to this file need not be valid (and logically cannot be). Unlike all the other structures in the MoPaQ format, entries in the extended attributes are NOT guaranteed to be aligned. Also note that in some archives, malicious zeroing of the attributes has been observed, perhaps with the intent of
breaking archive viewers.
 *
 * \todo Create common class for file based formats which do have latest file version and file name (like \ref map::FileFormat) since this class does have these attributes.
 * \todo MD5 implementation requires library Crypto++ but should only use Boost/not require an extra dependency.
 *
 * \sa Listfile
 * \sa Signature
 */
class Attributes : public MpqFile
{
	public:
		/**
		 * All types of attributes which can be stored for files.
		 */
		enum class ExtendedAttributes : uint32
		{
			None = 0x0,
			FileCrc32s = 0x00000001,
			FileTimeStamps = 0x00000002,
			FileMd5s = 0x00000004
		};

		static const int32 latestVersion = 100;

		/**
		 * All extended attributes are stored in a simple vector.
		 *
		 * @{
		 */
		typedef std::vector<CRC32> Crc32s;
		typedef std::vector<FILETIME> FileTimes;
		typedef std::vector<MD5> Md5s;
		/**
		 * @}
		 */

		/**
		 * @{
		 * Calculates checksum from given data.
		 * \param data Data from which checksum should be calculated.
		 * \param dataSize Size of given data.
		 * \return Returns the value of the calculated checksum
		 */
		static CRC32 crc32(const byte *data, std::size_t dataSize);
		static MD5 md5(const byte *data, std::size_t dataSize);
		/**@}*/

		/**
		 * Checks \p data of size \p dataSize with the given checksum and returns
		 * if the data matches the checksum.
		 *
		 * @{
		 */
		static bool checkCrc(const byte *data, std::size_t dataSize, CRC32 crc);
		static bool checkMd5(const byte *data, std::size_t dataSize, MD5 md5);
		/**
		 * @}
		 */


		/**
		 * Empties extended attributes.
		 * Sets them to \ref ExtendedAttributes::None and clears the lists.
		 */
		virtual void removeData() override;

		std::streamsize readHeader(istream &stream);


		/**
		 * \brief Reads the attributes from the MPQ archive intro the three parameters.
		 */
		std::streamsize attributes(Crc32s &crcs, FileTimes &fileTimes, Md5s &md5s);

		/**
		 * Writes data into corresponding file "(attributes)" of the archive.
		 * \param extenedAttributes Defines which attributes are actuall used.
		 */
		std::streamsize writeAttributes(int32 version, ExtendedAttributes extenedAttributes, const Crc32s &crcs = Crc32s(), const FileTimes &fileTimes = FileTimes(), const Md5s &md5s = Md5s());

		/**
		 * \ref latestVersion contains the latest format version.
		 */
		virtual uint32_t version() const;
		ExtendedAttributes extendedAttributes() const;

		virtual const char* fileName() const;

	protected:
		friend Mpq;

		Attributes(Mpq *mpq, Hash *hash);

		/*
		 * Header information:
		 */
		int32 m_version;
		ExtendedAttributes m_extendedAttributes;
};

inline constexpr bool operator&(Attributes::ExtendedAttributes x, Attributes::ExtendedAttributes y)
{
	return static_cast<bool>(static_cast<uint32>(x) & static_cast<uint32>(y));
}

inline uint32_t Attributes::version() const
{
	return this->m_version;
}

inline Attributes::ExtendedAttributes Attributes::extendedAttributes() const
{
	return this->m_extendedAttributes;
}

inline const char* Attributes::fileName() const
{
	return "(attributes)";
}

}

}

#endif
