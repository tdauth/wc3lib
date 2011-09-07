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

#include <QtGui>

#include <KLocale>

#include "terraineditor.hpp"
#include "editor.hpp"

namespace wc3lib
{

namespace editor
{

void TerrainEditor::loadEnvironment(const map::Environment &environment)
{
	this->m_terrainGroup->loadAllTerrains(true);
}

TerrainEditor::TerrainEditor(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : Module(source, parent, f), m_modelView(new ModelView(hasEditor() ? editor()->root() : 0)), m_terrainGlobals(new Ogre::TerrainGlobalOptions()), m_terrainGroup(0)
{
	setWindowTitle(i18n("Terrain Editor"));
	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget(this->m_modelView);
	topLayout()->addLayout(mainLayout);
}

TerrainEditor::~TerrainEditor()
{
	delete m_terrainGlobals;
}

void TerrainEditor::show()
{
	Module::show();
	this->m_modelView->show();

	if (m_terrainGroup != 0)
	{
		m_terrainGroup = new Ogre::TerrainGroup(modelView()->sceneManager(), Ogre::Terrain::ALIGN_X_Y, 513, 12000.0f);
		m_terrainGroup->setOrigin(Ogre::Vector3::ZERO);
		/*
		m_terrainGroup->setMaxPixelError(8);
		// testing composite map
		m_terrainGroup->setCompositeMapDistance(3000);
		// Important to set these so that the terrain knows what to use for derived (non-realtime) data
		m_terrainGroup->setLightMapDirection(modelView()->sceneManager()->light()->getDerivedDirection());
		m_terrainGroup->setCompositeMapAmbient(modelView()->sceneManager()->getAmbientLight());
		m_terrainGroup->setCompositeMapDiffuse(modelView()->sceneManager()->light()->getDiffuseColour());
		*/
	}
}

void TerrainEditor::createFileActions(class KMenu *menu)
{
}

void TerrainEditor::createEditActions(class KMenu *menu)
{
}

void TerrainEditor::createMenus(class KMenuBar *menuBar)
{
}

void TerrainEditor::createWindowsActions(class KMenu *menu)
{
}

void TerrainEditor::createToolButtons(class KToolBar *toolBar)
{
}

class SettingsInterface* TerrainEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void TerrainEditor::onSwitchToMap(Map* map)
{
}

#include "moc_terraineditor.cpp"

}

}
