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

#ifndef WC3LIB_EDITOR_TRIGGEREDITOR_HPP
#define WC3LIB_EDITOR_TRIGGEREDITOR_HPP

#include <QMap>
#include <QTreeWidget>

#include "module.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief The trigger editor is one of the most important modules of the World Editor when creating non-melee maps since it offers you the possibility to react on game events and therefore to define a completely new behaviour of the game.
 *
 * \note Bear in mind that all code of custom text triggers is stored in a separate file named "war3map.wtc" (\ref map::CustomTextTriggers) whereas all trigger data is stored in "war3map.wtg" (\ref map::Triggers). Therefore you have to load both files to make custom text triggers storable. When using editor mode (\ref hasEditor()) both files will be loaded automatically. Alternatively you could use \ref loadFromMap().
 *
 * \sa VariablesDialog
 */
class TriggerEditor : public Module
{
	Q_OBJECT

	public:
		typedef QVector<QTreeWidgetItem*> TreeItems;

		TriggerEditor(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);

		/**
		 * \return Returns empty string if there is no custom text triggers or no valid text entry.
		 * \note \p trigger has to be part of \ref triggers().
		 */
		string triggerText(map::Trigger *trigger) const;

		map::Triggers* triggers() const;
		map::CustomTextTriggers* customTextTriggers() const;
		QTreeWidget* treeWidget() const;
		QTreeWidgetItem* rootItem() const;
		class MapScriptWidget* mapScriptWidget() const;
		class TriggerWidget* triggerWidget() const;
		class VariablesDialog* variablesDialog() const;
		class KActionCollection* triggerActionCollection() const;

	public slots:
		/**
		 * Opens file dialog for loading triggers file (*.wtg).
		 * \sa triggersFilter()
		 */
		void openTriggers();
		/**
		 * Opens file dialog for loading custom text triggers file (*.wtc).
		 * \sa customTextTriggersFilter()
		 */
		void openCustomTextTriggers();
		/**
		 * Similar to \ref clear().
		 */
		void closeTriggers();
		void closeCustomTextTriggers();
		/**
		 * Closes open triggers and custom text triggers.
		 * \sa closeTriggers()
		 * \sa closeCustomTextTriggers()
		 */
		void closeAll();
		void resetTriggers();
		void renameTrigger();
		void showVariables();

		void loadTriggers(map::Triggers *triggers);
		void loadTriggers(Map *map);
		void loadCustomTextTriggers(map::CustomTextTriggers *customTextTriggers);
		void loadCustomTextTriggers(Map *map);
		/**
		 * Loads triggers and custom text triggers from map \p map.
		 * \sa loadTriggers()
		 * \sa loadCustomTextTriggers()
		 */
		void loadFromMap(Map *map);
		/**
		 * \note Clears only triggers not custom text triggers!
		 */
		void clear();
		/**
		 * Displays the custom map script instead of any trigger, category etc..
		 */
		void openMapScript();
		/**
		 * Displays trigger of \p index.
		 */
		void openTrigger(int32 index);
		void openTrigger(map::Trigger *trigger);
		
		void loadTriggerData();

	protected slots:
		void itemClicked(class QTreeWidgetItem *item, int column);

	protected:
		virtual void createFileActions(class KMenu *menu);
		virtual void createEditActions(class KMenu *menu);
		virtual void createMenus(class KMenuBar *menuBar);
		virtual void createWindowsActions(class WindowsMenu *menu);
		virtual void createToolButtons(class ModuleToolBar *toolBar);
		virtual class SettingsInterface* settings();
		virtual KAboutData moduleAboutData() const;
		virtual void onSwitchToMap(Map *map);
		virtual QString actionName() const;

		void setTriggers(map::Triggers *triggers);
		void setCustomTextTriggers(map::CustomTextTriggers *customTextTriggers);
		void setFreeTriggers(bool freeTriggers);
		void setFreeCustomTextTriggers(bool freeCustomTextTriggers);
		/**
		 * \return Returns true if triggers have been allocated for trigger editor only and do not belong to any object. In this case they will be deleted automatically when closed.
		 */
		bool freeTriggers() const;
		bool freeCustomTextTriggers() const;
		TreeItems& categories();
		TreeItems& variables();
		TreeItems& triggerEntries();

	private:
		map::Triggers *m_triggers;
		map::CustomTextTriggers *m_customTextTriggers;
		bool m_freeTriggers;
		bool m_freeCustomTextTriggers;
		TreeItems m_categories;
		TreeItems m_variables;
		TreeItems m_triggerEntries;

		QTreeWidget *m_treeWidget;
		QTreeWidgetItem *m_rootItem;
		class MapScriptWidget *m_mapScriptWidget;
		class TriggerWidget *m_triggerWidget;
		class VariablesDialog *m_variablesDialog;

		KActionCollection *m_triggerActionCollection;
};

inline string TriggerEditor::triggerText(map::Trigger *trigger) const
{
	if (customTextTriggers() == 0)
		return "";

	bool found = false;
	int32 index = 0;

	BOOST_FOREACH(map::Triggers::TriggerEntries::const_reference trig, triggers()->triggers())
	{
		if (&trig == trigger)
		{
			found = true;

			break;
		}

		++index;
	}


	// no corresponding number which should never occur actually
	if (!found)
		return "";

	int32 i = 0;

	BOOST_FOREACH(map::CustomTextTriggers::TriggerTexts::const_reference triggerText, customTextTriggers()->triggerTexts())
	{
		if (i == index)
			return triggerText;

		++i;
	}

	// no corresponding custom trigger text which should never occur actually
	return "";
}

inline map::Triggers* TriggerEditor::triggers() const
{
	return m_triggers;
}

inline map::CustomTextTriggers* TriggerEditor::customTextTriggers() const
{
	return m_customTextTriggers;
}

inline QTreeWidget* TriggerEditor::treeWidget() const
{
	return this->m_treeWidget;
}

inline QTreeWidgetItem* TriggerEditor::rootItem() const
{
	return this->m_rootItem;
}

inline class MapScriptWidget* TriggerEditor::mapScriptWidget() const
{
	return this->m_mapScriptWidget;
}

inline class TriggerWidget* TriggerEditor::triggerWidget() const
{
	return this->m_triggerWidget;
}

inline class VariablesDialog* TriggerEditor::variablesDialog() const
{
	return m_variablesDialog;
}

inline class KActionCollection* TriggerEditor::triggerActionCollection() const
{
	return m_triggerActionCollection;
}

inline KAboutData TriggerEditor::moduleAboutData() const
{
	KAboutData aboutData(Module::moduleAboutData());
	aboutData.setProgramName(ki18n("Trigger Editor"));

	return aboutData;
}

inline QString TriggerEditor::actionName() const
{
	return "triggereditor";
}

inline void TriggerEditor::setTriggers(map::Triggers *triggers)
{
	m_triggers = triggers;
}

inline void TriggerEditor::setCustomTextTriggers(map::CustomTextTriggers *customTextTriggers)
{
	m_customTextTriggers = customTextTriggers;
}

inline void TriggerEditor::setFreeTriggers(bool freeTriggers)
{
	m_freeTriggers = freeTriggers;
}

inline void TriggerEditor::setFreeCustomTextTriggers(bool freeCustomTextTriggers)
{
	m_freeCustomTextTriggers = freeCustomTextTriggers;
}

inline bool TriggerEditor::freeTriggers() const
{
	return m_freeTriggers;
}

inline bool TriggerEditor::freeCustomTextTriggers() const
{
	return m_freeCustomTextTriggers;
}

inline TriggerEditor::TreeItems& TriggerEditor::categories()
{
	return m_categories;
}

inline TriggerEditor::TreeItems& TriggerEditor::variables()
{
	return m_variables;
}

inline TriggerEditor::TreeItems& TriggerEditor::triggerEntries()
{
	return m_triggerEntries;
}

}

}

#endif
