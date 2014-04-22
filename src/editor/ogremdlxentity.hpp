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

#ifndef WC3LIB_EDITOR_OGREMDLXENTITTY_HPP
#define WC3LIB_EDITOR_OGREMDLXENTITTY_HPP

#include <Ogre.h>

#include "platform.hpp"
#include "ogremdlx.hpp"

#include <boost/foreach.hpp> // NOTE include foreach after Qt stuff because of namespace bug

namespace wc3lib
{

namespace editor
{

/**
 * Stores one single scene node of a corresponding \ref OgreMdlx instance which consists of several entities, for each Geoset one entity.
 * Animations can be assigned using \ref applyAnimation().
 *
 * If you have a Footman model for example and want to create several units sharing the same model as in the game, you'll have to create one \ref OgreMdlx instance, load the model file and add as many entities as you'd like to which can be moved around and animated separately.
 */
class OgreMdlxEntity : public Ogre::FrameListener
{
	public:
		typedef std::list<Ogre::Entity*> Entities;

		OgreMdlxEntity(const Ogre::String &name, OgreMdlx *mdlx, Ogre::SceneManager *sceneManager);
		virtual ~OgreMdlxEntity();

		/// \todo C++11 overide
		virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);

		/**
		 * \return Returns true if animation \p name exists. Otherwise it will return false.
		 */
		bool applyAnimation(const Ogre::String &name, bool loop);

		Ogre::SceneNode* sceneNode() const;

	private:
		OgreMdlx *m_mdlx;
		Ogre::SceneManager *m_sceneManager;
		Ogre::SceneNode *m_sceneNode;
		Entities m_entities;


		class GlobalSequence *m_globalSequence; /// Current global sequence which is played.
};

inline bool OgreMdlxEntity::applyAnimation(const Ogre::String &name, bool loop)
{
	BOOST_FOREACH(Entities::reference ref, m_entities)
	{
		if (!ref->hasAnimationState(name)) {
			return false;
		}

		Ogre::AnimationState *state = ref->getAnimationState(name);
		state->setLoop(loop);
		state->setEnabled(true);
	}
	
	return true;
}

inline Ogre::SceneNode* OgreMdlxEntity::sceneNode() const
{
	return this->m_sceneNode;
}

}

}

#endif
