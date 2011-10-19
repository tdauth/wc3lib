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

#include "module.hpp"
#include "ui/ui_triggereditor.h"
#include "../map.hpp"
#include "editor.hpp"

namespace wc3lib
{

namespace editor
{

class TriggerEditor : public Module, protected Ui::TriggerEditor
{
	Q_OBJECT

	public:
		typedef QMap<map::int32, QTreeWidgetItem*> Categories;

		TriggerEditor(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);

	public slots:
		void loadTriggers(map::Triggers *triggers);
		void clear();

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

		map::Triggers *m_triggers;
		Categories m_categories;
};

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

}

}

#endif
