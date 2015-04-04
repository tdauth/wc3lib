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

#include "campaigneditor.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

CampaignEditor::CampaignEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : Module(source, parent, f)
{
}

void CampaignEditor::openCampaign(const KUrl &url)
{
}

void CampaignEditor::closeCampaign()
{
}

void CampaignEditor::createFileActions(QMenu *menu)
{
}

void CampaignEditor::createEditActions(QMenu *menu)
{
}

void CampaignEditor::createMenus(QMenuBar *menuBar)
{
}

void CampaignEditor::createWindowsActions(WindowsMenu *menu)
{
}

void CampaignEditor::createToolButtons(ModuleToolBar *toolBar)
{
}

SettingsInterface* CampaignEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void CampaignEditor::onSwitchToMap(Map *map)
{
}

QIcon CampaignEditor::icon()
{
	return QIcon(this->source()->sharedData()->worldEditDataPixmap("ToolBarIcon_Module_Campaign", "WorldEditArt", this));
}

#include "moc_campaigneditor.cpp"

}

}
