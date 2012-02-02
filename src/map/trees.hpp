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

#include <boost/multi_index/random_access_index.hpp>

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
		typedef std::vector<TreePtr> TreeVector;

		struct Header2
		{
			int32 formatVersion;
			int32 specialNumber;
		};

		virtual std::streamsize read(InputStream& istream) throw (Exception);
		virtual std::streamsize write(OutputStream& ostream) const throw (Exception);

		virtual const char8* fileTextId() const;
		virtual const char8* fileName() const;
		virtual int32 latestFileVersion() const;
		virtual uint32_t version() const;

		int32 subVersion() const;
		TreeVector& trees();
		const TreeVector& trees() const;

	protected:
		int32 m_version;
		int32 m_subVersion;
		TreeVector m_trees;
};

inline const char8* Trees::fileTextId() const
{
	return "W3do";
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

inline Trees::TreeVector& Trees::trees()
{
	return this->m_trees;
}

inline const Trees::TreeVector& Trees::trees() const
{
	return this->m_trees;
}

}

}

#endif
