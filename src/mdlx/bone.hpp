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

#ifndef WC3LIB_MDLX_BONE_HPP
#define WC3LIB_MDLX_BONE_HPP

#include "object.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Bone".
 *
 * Taken from Art Tools Manual:
 * Use two different bones for unit models: "bone_head" and "bone_chest". These two bones will also be used by Warcraft when lock body-part facing action is called.
 * Use bone called "bone_turret" if you want a part of your model is rotated only when targeting another unit.
 *
 * \ingroup animations
 */
class Bone : public Object
{
	public:
		Bone();

		void setGeosetId(long32 geosetId);
		long32 geosetId() const;
		bool hasMultipleGeosetIds() const;
		void setGeosetAnimationId(long32 geosetAnimationId);
		long32 geosetAnimationId() const;
		bool hasNoneGeosetAnimationId() const;

	protected:
		long32 m_geosetId;
		long32 m_geosetAnimationId;
};

inline void Bone::setGeosetId(long32 geosetId)
{
	this->m_geosetId = geosetId;
}

inline long32 Bone::geosetId() const
{
	return this->m_geosetId;
}

inline bool Bone::hasMultipleGeosetIds() const
{
	return this->geosetId() == -1;
}

inline void Bone::setGeosetAnimationId(long32 geosetAnimationId)
{
	this->m_geosetAnimationId = geosetAnimationId;
}

inline long32 Bone::geosetAnimationId() const
{
	return this->m_geosetAnimationId;
}

inline bool Bone::hasNoneGeosetAnimationId() const
{
	return this->geosetAnimationId() == -1;
}

}

}

#endif
