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

#include <kdemacros.h>
#include <KTabWidget>

#include "../module.hpp"
#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \page objecteditorsection Object Editor
 * The Object Editor is responsible for editting any object data of a map.
 * Object data contains information of types which are used for instances (objects).
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
 * <li>Weather Entry Editor</li>
 * <li>Sound Entry Editor</li>
 * <li>Misc Editor - formerly known as "edit gameplay constants"</li>
 * </ul>
 *
 * \ingroup objectdata
 */
class KDE_EXPORT ObjectEditor : public Module
{
	Q_OBJECT

	signals:
		void objectCreated(map::Object *object);
		void objectRemoved(const map::id &id);
		//void objectValueChanged(const map::ObjectValue &value);

	public:
		typedef QScopedPointer<map::CustomObjectsCollection> Collection;

		ObjectEditor(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~ObjectEditor();

		class KTabWidget* tabWidget() const;

		class ObjectEditorTab* currentTab() const;

		class ObjectEditorTab* tab(int index) const;

		class UnitEditor* unitEditor() const;
		class DoodadEditor* doodadEditor() const;
		class DestructibleEditor* destructibleEditor() const;
		class ItemEditor* itemEditor() const;
		class AbilityEditor* abilityEditor() const;
		class BuffEditor* buffEditor() const;
		class UpgradeEditor* upgradeEditor() const;
		class ModelEntryEditor* modelEntryEditor() const;
		class SkinEntryEditor* skinEntryEditor() const;
		class UbersplatEntryEditor* ubersplatEntryEditor() const;
		class SplatEntryEditor* splatEntryEditor() const;
		class SpawnEntryEditor* spawnEntryEditor() const;
		class LightningEffectEntryEditor* lightningEffectEntryEditor() const;
		class CliffTypeEntryEditor* cliffTypeEntryEditor() const;
		class TilesetEntryEditor* tilesetEntryEditor() const;
		class WaterEntryEditor* waterEntryEditor() const;
		class WeatherEntryEditor* weatherEntryEditor() const;
		class SoundEntryEditor* soundEntryEditor() const;

		class KAction* newObjectAction() const;
		class KAction* renameObjectAction() const;
		class KAction* deleteObjectAction() const;
		class KAction* resetObjectAction() const;
		class KAction* resetAllObjectsAction() const;
		class KAction* exportAllObjectsAction() const;
		class KAction* importAllObjectsAction() const;
		class KAction* copyObjectAction() const;
		class KAction* pasteObjectAction() const;

	public slots:
		void exportAll();
		void importAll(const KUrl &url);
		void importAll();

	protected:
		virtual void createFileActions(class KMenu *menu);
		virtual void createEditActions(class KMenu *menu);
		virtual void createMenus(class KMenuBar *menuBar);
		virtual void createWindowsActions(class WindowsMenu *menu);
		virtual void createToolButtons(class ModuleToolBar *toolBar);
		virtual class SettingsInterface* settings();
		virtual void onSwitchToMap(class Map *map);
		virtual KAboutData moduleAboutData() const;
		virtual QString actionName() const;

	protected slots:
		/**
		 * Updates action textes, icons and corresponding slots to new tab one's.
		 */
		void currentChanged(int index);

	protected:
		void removeCurrentActions();
		void addCurrentActions();

		/**
		 * Updates the GUI of all object editor components.
		 */
		void updateCollection(Collection &collection);

		KTabWidget *m_tabWidget;
		// current widgets of corresponding tab widget
		class ObjectEditorTab *m_currentTab;

		class UnitEditor *m_unitEditor;
		class DoodadEditor *m_doodadEditor;
		class DestructibleEditor *m_destructibleEditor;
		class ItemEditor *m_itemEditor;
		class AbilityEditor *m_abilityEditor;
		class BuffEditor *m_buffEditor;
		class UpgradeEditor *m_upgradeEditor;
		// newly supported
		class ModelEntryEditor *m_modelEntryEditor;
		class SkinEntryEditor *m_skinEntryEditor;
		class UbersplatEntryEditor *m_ubersplatEntryEditor;
		class SplatEntryEditor *m_splatEntryEditor;
		class SpawnEntryEditor *m_spawnEntryEditor;
		class LightningEffectEntryEditor *m_lightningEffectEntryEditor;
		class CliffTypeEntryEditor *m_cliffTypeEntryEditor;
		class TilesetEntryEditor *m_tilesetEntryEditor;
		class WaterEntryEditor *m_waterEntryEditor;
		class WeatherEntryEditor *m_weatherEntryEditor;
		class SoundEntryEditor *m_soundEntryEditor;

		class KAction *m_newObjectAction;
		class KAction *m_renameObjectAction;
		class KAction *m_deleteObjectAction;
		class KAction *m_resetObjectAction;
		class KAction *m_resetAllObjectsAction;
		class KAction *m_exportAllObjectsAction;
		class KAction *m_importAllObjectsAction;
		class KAction *m_copyObjectAction;
		class KAction *m_pasteObjectAction;

		Collection m_collection;
};

inline class KTabWidget* ObjectEditor::tabWidget() const
{
	return m_tabWidget;
}

inline class ObjectEditorTab* ObjectEditor::currentTab() const
{
	return m_currentTab;
}


inline class UnitEditor* ObjectEditor::unitEditor() const
{
	return m_unitEditor;
}

inline class DoodadEditor* ObjectEditor::doodadEditor() const
{
	return m_doodadEditor;
}

inline class DestructibleEditor* ObjectEditor::destructibleEditor() const
{
	return m_destructibleEditor;
}

inline class ItemEditor* ObjectEditor::itemEditor() const
{
	return m_itemEditor;
}

inline class AbilityEditor* ObjectEditor::abilityEditor() const
{
	return m_abilityEditor;
}

inline class BuffEditor* ObjectEditor::buffEditor() const
{
	return m_buffEditor;
}

inline class UpgradeEditor* ObjectEditor::upgradeEditor() const
{
	return m_upgradeEditor;
}

inline class ModelEntryEditor* ObjectEditor::modelEntryEditor() const
{
	return m_modelEntryEditor;
}

inline class SkinEntryEditor* ObjectEditor::skinEntryEditor() const
{
	return m_skinEntryEditor;
}

inline class UbersplatEntryEditor* ObjectEditor::ubersplatEntryEditor() const
{
	return m_ubersplatEntryEditor;
}

inline class SplatEntryEditor* ObjectEditor::splatEntryEditor() const
{
	return m_splatEntryEditor;
}

inline class SpawnEntryEditor* ObjectEditor::spawnEntryEditor() const
{
	return m_spawnEntryEditor;
}

inline class LightningEffectEntryEditor* ObjectEditor::lightningEffectEntryEditor() const
{
	return m_lightningEffectEntryEditor;
}

inline class CliffTypeEntryEditor* ObjectEditor::cliffTypeEntryEditor() const
{
	return m_cliffTypeEntryEditor;
}

inline class TilesetEntryEditor* ObjectEditor::tilesetEntryEditor() const
{
	return m_tilesetEntryEditor;
}

inline class WaterEntryEditor* ObjectEditor::waterEntryEditor() const
{
	return m_waterEntryEditor;
}

inline class WeatherEntryEditor* ObjectEditor::weatherEntryEditor() const
{
	return m_weatherEntryEditor;
}

inline class SoundEntryEditor* ObjectEditor::soundEntryEditor() const
{
	return m_soundEntryEditor;
}

inline class KAction* ObjectEditor::newObjectAction() const
{
	return m_newObjectAction;
}

inline class KAction* ObjectEditor::renameObjectAction() const
{
	return m_renameObjectAction;
}

inline class KAction* ObjectEditor::deleteObjectAction() const
{
	return m_deleteObjectAction;
}

inline class KAction* ObjectEditor::resetObjectAction() const
{
	return m_resetObjectAction;
}

inline class KAction* ObjectEditor::resetAllObjectsAction() const
{
	return m_resetAllObjectsAction;
}

inline class KAction* ObjectEditor::exportAllObjectsAction() const
{
	return m_exportAllObjectsAction;
}

inline class KAction* ObjectEditor::importAllObjectsAction() const
{
	return m_importAllObjectsAction;
}

inline class KAction* ObjectEditor::copyObjectAction() const
{
	return m_copyObjectAction;
}

inline class KAction* ObjectEditor::pasteObjectAction() const
{
	return m_pasteObjectAction;
}

inline KAboutData ObjectEditor::moduleAboutData() const
{
	KAboutData aboutData(Module::moduleAboutData());
	aboutData.setProgramName(ki18n("Object Editor"));

	return aboutData;
}

inline QString ObjectEditor::actionName() const
{
	return "objecteditor";
}

}

}

#endif
