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

#include "groupmdxblockmember.hpp"
#include "object.hpp"
#include "collisionshapes.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "CollisionShape".
 */
class CollisionShape : public GroupMdxBlockMember, public Object
{
	public:
		BOOST_SCOPED_ENUM_START(Shape) // : long32
		{
			Box = 0,
			Sphere = 2
		};
		BOOST_SCOPED_ENUM_END

		CollisionShape(class CollisionShapes *collisionShapes);

		class CollisionShapes* collisionShapes() const;
		BOOST_SCOPED_ENUM(Shape) shape() const;
		const struct VertexData& vertexData() const;
		/// Only usable if shape is a box. Otherwise, it throws an exception.
		const struct VertexData& boxVertexData() const throw (class Exception);
		/// Only usable if shape is a sphere. Otherwise, it throws an exception.
		float32 boundsRadius() const throw (class Exception);

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		BOOST_SCOPED_ENUM(Shape) m_shape; //(0:box;2:sphere)
		struct VertexData m_vertexData;
		//if (Shape == 0)
		struct VertexData m_boxVertexData;
		//else
		float32 m_boundsRadius;
};

inline class CollisionShapes* CollisionShape::collisionShapes() const
{
	return boost::polymorphic_cast<class CollisionShapes*>(this->parent());
}

inline BOOST_SCOPED_ENUM(CollisionShape::Shape) CollisionShape::shape() const
{
	return this->m_shape;
}

inline const struct VertexData& CollisionShape::vertexData() const
{
	return this->m_vertexData;
}

inline const struct VertexData& CollisionShape::boxVertexData() const throw (class Exception)
{
	if (shape() != Shape::Box)
		throw Exception(_("Collision shape is not a box."));
	
	return this->m_boxVertexData;
}

inline float32 CollisionShape::boundsRadius() const throw (class Exception)
{
	if (shape() != Shape::Sphere)
		throw Exception(_("Collision shape is not a sphere."));
	
	return this->m_boundsRadius;
}

}

}

#endif
