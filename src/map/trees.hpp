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

#include "platform.hpp"
#include "tree.hpp"

namespace wc3lib
{

namespace map
{

/// \todo Add read and write member functions, add TFT version -> TreesX.
class Trees : public FileFormat
{
	public:
		typedef boost::shared_ptr<Tree> TreePtr;
		typedef boost::multi_index_container<TreePtr,
		boost::multi_index::indexed_by<
		// list
		boost::multi_index::sequenced<>,
		// ordered by its corresponding hash table index (like blocks)
		boost::multi_index::ordered_unique<boost::multi_index::tag<int32>, boost::multi_index::const_mem_fun<Tree, int32, &Tree::customId> >
		>
		>
		TreeContainer;

		struct Header2
		{
			int32 formatVersion;
			int32 specialNumber;
		};

		virtual std::streamsize read(InputStream& istream) throw (Exception);
		virtual std::streamsize write(OutputStream& ostream) const throw (Exception);

		virtual int32 fileId() const;
		virtual const char8* fileName() const;
		virtual int32 latestFileVersion() const;
		virtual uint32_t version() const;

		int32 subVersion() const;
		const TreeContainer& trees() const;

	protected:
		int32 m_version;
		int32 m_subVersion;
		TreeContainer m_trees;
};

inline int32 Trees::fileId() const
{
	return (int32)'W3do';
}

inline const char8* Trees::fileName() const
{
	return "war3map.doo";
}

inline int32 Trees::latestFileVersion() const
{
	return 7;
}

inline uint32_t Trees::version() const
{
	return this->m_version;
}

inline int32 Trees::subVersion() const
{
	return this->m_subVersion;
}

inline const Trees::TreeContainer& Trees::trees() const
{
	return this->m_trees;
}

}

}

#endif
