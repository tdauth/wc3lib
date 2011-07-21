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

#ifndef WC3LIB_EDITOR_TERRAINEDITOR_HPP
#define WC3LIB_EDITOR_TERRAINEDITOR_HPP

#include <Ogre.h>
#include <OgreTerrain.h>
#include <OgreTerrainGroup.h>

#include "../map.hpp"

#include "module.hpp"
#include "modelview.hpp"

namespace wc3lib
{

namespace editor
{

/**
* \todo Model view (Ogre view) should be splittable.
* \todo Use customized version of model view which sends selection events to terrain editor.
* \todo Maybe you should add a custom UI like for other sub editors.
*/
class TerrainEditor : public Module
{
	Q_OBJECT

	public slots:
		void loadEnvironment(const map::Environment &environment);

	public:
		TerrainEditor(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~TerrainEditor();

		virtual void show();

		class ModelView* modelView() const;

	protected:
		virtual void createFileActions(class KMenu *menu);
		virtual void createEditActions(class KMenu *menu);
		virtual void createMenus(class KMenuBar *menuBar);
		virtual void createWindowsActions(class KMenu *menu);
		virtual void createToolButtons(class KToolBar *toolBar);
		virtual class SettingsInterface* settings();
		virtual QString actionName();

		class ModelView *m_modelView;

		Ogre::TerrainGlobalOptions *m_terrainGlobals;
		Ogre::TerrainGroup *m_terrainGroup;
};

inline class ModelView* TerrainEditor::modelView() const
{
	return this->m_modelView;
}

inline QString TerrainEditor::actionName()
{
	return "terraineditor";
}

}

}

#endif
