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

#ifndef WC3LIB_EDITOR_POLYGONMODECOMBOBOX_HPP
#define WC3LIB_EDITOR_POLYGONMODECOMBOBOX_HPP

#include <kcombobox.h>

#include <Ogre.h>

namespace wc3lib
{

namespace editor
{

/**
* Combo box used to select a polygon mode.
* Polygon modes are used by OGRE cameras to display rendered views in a specific way.
*/
class PolygonModeComboBox : public KComboBox
{
	public:
		explicit PolygonModeComboBox(QWidget* parent = 0);

		Ogre::PolygonMode polygonMode() const;
};

inline Ogre::PolygonMode PolygonModeComboBox::polygonMode() const
{
	return (Ogre::PolygonMode)(this->currentIndex());
}

}

}

#endif
