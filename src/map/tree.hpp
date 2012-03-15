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
		BOOST_SCOPED_ENUM_START(Flags) /// \todo C++11 : byte
		{
			Invisible = 0,
			Visibile = 1,
			Normal = 2,
			OutsideCameraBounds = 3 // (set to 3 when it's outside the camerabounds?)
		};
		BOOST_SCOPED_ENUM_END

		virtual std::streamsize read(InputStream &istream) throw (Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

		id treeId() const;
		int32 variation() const;
		const Vertex& position() const;
		/**
		 * (radian angle value)(degree = radian*180/pi)
		 */
		float32 angle() const;
		const Vertex& scale() const;
		BOOST_SCOPED_ENUM(Flags) flags() const;
		byte life() const;
		int32 customId() const;

		bool operator<(const Tree &other) const { return this->customId() < other.customId(); }

	protected:
		id m_treeId;
		int32 m_variation;
		Vertex m_position;
		float32 m_angle;
		Vertex m_scale;
		BOOST_SCOPED_ENUM(Flags) m_flags;
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

inline const Vertex& Tree::position() const
{
	return m_position;
}

inline float32 Tree::angle() const
{
	return m_angle;
}

inline const Vertex& Tree::scale() const
{
	return m_scale;
}

inline BOOST_SCOPED_ENUM(Tree::Flags) Tree::flags() const
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
