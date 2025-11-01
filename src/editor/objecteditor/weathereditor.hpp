/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_WEATHEREDITOR_HPP
#define WC3LIB_EDITOR_WEATHEREDITOR_HPP

#include "objecteditortab.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

class WeatherData;

class WeatherEditor : public ObjectEditorTab
{
	public:
		WeatherEditor(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent = nullptr);
		virtual ~WeatherEditor();

		WeatherData* weatherData() const;

		virtual QString name() const override;

		virtual QIcon tabIcon(QWidget *widget) const override;

	protected:
		virtual ObjectTreeModel* createTreeModel() override;
		virtual void onSwitchToMap(Map *map) override;

		virtual void onNewObject() override;

		virtual QString newObjectText() const override;
		virtual QString renameObjectText() const override;
		virtual QString deleteObjectText() const override;
		virtual QString resetObjectText() const override;
		virtual QString resetAllObjectsText() const override;
		virtual QString exportAllObjectsText() const override;
		virtual QString importAllObjectsText() const override;
		virtual QString copyObjectText() const override;
		virtual QString pasteObjectText() const override;

		virtual QUrl copyObjectIconUrl() const override;
		virtual QUrl pasteObjectIconUrl() const override;
		virtual QUrl newObjectIconUrl() const override;
};

inline QString WeatherEditor::newObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_UE_CREATECUSTOMUNIT", "WorldEditStrings");
}

inline QString WeatherEditor::renameObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RENAME", "WorldEditStrings");
}

inline QString WeatherEditor::deleteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_DELETE", "WorldEditStrings");
}

inline QString WeatherEditor::resetObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RESETSEL", "WorldEditStrings");
}

inline QString WeatherEditor::resetAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RESETALL", "WorldEditStrings");
}

inline QString WeatherEditor::exportAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_EXPORT", "WorldEditStrings");
}

inline QString WeatherEditor::importAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_IMPORT", "WorldEditStrings");
}

inline QString WeatherEditor::copyObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_COPY", "WorldEditStrings");
}

inline QString WeatherEditor::pasteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_PASTE", "WorldEditStrings");
}

inline QIcon WeatherEditor::tabIcon(QWidget *widget) const
{
	return objectEditor()->source()->sharedData()->worldEditDataIcon("ToolBarIcon_OE_NewUnit", "WorldEditArt", widget);
}

inline QUrl WeatherEditor::copyObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Copy.blp");
}

inline QUrl WeatherEditor::pasteObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Paste.blp");
}

inline QUrl WeatherEditor::newObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Unit.blp");
}

}

}

#endif // WC3LIB_EDITOR_WEATHEREDITOR_HPP
