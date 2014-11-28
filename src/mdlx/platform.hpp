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

#ifndef WC3LIB_MDLX_PLATFORM_HPP
#define WC3LIB_MDLX_PLATFORM_HPP

#include <iomanip>

#include <boost/foreach.hpp>

#include "../platform.hpp"
#include "../vertex.hpp"
#include "../utilities.hpp"
#include "../format.hpp"

namespace wc3lib
{

// TODO C++11
/*
extern template class BasicFormat<byte>; // increase compile time
extern template class BasicVertex<float32, 2>;
extern template class BasicVertex<float32, 3>;
extern template class BasicVertex<float32, 4>;
*/

namespace mdlx
{

// have to be signed because of PriorityPlane -1 and other examples of negative values
typedef int16 short16; /// @todo undefined length?!
typedef int32 long32;

const long32 noneId = 0xFFFFFFFF;

/**
 * \defgroup animations MDX and MDL animations
 * The 3D formats MDX and MDL support different types of animations to apply transformations on nodes at specific time frames.
 *
 */

/**
 * MDLX format supports interpolation for scalings, translations and rotations.
 * If interpolation is not used value should be DontInterpolate.
 * If line type is Hermite or Bezier additional interpolation data is used (see structures).
 * Line type is stored as long32 in MDX files.
 * \sa InterpolationData InterpolationRotationData
 * \ingroup animations
 */
enum class LineType : long32
{
	DontInterpolate = 0,
	Linear = 1,
	Hermite = 2,
	Bezier = 3
};

/**
 * \todo Finish enum.
 * \sa Texture, ParticleEmitter2
 */
enum class ReplaceableId : long32
{
	None = 0,
	TeamColor = 1,
	TeamGlow = 2,
	Cliff = 11,
	LordaeronTree = 31,
	AshenvaleTree = 32,
	BarrensTree = 33,
	NorthrendTree = 34,
	MushroomTree = 35
};

typedef Vertex3d<float32> VertexData;

struct InterpolationData //: public Format
{
	InterpolationData(const VertexData &inTan, const VertexData &outTan) : inTan(inTan), outTan(outTan)
	{
	}

	VertexData inTan;
	VertexData outTan;
};

typedef Quaternion<float32> QuaternionData;

struct InterpolationRotationData// : public Format
{
	InterpolationRotationData(const QuaternionData &inTan, const QuaternionData &outTan) : inTan(inTan), outTan(outTan)
	{
	}

	QuaternionData inTan;
	QuaternionData outTan;
};

}

}

#endif
