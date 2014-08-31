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
#include "unitmetadata.hpp"
#include "../mpqprioritylist.hpp"
#include "../warcraftiiishared.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief The Frozen Throne Unit Editor module of the Object Editor.
 *
 * \ingroup objectdata
 */
class UnitEditor : public ObjectEditorTab
{
	public:
		UnitEditor(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~UnitEditor();

		UnitMetaData* unitMetaData() const;

		virtual QString name() const;

		virtual void onUpdateCollection(const map::CustomObjects& objects);

		virtual map::CustomUnits customUnits() const override;
		virtual bool hasCustomUnits() const override;
		virtual map::CustomObjects customObjects() const override;
		virtual bool hasCustomObjects() const override;

		virtual map::CustomUnits::Unit currentUnit() const override;
		virtual map::CustomObjects::Object currentObject() const override;

		bool objectIsHero(const QString &objectId) const;
		bool objectIsUnit(const QString &objectId) const;
		bool objectIsBuilding(const QString &objectId) const;

	protected:
		void addItemAsChild(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem *item);

		virtual void setupTreeWidget(ObjectTreeWidget *treeWidget) override;
		virtual void fillTreeItem(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem *item);
		virtual ObjectTableWidget* createTableWidget() override;
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

		virtual void onShowRawData(bool show) override;

		virtual void activateObject(QTreeWidgetItem *item, int column, const QString &originalObjectId, const QString &customObjectId) override;
		virtual void activateFolder(QTreeWidgetItem* item, int column) override;

		virtual QString newObjectText() const override;
		virtual QString renameObjectText() const override;
		virtual QString deleteObjectText() const override;
		virtual QString resetObjectText() const override;
		virtual QString resetAllObjectsText() const override;
		virtual QString exportAllObjectsText() const override;
		virtual QString importAllObjectsText() const override;
		virtual QString copyObjectText() const;
		virtual QString pasteObjectText() const;

		virtual KUrl copyObjectIconUrl() const;
		virtual KUrl pasteObjectIconUrl() const;
		virtual KUrl newObjectIconUrl() const;

		QTreeWidgetItem *m_humanItem;
		QTreeWidgetItem *m_orcItem;
		QTreeWidgetItem *m_undeadItem;
		QTreeWidgetItem *m_nightElfItem;
		QTreeWidgetItem *m_neutralNagaItem;
		QTreeWidgetItem *m_neutralHostileItem;
		QTreeWidgetItem *m_neutralPassiveItem;

		QTreeWidgetItem *m_customHumanItem;
		QTreeWidgetItem *m_customOrcItem;
		QTreeWidgetItem *m_customUndeadItem;
		QTreeWidgetItem *m_customNightElfItem;
		QTreeWidgetItem *m_customNeutralNagaItem;
		QTreeWidgetItem *m_customNeutralHostileItem;
		QTreeWidgetItem *m_customNeutralPassiveItem;
};

inline UnitMetaData* UnitEditor::unitMetaData() const
{
	return boost::polymorphic_cast<UnitMetaData*>(this->metaData());
}

inline QString UnitEditor::name() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_OBJTAB_UNITS", "WorldEditStrings");
}

inline QString UnitEditor::newObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_UE_CREATECUSTOMUNIT", "WorldEditStrings");
}

inline QString UnitEditor::renameObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RENAME", "WorldEditStrings");
}

inline QString UnitEditor::deleteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_DELETE", "WorldEditStrings");
}

inline QString UnitEditor::resetObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RESETSEL", "WorldEditStrings");
}

inline QString UnitEditor::resetAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_RESETALL", "WorldEditStrings");
}

inline QString UnitEditor::exportAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_EXPORT", "WorldEditStrings");
}

inline QString UnitEditor::importAllObjectsText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_IMPORT", "WorldEditStrings");
}

inline QString UnitEditor::copyObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_COPY", "WorldEditStrings");
}

inline QString UnitEditor::pasteObjectText() const
{
	return objectEditor()->source()->sharedData()->tr("WESTRING_MENU_OE_UNIT_PASTE", "WorldEditStrings");
}

inline KUrl UnitEditor::copyObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Copy.blp");
}

inline KUrl UnitEditor::pasteObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Toolbar-Paste.blp");
}

inline KUrl UnitEditor::newObjectIconUrl() const
{
	return KUrl("ReplaceableTextures/WorldEditUI/Editor-Unit.blp");
}

}

}

#endif
