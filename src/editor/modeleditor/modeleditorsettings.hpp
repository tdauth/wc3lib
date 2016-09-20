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

#ifndef WC3LIB_EDITOR_MODELEDITORSETTINGS_HPP
#define WC3LIB_EDITOR_MODELEDITORSETTINGS_HPP

#include "../settingsinterface.hpp"

namespace wc3lib
{

namespace editor
{

class ModelEditor;

class ModelEditorSettings : public SettingsInterface
{
	public:
		ModelEditorSettings(ModelEditor *modelEditor);

		virtual void read(QSettings &settings) override;
		virtual void write(QSettings &settings) const override;
		virtual QString groupName() const;

		ModelEditor* modelEditor() const;

	protected:
		ModelEditor *m_modelEditor;
};

inline ModelEditor* ModelEditorSettings::modelEditor() const
{
	return this->m_modelEditor;
}

}

}

#endif
