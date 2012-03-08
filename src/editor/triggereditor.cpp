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

#include <boost/filesystem/fstream.hpp>

#include <QtGui>

#include <KFileDialog>
#include <KMessageBox>
#include <KMenu>
#include <KAction>
#include <KActionCollection>

#include "triggereditor.hpp"
#include "mapscriptwidget.hpp"
#include "triggerwidget.hpp"
#include "editor.hpp"
#include "map.hpp"
#include "variablesdialog.hpp"

namespace wc3lib
{

namespace editor
{

TriggerEditor::TriggerEditor(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : m_triggers(0), m_customTextTriggers(0), m_freeTriggers(false), m_freeCustomTextTriggers(false), m_treeWidget(new QTreeWidget(this)), m_mapScriptWidget(new MapScriptWidget(this)), m_triggerWidget(new TriggerWidget(this)), m_variablesDialog(0), m_triggerActionCollection(0), Module(source, parent, f)
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
	triggerActionCollection()->action("closetriggers")->setEnabled(false);
	triggerActionCollection()->action("closecustomtexttriggers")->setEnabled(false);
	triggerActionCollection()->action("closeall")->setEnabled(false);
}

void TriggerEditor::openTriggers()
{
	KUrl url = KFileDialog::getOpenUrl(KUrl(), triggersFilter(), this, i18n("Open triggers"));

	if (url.isEmpty())
		return;

	QString target;

	if (!source()->download(url, target, this))
	{
		KMessageBox::error(this, i18n("Unable to download triggers from \"%1\".", url.toEncoded().constData()));

		return;
	}

	map::Triggers *triggers = 0;

	try
	{
		triggers = new map::Triggers(0);
		boost::filesystem::ifstream ifstream(target.toUtf8().constData(), std::ios::in | std::ios::binary);
		triggers->read(ifstream);
	}
	catch (std::exception &exception)
	{
		KMessageBox::error(this, i18n("Unable to read triggers from file \"%1\".\nException: \"%2\".", target, exception.what()));

		return;
	}

	loadTriggers(triggers);
	setFreeTriggers(true); // for proper deletion
}

void TriggerEditor::openCustomTextTriggers()
{
	KUrl url = KFileDialog::getOpenUrl(KUrl(), customTextTriggersFilter(), this, i18n("Open custom text triggers"));

	if (url.isEmpty())
		return;

	QString target;

	if (!source()->download(url, target, this))
	{
		KMessageBox::error(this, i18n("Unable to download custom text triggers from \"%1\".", url.toEncoded().constData()));

		return;
	}

	map::CustomTextTriggers *triggers = 0;

	try
	{
		triggers = new map::CustomTextTriggers();
		boost::filesystem::ifstream ifstream(target.toUtf8().constData(), std::ios::in | std::ios::binary);
		triggers->read(ifstream);
	}
	catch (std::exception &exception)
	{
		KMessageBox::error(this, i18n("Unable to read custom text triggers from file \"%1\".\nException: \"%2\".", target, exception.what()));

		return;
	}

	loadCustomTextTriggers(triggers);
	setFreeCustomTextTriggers(true); // for proper deletion
}

void TriggerEditor::closeTriggers()
{
	clear();
}

void TriggerEditor::closeCustomTextTriggers()
{
	if (customTextTriggers() != 0 && freeCustomTextTriggers())
		delete customTextTriggers();

	setCustomTextTriggers(0);
	// TODO update open trigger text (clear, warn?? consider situations when having a open map!!)

	triggerActionCollection()->action("closecustomtexttriggers")->setEnabled(false);

	if (triggers() == 0)
		triggerActionCollection()->action("closeall")->setEnabled(false);
}

void TriggerEditor::closeAll()
{
	closeTriggers();
	closeCustomTextTriggers();
}

void TriggerEditor::resetTriggers()
{
	// TODO read default triggers from class \ref TriggerData.
}

void TriggerEditor::renameTrigger()
{
	treeWidget()->editItem(treeWidget()->currentItem());
}

void TriggerEditor::showVariables()
{
	if (variablesDialog() == 0)
		m_variablesDialog = new VariablesDialog(this);

	if (triggers() != 0)
		variablesDialog()->showVariables(triggers());

	variablesDialog()->show();
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
		qDebug() << "Root item: " << editor()->currentMap()->map()->info()->name().c_str();

		if (source()->download(src, file, this))
			rootItem()->setIcon(0, QIcon(file));
	}
	else
	{
		rootItem()->setText(0, tr("Current map script"));
	}

	const int32 categoriesNumber = triggers->categories().size();
	const int32 triggersNumber = triggers->triggers().size();
	categories().resize(categoriesNumber);


	for (int32 i = 0; i < categoriesNumber; ++i)
	{
		categories()[i] = new QTreeWidgetItem(rootItem());
		categories()[i]->setText(0, triggers->categories()[i]->name().c_str());
		/// \todo set folder icon
		qDebug() << "Category: " << triggers->categories()[i]->name().c_str();
	}

	triggerEntries().resize(triggersNumber);

	for (int32 i = 0; i < triggersNumber; ++i)
	{
		triggerEntries()[i] = new QTreeWidgetItem(rootItem());
		/// \todo set icon (initially on, disabled etc.)
		triggerEntries()[i]->setText(0, triggers->triggers()[i]->name().c_str());
		qDebug() << "Trigger: " << triggers->triggers()[i]->name().c_str();
	}

	m_triggers = triggers;
	triggerActionCollection()->action("closetriggers")->setEnabled(true);
	triggerActionCollection()->action("closeall")->setEnabled(true);
}

void TriggerEditor::loadTriggers(Map *map)
{
	loadTriggers(map->map()->triggers().get());
}

void TriggerEditor::loadCustomTextTriggers(map::CustomTextTriggers *customTextTriggers)
{
	closeCustomTextTriggers();
	m_customTextTriggers = customTextTriggers;
	triggerActionCollection()->action("closecustomtexttriggers")->setEnabled(true);
	triggerActionCollection()->action("closeall")->setEnabled(true);

	if (triggers() == 0)
		return;

	if (triggerWidget()->trigger() != 0 && triggerWidget()->trigger()->isCustomText())
		triggerWidget()->textEdit()->setText(triggerText(triggerWidget()->trigger()).c_str());
}

void TriggerEditor::loadCustomTextTriggers(Map *map)
{
	loadCustomTextTriggers(map->map()->customTextTriggers().get());
}

void TriggerEditor::loadFromMap(Map* map)
{
	if (map->map()->findFile(map->map()->triggers()->fileName()) != 0)
		loadTriggers(map);
	else
		KMessageBox::error(this, i18n("Triggers file \"%1\" doesn't exist.", map->map()->triggers()->fileName()));

	if (map->map()->findFile(map->map()->customTextTriggers()->fileName()) != 0)
		loadCustomTextTriggers(map);
	else
		KMessageBox::error(this, i18n("Custom text triggers file \"%1\" doesn't exist.", map->map()->customTextTriggers()->fileName()));
}

void TriggerEditor::clear()
{
	if (triggers() != 0 && freeTriggers())
		delete triggers();

	treeWidget()->clear();
	categories().clear();
	variables().clear();
	triggerEntries().clear();
	setTriggers(0);
	m_rootItem = 0;
	triggerActionCollection()->action("closetriggers")->setEnabled(false);

	if (customTextTriggers() == 0)
		triggerActionCollection()->action("closeall")->setEnabled(false);
}

void TriggerEditor::openMapScript()
{
	this->triggerWidget()->hide();
	this->mapScriptWidget()->show();
}

void TriggerEditor::openTrigger(int32 index)
{
	openTrigger(triggers()->triggers()[index].get());
}

void TriggerEditor::openTrigger(map::Trigger *trigger)
{
	triggerWidget()->showTrigger(trigger, triggerText(trigger));
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
	m_triggerActionCollection = new KActionCollection((QObject*)this);

	KAction *action = new KAction(KIcon(":/actions/opentriggers.png"), i18n("Open triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(openTriggers()));
	triggerActionCollection()->addAction("opentriggers", action);

	action = new KAction(KIcon(":/actions/opencustomtexttriggers.png"), i18n("Open custom text triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(openCustomTextTriggers()));
	triggerActionCollection()->addAction("opencustomtexttriggers", action);

	action = new KAction(KIcon(":/actions/closetriggers.png"), i18n("Close triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(closeTriggers()));
	triggerActionCollection()->addAction("closetriggers", action);

	action = new KAction(KIcon(":/actions/closecustomtexttriggers.png"), i18n("Close custom text triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(closeCustomTextTriggers()));
	triggerActionCollection()->addAction("closecustomtexttriggers", action);

	action = new KAction(KIcon(":/actions/closeall.png"), i18n("Close all"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(closeAll()));
	triggerActionCollection()->addAction("closeall", action);

	action = new KAction(KIcon(":/actions/resettriggers.png"), i18n("Reset triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(resetTriggers()));
	triggerActionCollection()->addAction("resettriggers", action);

	action = new KAction(KIcon(":/actions/rename.png"), i18n("Rename"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(renameTrigger()));
	triggerActionCollection()->addAction("rename", action);

	triggerActionCollection()->associateWidget(menu);
}

void TriggerEditor::createEditActions(class KMenu *menu)
{
	KAction *action = new KAction(KIcon(":/actions/opentriggers.png"), i18n("Variables ..."), this);
	action->setShortcut(i18n("Ctrl+B"));
	connect(action, SIGNAL(triggered()), this, SLOT(showVariables()));
	triggerActionCollection()->addAction("variables", action);
	menu->addAction(action);
}

void TriggerEditor::createMenus(class KMenuBar *menuBar)
{
}

void TriggerEditor::createWindowsActions(class WindowsMenu *menu)
{
}

void TriggerEditor::createToolButtons(class ModuleToolBar *toolBar)
{
}

class SettingsInterface* TriggerEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void TriggerEditor::onSwitchToMap(Map *map)
{
	loadFromMap(map);
}

#include "moc_triggereditor.cpp"

}

}
