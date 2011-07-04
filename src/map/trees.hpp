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

namespace wc3lib
{

namespace map
{

/// \todo Add read and write member functions, add TFT version -> TreesX.
class Trees : public FileFormat
{
	public:
		struct Header
		{
			id fileId;
			int32 version;
			int32 subversion;
			int32 treeNumber;
		};

		struct Header2
		{
			int32 formatVersion;
			int32 specialNumber;
		};
		
		virtual int32 fileId() const;
		virtual const char8* fileName() const;
		virtual int32 latestFileVersion() const;

	protected:
		boost::bimap<id, class Tree> m_trees;
};

inline int32 Trees::fileId() const
{
	return (int32)"W3do";
}

inline const char8* Trees::fileName() const
{
	return "war3map.doo";
}

inline int32 Trees::latestFileVersion() const
{
	return 7;
}

}

}

#endif
