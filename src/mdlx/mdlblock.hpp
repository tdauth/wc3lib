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

#error mdlblock.hpp is deprecated. Use mdxblock.hpp instead!
#ifndef WC3LIB_MDLX_MDLBLOCK_HPP
#define WC3LIB_MDLX_MDLBLOCK_HPP

#include "../exception.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL blocks start with their identifier followed by a bracked indicating the block's scope.
 * \sa MdxBlock, MdlValueBlock
 */
class MdlBlock
{
	public:
		MdlBlock(const string &mdlIdentifier, bool optional = true);
		virtual ~MdlBlock();

		const string& mdlIdentifier() const;
		bool optional() const;
		bool exists() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);

	protected:
		const string m_mdlIdentifier;
		const bool m_optional;
		bool m_exists;
};

inline const string& MdlBlock::mdlIdentifier() const
{
	return this->m_mdlIdentifier;
}

inline bool MdlBlock::optional() const
{
	return this->m_optional;
}

inline bool MdlBlock::exists() const
{
	return this->m_exists;
}

}

}

#endif
