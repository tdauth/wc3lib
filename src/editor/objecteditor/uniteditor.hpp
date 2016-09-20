/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_UNITEDITOR_HPP
#define WC3LIB_EDITOR_UNITEDITOR_HPP

#include "objecteditortab.hpp"
#include "../unitdata.hpp"
#include "../mpqprioritylist.hpp"
#include "../warcraftiiishared.hpp"

namespace wc3lib
{

namespace editor
{

class UnitSelectionDialog;

/**
 * \brief The Frozen Throne Unit Editor module of the Object Editor.
 *
 * \ingroup objectdata
 */
class UnitEditor : public ObjectEditorTab
{
	public:
		UnitEditor(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~UnitEditor();

		UnitData* unitData() const;

		virtual QString name() const override;

		UnitSelectionDialog* unitSelectionDialog() const;

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

		UnitSelectionDialog *m_unitSelectionDialog;
};

inline UnitData* UnitEditor::unitData() const
{
	return boost::polymorphic_cast<UnitData*>(this->objectData());
}

inline QString UnitEditor::name() const
{
	return this->source()->sharedData()->tr("WESTRING_OBJTAB_UNITS", "WorldEditStrings");
}

inline UnitSelectionDialog* UnitEditor::unitSelectionDialog() const
{
	return this->m_unitSelectionDialog;
}

inline QString UnitEditor::newObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_UE_CREATECUSTOMUNIT", "WorldEditStrings");
}

inline QString UnitEditor::renameObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RENAME", "WorldEditStrings");
}

inline QString UnitEditor::deleteObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_DELETE", "WorldEditStrings");
}

inline QString UnitEditor::resetObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RESETSEL", "WorldEditStrings");
}

inline QString UnitEditor::resetAllObjectsText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RESETALL", "WorldEditStrings");
}

inline QString UnitEditor::exportAllObjectsText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_EXPORT", "WorldEditStrings");
}

inline QString UnitEditor::importAllObjectsText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_IMPORT", "WorldEditStrings");
}

inline QString UnitEditor::copyObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_COPY", "WorldEditStrings");
}

inline QString UnitEditor::pasteObjectText() const
{
	return this->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_PASTE", "WorldEditStrings");
}

inline QIcon UnitEditor::tabIcon(QWidget *widget) const
{
	return objectEditor()->source()->sharedData()->worldEditDataIcon("ToolBarIcon_OE_NewUnit", "WorldEditArt", widget);
}

inline QUrl UnitEditor::copyObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Copy.blp");
}

inline QUrl UnitEditor::pasteObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Paste.blp");
}

inline QUrl UnitEditor::newObjectIconUrl() const
{
	return QUrl("ReplaceableTextures/WorldEditUI/Editor-Unit.blp");
}

}

}

#endif
