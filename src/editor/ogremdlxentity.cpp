/***************************************************************************
 *   Copyright (C) 2012 by Tamino Dauth                                    *
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

#include "ogremdlxentity.hpp"

namespace wc3lib
{

namespace editor
{

OgreMdlxEntity::OgreMdlxEntity(const Ogre::String &name, OgreMdlx *mdlx, Ogre::SceneManager *sceneManager) : m_mdlx(mdlx), m_sceneManager(sceneManager), m_sceneNode(sceneManager->getRootSceneNode()->createChildSceneNode(name)), m_globalSequence(0)
{
	mdlx::long32 id = 0;

	BOOST_FOREACH(OgreMdlx::Geosets::const_reference ref, mdlx->geosets())
	{
		Ogre::Entity *entity = sceneManager->createEntity(boost::str(boost::format("%1%.Geoset%2%") % name.c_str() % id).c_str(), ref.second->getName());
		this->m_entities.push_back(entity);

		this->m_sceneNode->attachObject(entity);

		++id;
	}
}

OgreMdlxEntity::~OgreMdlxEntity()
{
	destroySceneNode(this->m_sceneNode);
}

bool OgreMdlxEntity::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	BOOST_FOREACH(Entities::reference ref, m_entities)
	{
		Ogre::ConstEnabledAnimationStateIterator it = ref->getAllAnimationStates()->getEnabledAnimationStateIterator();

		while (it.hasMoreElements())
			it.getNext()->addTime(evt.timeSinceLastFrame);
	}
}

}

}
