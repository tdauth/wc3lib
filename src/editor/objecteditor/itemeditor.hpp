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

#ifndef WC3LIB_EDITOR_ITEMEDITOR_HPP
#define WC3LIB_EDITOR_ITEMEDITOR_HPP

#include "objecteditortab.hpp"
#include "../itemdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

class ItemData;

class KDE_EXPORT ItemEditor : public ObjectEditorTab
{
	public:
		ItemEditor(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~ItemEditor();

		ItemData* itemData() const;

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

		virtual KUrl copyObjectIconUrl() const override;
		virtual KUrl pasteObjectIconUrl() const override;
		virtual KUrl newObjectIconUrl() const override;
};

inline ItemData* ItemEditor::itemData() const
{
	return boost::polymorphic_cast<ItemData*>(this->objectData());
}

inline QString ItemEditor::name() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_OBJTAB_ITEMS", "WorldEditStrings");
}

inline QString ItemEditor::newObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ITEM_NEW", "WorldEditStrings");
}

inline QString ItemEditor::renameObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ITEM_RENAME", "WorldEditStrings");
}

inline QString ItemEditor::deleteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ITEM_DELETE", "WorldEditStrings");
}

inline QString ItemEditor::resetObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ITEM_RESETSEL", "WorldEditStrings");
}

inline QString ItemEditor::resetAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ITEM_RESETALL", "WorldEditStrings");
}

inline QString ItemEditor::exportAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ITEM_EXPORT", "WorldEditStrings");
}

inline QString ItemEditor::importAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ITEM_IMPORT", "WorldEditStrings");
}

inline QString ItemEditor::copyObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ITEM_COPY", "WorldEditStrings");
}

inline QString ItemEditor::pasteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ITEM_PASTE", "WorldEditStrings");
}

inline QIcon ItemEditor::tabIcon(QWidget *widget) const
{
	return objectEditor()->source()->sharedData()->worldEditDataIcon("ToolBarIcon_OE_NewItem", "WorldEditArt", widget);
}

inline KUrl ItemEditor::copyObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Copy.blp");
}

inline KUrl ItemEditor::pasteObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Paste.blp");
}

inline KUrl ItemEditor::newObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Unit.blp");
}

}

}

#endif // WC3LIB_EDITOR_ITEMEDITOR_HPP
