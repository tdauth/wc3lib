/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_DOODADEDITOR_HPP
#define WC3LIB_EDITOR_DOODADEDITOR_HPP

#include "objecteditortab.hpp"
#include "../doodaddata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

class DoodadEditor : public ObjectEditorTab
{
	public:
		DoodadEditor(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent = nullptr);
		virtual ~DoodadEditor();

		DoodadData* doodadData() const;

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

inline DoodadData* DoodadEditor::doodadData() const
{
	return boost::polymorphic_cast<DoodadData*>(this->objectData());
}

inline QString DoodadEditor::name() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_OBJTAB_DOODADS", "WorldEditStrings");
}

inline QString DoodadEditor::newObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DOOD_NEW", "WorldEditStrings");
}

inline QString DoodadEditor::renameObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DOOD_RENAME", "WorldEditStrings");
}

inline QString DoodadEditor::deleteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DOOD_DELETE", "WorldEditStrings");
}

inline QString DoodadEditor::resetObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DOOD_RESETSEL", "WorldEditStrings");
}

inline QString DoodadEditor::resetAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_RESETALL", "WorldEditStrings");
}

inline QString DoodadEditor::exportAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DOOD_EXPORT", "WorldEditStrings");
}

inline QString DoodadEditor::importAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DOOD_IMPORT", "WorldEditStrings");
}

inline QString DoodadEditor::copyObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DOOD_COPY", "WorldEditStrings");
}

inline QString DoodadEditor::pasteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DOOD_PASTE", "WorldEditStrings");
}

inline QIcon DoodadEditor::tabIcon(QWidget *widget) const
{
	return objectEditor()->source()->sharedData()->worldEditDataIcon("ToolBarIcon_OE_NewDood", "WorldEditArt", widget);
}

inline QUrl DoodadEditor::copyObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Copy.blp");
}

inline QUrl DoodadEditor::pasteObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Paste.blp");
}

inline QUrl DoodadEditor::newObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Unit.blp");
}

}

}

#endif // WC3LIB_EDITOR_DOODADEDITOR_HPP
