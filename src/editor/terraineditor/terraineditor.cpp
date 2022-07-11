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

#include "terraineditor.hpp"
#include "../editor.hpp"

namespace wc3lib
{

namespace editor
{

void TerrainEditor::loadEnvironment(const QUrl &url)
{
    m_terrain.reset(new OgreTerrain(url, modelView()));
}

TerrainEditor::TerrainEditor(MpqPriorityList *source, const QString &organization, const QString &applicationName, QWidget *parent, Qt::WindowFlags f) : Module(source, organization, applicationName, parent, f), m_modelView(new ModelView(hasEditor() ? editor()->root() : 0)), m_terrain(nullptr)
{
	setWindowTitle(tr("Terrain Editor"));
	Module::setupUi();
	centerLayout()->addWidget(modelView());
}

TerrainEditor::~TerrainEditor()
{
}

void TerrainEditor::show()
{
	Module::show();
	this->m_modelView->show();
}

void TerrainEditor::createFileActions(QMenu *menu)
{
}

void TerrainEditor::createEditActions(QMenu *menu)
{
}

void TerrainEditor::createMenus(QMenuBar *menuBar)
{
}

void TerrainEditor::createWindowsActions(WindowsMenu *menu)
{
}

void TerrainEditor::createToolButtons(ModuleToolBar *toolBar)
{
}

SettingsInterface* TerrainEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void TerrainEditor::onSwitchToMap(Map *map)
{
}

QIcon TerrainEditor::icon()
{
	return QIcon(this->source()->sharedData()->worldEditDataPixmap("ToolBarIcon_Module_Terrain", "WorldEditArt", this));
}

#include "moc_terraineditor.cpp"

}

}
