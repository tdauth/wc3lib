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

#ifndef WC3LIB_EDITOR_ABILITYEDITOR_HPP
#define WC3LIB_EDITOR_ABILITYEDITOR_HPP

#include "objecteditortab.hpp"
#include "../abilitydata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

class AbilityEditor : public ObjectEditorTab
{
	public:
		AbilityEditor(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent = nullptr);
		virtual ~AbilityEditor();

		AbilityData* abilityData() const;

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

inline AbilityData* AbilityEditor::abilityData() const
{
	return boost::polymorphic_cast<AbilityData*>(this->objectData());
}

inline QString AbilityEditor::name() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_OBJTAB_ABILITIES", "WorldEditStrings");
}

inline QString AbilityEditor::newObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_NEW", "WorldEditStrings");
}

inline QString AbilityEditor::renameObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_RENAME", "WorldEditStrings");
}

inline QString AbilityEditor::deleteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_DELETE", "WorldEditStrings");
}

inline QString AbilityEditor::resetObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_RESETSEL", "WorldEditStrings");
}

inline QString AbilityEditor::resetAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_RESETALL", "WorldEditStrings");
}

inline QString AbilityEditor::exportAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_EXPORT", "WorldEditStrings");
}

inline QString AbilityEditor::importAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_IMPORT", "WorldEditStrings");
}

inline QString AbilityEditor::copyObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_COPY", "WorldEditStrings");
}

inline QString AbilityEditor::pasteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_PASTE", "WorldEditStrings");
}

inline QIcon AbilityEditor::tabIcon(QWidget *widget) const
{
	return objectEditor()->source()->sharedData()->worldEditDataIcon("ToolBarIcon_OE_NewAbil", "WorldEditArt", widget);
}

inline QUrl AbilityEditor::copyObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Copy.blp");
}

inline QUrl AbilityEditor::pasteObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Paste.blp");
}

inline QUrl AbilityEditor::newObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Unit.blp");
}

}

}

#endif // WC3LIB_EDITOR_ABILITYEDITOR_HPP
