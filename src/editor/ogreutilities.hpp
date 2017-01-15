/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_OGREUTILITIES_HPP
#define WC3LIB_EDITOR_OGREUTILITIES_HPP

#include <boost/cast.hpp>

#include <Ogre.h>

#include "platform.hpp"

namespace wc3lib
{

namespace editor
{

/// Global type cast function.
inline Ogre::Real ogreReal(float32 value)
{
	return boost::numeric_cast<Ogre::Real>(value);
}

/// Global type cast function.
inline Ogre::Vector3 ogreVector3(const Vertex3d<float32> &vertexData)
{
	return Ogre::Vector3(vertexData.x(), vertexData.y(), vertexData.z());
}

/// Global type cast function.
inline Ogre::Vector2 ogreVector2(const Vertex2d<float32> &textureVertexData)
{
	return Ogre::Vector2(textureVertexData.x(), textureVertexData.y());
}

template<typename R, typename ValueType, std::size_t N>
inline R ogreVertex(const BasicVertex<ValueType, N> & /* vertex */)
{
	throw Exception(_("No valid specialization!")); // TODO static assert
}

template<>
inline Ogre::Vector2 ogreVertex<Ogre::Vector2, float32, 2>(const BasicVertex<float32, 2> &vertex)
{
	return Ogre::Vector2(ogreReal(vertex[0]), ogreReal(vertex[1]));
}

template<>
inline Ogre::Vector3 ogreVertex<Ogre::Vector3, float32, 3>(const BasicVertex<float32, 3> &vertex)
{
	return Ogre::Vector3(ogreReal(vertex[0]), ogreReal(vertex[1]), ogreReal(vertex[2]));
}

template<>
inline Ogre::Vector4 ogreVertex<Ogre::Vector4, float32, 4>(const BasicVertex<float32, 4> &vertex)
{
	return Ogre::Vector4(ogreReal(vertex[0]), ogreReal(vertex[1]), ogreReal(vertex[2]), ogreReal(vertex[3]));
}

template<>
inline Ogre::Quaternion ogreVertex<Ogre::Quaternion, float32, 4>(const BasicVertex<float32, 4> &vertex)
{
	return Ogre::Quaternion(ogreReal(vertex[0]), ogreReal(vertex[1]), ogreReal(vertex[2]), ogreReal(vertex[3]));
}

/**
 * \brief Destroys \p node and all attached movable objects recursively.
 *
 * From http://www.ogre3d.org/forums/viewtopic.php?f=2&t=53647
 */
inline void destroyAllAttachedMovableObjects(Ogre::SceneNode *node)
{
	if(!node) return;

	// Destroy all the attached objects
	Ogre::SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator(); // FIXME segmentation fault

	while (itObject.hasMoreElements())
	{
		node->getCreator()->destroyMovableObject(itObject.getNext());
	}

	// Recurse to child SceneNodes
	Ogre::SceneNode::ChildNodeIterator itChild = node->getChildIterator();

	while ( itChild.hasMoreElements() )
	{
		Ogre::SceneNode* pChildNode = boost::polymorphic_cast<Ogre::SceneNode*>(itChild.getNext());
		destroyAllAttachedMovableObjects(pChildNode);
	}
}

/// From http://www.ogre3d.org/forums/viewtopic.php?f=2&t=53647
inline void destroySceneNode(Ogre::SceneNode *node)
{
	if(!node) return;
	destroyAllAttachedMovableObjects(node);
	node->removeAndDestroyAllChildren();
	node->getCreator()->destroySceneNode(node);
}

}

}

#endif
