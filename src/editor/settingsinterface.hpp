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

#ifndef WC3LIB_EDITOR_SETTINGSINTERFACE_HPP
#define WC3LIB_EDITOR_SETTINGSINTERFACE_HPP

#include <QString>

#include <kconfiggroup.h>

namespace wc3lib
{

namespace editor
{

/**
* Interface which has to be implemented by all settings classes of Editor.
* Uses KConfigGroup to store and restore various settings.
* SettingsInterface::groupName returns the group's name which is used for a newly created group of another KConfigGroup instance.
*/
class SettingsInterface
{
	public:
		virtual void read(const KConfigGroup &group) = 0;
		virtual void write(KConfigGroup &group) const = 0;
		virtual QString groupName() const = 0;
};

}

}

#endif
