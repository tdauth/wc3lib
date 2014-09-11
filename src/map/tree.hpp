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

/**
 * In Warcraft III: Reign of Chaos each tree/destructible is based on a type defined in "Units\DestructableData.slk".
 * Each tree is stored with a unique id in its map which is returned by \ref customId().
 */
/// \todo Add read and write member functions, add TFT version -> TreeX.
class Tree : public Format
{
	public:
		enum class Flags : byte
		{
			Invisible = 0,
			Visibile = 1,
			Normal = 2,
			OutsideCameraBounds = 3 // (set to 3 when it's outside the camerabounds?)
		};

		virtual std::streamsize read(InputStream &istream);
		virtual std::streamsize write(OutputStream &ostream) const;

		id treeId() const;
		int32 variation() const;
		const Vertex3d<float32>& position() const;
		/**
		 * (radian angle value)(degree = radian*180/pi)
		 */
		float32 angle() const;
		const Vertex3d<float32>& scale() const;
		Flags flags() const;
		byte life() const;
		int32 customId() const;

		bool operator<(const Tree &other) const { return this->customId() < other.customId(); }

	protected:
		id m_treeId;
		int32 m_variation;
		Vertex3d<float32> m_position;
		float32 m_angle;
		Vertex3d<float32> m_scale;
		Flags m_flags;
		byte m_life; // (integer stored in %, 100% is 0x64, 170% is 0xAA for example)
		//tft
		//int randomItemTable;
		//int itemSetsNumber;
		//endtft
		int32 m_customId;
};

inline id Tree::treeId() const
{
	return m_treeId;
}

inline int32 Tree::variation() const
{
	return m_variation;
}

inline const Vertex3d<float32>& Tree::position() const
{
	return m_position;
}

inline float32 Tree::angle() const
{
	return m_angle;
}

inline const Vertex3d<float32>& Tree::scale() const
{
	return m_scale;
}

inline Tree::Flags Tree::flags() const
{
	return m_flags;
}

inline byte Tree::life() const
{
	return m_life;
}

inline int32 Tree::customId() const
{
	return m_customId;
}


}

}

#endif
