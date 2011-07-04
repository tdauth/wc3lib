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

#include "platform.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * Simple base class for generating hash comparisons (can be used in combination with unordered types from Boost C++ Libraries as in \ref mpq::Mpq::Hashes)
 */
class HashData
{
	public:
		HashData(int32 filePathHashA, int32 filePathHashB, int16 locale, int16 platfom);
		void setFilePathHashA(int32 filePathHashA);
		int32 filePathHashA() const;
		void setFilePathHashB(int32 filePathHashB);
		int32 filePathHashB() const;
		void setLocale(int16 locale);
		int16 locale() const;
		void setPlatform(int16 platform);
		int16 platform() const;
		
		bool isHash(int32 nameHashA, int32 nameHashB, int16 locale, int16 platform) const;
		
		/**
		 * Compares both hash values of both hashes.
		 * \return Returns true if hash values A and B are equal, otherwise false is being returned.
		 */
		bool operator==(const HashData &hash) const;
		
	protected:
		int32 m_filePathHashA;
		int32 m_filePathHashB;
		int16 m_locale;
		int16 m_platform;
};

inline void HashData::setFilePathHashA(int32 filePathHashA)
{
	m_filePathHashA = filePathHashA;
}

inline int32 HashData::filePathHashA() const
{
	return this->m_filePathHashA;
}

inline void HashData::setFilePathHashB(int32 filePathHashB)
{
	m_filePathHashB = filePathHashB;
}

inline int32 HashData::filePathHashB() const
{
	return this->m_filePathHashB;
}

inline void HashData::setLocale(int16 locale)
{
	m_locale = locale;
}

inline int16 HashData::locale() const
{
	return this->m_locale;
}

inline void HashData::setPlatform(int16 platform)
{
	m_platform = platform;
}

inline int16 HashData::platform() const
{
	return this->m_platform;
}

inline bool HashData::operator==(const HashData &hashData) const
{
	return this->isHash(hashData.m_filePathHashA, hashData.m_filePathHashB, hashData.m_locale, hashData.m_platform);
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


class Hash : public Format
{
	public:
		Hash(class Mpq *mpq);
		
		std::streamsize read(istream &istream) throw (class Exception);
		std::streamsize write(ostream &ostream) const throw (class Exception) { throw Exception(_("Do not use write member function of class Hash.")); }
		virtual uint32_t version() const { return 0; }
		
		void clear();

		/**
		 * Updates both hash values to new file path \p path.
		 * \todo Might be protected and only be used by class \ref MpqFile. Use \ref MpqFile::move for changing file path.
		 * \sa Hash::filePathHashA, Hash::filePathHashB
		 */
		void changePath(const boost::filesystem::path &path);

		bool check() const;
		/**
		 * \return Returns true if the hash table entry has been deleted.
		 * \sa Hash::empty
		 */
		bool deleted() const;
		/**
		 * Hash table entries are empty if they do not link to any block table entry.
		 * Consider that a hash table entry can not be empty when it's been deleted.
		 * \return Returns true if the hash table entry has never been used.
		 * \sa Hash::deleted
		 */
		bool empty() const;
		
		class Mpq* mpq() const;
		uint32 index() const;
		const HashData& hashData() const;
		HashData& hashData();
		class MpqFile* mpqFile() const;
		class Block* block() const;
		
	protected:
		friend class Mpq;
		friend class MpqFile;
		
		static const uint32 blockIndexDeleted;
		static const uint32 blockIndexEmpty;
		
		class Mpq *m_mpq;
		uint32 m_index;
		struct HashData m_hashData;
		class MpqFile *m_mpqFile;
		class Block *m_block; // if this value is 0 it has never been used
		bool m_deleted; // can not be true if m_block is 0
};

inline bool Hash::check() const
{
	return true;
}

inline bool Hash::deleted() const
{
	return this->m_deleted;
}

inline bool Hash::empty() const
{
	return !this->m_deleted && this->m_block == 0;
}

inline class Mpq* Hash::mpq() const
{
	return this->m_mpq;
}

inline uint32 Hash::index() const
{
	return m_index;
}

inline const HashData& Hash::hashData() const
{
	return m_hashData;
}

inline HashData& Hash::hashData()
{
	return m_hashData;
}

inline class MpqFile* Hash::mpqFile() const
{
	return this->m_mpqFile;
}

inline class Block* Hash::block() const
{
	return this->m_block;
}

}

}

#endif
