/***************************************************************************
 *   Copyright (C) 2022 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_OGRETERRAIN_HPP
#define WC3LIB_EDITOR_OGRETERRAIN_HPP

#include <map>
#include <list>
#include <memory>

#include <QDebug>

#include <Ogre.h>

#include "resource.hpp"
#include "platform.hpp"
#include "collisionshape.hpp"
#include "../map.hpp"
#include "ogreutilities.hpp"

namespace wc3lib
{

namespace editor
{

class ModelView;

/**
 * \brief This class can be used to display Warcraft III map terrain by using the OGRE 3D rendering engine.
 */
class OgreTerrain : public Resource, public Ogre::FrameListener
{
	public:
		typedef std::unique_ptr<map::Environment> EnvironmentPtr;


		OgreTerrain(const QUrl &url, ModelView *modelView);
		virtual ~OgreTerrain();

		virtual void clear() throw ();

		const EnvironmentPtr& environment() const;
		/**
		 * \return Returns the corresponding model view which the scene node does belong to.
		 */
		ModelView* modelView() const;
		/**
		 * \return Returns the corresponding scene node which is the parent node of all OGRE instances from this particular model.
		 */
		Ogre::SceneNode* sceneNode() const;

		/**
		 * Loads and analyses all data of corresponding MDLX model and refreshes displayed OGRE mesh.
		 * If \ref modelView() is based on
		 */
		virtual void load() override;
		virtual void reload() override;

		virtual void save(const QUrl &url, const QString &format) const;
		virtual void save(const QUrl &url) const override
		{
			save(url, "");
		}

		QString namePrefix() const;

	protected:
		EnvironmentPtr m_environment;
		ModelView *m_modelView;
		Ogre::SceneNode *m_sceneNode;
};

inline const OgreTerrain::EnvironmentPtr& OgreTerrain::environment() const
{
	return this->m_environment;
}

inline ModelView* OgreTerrain::modelView() const
{
	return this->m_modelView;
}

inline Ogre::SceneNode* OgreTerrain::sceneNode() const
{
	return this->m_sceneNode;
}

}

}

#endif
