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

#ifndef WC3LIB_MAP_TILEPOINT_HPP
#define WC3LIB_MAP_TILEPOINT_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

class Tilepoint : public Format
{
	public:
		enum class Flags : uint16
		{
			// FIXME does not fit into 4 bits
			ShadowBoundary = 0x4000,
			Ramp = 0x0010,
			Blight = 0x0020,
			Water = 0x0040,
			CameraBoundary = 0x0080
		};

		Tilepoint();

		virtual std::streamsize read(InputStream &istream);
		virtual std::streamsize write(OutputStream &ostream) const;

		int16 worldEditorHeight(int16 layer, int16 groundZeroLevel, int16 layerZeroLevel) const;
		float32 worldEditorWaterLevel(int16 groundZeroLevel, float32 waterZeroLevel) const;

		int16 groundHeight() const;
		int16 waterLevel() const;
		Tilepoint::Flags flags() const;
		int8 groundTextureType() const;
		byte textureDetails() const;
		int8 cliffTextureType() const;
		int8 layerHeight() const;

	protected:
		int16 m_groundHeight;
		int16 m_waterLevel;
		Tilepoint::Flags m_flags;
		int8 m_groundTextureType;
		byte m_textureDetails;
		int8 m_cliffTextureType;
		int8 m_layerHeight;
};

inline constexpr bool operator&(Tilepoint::Flags x, Tilepoint::Flags y)
{
	return static_cast<bool>(static_cast<uint16>(x) & static_cast<uint16>(y));
}

inline constexpr Tilepoint::Flags operator|(Tilepoint::Flags x, Tilepoint::Flags y)
{
	return static_cast<Tilepoint::Flags>(static_cast<uint16>(x) | static_cast<uint16>(y));
}

inline int16 Tilepoint::worldEditorHeight(int16 layer, int16 groundZeroLevel, int16 layerZeroLevel) const
{
	return (this->m_groundHeight - groundZeroLevel + (layer - layerZeroLevel) * this->m_layerHeight) / 4;
}

inline float32 Tilepoint::worldEditorWaterLevel(int16 groundZeroLevel, float32 waterZeroLevel) const
{
	return (this->m_waterLevel - groundZeroLevel) / 4 + waterZeroLevel;
}

}

}

#endif
