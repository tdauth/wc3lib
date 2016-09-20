/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#include "modeleditorsettings.hpp"
#include "modeleditor.hpp"
#include "modeleditorview.hpp"

namespace wc3lib
{

namespace editor
{

ModelEditorSettings::ModelEditorSettings(ModelEditor *modelEditor) : m_modelEditor(modelEditor)
{
}

void ModelEditorSettings::read(QSettings &settings)
{
	settings.beginGroup(this->groupName());
	this->modelEditor()->m_recentUrl = settings.value("FilePath", "").toUrl();
	/// @todo Read some preview settings which are not related to OGRE rather than to Warcraft 3 (fog, environment lighting, time of day)
}

void ModelEditorSettings::write(QSettings &settings) const
{
	settings.beginGroup(this->groupName());
	settings.setValue("FilePath", this->modelEditor()->m_recentUrl);
}

QString ModelEditorSettings::groupName() const
{
	return "ModelEditor";
}

}

}
