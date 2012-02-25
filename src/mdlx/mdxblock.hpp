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

#ifndef WC3LIB_MDLX_MDXBLOCK_HPP
#define WC3LIB_MDLX_MDXBLOCK_HPP

#include "platform.hpp"
#include "mdlxproperty.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * \todo Rename to MdlxBlock since it includes MDL support by now.
 */
class MdxBlock : public MdlxProperty
{
	public:
		static const std::size_t mdxIdentifierSize = 4;

		MdxBlock(const byte mdxIdentifier[mdxIdentifierSize], const string &mdlKeyword, bool optional = true);
		virtual ~MdxBlock();

		/**
		 * \return Returns the block name with size \ref MdxBlock::blockNameSize.
		 */
		const byte* mdxIdentifier() const;
		const string& mdlKeyword() const;
		/**
		 * Some MDX blocks such as translations, scalings and rotations are optional and do only appear if they're not inherited by parent object.
		 * \return Returns true if the MDX block is optional which means it doesn't appear in output stream.
		 */
		bool optional() const;
		bool exists() const;

		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);

		bool moveToMdxIdentifier(istream &istream) const;

	protected:
		byte m_mdxIdentifier[mdxIdentifierSize];
		string m_mdlKeyword;
		bool m_optional;
		bool m_exists;
};

inline const byte* MdxBlock::mdxIdentifier() const
{
	return this->m_mdxIdentifier;
}

inline const string& MdxBlock::mdlKeyword() const
{
	return m_mdlKeyword;
}

inline bool MdxBlock::optional() const
{
	return this->m_optional;
}

inline bool MdxBlock::exists() const
{
	return this->m_exists;
}

}

}

#endif
