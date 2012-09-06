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

#include "ogremdlx.hpp"

namespace wc3lib
{

namespace editor
{

class OgreMdlxEntity : public Ogre::FrameListener
{
	public:

		OgreMdlxEntity(const Ogre::String &name, OgreMdlx *mdlx, Ogre::SceneManager *sceneManager);
		virtual ~OgreMdlxEntity();

		/// \todo C++11 overide
		virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);

		Ogre::AnimationState* animationState() const;
		bool applyAnimation(const Ogre::String &name, bool loop);

	private:
		OgreMdlx *m_mdlx;
		Ogre::Entity *m_entity;


		class GlobalSequence *m_globalSequence; /// Current global sequence which is played.

		Ogre::AnimationState *m_animationState;
};

inline Ogre::AnimationState* OgreMdlx::animationState() const
{
	return this->m_animationState;
}

inline bool OgreMdlx::applyAnimation(const Ogre::String &name, bool loop)
{
	if (!entity()->hasAnimationState(name))
		return false;

	Ogre::AnimationState *state = entity()->getAnimationState(name);
	state->setLoop(loop);
	state->setEnabled(true);
	m_animationState = state;
}


}

}
