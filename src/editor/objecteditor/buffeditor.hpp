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
 *   MERCHANTBUFFITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef WC3LIB_EDITOR_BUFFEDITOR_HPP
#define WC3LIB_EDITOR_BUFFEDITOR_HPP

#include "objecteditortab.hpp"
#include "../buffdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

class KDE_EXPORT BuffEditor : public ObjectEditorTab
{
	public:
		BuffEditor(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~BuffEditor();

		BuffData* buffData() const;

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

inline BuffData* BuffEditor::buffData() const
{
	return boost::polymorphic_cast<BuffData*>(this->objectData());
}

inline QString BuffEditor::name() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_OBJTAB_BUFFS", "WorldEditStrings");
}

inline QString BuffEditor::newObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_BUFF_NEW", "WorldEditStrings");
}

inline QString BuffEditor::renameObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_BUFF_RENAME", "WorldEditStrings");
}

inline QString BuffEditor::deleteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_BUFF_DELETE", "WorldEditStrings");
}

inline QString BuffEditor::resetObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_BUFF_RESETSEL", "WorldEditStrings");
}

inline QString BuffEditor::resetAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_BUFF_RESETALL", "WorldEditStrings");
}

inline QString BuffEditor::exportAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_BUFF_EXPORT", "WorldEditStrings");
}

inline QString BuffEditor::importAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_BUFF_IMPORT", "WorldEditStrings");
}

inline QString BuffEditor::copyObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_BUFF_COPY", "WorldEditStrings");
}

inline QString BuffEditor::pasteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_BUFF_PASTE", "WorldEditStrings");
}

inline QIcon BuffEditor::tabIcon(QWidget *widget) const
{
	return objectEditor()->source()->sharedData()->worldEditDataIcon("ToolBarIcon_OE_NewBuff", "WorldEditArt", widget);
}

inline KUrl BuffEditor::copyObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Copy.blp");
}

inline KUrl BuffEditor::pasteObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Paste.blp");
}

inline KUrl BuffEditor::newObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Unit.blp");
}

}

}

#endif // WC3LIB_EDITOR_BUFFEDITOR_HPP
