/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_OBJECTEDITOR_HPP
#define WC3LIB_EDITOR_OBJECTEDITOR_HPP

#include <QVariant>
#include <QTabWidget>

#include "../module.hpp"
#include "../mpqprioritylist.hpp"
#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class ObjectEditorTab;
class UnitEditor;
class DoodadEditor;
class DestructableEditor;
class ItemEditor;
class AbilityEditor;
class BuffEditor;
class UpgradeEditor;
// newly supported
class ModelEntryEditor;
class SkinEntryEditor;
class UbersplatEntryEditor;
class SplatEntryEditor;
class SpawnEntryEditor;
class LightningEffectEntryEditor;
class CliffTypeEntryEditor;
class TilesetEntryEditor;
class WaterEditor;
class WeatherEditor;
class SoundEntryEditor;
class MiscEditor;
class SkinEditor;

/**
 * \page objecteditorsection Object Editor
 * The Object Editor is responsible for editting any object data of a map.
 * Object data contains information of types which are used for instances (objects).
 *
 * There is the central abstract \ref ObjectData which stores standard and custom objects and allows retrieving information about all field types etc. etc.
 * For concrete object data such as units or items it has to be derived.
 * The following classes exist for concrete object data:
 * <ul>
 * <li>\ref UnitData - Units</li>
 * <li>\ref ItemData - Items</li>
 * <li>\ref DestructableData - Destructables</li>
 * <li>\ref DoodadData - Doodads</li>
 * <li>\ref AbilityData - Abilities</li>
 * <li>\ref BuffData - Buffs/Effects</li>
 * <li>\ref UpgradeData - Upgrades</li>
 * <li>\ref WeatherData - Weather Effects</li>
 * <li>\ref MiscData - Misc Data</li>
 * <li>\ref SkinData - Game User Interface Data</li>
 * </ul>
 *
 * The object editor itself is represented by the class \ref ObjectEditor.
 * For each kind of concrete objects it provides a single tab which is derived from \ref ObjectEditorTab.
 * This is a list of all available tabs;
 * <ul>
 * <li>\ref UnitEditor - Units</li>
 * <li>\ref ItemEditor - Items</li>
 * <li>\ref DestructableEditor - Destructables</li>
 * <li>\ref AbilityEditor - Abilities</li>
 * <li>\ref BuffEditor - Buffs/Effects</li>
 * <li>\ref UpgradeEditor - Upgrades</li>
 * <li>\ref WeatherEditor - Weather Effects</li>
 * <li>\ref MiscEditor - Game Misc Data</li>
 * <li>\ref SkinEditor - Game User Interface Data</li>
 * </ul>
 *
 * One single tab provides two views. At the left edge there is a tree view listing all objects. At the right edge there is a table view listing the field values
 * of the currently selected object from the tree view.
 * The two classes that provide these views are \ref ObjectTreeView and \ref ObjectTableView.
 *
 *
 * As the MVC concept of abstract item views in Qt requires the developer to provide an item model for the views there is the abstract class
 * \ref ObjectTreeModel for the tree view's model.
 * It has to be derived as well for concrete object data.
 * The following classes derive from the model and provide concrete object data as model:
 * <ul>
 * <li>\ref UnitTreeModel - Units</li>
 * </ul>
 *
 *
 * There is a bunch of supporting GUI dialogs to select different types of field values for objects.
 * \ref ObjectValueDialog allows the user to select a new field value of any type.
 * With \ref ObjectValueDialog::show() you can display an instance of the dialog and retrieve a new value.
 *
 * For new objects the unique object ID of four letters/numbers is precalculated but can be specified manually as well using a widget of type \ref ObjectIdDialog.
 *
 * \ref UnitSelectionDialog allows selecting a standard or custom unit listing all unit icons and separating them by their races and other properties.
 *
 * \ingroup objectdata
 */

/**
 * Supports usual tabbed object editors:
 * <ul>
 * <li>Unit Editor</li>
 * <li>Doodad Editor</li>
 * <li>Destructible Editor</li>
 * <li>Item Editor</li>
 * <li>Ability Editor</li>
 * <li>Buff Editor</li>
 * <li>Upgrade Editor</li>
 * </ul>
 * Additionally it supports newly added object editors (SLK-based):
 * <ul>
 * <li>Model Entry Editor</li>
 * <li>Skin Entry Editor</li>
 * <li>Uberslpat Entry Editor</li>
 * <li>Splat Entry Editor</li>
 * <li>Spawn Entry Editor</li>
 * <li>Lightning Effect Entry Editor</li>
 * <li>Cliff Type Entry Editor</li>
 * <li>Tileset Entry Editor</li>
 * <li>Water Entry Editor</li>
 * <li>Weather Editor</li>
 * <li>Sound Entry Editor</li>
 * <li>Misc Editor - formerly known as "edit gameplay constants"</li>
 * <li>Skin Editor - formerly known as "edit custom interface"</li>
 * </ul>
 *
 * \note The tab loads the object data \p objectData on request when it is shown for the first time. This should reduce performance on start up of the object editor. Besides it is the same behaviour as in the World Editor.
 *
 * \ingroup objectdata
 */
class ObjectEditor : public Module
{
	Q_OBJECT

	public:
		/**
		 * \brief All tab indices in correct order.
		 */
		enum Tab
		{
			Units,
			Items,
			Destructibles,
			Doodads,
			Abilities,
			Buffs,
			Upgrades,
			Water,
			Weather,
			GameplayConstants,
			Skin
		};

		ObjectEditor(MpqPriorityList *source, const QString &organization, const QString &applicationName, QWidget *parent = nullptr);
		virtual ~ObjectEditor();

		QTabWidget* tabWidget() const;

		ObjectEditorTab* currentTab() const;

		ObjectEditorTab* tab(int index) const;

		UnitEditor* unitEditor() const;
		DoodadEditor* doodadEditor() const;
		DestructableEditor* destructableEditor() const;
		ItemEditor* itemEditor() const;
		AbilityEditor* abilityEditor() const;
		BuffEditor* buffEditor() const;
		UpgradeEditor* upgradeEditor() const;
		ModelEntryEditor* modelEntryEditor() const;
		SkinEntryEditor* skinEntryEditor() const;
		UbersplatEntryEditor* ubersplatEntryEditor() const;
		SplatEntryEditor* splatEntryEditor() const;
		SpawnEntryEditor* spawnEntryEditor() const;
		LightningEffectEntryEditor* lightningEffectEntryEditor() const;
		CliffTypeEntryEditor* cliffTypeEntryEditor() const;
		TilesetEntryEditor* tilesetEntryEditor() const;
		WaterEditor* waterEditor() const;
		WeatherEditor* weatherEditor() const;
		SoundEntryEditor* soundEntryEditor() const;
		MiscEditor* miscEditor() const;
		SkinEditor* skinEditor() const;

		QAction* newObjectAction() const;
		QAction* renameObjectAction() const;
		QAction* deleteObjectAction() const;
		QAction* resetObjectAction() const;
		QAction* resetAllObjectsAction() const;
		QAction* exportAllObjectsAction() const;
		QAction* importAllObjectsAction() const;
		QAction* copyObjectAction() const;
		QAction* pasteObjectAction() const;
		QAction* modifyFieldAction() const;
		QAction* resetFieldAction() const;
		QAction* compressAction() const;
		QAction* validateAction() const;

		virtual bool configure() override;
		virtual void retranslateUi() override;

		void importCustomObjectsCollection(const map::CustomObjectsCollection &collection);
		void importCustomUnits(const map::CustomUnits &customUnits);

		void exportCustomObjectsCollection(map::CustomObjectsCollection &collection);

	public slots:
		/**
		 * Exports all objects from all object editor tabs as \ref wc3lib::map::CustomObjectsCollection (.w3o) file.
		 */
		void exportAll();
		void importAll(const QUrl &url);
		/**
		 * Imports all objects from a \ref wc3lib::map::CustomObjectsCollection (.w3o) file into the different object editor tabs.
		 */
		void importAll();

		/**
		 * Compresses the objects of all object editor tabs.
		 */
		void compressAll();
		void widgetizeAll();

		void find();
		void findNext();
		void findPrevious();

		/**
		 * Compresses the object data of the currently open tab.
		 */
		void compress();
		void validate();
		/**
		 * Widgetizes the object data of the currently open tab.
		 */
		void widgetize();

	protected:
		virtual void createFileActions(QMenu *menu) override;
		virtual void createEditActions(QMenu *menu) override;
		virtual void createMenus(QMenuBar *menuBar) override;
		virtual void createWindowsActions(WindowsMenu *menu) override;
		virtual void createToolButtons(ModuleToolBar *toolBar) override;
		virtual SettingsInterface* settings() override;
		virtual void onSwitchToMap(Map *map) override;
		virtual QString actionName() const override;
		virtual QIcon icon() override;

	protected slots:
		/**
		 * Updates action textes, icons and corresponding slots to new tab one's.
		 */
		void currentChanged(int index);

	protected:
		void loadTabDataOnRequest(int index);

		void removeCurrentActions();
		void addCurrentActions();

		QTabWidget *m_tabWidget;
		// current widgets of corresponding tab widget
		ObjectEditorTab *m_currentTab;

		UnitEditor *m_unitEditor;
		DoodadEditor *m_doodadEditor;
		DestructableEditor *m_destructableEditor;
		ItemEditor *m_itemEditor;
		AbilityEditor *m_abilityEditor;
		BuffEditor *m_buffEditor;
		UpgradeEditor *m_upgradeEditor;
		// newly supported
		ModelEntryEditor *m_modelEntryEditor;
		SkinEntryEditor *m_skinEntryEditor;
		UbersplatEntryEditor *m_ubersplatEntryEditor;
		SplatEntryEditor *m_splatEntryEditor;
		SpawnEntryEditor *m_spawnEntryEditor;
		LightningEffectEntryEditor *m_lightningEffectEntryEditor;
		CliffTypeEntryEditor *m_cliffTypeEntryEditor;
		TilesetEntryEditor *m_tilesetEntryEditor;
		WaterEditor *m_waterEditor;
		WeatherEditor *m_weatherEditor;
		SoundEntryEditor *m_soundEntryEditor;
		MiscEditor *m_miscEditor;
		SkinEditor *m_skinEditor;

		QAction *m_newObjectAction;
		QAction *m_renameObjectAction;
		QAction *m_deleteObjectAction;
		QAction *m_resetObjectAction;
		QAction *m_resetAllObjectsAction;
		QAction *m_exportAllObjectsAction;
		QAction *m_importAllObjectsAction;
		QAction *m_exportAllAction;
		QAction *m_importAllAction;
		QAction *m_compressAllAction;
		QAction *m_widgetizeAllAction;
		QAction *m_copyObjectAction;
		QAction *m_pasteObjectAction;
		QAction *m_viewInPaletteAction;
		QAction *m_findAction;
		QAction *m_findNextAction;
		QAction *m_findPreviousAction;
		QAction *m_autoFillAction;
		QAction *m_modifyFieldAction;
		QAction *m_resetFieldAction;
		QAction *m_compressAction;
		QAction *m_validateAction;
		QAction *m_widgetizeAction;

		QMenu *m_viewMenu;
		QAction *m_rawDataAction;
		QAction *m_sortByNameAction;

	private slots:
		void showRawData(bool checked);
		void sortByName(bool checked);
};

inline QTabWidget* ObjectEditor::tabWidget() const
{
	return m_tabWidget;
}

inline ObjectEditorTab* ObjectEditor::currentTab() const
{
	return m_currentTab;
}


inline UnitEditor* ObjectEditor::unitEditor() const
{
	return m_unitEditor;
}

inline DoodadEditor* ObjectEditor::doodadEditor() const
{
	return m_doodadEditor;
}

inline DestructableEditor* ObjectEditor::destructableEditor() const
{
	return m_destructableEditor;
}

inline ItemEditor* ObjectEditor::itemEditor() const
{
	return m_itemEditor;
}

inline AbilityEditor* ObjectEditor::abilityEditor() const
{
	return m_abilityEditor;
}

inline BuffEditor* ObjectEditor::buffEditor() const
{
	return m_buffEditor;
}

inline UpgradeEditor* ObjectEditor::upgradeEditor() const
{
	return m_upgradeEditor;
}

inline ModelEntryEditor* ObjectEditor::modelEntryEditor() const
{
	return m_modelEntryEditor;
}

inline SkinEntryEditor* ObjectEditor::skinEntryEditor() const
{
	return m_skinEntryEditor;
}

inline UbersplatEntryEditor* ObjectEditor::ubersplatEntryEditor() const
{
	return m_ubersplatEntryEditor;
}

inline SplatEntryEditor* ObjectEditor::splatEntryEditor() const
{
	return m_splatEntryEditor;
}

inline SpawnEntryEditor* ObjectEditor::spawnEntryEditor() const
{
	return m_spawnEntryEditor;
}

inline LightningEffectEntryEditor* ObjectEditor::lightningEffectEntryEditor() const
{
	return m_lightningEffectEntryEditor;
}

inline CliffTypeEntryEditor* ObjectEditor::cliffTypeEntryEditor() const
{
	return m_cliffTypeEntryEditor;
}

inline TilesetEntryEditor* ObjectEditor::tilesetEntryEditor() const
{
	return m_tilesetEntryEditor;
}

inline WaterEditor* ObjectEditor::waterEditor() const
{
	return m_waterEditor;
}

inline WeatherEditor* ObjectEditor::weatherEditor() const
{
	return m_weatherEditor;
}

inline SoundEntryEditor* ObjectEditor::soundEntryEditor() const
{
	return m_soundEntryEditor;
}

inline MiscEditor* ObjectEditor::miscEditor() const
{
	return this->m_miscEditor;
}

inline SkinEditor* ObjectEditor::skinEditor() const
{
	return this->m_skinEditor;
}

inline QAction* ObjectEditor::newObjectAction() const
{
	return m_newObjectAction;
}

inline QAction* ObjectEditor::renameObjectAction() const
{
	return m_renameObjectAction;
}

inline QAction* ObjectEditor::deleteObjectAction() const
{
	return m_deleteObjectAction;
}

inline QAction* ObjectEditor::resetObjectAction() const
{
	return m_resetObjectAction;
}

inline QAction* ObjectEditor::resetAllObjectsAction() const
{
	return m_resetAllObjectsAction;
}

inline QAction* ObjectEditor::exportAllObjectsAction() const
{
	return m_exportAllObjectsAction;
}

inline QAction* ObjectEditor::importAllObjectsAction() const
{
	return m_importAllObjectsAction;
}

inline QAction* ObjectEditor::copyObjectAction() const
{
	return m_copyObjectAction;
}

inline QAction* ObjectEditor::pasteObjectAction() const
{
	return m_pasteObjectAction;
}

inline QAction* ObjectEditor::modifyFieldAction() const
{
	return this->m_modifyFieldAction;
}

inline QAction* ObjectEditor::resetFieldAction() const
{
	return this->m_resetFieldAction;
}

inline QAction* ObjectEditor::compressAction() const
{
	return this->m_compressAction;
}

inline QAction* ObjectEditor::validateAction() const
{
	return this->m_validateAction;
}

inline QString ObjectEditor::actionName() const
{
	return "objecteditor";
}

}

}

#endif
