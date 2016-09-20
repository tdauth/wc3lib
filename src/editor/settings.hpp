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

#ifndef WC3LIB_EDITOR_SETTINGS_HPP
#define WC3LIB_EDITOR_SETTINGS_HPP

#include "settingsinterface.hpp"

namespace wc3lib
{

namespace editor
{

class Editor;

/**
* Settings widget for model view settings.
* Allows you to configure OGRE rendering settings (e. g. resolution, renderer, lighting etc.).
* \sa ModelView, ModelEditor, TerrainEditor
*/
class Settings : public SettingsInterface
{
	public:
		Settings(Editor *editor);

		virtual void read(QSettings &settings);
		virtual void write(QSettings &settings) const;
		virtual QString groupName() const;

		Editor* editor() const;

	protected:
		Editor *m_editor;
};

inline Editor* Settings::editor() const
{
	return this->m_editor;
}

}

}

#endif
