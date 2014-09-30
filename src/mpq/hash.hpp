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

#ifndef WC3LIB_MPQ_HASH_HPP
#define WC3LIB_MPQ_HASH_HPP

#include <boost/functional/hash.hpp>

#include "platform.hpp"
#include "file.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * Simple base class for generating hash comparisons (can be used in combination with unordered types from Boost C++ Libraries as in \ref mpq::Archive:Hashes)
 * \sa hash_value()
 * \sa Hash
 */
class HashData : public boost::operators<HashData>
{
	public:
		HashData(int32 filePathHashA, int32 filePathHashB, uint16 locale, uint16 platfom);
		HashData(const boost::filesystem::path &path, File::Locale locale = File::Locale::Neutral, File::Platform platform = File::Platform::Default);
		HashData(const HashTableEntry &entry);

		HashData(const HashData &other);
		HashData& operator=(const HashData &other);
		HashData& operator=(const HashTableEntry &entry);
		HashTableEntry toEntry() const;

		void setFilePathHashA(int32 filePathHashA);
		int32 filePathHashA() const;
		void setFilePathHashB(int32 filePathHashB);
		int32 filePathHashB() const;
		void setLocale(uint16 locale);
		void setLocale(File::Locale locale);
		uint16 locale() const;
		void setPlatform(uint16 platform);
		void setPlatform(File::Platform platform);
		uint16 platform() const;

		bool isHash(int32 nameHashA, int32 nameHashB, uint16 locale, uint16 platform) const;
		bool isHash(const boost::filesystem::path &path, File::Locale locale = File::Locale::Neutral, File::Platform platform = File::Platform::Default) const;

		/**
		 * Compares both hash values of both hashes.
		 * \return Returns true if hash values A and B are equal, otherwise false is being returned.
		 */
		bool operator==(const HashData &hash) const;
		bool operator==(const boost::filesystem::path &path) const;

	protected:
		int32 m_filePathHashA;
		int32 m_filePathHashB;
		uint16 m_locale;
		uint16 m_platform;
};

inline void HashData::setFilePathHashA(int32 filePathHashA)
{
	this->m_filePathHashA = filePathHashA;
}

inline int32 HashData::filePathHashA() const
{
	return this->m_filePathHashA;
}

inline void HashData::setFilePathHashB(int32 filePathHashB)
{
	this->m_filePathHashB = filePathHashB;
}

inline int32 HashData::filePathHashB() const
{
	return this->m_filePathHashB;
}

inline void HashData::setLocale(uint16 locale)
{
	this->m_locale = locale;
}

inline void HashData::setLocale(File::Locale locale)
{
	this->m_locale = static_cast<uint16>(locale);
}

inline uint16 HashData::locale() const
{
	return this->m_locale;
}

inline void HashData::setPlatform(uint16 platform)
{
	this->m_platform = platform;
}

inline void HashData::setPlatform(File::Platform platform)
{
	this->m_platform = static_cast<uint16>(platform);
}

inline uint16 HashData::platform() const
{
	return this->m_platform;
}

inline bool HashData::operator==(const HashData &hashData) const
{
	return this->isHash(hashData.m_filePathHashA, hashData.m_filePathHashB, hashData.m_locale, hashData.m_platform);
}

inline bool HashData::operator==(const boost::filesystem::path &path) const
{
	return this->isHash(path);
}


inline std::size_t hash_value(const HashData &hashData)
{
	std::size_t seed = 0;
	boost::hash_combine(seed, hashData.filePathHashA());
	boost::hash_combine(seed, hashData.filePathHashB());
	boost::hash_combine(seed, hashData.locale());
	boost::hash_combine(seed, hashData.platform());

	return seed;
}

class Archive;
class Block;

/**
 * \brief Provides access to the hash entry of one single \ref File of an archive.
 *
 * MPQ format supports file hashing which helps you to find files. Therefore hash values are generated by a file's path (\ref File::path()), locale (\ref File::locale()) and platform (\ref File::platform()). For its path there are stored two different hash values generated via method \ref HashType::NameA and \ref HashType::NameB.
 * While this class refers to one single hash table entry for one corresponding file, class \ref HashData provides the actual hashing functionality.
 * The two classes are separated because oftenly you have to generate hash values without creating a hash table entry (for example when you're searching for a file using some of its attributes).
 * Hash instances do always have one corresponding \ref HashData object which can be accessed via \ref hashData().
 * Like \ref Block instances each Hash instance belongs to one single \ref File instance.
 *
 * \note Modifying a hash can only be done by the class \ref Archive which is responsible for holding all hashes.
 *
 * \sa Block
 * \sa File
 */
class Hash : public Format, private boost::noncopyable
{
	public:
		static const uint32 blockIndexDeleted;
		static const uint32 blockIndexEmpty;

		std::streamsize write(ostream &ostream) const;

		/**
		 * Clears the hash entry and writes it back to the archive.
		 */
		virtual void removeData();

		/**
		 * \return Returns true if the hash table entry has been deleted.
		 * \sa empty()
		 */
		bool deleted() const;
		/**
		 * Hash table entries are empty if they do not link to any block table entry.
		 * Consider that a hash table entry can not be empty when it's been deleted.
		 * \return Returns true if the hash table entry has never been used.
		 * \sa deleted()
		 */
		bool empty() const;

		/**
		 * \return Returns the corresponding MPQ archive which holds the block table from which a block can be assigned to the hash.
		 */
		  Archive* mpq() const;

		uint32 index() const;
		/**
		 * \return Returns the corresponding hash data which holds the actual data for hashing.
		 *
		 * @{
		 */
		const HashData& cHashData() const;
		/**
		 * @}
		 */

		/**
		 * \return Returns the corresponding block. Each hash entry has one corresponding block entry or it is deleted or empty.
		 */
		Block* block() const;

	protected:
		friend Archive;
		friend void boost::checked_delete<>(Hash*);
		friend void boost::checked_delete<>(Hash const*);
		friend std::auto_ptr<Hash>;

		/**
		 * The constructor has to be filled with the corresponding index of the hash entry
		 * which cannot be read directly from a \ref HashTableEntry.
		 * The corresponding block is set to 0 by default and deleted to "false".
		 *
		 * \param mpq The corresponding MPQ archive to which teh hash belongs to. The archive is used for finding the corresponding block.
		 * \param index The corresponding index of the hash table entry.
		 *
		 * \note Other data such as the hash entry (\ref hashData()) and the corresponding block entry (\ref block()) is filled on reading the data from the MPQ archive using \ref read().
		 */
		Hash(Archive *mpq, uint32 index);
		virtual ~Hash();

		/**
		 * Reads all hashing data into the class.
		 * \param index Assigns the index to the hash entry.
		 */
		std::streamsize read(istream &istream);

		friend File; // TODO remove when changePath() is public

		/**
		 * Updates both hash values to new file path \p path.
		 * \todo Might be protected and only be used by class \ref File. Use \ref File::move for changing file path.
		 * \sa HashData::filePathHashA, HashData::filePathHashB
		 *
		 * \todo synchronize data and then make public
		 */
		void changePath(const boost::filesystem::path &path);

		HashData& hashData();

	private:
		Archive *m_mpq;
		uint32 m_index;
		HashData m_hashData;
		Block *m_block; // if this value is 0 it has never been used
		bool m_deleted; // can not be true if m_block is 0
};

inline bool Hash::deleted() const
{
	return this->m_deleted;
}

inline bool Hash::empty() const
{
	return !this->m_deleted && this->m_block == 0;
}

inline Archive* Hash::mpq() const
{
	return m_mpq;
}

inline uint32 Hash::index() const
{
	return m_index;
}

inline const HashData& Hash::cHashData() const
{
	return m_hashData;
}

inline HashData& Hash::hashData()
{
	return m_hashData;
}

inline Block* Hash::block() const
{
	return this->m_block;
}

}

}

#endif
