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

#ifndef WC3LIB_MAP_TREE_HPP
#define WC3LIB_MAP_TREE_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/// \todo Add read and write member functions, add TFT version -> TreeX.
class Tree
{
	public:
		BOOST_SCOPED_ENUM_START(Flags)
		{
			Invisible = 0,
			Visibile = 1,
			Normal = 2
		};
		BOOST_SCOPED_ENUM_END
		
		bool operator<(const Tree &other) const { return this->m_id < other.m_id; }
		
		//id id() const { return m_id; }

	protected:
		id m_treeId;
		int32 m_variation;
		float32 m_x;
		float32 m_y;
		float32 m_z;
		float32 m_angle;
		float32 m_scaleX;
		float32 m_scaleY;
		float32 m_scaleZ;
		byte m_flags;
		byte m_life;
		//tft
		//int randomItemTable;
		//int itemSetsNumber;
		//endtft
		int32 m_id;
};

}

}

#endif
