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

#include <KMenu>
#include <KAction>
#include <KActionCollection>
#include <KMenuBar>
#include <KLocale>
#include <KConfig>
#include <KFileDialog>
#include <KMessageBox>

#include <Ogre.h>

#include "editor.hpp"
#include "module.hpp"
#include "modulemenu.hpp"
#include "settings.hpp"
#include "map.hpp"

namespace wc3lib
{

namespace editor
{

KAboutData Editor::m_aboutData = KAboutData("editor", "", ki18n("World Editor"), "0.1", ki18n("Clone of Blizzard's Warcraft III: Reign of Chaos and Warcraft III: The Frozen Throne World Editor."), KAboutData::License_GPL_V2, ki18n("Copyright (C) 2009 by Tamino Dauth <tamino@cdauth.eu>"), ki18n("Other"), "https://gitorious.org/wc3lib", "tamino@cdauth.eu")
.addAuthor(ki18n("Tamino Dauth"), ki18n("Maintainer"), "tamino@cdauth.eu", "https://tdauth.cdauth.eu/")
;
KAboutData Editor::m_wc3libAboutData = KAboutData("wc3lib", "", ki18n("Warcraft III Library"), "0.1", ki18n("Library which supports some of Blizzard's file formats used in Warcraft III: Reign of Chaos and Warcraft III: The Frozen Throne."), KAboutData::License_GPL_V2, ki18n("Copyright (C) 2009 by Tamino Dauth <tamino@cdauth.eu>"), ki18n("Other"), "https://gitorious.org/wc3lib", "tamino@cdauth.eu")
.addAuthor(ki18n("Tamino Dauth"), ki18n("Maintainer"), "tamino@cdauth.eu", "https://tdauth.cdauth.eu/")
;

const KAboutData& Editor::aboutData()
{
	return Editor::m_aboutData;
}

const KAboutData& Editor::wc3libAboutData()
{
	return Editor::m_wc3libAboutData;
}

Editor::Editor(QWidget *parent, Qt::WindowFlags f) : KMainWindow(parent, f), m_root(0), m_actionCollection(new KActionCollection(this)), m_modulesActionCollection(new KActionCollection(this)), m_newMapDialog(0)
{
	class KAction *action = new KAction(KIcon(":/actions/newmap.png"), i18n("New map ..."), this);
	action->setShortcut(KShortcut(i18n("Ctrl+N")));
	connect(action, SIGNAL(triggered()), this, SLOT(newMap()));
	this->m_actionCollection->addAction("newmap", action);

	action = new KAction(KIcon(":/actions/openmap.png"), i18n("Open map ..."), this);
	action->setShortcut(KShortcut(i18n("Ctrl+O")));
	connect(action, SIGNAL(triggered()), this, SLOT(openMap()));
	this->m_actionCollection->addAction("openmap", action);

	action = new KAction(KIcon(":/actions/closemap.png"), i18n("Close map"), this);
	action->setShortcut(KShortcut(i18n("Strg+W")));
	connect(action, SIGNAL(triggered()), this, SLOT(closeMap()));
	this->m_actionCollection->addAction("closemap", action);

	// --

	action = new KAction(KIcon(":/actions/savemap.png"), i18n("Save map"), this);
	action->setShortcut(KShortcut(i18n("Ctrl+S")));
	connect(action, SIGNAL(triggered()), this, SLOT(saveMap()));
	this->m_actionCollection->addAction("savemap", action);

	action = new KAction(KIcon(":/actions/savemapas.png"), i18n("Save map as ..."), this);
	//action->setShortcut(KShortcut(i18n("Strg+S")));
	connect(action, SIGNAL(triggered()), this, SLOT(saveMapAs()));
	this->m_actionCollection->addAction("savemapas", action);

	action = new KAction(KIcon(":/actions/savemapshadows.png"), i18n("Calculate shadows and save map ..."), this);
	//action->setShortcut(KShortcut(i18n("Strg+S")));
	connect(action, SIGNAL(triggered()), this, SLOT(saveMapShadow()));
	this->m_actionCollection->addAction("savemapshadows", action);

	// --

	action = new KAction(KIcon(":/actions/testmap.png"), i18n("Test map"), this);
	action->setShortcut(KShortcut(i18n("Ctrl+F9")));
	connect(action, SIGNAL(triggered()), this, SLOT(testMap()));
	this->m_actionCollection->addAction("testmap", action);

	// --

	action = new KAction(KIcon(":/actions/closemodule.png"), i18n("Close module"), this);
	action->setShortcut(KShortcut(i18n("Ctrl+Shift+W")));
	connect(action, SIGNAL(triggered()), this, SLOT(closeModule()));
	this->m_actionCollection->addAction("closemodule", action);

	this->setMapActionsEnabled(false);

	this->readSettings();

	if (!addDefaultSources())
		KMessageBox::error(this, i18n("One or several MPQ archives of Warcraft III are missing."));

	/*
	QSettings settings("Blizzard Entertainment", "WorldEdit", this);
	settings.beginGroup("shortcuts");
	// Read shortcuts
	newMapAction->setShortcut(settings.value("newmap", KShortcut(i18n("Strg+N"))).toString());
	*/
	/*
	showTerrainEditor(); // test
	this->m_terrainEditor->resize(QSize(300, 300));
	*/
}

Editor::~Editor()
{
	this->writeSettings();
	// do not delete allocated sub widgets (parent system of Qt already considers)


	if (m_root != 0)
		delete m_root;

	// delete resources
	BOOST_FOREACH(Resources::reference value, this->m_resources)
	{
		qDebug() << "Cleaning up resource " << value.first;

		value.second.reset();
	}
}

void Editor::addModule(class Module *module)
{
	this->m_modules.append(module);

	KAction *action = new KAction(KIcon(":/actions/" + module->actionName() + ".png"), module->windowTitle(), this);
	action->setShortcut(KShortcut(i18n("F%1%", this->m_modulesActionCollection->actions().size() + 1)));
	action->setCheckable(true);
	this->m_modulesActionCollection->addAction(module->actionName(), action);
	connect(action, SIGNAL(triggered()), module, SLOT(show()));
	connect(action, SIGNAL(triggered()), module, SLOT(setFocus()));

	emit this->createdModule(module, action);
}

Ogre::Root* Editor::root() const
{
	// setup a renderer
	if (m_root == 0)
	{
		const_cast<class Editor*>(this)->m_root = new Ogre::Root();

		const Ogre::RenderSystemList &renderers = m_root->getAvailableRenderers();

		// we need at least one renderer to do anything useful
		if (renderers.empty())
			throw Exception();

		Ogre::RenderSystem *renderSystem = renderers.front();

		// user might pass back a null renderer, which would be bad!
		if (renderSystem == 0)
			throw Exception();

		// configuration is setup automatically by ogre.cfg file
		renderSystem->setConfigOption("Full Screen", "No");
		m_root->setRenderSystem(renderSystem);
		// initialize without creating window
		m_root->saveConfig();
		m_root->initialise(false); // don't create a window
	}
	//else if (m_root->getRenderSystem()->getConfigOptions()["Full Screen"].currentValue == "Yes")
		//KMessageBox::information(this, i18n("Full screen is enabled."));

	return m_root;
}

void Editor::changeEvent(QEvent *event)
{
	/// TODO load new language MPQ archives and retranslate all textes which has been translated via \ref Editor::tr().
	if (event->type() == QEvent::LanguageChange)
	{

	}
}

void Editor::newMap()
{
	this->newMapDialog()->show();
}

void Editor::openMap()
{
	KUrl::List urls = KFileDialog::getOpenUrls(KUrl(), mapFilter(), this, i18n("Open map"));

	if (urls.empty())
		return;

	foreach(KUrl::List::const_reference url, urls)
		openMap(url);
}

void Editor::openMap(const KUrl &url)
{
	Map  *ptr = new Map(this, url);

	try
	{
		ptr->load();
	}
	catch (Exception &exception)
	{
		delete ptr;

		KMessageBox::error(this, i18n("Error while opening map \"%1\":\n\"%2\".", url.toEncoded().constData(), exception.what().c_str()));

		return;
	}

	this->addResource(ptr);
	switchToMap(ptr);
}

void Editor::switchToMap(class Map *map)
{
	m_currentMap = map;

	// TODO update all created modules
	emit switchedToMap(map);
}

void Editor::closeMap(class Map *map)
{
}

void Editor::setMapActionsEnabled(bool enabled)
{
	this->m_actionCollection->action("closemap")->setEnabled(enabled);
	this->m_actionCollection->action("savemap")->setEnabled(enabled);
	this->m_actionCollection->action("savemapas")->setEnabled(enabled);
	this->m_actionCollection->action("savemapshadows")->setEnabled(enabled);
}

void Editor::readSettings()
{
	/// @todo Actions should get the same entry names and shortcuts as in the original World Editor?
	this->m_actionCollection->setConfigGroup("Shortcuts");
	this->m_actionCollection->readSettings(); // load shortcuts after setting default

	Settings settings(this);
	settings.read(KGlobal::config()->group("Settings"));
	/*
	TODO create from component data?!
	KConfig config(this->com
	Settings settings(this);
	settings.read(settings.groupName()
	*/
}

void Editor::writeSettings()
{
	this->m_actionCollection->setConfigGroup("Shortcuts");
	this->m_actionCollection->writeSettings();

	Settings settings(this);
	KConfigGroup settingsConfigGroup = KGlobal::config()->group("Settings");
	settings.write(settingsConfigGroup);

	/*
	TODO create from component data?!
	KConfig config
	Settings settings(this);
	settings.write(settings.groupName());
	*/
}

#include "moc_editor.cpp"

}

}
