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

#ifndef WC3LIB_MAP_TREES_HPP
#define WC3LIB_MAP_TREES_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"
#include "tree.hpp"

namespace wc3lib
{

namespace map
{

/**
 * Trees are destructibles for Warcraft III: Reign of Chaos.
 * They can be accessed using \ref trees().
 * \sa Tree
 */
/// \todo Add read and write member functions, add TFT version -> TreesX.
class Trees : public FileFormat
{
	public:
		typedef boost::ptr_vector<Tree> TreeContainer;

		virtual std::streamsize read(InputStream& istream) override;
		virtual std::streamsize write(OutputStream& ostream) const override;

		virtual const byte* fileTextId() const override;
		virtual const byte* fileName() const override;
		virtual uint32 latestFileVersion() const override;
		virtual uint32 version() const override;

		uint32 subVersion() const;
		TreeContainer& trees();
		const TreeContainer& trees() const;

	protected:
		uint32 m_subVersion;
		TreeContainer m_trees;
};

inline const byte* Trees::fileTextId() const
{
	return "W3do";
}

inline const byte* Trees::fileName() const
{
	return "war3map.doo";
}

inline uint32 Trees::latestFileVersion() const
{
	return 7;
}

inline uint32 Trees::version() const
{
	return this->m_version;
}

inline uint32 Trees::subVersion() const
{
	return this->m_subVersion;
}

inline Trees::TreeContainer& Trees::trees()
{
	return this->m_trees;
}

inline const Trees::TreeContainer& Trees::trees() const
{
	return this->m_trees;
}

}

}

#endif
