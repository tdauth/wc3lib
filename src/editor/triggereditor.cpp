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
#include "mapscriptwidget.hpp"
#include "triggerwidget.hpp"
#include "editor.hpp"
#include "map.hpp"

namespace wc3lib
{

namespace editor
{

TriggerEditor::TriggerEditor(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : m_treeWidget(new QTreeWidget(this)), m_mapScriptWidget(new MapScriptWidget(this)), m_triggerWidget(new TriggerWidget(this)), Module(source, parent, f)
{
	Module::setupUi();
	QHBoxLayout *hLayout = new QHBoxLayout(this);
	hLayout->addWidget(treeWidget());
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->addWidget(mapScriptWidget());
	vLayout->addWidget(triggerWidget());
	hLayout->addLayout(vLayout);
	topLayout()->addLayout(hLayout);
	treeWidget()->setHeaderHidden(true);
	//centerLayout()->addWidget(triggerWidget());

	connect(treeWidget(), SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*,int)));

	triggerWidget()->setEnabled(false);
	mapScriptWidget()->hide();

	if (hasEditor())
	{
		connect(editor(), SIGNAL(openedMap(Map*)), this, SLOT(loadTriggers(Map*)));
	}
}

void TriggerEditor::loadTriggers(map::Triggers *triggers)
{
	clear();
	m_rootItem= new QTreeWidgetItem(treeWidget());

	if (hasEditor())
	{
		/// \todo get w3x and w3m icon paths
		const KUrl src(editor()->currentMap()->isW3x() ? "" : "");
		QString file;
		rootItem()->setText(0, editor()->currentMap()->map()->info()->name().c_str());

		if (source()->download(src, file, this))
			rootItem()->setIcon(0, QIcon(file));
	}
	else
	{
		rootItem()->setText(0, tr("Current map script"));
	}

	const map::int32 categoriesNumber = triggers->categories().size();
	const map::int32 triggersNumber = triggers->triggers().size();
	categories().resize(categoriesNumber);


	for (map::int32 i = 0; i < categoriesNumber; ++i)
	{
		categories()[i] = new QTreeWidgetItem(rootItem());
		categories()[i]->setText(0, triggers->categories()[i]->name().c_str());
		/// \todo set folder icon
	}

	triggerEntries().resize(triggersNumber);

	for (map::int32 i = 0; i < triggersNumber; ++i)
	{
		triggerEntries()[i] = new QTreeWidgetItem(rootItem());
		/// \todo set icon (initially on, disabled etc.)
		triggerEntries()[i]->setText(0, triggers->triggers()[i]->name().c_str());
	}

	m_triggers = triggers;
}

void TriggerEditor::loadTriggers(Map *map)
{
	loadTriggers(map->map()->triggers().get());
}

void TriggerEditor::clear()
{
	if (m_triggers == 0)
		return;

	m_treeWidget->clear();
	m_categories.clear();
	m_variables.clear();
	m_triggerEntries.clear();
	m_triggers = 0;
	m_rootItem = 0;
}

void TriggerEditor::openMapScript()
{
	this->triggerWidget()->hide();
	this->mapScriptWidget()->show();
}

void TriggerEditor::openTrigger(map::int32 index)
{
	openTrigger(triggers()->triggers()[index].get());
}

void TriggerEditor::openTrigger(map::Trigger *trigger)
{
	triggerWidget()->showTrigger(trigger);
	triggerWidget()->setEnabled(true);
}

void TriggerEditor::itemClicked(QTreeWidgetItem *item, int column)
{
	if (item == rootItem())
	{
		openMapScript();
	}
	else if (categories().contains(item))
	{
		triggerWidget()->hide();
		mapScriptWidget()->hide();
	}
	else if (triggerEntries().contains(item))
	{
		openTrigger(rootItem()->indexOfChild(item));
	}
	else
		qDebug() << "Unknown item: " << item->text(0);
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

void TriggerEditor::onSwitchToMap(Map *map)
{
}

#include "moc_triggereditor.cpp"

}

}
