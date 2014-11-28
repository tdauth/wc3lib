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

#ifndef WC3LIB_MDLX_COLLISIONSHAPE_HPP
#define WC3LIB_MDLX_COLLISIONSHAPE_HPP

#include <vector>

#include "object.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * \brief Collision shapes are used for the "hit test" which is run when the user selects a 3D model using the cursor.
 *
 * MDL tag "CollisionShape".
 */
class CollisionShape : public Object
{
	public:
		typedef std::vector<VertexData> Vertices;

		enum class Shape : long32
		{
			Box = 0,
			Sphere = 2
		};

		CollisionShape();

		Shape shape() const;
		/// If shape is a box this returns two vertices, otherwise it returns one which is the center of the sphere.
		Vertices& vertices();
		const Vertices& vertices() const;
		/// Only usable if shape is a sphere. Otherwise, it throws an exception.
		float32 boundsRadius() const;

	protected:
		Shape m_shape; //(0:box;2:sphere)
		Vertices m_vertices;
		float32 m_boundsRadius;
};

inline CollisionShape::Shape CollisionShape::shape() const
{
	return this->m_shape;
}

inline CollisionShape::Vertices& CollisionShape::vertices()
{
	return this->m_vertices;
}

inline const CollisionShape::Vertices& CollisionShape::vertices() const
{
	return this->m_vertices;
}

inline float32 CollisionShape::boundsRadius() const
{
	if (shape() != Shape::Sphere)
	{
		throw Exception(_("Collision shape is not a sphere."));
	}

	return this->m_boundsRadius;
}

}

}

#endif
