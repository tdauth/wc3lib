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

#ifndef WC3LIB_MDLX_PARTICLEEMITTER2_HPP
#define WC3LIB_MDLX_PARTICLEEMITTER2_HPP

#include <array>

#include "platform.hpp"
#include "node.hpp"
#include "mdlxanimatedproperties.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "ParticleEmitter2".
 */
class ParticleEmitter2 : public Node
{
	public:
		static const std::size_t segmentColorsSize = 3;
		typedef std::array<VertexData, segmentColorsSize> SegmentColors;

		enum class FilterMode : long32
		{
			Blend = 0,
			Additive = 1,
			Modulate = 2,
			AlphaKey = 4
		};

		enum class Flags : long32
		{
			Head = 0,
			Tail = 1,
			Both = 2
		};

		ParticleEmitter2();
		virtual ~ParticleEmitter2();

		float32 speed() const;
		float32 variation() const;
		float32 latitude() const;
		float32 gravity() const;
		float32 lifespan() const;
		float32 emissionRate() const;
		float32 length() const;
		float32 width() const;
		FilterMode filterMode() const;
		long32 rows() const;
		long32 columns() const;
		Flags flags() const;
		float32 tailLength() const;
		float32 time() const;
		SegmentColors& segmentColors();
		const SegmentColors& segmentColors() const;
		byte alpha1() const;
		byte alpha2() const;
		byte alpha3() const;
		float32 scalingX() const;
		float32 scalingY() const;
		float32 scalingZ() const;
		long32 lifeSpanUvAnim1() const;
		long32 lifeSpanUvAnim2() const;
		long32 lifeSpanUvAnim3() const;
		long32 decayUvAnim1() const;
		long32 decayUvAnim2() const;
		long32 decayUvAnim3() const;
		long32 tailUvAnim1() const;
		long32 tailUvAnim2() const;
		long32 tailUvAnim3() const;
		long32 tailDecayUvAnim1() const;
		long32 tailDecayUvAnim2() const;
		long32 tailDecayUvAnim3() const;
		long32 textureId() const;
		long32 squirt() const;
		long32 priorityPlane() const;
		ReplaceableId replaceableId() const;
		const Alphas& speeds() const;
		const Alphas& latitudes() const;
		const Alphas& emissionRates() const;
		const Alphas& visibilities() const;
		const Alphas& numbers() const;
		const Alphas& widths() const;

	protected:
		//long32 nbytesi;
		//long32 nbytesikg; // inclusive bytecount including KGXXs
		float32 m_speed;
		float32 m_variation;
		float32 m_latitude;
		float32 m_gravity;
		float32 m_lifespan;
		float32 m_emissionRate;
		float32 m_length;
		float32 m_width;
		FilterMode m_filterMode; //(0:Blend;1:Additive;2:Modulate;4:AlphaKey)
		long32 m_rows;
		long32 m_columns;
		Flags m_flags; //(0:Head;1:Tail;2:Both)
		float32 m_tailLength;
		float32 m_time;
		SegmentColors m_segmentColors;
		byte m_alpha1, m_alpha2, m_alpha3; //byte
		float32 m_scalingX, m_scalingY, m_scalingZ;
		long32 m_lifeSpanUvAnim1, m_lifeSpanUvAnim2, m_lifeSpanUvAnim3;
		long32 m_decayUvAnim1, m_decayUvAnim2, m_decayUvAnim3;
		long32 m_tailUvAnim1, m_tailUvAnim2, m_tailUvAnim3;
		long32 m_tailDecayUvAnim1, m_tailDecayUvAnim2, m_tailDecayUvAnim3;
		long32 m_textureId;
		long32 m_squirt; //(1:Squirt)
		long32 m_priorityPlane;
		ReplaceableId m_replaceableId;
		Alphas m_speeds; //(KP2S)
		Alphas m_latitudes; //(KP2L)
		Alphas m_emissionRates; //(KP2E)
		Alphas m_visibilities; //(KP2V)
		Alphas m_numbers; //(KP2N)
		Alphas m_widths; //(KP2W)
};

inline float32 ParticleEmitter2::speed() const
{
	return this->m_speed;
}

inline float32 ParticleEmitter2::variation() const
{
	return this->m_variation;
}

inline float32 ParticleEmitter2::latitude() const
{
	return this->m_latitude;
}

inline float32 ParticleEmitter2::gravity() const
{
	return this->m_gravity;
}

inline float32 ParticleEmitter2::lifespan() const
{
	return this->m_lifespan;
}

inline float32 ParticleEmitter2::emissionRate() const
{
	return this->m_emissionRate;
}

inline float32 ParticleEmitter2::length() const
{
	return this->m_length;
}

inline float32 ParticleEmitter2::width() const
{
	return this->m_width;
}

inline ParticleEmitter2::FilterMode ParticleEmitter2::filterMode() const
{
	return this->m_filterMode;
}

inline long32 ParticleEmitter2::rows() const
{
	return this->m_rows;
}

inline long32 ParticleEmitter2::columns() const
{
	return this->m_columns;
}

inline ParticleEmitter2::Flags ParticleEmitter2::flags() const
{
	return this->m_flags;
}

inline float32 ParticleEmitter2::tailLength() const
{
	return this->m_tailLength;
}

inline float32 ParticleEmitter2::time() const
{
	return this->m_time;
}

inline ParticleEmitter2::SegmentColors& ParticleEmitter2::segmentColors()
{
	return this->m_segmentColors;
}

inline const ParticleEmitter2::SegmentColors& ParticleEmitter2::segmentColors() const
{
	return this->m_segmentColors;
}

inline byte ParticleEmitter2::alpha1() const
{
	return this->m_alpha1;
}

inline byte ParticleEmitter2::alpha2() const
{
	return this->m_alpha2;
}

inline byte ParticleEmitter2::alpha3() const
{
	return this->m_alpha3;
}

inline float32 ParticleEmitter2::scalingX() const
{
	return this->m_scalingX;
}

inline float32 ParticleEmitter2::scalingY() const
{
	return this->m_scalingY;
}

inline float32 ParticleEmitter2::scalingZ() const
{
	return this->m_scalingZ;
}

inline long32 ParticleEmitter2::lifeSpanUvAnim1() const
{
	return this->m_lifeSpanUvAnim1;
}

inline long32 ParticleEmitter2::lifeSpanUvAnim2() const
{
	return this->m_lifeSpanUvAnim2;
}

inline long32 ParticleEmitter2::lifeSpanUvAnim3() const
{
	return this->m_lifeSpanUvAnim3;
}

inline long32 ParticleEmitter2::decayUvAnim1() const
{
	return this->m_decayUvAnim1;
}

inline long32 ParticleEmitter2::decayUvAnim2() const
{
	return this->m_decayUvAnim2;
}

inline long32 ParticleEmitter2::decayUvAnim3() const
{
	return this->m_decayUvAnim3;
}

inline long32 ParticleEmitter2::tailUvAnim1() const
{
	return this->m_tailUvAnim1;
}

inline long32 ParticleEmitter2::tailUvAnim2() const
{
	return this->m_tailUvAnim2;
}

inline long32 ParticleEmitter2::tailUvAnim3() const
{
	return this->m_tailUvAnim3;
}

inline long32 ParticleEmitter2::tailDecayUvAnim1() const
{
	return this->m_tailDecayUvAnim1;
}

inline long32 ParticleEmitter2::tailDecayUvAnim2() const
{
	return this->m_tailDecayUvAnim2;
}

inline long32 ParticleEmitter2::tailDecayUvAnim3() const
{
	return this->m_tailDecayUvAnim3;
}

inline long32 ParticleEmitter2::textureId() const
{
	return this->m_textureId;
}

inline long32 ParticleEmitter2::squirt() const
{
	return this->m_squirt;
}

inline long32 ParticleEmitter2::priorityPlane() const
{
	return this->m_priorityPlane;
}

inline ReplaceableId ParticleEmitter2::replaceableId() const
{
	return this->m_replaceableId;
}

inline const Alphas& ParticleEmitter2::speeds() const
{
	return this->m_speeds;
}

inline const Alphas& ParticleEmitter2::latitudes() const
{
	return this->m_latitudes;
}

inline const Alphas& ParticleEmitter2::emissionRates() const
{
	return this->m_emissionRates;
}

inline const Alphas& ParticleEmitter2::visibilities() const
{
	return this->m_visibilities;
}

inline const Alphas& ParticleEmitter2::numbers() const
{
	return this->m_numbers;
}

inline const Alphas& ParticleEmitter2::widths() const
{
	return this->m_widths;
}

}

}

#endif
