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

#ifndef WC3LIB_EDITOR_MISCEDITOR_HPP
#define WC3LIB_EDITOR_MISCEDITOR_HPP

#include "objecteditortab.hpp"
#include "../miscdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

class MiscData;

class MiscEditor : public ObjectEditorTab
{
	public:
		public:
		MiscEditor(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~MiscEditor();

		MiscData* miscData() const;

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

inline MiscData* MiscEditor::miscData() const
{
	return boost::polymorphic_cast<MiscData*>(this->objectData());
}

inline QString MiscEditor::name() const
{
	return this->source()->sharedData()->tr("WESTRING_MISCDATADLG_TITLE", "WorldEditStrings");
}

inline QString MiscEditor::newObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_UE_CREATECUSTOMUNIT", "WorldEditStrings");
}

inline QString MiscEditor::renameObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RENAME", "WorldEditStrings");
}

inline QString MiscEditor::deleteObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_DELETE", "WorldEditStrings");
}

inline QString MiscEditor::resetObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RESETSEL", "WorldEditStrings");
}

inline QString MiscEditor::resetAllObjectsText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RESETALL", "WorldEditStrings");
}

inline QString MiscEditor::exportAllObjectsText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_EXPORT", "WorldEditStrings");
}

inline QString MiscEditor::importAllObjectsText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_IMPORT", "WorldEditStrings");
}

inline QString MiscEditor::copyObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_COPY", "WorldEditStrings");
}

inline QString MiscEditor::pasteObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_PASTE", "WorldEditStrings");
}

inline QIcon MiscEditor::tabIcon(QWidget* widget) const
{
	return objectEditor()->source()->sharedData()->worldEditDataIcon("ToolBarIcon_OE_NewItem", "WorldEditArt", widget);
}

inline QUrl MiscEditor::copyObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Copy.blp");
}

inline QUrl MiscEditor::pasteObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Paste.blp");
}

inline QUrl MiscEditor::newObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Unit.blp");
}

}

}

#endif // WC3LIB_EDITOR_MISCEDITOR_HPP
