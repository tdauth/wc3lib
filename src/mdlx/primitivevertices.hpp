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

#ifndef WC3LIB_MDLX_PRIMITIVEVERTICES_HPP
#define WC3LIB_MDLX_PRIMITIVEVERTICES_HPP

#include "groupmdxblock.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDX tag "PVTX".
 * MDL tag "Faces".
 * Primitives consist of at least three vertices which the indices are used of.
 * PrimitiveVertex provides such a single index value (type short16).
 * The number of vertices which belong to a single primitive depends on the primitive's type (PrimitiveType).
 */
class PrimitiveVertices : public GroupMdxBlock
{
	public:
		typedef std::list<class PrimitiveVertex*> Vertices;
		
		PrimitiveVertices(class Geoset *geoset);
		virtual ~PrimitiveVertices();

		class Geoset* geoset() const;
		const Vertices& primitiveVertices() const;

	protected:
		virtual class GroupMdxBlockMember* createNewMember();

		class Geoset *m_geoset;
};

inline class Geoset* PrimitiveVertices::geoset() const
{
	return this->m_geoset;
}

inline const PrimitiveVertices::Vertices& PrimitiveVertices::primitiveVertices() const
{
	return *reinterpret_cast<const std::list<class PrimitiveVertex*>*>(&this->m_members);
}

}

}

#endif

