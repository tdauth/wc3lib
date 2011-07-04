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

#include <klocale.h>

#include "terraineditor.hpp"
#include "editor.hpp"

namespace wc3lib
{

namespace editor
{

TerrainEditor::TerrainEditor(class Editor *editor, Qt::WFlags f) : Module(editor), m_modelView(new ModelView(this, 0, f))
{
	setWindowTitle(i18n("Terrain Editor"));
	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget(this->m_modelView);
	setLayout(mainLayout);
}

TerrainEditor::~TerrainEditor()
{
	delete this->m_modelView;
}

void TerrainEditor::show()
{
	Module::show();
	this->m_modelView->show();
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

}

}
