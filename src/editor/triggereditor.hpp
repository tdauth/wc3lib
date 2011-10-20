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

class TriggerEditor : public Module
{
	Q_OBJECT

	public:
		typedef QVector<QTreeWidgetItem*> TreeItems;

		TriggerEditor(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);

		map::Triggers* triggers() const;
		QTreeWidget* treeWidget() const;
		QTreeWidgetItem* rootItem() const;
		class MapScriptWidget* mapScriptWidget() const;
		class TriggerWidget* triggerWidget() const;

	public slots:
		void loadTriggers(map::Triggers *triggers);
		void loadTriggers(Map *map);
		void clear();
		void openMapScript();
		void openTrigger(map::int32 index);
		void openTrigger(map::Trigger *trigger);

	protected slots:
		void itemClicked(class QTreeWidgetItem *item, int column);

	protected:
		virtual void createFileActions(class KMenu *menu);
		virtual void createEditActions(class KMenu *menu);
		virtual void createMenus(class KMenuBar *menuBar);
		virtual void createWindowsActions(class KMenu *menu);
		virtual void createToolButtons(class KToolBar *toolBar);
		virtual class SettingsInterface* settings();
		virtual KAboutData moduleAboutData() const;
		virtual void onSwitchToMap(Map *map);
		virtual QString actionName() const;

		TreeItems& categories();
		TreeItems& variables();
		TreeItems& triggerEntries();

	private:
		map::Triggers *m_triggers;
		TreeItems m_categories;
		TreeItems m_variables;
		TreeItems m_triggerEntries;

		QTreeWidget *m_treeWidget;
		QTreeWidgetItem *m_rootItem;
		class MapScriptWidget *m_mapScriptWidget;
		class TriggerWidget *m_triggerWidget;
};

inline map::Triggers* TriggerEditor::triggers() const
{
	return m_triggers;
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
