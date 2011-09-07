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
#include "editor.hpp"

namespace wc3lib
{

namespace editor
{

class UnitEditor : public ObjectEditorTab
{
	public:
		UnitEditor(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);

		virtual QString name() const;

	protected:
		virtual class ObjectTreeWidget* createTreeWidget();
		virtual class ObjectTableWidget* createTableWidget();
		virtual void onSwitchToMap(class Map *map);

		virtual void onNewObject();
		virtual void onRenameObject();
		virtual void onDeleteObject();
		virtual void onResetObject();
		virtual void onResetAllObjects();
		virtual void onExportAllObjects();
		virtual void onImportAllObjects();
		virtual void onCopyObject();
		virtual void onPasteObject();

		virtual QString newObjectText() const;
		virtual QString renameObjectText() const;
		virtual QString deleteObjectText() const;
		virtual QString resetObjectText() const;
		virtual QString resetAllObjectsText() const;
		virtual QString exportAllObjectsText() const;
		virtual QString importAllObjectsText() const;
		virtual QString copyObjectText() const;
		virtual QString pasteObjectText() const;

		virtual KUrl copyObjectIconUrl() const;
		virtual KUrl pasteObjectIconUrl() const;
		virtual KUrl newObjectIconUrl() const;

		class MetaData *m_metaData;

		QTreeWidgetItem *m_standardUnitsItem;
		QTreeWidgetItem *m_customUnitsItem;
};

inline QString UnitEditor::name() const
{
	return objectEditor()->editor()->tr("WESTRING_OBJTAB_UNITS", "WorldEditStrings");
}

inline QString UnitEditor::newObjectText() const
{
	return objectEditor()->editor()->tr("WESTRING_MENU_OE_UNIT_NEW", "WorldEditStrings");
}

inline QString UnitEditor::renameObjectText() const
{
	return objectEditor()->editor()->tr("WESTRING_MENU_OE_UNIT_RENAME", "WorldEditStrings");
}

inline QString UnitEditor::deleteObjectText() const
{
	return objectEditor()->editor()->tr("WESTRING_MENU_OE_UNIT_DELETE", "WorldEditStrings");
}

inline QString UnitEditor::resetObjectText() const
{
	return objectEditor()->editor()->tr("WESTRING_MENU_OE_UNIT_RESETSEL", "WorldEditStrings");
}

inline QString UnitEditor::resetAllObjectsText() const
{
	return objectEditor()->editor()->tr("WESTRING_MENU_OE_UNIT_RESETALL", "WorldEditStrings");
}

inline QString UnitEditor::exportAllObjectsText() const
{
	return objectEditor()->editor()->tr("WESTRING_MENU_OE_UNIT_EXPORT", "WorldEditStrings");
}

inline QString UnitEditor::importAllObjectsText() const
{
	return objectEditor()->editor()->tr("WESTRING_MENU_OE_UNIT_IMPORT", "WorldEditStrings");
}

inline QString UnitEditor::copyObjectText() const
{
	return objectEditor()->editor()->tr("WESTRING_MENU_OE_UNIT_COPY", "WorldEditStrings");
}

inline QString UnitEditor::pasteObjectText() const
{
	return objectEditor()->editor()->tr("WESTRING_MENU_OE_UNIT_PASTE", "WorldEditStrings");
}

inline KUrl UnitEditor::copyObjectIconUrl() const
{
	return KUrl("ReplaceableTextures\\WorldEditUI\\Editor-Toolbar-Copy.blp");
}

inline KUrl UnitEditor::pasteObjectIconUrl() const
{
	return KUrl("ReplaceableTextures\\WorldEditUI\\Editor-Toolbar-Paste.blp");
}

inline KUrl UnitEditor::newObjectIconUrl() const
{
	return KUrl("ReplaceableTextures\\WorldEditUI\\Editor-Unit.blp");
}

}

}

#endif
