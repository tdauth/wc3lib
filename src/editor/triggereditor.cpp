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

#include <QtGui>

#include "triggereditor.hpp"
#include "editor.hpp"
#include "map.hpp"

namespace wc3lib
{

namespace editor
{

TriggerEditor::TriggerEditor(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : Module(source, parent, f)
{
	Ui::TriggerEditor::setupUi(this);
	Module::setupUi();
}

void TriggerEditor::loadTriggers(map::Triggers *triggers)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
	
	if (hasEditor())
	{
		/// \todo get w3x and w3m icon paths
		const KUrl src(editor()->currentMap()->isW3x() ? "" : "");
		QString file;
		item->setText(0, editor()->currentMap()->map()->info()->name().c_str());
		
		if (source()->download(src, file, this))
			item->setIcon(0, QIcon(file));
	}
	else
	{
		item->setText(0, tr("Current map script"));
	}
	
	foreach (map::Triggers::Categories::const_reference category, triggers->categories())
	{
		QTreeWidgetItem *categoryItem = new QTreeWidgetItem();
		categoryItem->setText(0, category.second->name().c_str());
		
		item->addChild(categoryItem);
		m_categories.insert(category.first, categoryItem);
	}
	
	foreach (map::Triggers::TriggerList::const_reference trigger, triggers->triggers())
	{
		QTreeWidgetItem *triggerItem = new QTreeWidgetItem();
		triggerItem->setText(0, trigger->name().c_str());
		
		/// \todo set icon (initially on, disabled etc.)
		
		m_categories[trigger->category()->index()]->addChild(triggerItem);
	}
}

void TriggerEditor::createFileActions(class KMenu *menu)
{
}

void TriggerEditor::createEditActions(class KMenu *menu)
{
}

void TriggerEditor::createMenus(class KMenuBar *menuBar)
{
}

void TriggerEditor::createWindowsActions(class KMenu *menu)
{
}

void TriggerEditor::createToolButtons(class KToolBar *toolBar)
{
}

class SettingsInterface* TriggerEditor::settings()
{
	/// @todo FIXME
	return 0;
}

#include "moc_triggereditor.cpp"

}

}
