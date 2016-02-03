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

#ifndef WC3LIB_EDITOR_DESTRUCTABLEEDITOR_HPP
#define WC3LIB_EDITOR_DESTRUCTABLEEDITOR_HPP

#include "objecteditortab.hpp"
#include "../destructabledata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

class KDE_EXPORT DestructableEditor : public ObjectEditorTab
{
	public:
		DestructableEditor(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~DestructableEditor();

		DestructableData* destructableData() const;

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

inline DestructableData* DestructableEditor::destructableData() const
{
	return boost::polymorphic_cast<DestructableData*>(this->objectData());
}

inline QString DestructableEditor::name() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_OBJTAB_DESTRUCTABLES", "WorldEditStrings");
}

inline QString DestructableEditor::newObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DEST_NEW", "WorldEditStrings");
}

inline QString DestructableEditor::renameObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DEST_RENAME", "WorldEditStrings");
}

inline QString DestructableEditor::deleteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DEST_DELETE", "WorldEditStrings");
}

inline QString DestructableEditor::resetObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DEST_RESETSEL", "WorldEditStrings");
}

inline QString DestructableEditor::resetAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_ABIL_RESETALL", "WorldEditStrings");
}

inline QString DestructableEditor::exportAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DEST_EXPORT", "WorldEditStrings");
}

inline QString DestructableEditor::importAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DEST_IMPORT", "WorldEditStrings");
}

inline QString DestructableEditor::copyObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DEST_COPY", "WorldEditStrings");
}

inline QString DestructableEditor::pasteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_DEST_PASTE", "WorldEditStrings");
}

inline QIcon DestructableEditor::tabIcon(QWidget* widget) const
{
	return objectEditor()->source()->sharedData()->worldEditDataIcon("ToolBarIcon_OE_NewDest", "WorldEditArt", widget);
}

inline KUrl DestructableEditor::copyObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Copy.blp");
}

inline KUrl DestructableEditor::pasteObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Paste.blp");
}

inline KUrl DestructableEditor::newObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Unit.blp");
}

}

}

#endif // WC3LIB_EDITOR_DESTRUCTABLEEDITOR_HPP
