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

#ifndef WC3LIB_MDLX_GROUPMDXBLOCK_HPP
#define WC3LIB_MDLX_GROUPMDXBLOCK_HPP

#include <boost/ptr_container/ptr_list.hpp>

#include "mdxblock.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * Some MDX blocks are structured like:
 * byte[4]	MDX block name
 * long32 	group members count / group members bytes;
 * ...		group members list
 * This class provides a simple abstraction layer for those MDX block classes.
 * \todo Rename to GroupMdlxBlock
 */
class GroupMdxBlock : public MdxBlock
{
	public:
		/**
		 * \note We cannot use \ref boost::ptr_vector since blocks without counter can only estimate the number of members.
		 */
		typedef boost::ptr_list<class GroupMdxBlockMember> Members;

		/**
		 * \param mdxIdentifier The MDX tag of four characters. This parameter can be specified as string literal for simplified usage. It must contain exactly four characters.
		 */
		GroupMdxBlock(const string &mdxIdentifier, const string &mdlKeyword, bool usesCounter = true, bool optional = true, bool usesMdlCounter = false);
		virtual ~GroupMdxBlock();

		/**
		 * \return Returns true if members are stored by number of them. Otherwise their size in bytes is stored (exclusive byte count -> does not include its own size).
		 */
		bool usesCounter() const;
		/**
		 * \return Returns true if members are stored in a separate MDL block like "Sequences <n>". Otherwise members are listed like attachments, for instance.
		 */
		bool usesMdlCounter() const;
		Members& members();
		const Members& members() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		/**
		 * This method should be overwritten in child class.
		 * \return Returns a new allocated group member which will be added to list.
		 */
		virtual class GroupMdxBlockMember* createNewMember() = 0;

		bool m_usesCounter;
		bool m_usesMdlCounter;
		/**
		 * Provides access to all read members for child class.
		 */
		Members m_members;
};

inline bool GroupMdxBlock::usesCounter() const
{
	return m_usesCounter;
}

inline bool GroupMdxBlock::usesMdlCounter() const
{
	return m_usesMdlCounter;
}

inline GroupMdxBlock::Members& GroupMdxBlock::members()
{
	return m_members;
}

inline const GroupMdxBlock::Members& GroupMdxBlock::members() const
{
	return m_members;
}

}

}

#endif
