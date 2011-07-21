/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_TERRAINBRUSH_HPP
#define WC3LIB_EDITOR_TERRAINBRUSH_HPP

#include "brush.hpp"
#include "../map/platform.hpp"
#include "../map/tilepoint.hpp"

namespace wc3lib
{

namespace editor
{

class TerrainBrush : public Brush
{
	public:
		TerrainBrush(class TerrainEditor *terrainEditor);
		virtual ~TerrainBrush();

		virtual void onPlace(map::int32 x, map::int32 y);

	protected:
		map::short16 m_groundHeight;
		map::short16 m_waterLevel:15;
		BOOST_SCOPED_ENUM(map::Tilepoint::Flags) m_flags;
		unsigned int m_groundTextureType:4;
		map::char8 m_textureDetails;
		unsigned int m_cliffTextureType:4;
		unsigned int m_layerHeight:4;
};

}

}

#endif
