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
#include <QMessageBox>
#include <QFileDialog>

#include <Ogre.h>

#include "editor.hpp"
#include "warcraftiiishared.hpp"
#include "module.hpp"
#include "modulemenu.hpp"
#include "settings.hpp"
#include "map.hpp"
#include "sourcesdialog.hpp"
#include "config.h"
#ifdef DEBUG
#include "Plugin_BlpCodec/blpcodec.hpp"
#endif

namespace wc3lib
{

namespace editor
{

Editor::Editor(Root *root, QObject *parent) : QObject(parent)
, m_root(root)
, m_currentMap(0)
, m_newMapDialog(0)
, m_sourcesDialog(0)
{
#ifdef DEBUG
	BlpCodec::startup(); // make sure we have BLP support even if it has not been installed
#endif

	QAction *action = new QAction(QIcon(":/actions/newmap.png"), tr("New map ..."), this);
	//action->setShortcut(KShortcut(tr("Ctrl+N")));
	connect(action, SIGNAL(triggered()), this, SLOT(newMap()));

	action = new QAction(QIcon(":/actions/openmap.png"), tr("Open map ..."), this);
	//action->setShortcut(KShortcut(tr("Ctrl+O")));
	connect(action, SIGNAL(triggered()), this, SLOT(openMap()));

	action = new QAction(QIcon(":/actions/closemap.png"), tr("Close map"), this);
	//action->setShortcut(KShortcut(tr("Strg+W")));
	connect(action, SIGNAL(triggered()), this, SLOT(closeMap()));

	// --

	action = new QAction(QIcon(":/actions/savemap.png"), tr("Save map"), this);
	//action->setShortcut(KShortcut(tr("Ctrl+S")));
	connect(action, SIGNAL(triggered()), this, SLOT(saveMap()));

	action = new QAction(QIcon(":/actions/savemapas.png"), tr("Save map as ..."), this);
	//action->setShortcut(KShortcut(tr("Strg+S")));
	connect(action, SIGNAL(triggered()), this, SLOT(saveMapAs()));

	action = new QAction(QIcon(":/actions/savemapshadows.png"), tr("Calculate shadows and save map ..."), this);
	//action->setShortcut(KShortcut(tr("Strg+S")));
	connect(action, SIGNAL(triggered()), this, SLOT(saveMapShadow()));

	// --

	action = new QAction(QIcon(":/actions/testmap.png"), tr("Test map"), this);
	//action->setShortcut(KShortcut(tr("Ctrl+F9")));
	connect(action, SIGNAL(triggered()), this, SLOT(testMap()));

	// --

	action = new QAction(QIcon(":/actions/closemodule.png"), this->sharedData()->tr("WESTRING_MENU_CLOSEMODULE"), this);
	//action->setShortcut(KShortcut(tr("Ctrl+Shift+W")));

	this->setMapActionsEnabled(false);

	/*
	QSettings settings("Blizzard Entertainment", "WorldEdit", this);
	settings.beginGroup("shortcuts");
	// Read shortcuts
	newMapAction->setShortcut(settings.value("newmap", KShortcut(tr("Strg+N"))).toString());
	*/
	/*
	showTerrainEditor(); // test
	this->m_terrainEditor->resize(QSize(300, 300));
	*/
}

Editor::~Editor()
{
	this->writeSettings("WorldEditor");

	// do not delete allocated sub widgets (parent system of Qt already considers) BUT
	// we should remove modules (e. g. model editor) before freeing root!
	foreach (Modules::value_type module, modules())
	{
		delete module;
	}

	foreach (Maps::value_type map, maps())
	{
		delete map;
	}
}

void Editor::addModule(Module *module)
{
	this->m_modules.append(module);

	QAction *action = new QAction(module->icon(), module->objectName(), this);
	//action->setShortcut(KShortcut(tr("F%1%", this->m_modulesActionCollection->actions().size() + 1)));
	action->setCheckable(true);
	action->setChecked(module->hasFocus());
	connect(action, SIGNAL(triggered()), module, SLOT(onEditorActionTrigger()));

	modulesActions().insert(module, action);

	emit this->createdModule(module);
}

bool Editor::configure(QWidget *parent)
{
	this->readSettings("WorldEditor"); // fill sources first

	if (!MpqPriorityList::configure(parent))
	{
		return false;
	}

	// TODO refresh all shared files
	try
	{
		this->sharedData()->refreshDefaultFiles(parent);
		this->sharedData()->sharedObjectData()->unitEditorData()->setSource(this);
		this->sharedData()->sharedObjectData()->unitEditorData()->load();

		this->sharedData()->refreshWorldEditorStrings(parent);
		this->sharedData()->refreshWorldEditData(parent);
	}
	catch (Exception &e)
	{
		QMessageBox::critical(parent, tr("Configuration Error"), e.what());

		return false;
	}

	// FIXME
	//retranslateUi();

	return true;
}

Root* Editor::root() const
{
	return m_root;
}

void Editor::changeEvent(QEvent *event)
{
	/// TODO load new language MPQ archives and retranslate all textes which has been translated via \ref wc3lib::editor::WarcraftIIIShared::tr().
	if (event->type() == QEvent::LanguageChange)
	{

	}
}

void Editor::newMap()
{
	this->newMapDialog()->show();
}

void Editor::openMap(QWidget *window)
{
	const QUrl &url = QUrl::fromLocalFile(QFileDialog::getOpenFileName(window, tr("Open map"), QString(), mapFilter()));

	if (url.isEmpty())
	{
		return;
	}

	openMap(url, true);
}

void Editor::openMap(const QUrl &url, bool switchTo, QWidget *window)
{
	Map *ptr = new Map(url);

	try
	{
		ptr->setSource(this);
		ptr->load();
	}
	catch (Exception &exception)
	{
		delete ptr;

		QMessageBox::critical(window, tr("Error while opening map"), tr("\"%1\":\n\"%2\".").arg(url.toEncoded().constData()).arg(exception.what()));

		return;
	}

	// TODO set icon to w3m or w3x icon
	QAction *action = new QAction(tr("%1").arg(ptr->map()->name().c_str()), this);
	//action->setShortcut(KShortcut(tr("F%1%", this->m_modulesActionCollection->actions().size() + 1)));
	action->setCheckable(true);
	maps().append(ptr);
	mapActions().insert(ptr, action);
	emit openedMap(ptr);

	if (switchTo)
	{
		switchToMap(ptr);
	}
}

void Editor::switchToMap(class Map *map)
{
	if (currentMap() == 0)
	{
		this->setMapActionsEnabled(true);
	}

	m_currentMap = map;

	// TODO update all created modules
	emit switchedToMap(map);
}

void Editor::closeMap(class Map *map)
{
	// TODO switch to other open map!
	if (currentMap() == map)
	{
		m_currentMap = 0;
	}

	const int index = maps().indexOf(map);
	emit aboutToCloseMap(map);
	maps().removeAt(index);
	mapActions().remove(map);
	removeResource(map);
	// TODO update shortcuts of remaining actions

	if (!maps().isEmpty())
	{
		if (index - 1 >= 0)
		{
			switchToMap(maps()[index - 1]);
		}
		else
		{
			switchToMap(maps()[index]);
		}
	}
	else
	{
		this->setMapActionsEnabled(false);
	}
}

void Editor::closeMap()
{
	closeMap(currentMap());
}

void Editor::saveMap()
{
	currentMap()->save(currentMap()->url());
}

void Editor::saveMapAs(QWidget *window)
{
	QUrl url = QUrl::fromLocalFile(QFileDialog::getSaveFileName(window, tr("Save map"), QString(), mapFilter()));

	if (url.isEmpty())
	{
		return;
	}

	currentMap()->save(url);
}

void Editor::showSourcesDialog(QWidget *window)
{
	if (m_sourcesDialog == 0)
	{
		m_sourcesDialog = new SourcesDialog(this, window);
	}

	m_sourcesDialog->setParent(window);
	m_sourcesDialog->update();
	m_sourcesDialog->show();
}

void Editor::setMapActionsEnabled(bool enabled)
{
	// TODO set
}

#include "moc_editor.cpp"

}

}
