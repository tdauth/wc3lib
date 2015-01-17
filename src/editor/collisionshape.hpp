/***************************************************************************
 *   Copyright (C) 2015 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_COLLISIONSHAPE_HPP
#define WC3LIB_EDITOR_COLLISIONSHAPE_HPP

#include <Ogre.h>

#include "../mdlx/collisionshape.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * This structure is required for model's collision shapes which either can be boxes or spheres.
 * One instance should be created per collision shape of the corresponding \ref mdlx::Mdlx instance.
 */
class CollisionShape
{
	public:
		CollisionShape() : box(0), shape(mdlx::CollisionShape::Shape::Box)
		{
		}

		~CollisionShape()
		{
			if (shape == mdlx::CollisionShape::Shape::Box)
			{
				if (box != 0)
				{
					delete box;
				}
			}
			else
			{
				if (sphere != 0)
				{
					delete sphere;
				}
			}
		}

		union
		{
			Ogre::AxisAlignedBox *box;
			Ogre::Sphere *sphere;
		};

		mdlx::CollisionShape::Shape shape;
};

}

}

#endif // WC3LIB_EDITOR_COLLISIONSHAPE_HPP
