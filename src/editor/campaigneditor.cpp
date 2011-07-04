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

namespace wc3lib
{

namespace editor
{

CampaignEditor::CampaignEditor(class Editor *editor) : Module(editor)
{
}

void CampaignEditor::openCampaign(const KUrl &url)
{
}

void CampaignEditor::closeCampaign()
{
}

void CampaignEditor::createFileActions(class KMenu *menu)
{
}

void CampaignEditor::createEditActions(class KMenu *menu)
{
}

void CampaignEditor::createMenus(class KMenuBar *menuBar)
{
}

void CampaignEditor::createWindowsActions(class KMenu *menu)
{
}

void CampaignEditor::createToolButtons(class KToolBar *toolBar)
{
}

class SettingsInterface* CampaignEditor::settings()
{
	/// @todo FIXME
	return 0;
}

#include "moc_campaigneditor.cpp"

}

}
