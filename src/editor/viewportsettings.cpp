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

#include "viewportsettings.hpp"

namespace wc3lib
{

namespace editor
{

ViewPortSettings::ViewPortSettings(Ogre::Viewport *viewPort) : m_viewPort(viewPort)
{
}

void ViewPortSettings::read(const KConfigGroup &group)
{
	// read also camera settings (camera is assigned automatically by class ModelViewSettings)

	/*
	* - camera position
	* - camera target/orientation
	* - polygon mode
	* - background color
	* - auto update (if not there should be a refresh button, improves performance)
	* - orientation mode
	* - dimensions
	* - skies enabled
	* - shadows enabled
	* - visibility mask
	* - overlays enabled
	*/
}

void ViewPortSettings::write(KConfigGroup &group) const
{
}

QString ViewPortSettings::groupName() const
{
	return "ViewPort";
}

}

}
