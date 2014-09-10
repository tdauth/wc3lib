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
#include <KAction>

#include "../module.hpp"
#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class ObjectEditorTab;
class UnitEditor;
class DoodadEditor;
class DestructibleEditor;
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
class WaterEntryEditor;
class WeatherEntryEditor;
class SoundEntryEditor;

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

	public:
		ObjectEditor(MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~ObjectEditor();

		KTabWidget* tabWidget() const;

		ObjectEditorTab* currentTab() const;

		ObjectEditorTab* tab(int index) const;

		UnitEditor* unitEditor() const;
		DoodadEditor* doodadEditor() const;
		DestructibleEditor* destructibleEditor() const;
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
		WaterEntryEditor* waterEntryEditor() const;
		WeatherEntryEditor* weatherEntryEditor() const;
		SoundEntryEditor* soundEntryEditor() const;

		KAction* newObjectAction() const;
		KAction* renameObjectAction() const;
		KAction* deleteObjectAction() const;
		KAction* resetObjectAction() const;
		KAction* resetAllObjectsAction() const;
		KAction* exportAllObjectsAction() const;
		KAction* importAllObjectsAction() const;
		KAction* copyObjectAction() const;
		KAction* pasteObjectAction() const;
		KAction* modifyFieldAction() const;
		KAction* resetFieldAction() const;

	public slots:
		void exportAll();
		void importAll(const KUrl &url);
		void importAll();

	protected:
		virtual void createFileActions(KMenu *menu);
		virtual void createEditActions(KMenu *menu);
		virtual void createMenus(KMenuBar *menuBar);
		virtual void createWindowsActions(WindowsMenu *menu);
		virtual void createToolButtons(ModuleToolBar *toolBar);
		virtual SettingsInterface* settings();
		virtual void onSwitchToMap(Map *map);
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

		KTabWidget *m_tabWidget;
		// current widgets of corresponding tab widget
		ObjectEditorTab *m_currentTab;

		UnitEditor *m_unitEditor;
		DoodadEditor *m_doodadEditor;
		DestructibleEditor *m_destructibleEditor;
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
		WaterEntryEditor *m_waterEntryEditor;
		WeatherEntryEditor *m_weatherEntryEditor;
		SoundEntryEditor *m_soundEntryEditor;

		KAction *m_newObjectAction;
		KAction *m_renameObjectAction;
		KAction *m_deleteObjectAction;
		KAction *m_resetObjectAction;
		KAction *m_resetAllObjectsAction;
		KAction *m_exportAllObjectsAction;
		KAction *m_importAllObjectsAction;
		KAction *m_copyObjectAction;
		KAction *m_pasteObjectAction;
		KAction *m_modifyFieldAction;
		KAction *m_resetFieldAction;

		QMenu *m_viewMenu;

	private slots:
		void showRawData(bool checked);
};

inline KTabWidget* ObjectEditor::tabWidget() const
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

inline DestructibleEditor* ObjectEditor::destructibleEditor() const
{
	return m_destructibleEditor;
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

inline WaterEntryEditor* ObjectEditor::waterEntryEditor() const
{
	return m_waterEntryEditor;
}

inline WeatherEntryEditor* ObjectEditor::weatherEntryEditor() const
{
	return m_weatherEntryEditor;
}

inline SoundEntryEditor* ObjectEditor::soundEntryEditor() const
{
	return m_soundEntryEditor;
}

inline KAction* ObjectEditor::newObjectAction() const
{
	return m_newObjectAction;
}

inline KAction* ObjectEditor::renameObjectAction() const
{
	return m_renameObjectAction;
}

inline KAction* ObjectEditor::deleteObjectAction() const
{
	return m_deleteObjectAction;
}

inline KAction* ObjectEditor::resetObjectAction() const
{
	return m_resetObjectAction;
}

inline KAction* ObjectEditor::resetAllObjectsAction() const
{
	return m_resetAllObjectsAction;
}

inline KAction* ObjectEditor::exportAllObjectsAction() const
{
	return m_exportAllObjectsAction;
}

inline KAction* ObjectEditor::importAllObjectsAction() const
{
	return m_importAllObjectsAction;
}

inline KAction* ObjectEditor::copyObjectAction() const
{
	return m_copyObjectAction;
}

inline KAction* ObjectEditor::pasteObjectAction() const
{
	return m_pasteObjectAction;
}

inline KAction* ObjectEditor::modifyFieldAction() const
{
	return this->m_modifyFieldAction;
}

inline KAction* ObjectEditor::resetFieldAction() const
{
	return this->m_resetFieldAction;
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
