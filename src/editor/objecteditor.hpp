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

#include <KTabWidget>

#include "module.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

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
 * <li>Skin Editor</li>
 * <li>Uberslpat Editor</li>
 * <li>Slpat Editor</li>
 * <li>Spawn Editor</li>
 * <li>Lightning Effect Editor</li>
 * <li>Cliff Type Editor</li>
 * <li>Tileset Editor</li>
 * <li>Water Editor</li>
 * <li>Weather Editor</li>
 * <li>Sound Entry Editor</li>
 * <li>Misc Editor - formerly known as "edit gameplay constants"</li>
 * </ul>
 */
class ObjectEditor : public Module
{
	Q_OBJECT
	
	signals:
		void objectCreated(map::Object *object);
		void objectRemoved(const map::id &id);
		//void objectValueChanged(const map::ObjectValue &value);
	
	public:
		static QVariant valueToVariant(const map::Value &value);
		
		ObjectEditor(class Editor *editor);
		~ObjectEditor();
		
		class KTabWidget* tabWidget() const;
		
		class ObjectEditorTab* currentTab() const;
		
		class ObjectEditorTab* tab(int index) const;
		
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
		void importAll();

	protected:
		virtual void createFileActions(class KMenu *menu);
		virtual void createEditActions(class KMenu *menu);
		virtual void createMenus(class KMenuBar *menuBar);
		virtual void createWindowsActions(class KMenu *menu);
		virtual void createToolButtons(class KToolBar *toolBar);
		virtual class SettingsInterface* settings();
		
	protected slots:
		/**
		 * Updates action textes, icons and corresponding slots to new tab one's.
		 */
		void currentChanged(int index);
	
	protected:
		
		QList<map::Object> m_objects;
		
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
		class SkinEditor *m_skinEditor;
		class UbersplatEditor *m_ubersplatEditor;
		class SlpatEditor *m_splatEditor;
		class SpawnEditor *m_spawnEditor;
		class LightningEffectEditor *m_lightningEffectEditor;
		class CliffTypeEditor *m_cliffTypeEditor;
		class TilesetEditor *m_tilesetEditor;
		class WaterEditor *m_waterEditor;
		class WeatherEditor *m_weatherEditor;
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
};

namespace
{

inline QList<QVariant> stringList(const map::List &list)
{
	QList<QVariant> result;
	
	foreach (const std::string &value, list)
		result << QVariant(value.c_str());
	
	return result;
}

}

inline QVariant ObjectEditor::valueToVariant(const map::Value &value)
{
	switch (value.type())
	{
		case map::Value::Integer:
			return QVariant(value.toInteger());
			
		case map::Value::Real:
			return QVariant(value.toReal());
			
		case map::Value::Unreal:
			return QVariant(value.toUnreal());
			
		case map::Value::String:
			return QVariant(value.toString().c_str());
			
		case map::Value::Boolean:
			return QVariant(value.toBoolean());
			
		case map::Value::Character:
			return QVariant(value.toCharacter());
			
		case map::Value::UnitList:
			return QVariant(stringList(value.toUnitList()));
			
		case map::Value::ItemList:
			return QVariant(stringList(value.toItemList()));
			
		case map::Value::RegenerationType:
			return QVariant(value.toRegenerationType().c_str());
			
		case map::Value::AttackType:
			return QVariant(value.toAttackType().c_str());
			
		case map::Value::WeaponType:
			return QVariant(value.toWeaponType().c_str());
			
		case map::Value::TargetType:
			return QVariant(value.toTargetType().c_str());
			
		case map::Value::MoveType:
			return QVariant(value.toMoveType().c_str());
			
		case map::Value::DefenseType:
			return QVariant(value.toDefenseType().c_str());
			
		case map::Value::PathingTexture:
			return QVariant(value.toPathingTexture().c_str());
			
		case map::Value::UpgradeList:
			return QVariant(stringList(value.toUpgradeList()));
			
		case map::Value::StringList:
			return QVariant(stringList(value.toStringList()));
			
		case map::Value::AbilityList:
			return QVariant(stringList(value.toAbilityList()));
			
		case map::Value::HeroAbilityList:
			return QVariant(stringList(value.toHeroAbilityList()));
			
		case map::Value::MissileArt:
			return QVariant(value.toMissileArt().c_str());
			
		case map::Value::AttributeType:
			return QVariant(value.toAttributeType().c_str());
			
		case map::Value::AttackBits:
			return QVariant(value.toAttackBits().c_str());
	}
	
	return QVariant();
}

inline class KTabWidget* ObjectEditor::tabWidget() const
{
	return m_tabWidget;
}

inline class ObjectEditorTab* ObjectEditor::currentTab() const
{
	return m_currentTab;
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

}

}

#endif
