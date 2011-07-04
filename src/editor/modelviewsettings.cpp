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

#include <boost/format.hpp>

#include "modelviewsettings.hpp"
#include "modelview.hpp"
#include "viewportsettings.hpp"
#include "rendersystemsettings.hpp"

namespace wc3lib
{

namespace editor
{

ModelViewSettings::ModelViewSettings(class ModelView *modelView) : m_modelView(modelView)
{
}

void ModelViewSettings::read(const KConfigGroup &group)
{
	// render system
	RenderSystemSettings renderSystemSettings(this->modelView()->root()->getRenderSystem());
	renderSystemSettings.read(group.group(renderSystemSettings.groupName()));

	// view ports
	// short unsigned int
	short unsigned int viewPorts = 1; // FIXME group.readEntry<short unsigned int>("ViewPorts", (short unsigned int)1);

	while (viewPorts > 0)
	{
		Ogre::Viewport *viewPort = this->modelView()->renderWindow()->getNumViewports() >= viewPorts ? this->modelView()->renderWindow()->getViewport(viewPorts - 1) : this->modelView()->renderWindow()->addViewport(this->modelView()->sceneManager()->createCamera((boost::format("ViewPortCamera%1%") % (viewPorts - 1)).str().c_str())); // add new view port if there aren't enough
		ViewPortSettings viewPortSettings(viewPort);
		viewPortSettings.read(group.group(QString("%1%2").arg(viewPortSettings.groupName()).arg(viewPorts - 1)));
		--viewPorts;
	}
}

void ModelViewSettings::write(KConfigGroup &group) const
{
	// render system
	RenderSystemSettings renderSystemSettings(this->modelView()->root()->getRenderSystem());
	KConfigGroup renderSystemSettingsGroup = group.group(renderSystemSettings.groupName());
	renderSystemSettings.write(renderSystemSettingsGroup);

	// view ports
	short unsigned int viewPorts = this->modelView()->renderWindow()->getNumViewports();
	// TODO FIXME
	//group.writeEntry("ViewPorts", viewPorts);

	for (short unsigned int i = 0; i < viewPorts; ++i)
	{
		Ogre::Viewport *viewPort = this->modelView()->renderWindow()->getViewport(i);
		ViewPortSettings viewPortSettings(viewPort);
		KConfigGroup  viewPortSettingsGroup(group.group(QString("%1%2").arg(viewPortSettings.groupName()).arg(i)));
		viewPortSettings.write(viewPortSettingsGroup);
	}
}

QString ModelViewSettings::groupName() const
{
	return "ModelView";
}

}

}
