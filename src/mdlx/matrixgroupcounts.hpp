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

#ifndef WC3LIB_MDLX_MATRIXGROUPCOUNTS_HPP
#define WC3LIB_MDLX_MATRIXGROUPCOUNTS_HPP

#include "groupmdxblock.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDX tag "MTGC".
 * MDL tag "Groups".
 */
class MatrixGroupCounts : public GroupMdxBlock
{
	public:
		typedef std::list<class MatrixGroupCount*> GroupCounts;
		
		MatrixGroupCounts(class Geoset *geoset);
		virtual ~MatrixGroupCounts();

		class Geoset* geoset() const;
		const GroupCounts& matrixGroupCounts() const;

	protected:
		virtual class GroupMdxBlockMember* createNewMember();

		class Geoset *m_geoset;
};

inline class Geoset* MatrixGroupCounts::geoset() const
{
	return this->m_geoset;
}

inline const MatrixGroupCounts::GroupCounts& MatrixGroupCounts::matrixGroupCounts() const
{
	return *reinterpret_cast<const GroupCounts*>(&this->m_members);
}

}

}

#endif
