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
#include "sourcesdialog.hpp"
#include "../config.h"
#ifdef DEBUG
#include "Plugin_BlpCodec/blpcodec.hpp"
#endif

namespace wc3lib
{

namespace editor
{

KAboutData Editor::m_aboutData = KAboutData("editor", "", ki18n("World Editor"), "0.1", ki18n("Clone of Blizzard's Warcraft III: Reign of Chaos and Warcraft III: The Frozen Throne World Editor."), KAboutData::License_GPL_V2, ki18n("Copyright (C) 2009 by Tamino Dauth <tamino@cdauth.eu>"), ki18n("Other"), "https://wc3lib.org", "tamino@cdauth.eu")
.addAuthor(ki18n("Tamino Dauth"), ki18n("Maintainer"), "tamino@cdauth.eu", "https://tdauth.cdauth.eu/")
;
KAboutData Editor::m_wc3libAboutData = KAboutData("wc3lib", "", ki18n("Warcraft III Library"), "0.1", ki18n("Library which supports some of Blizzard's file formats used in Warcraft III: Reign of Chaos and Warcraft III: The Frozen Throne."), KAboutData::License_GPL_V2, ki18n("Copyright (C) 2009 by Tamino Dauth <tamino@cdauth.eu>"), ki18n("Other"), "https://wc3lib.org", "tamino@cdauth.eu")
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

Editor::Editor(QWidget *parent, Qt::WindowFlags f) : KMainWindow(parent, f), m_root(0), m_currentMap(0), m_actionCollection(new KActionCollection(this)), m_modulesActionCollection(new KActionCollection(this)), m_mapsActionCollection(new KActionCollection(this)), m_newMapDialog(0), m_sourcesDialog(0)
{
#ifdef DEBUG
	BlpCodec::startup(); // make sure we have BLP support even if it has not been installed
#endif
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
	this->m_actionCollection->addAction("closemodule", action);

	this->setMapActionsEnabled(false);

	this->readSettings(aboutData().appName());

	if (sources().empty() && !addDefaultSources())
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
	this->writeSettings(aboutData().appName());
	// do not delete allocated sub widgets (parent system of Qt already considers) BUT
	// we should remove modules (e. g. model editor) before freeing root!
	foreach (Modules::value_type module, modules())
		delete module;

	if (m_root != 0)
		delete m_root;
}

void Editor::addModule(class Module *module)
{
	this->m_modules.append(module);

	KAction *action = new KAction(KIcon(":/actions/" + module->actionName() + ".png"), module->componentData().aboutData()->programName(), this);
	action->setShortcut(KShortcut(i18n("F%1%", this->m_modulesActionCollection->actions().size() + 1)));
	action->setCheckable(true);
	action->setChecked(module->hasFocus());
	this->m_modulesActionCollection->addAction(module->actionName(), action);
	connect(action, SIGNAL(triggered()), module, SLOT(onEditorActionTrigger()));

	modulesActions().insert(module, action);

	emit this->createdModule(module);
}

Ogre::Root* Editor::root() const
{
	// setup a renderer
	if (m_root == 0)
	{
		const_cast<class Editor*>(this)->m_root = new Ogre::Root();

		const Ogre::RenderSystemList &renderers = m_root->getAvailableRenderers();

		// we need at least one renderer to do anything useful
		if (!renderers.empty())
		{
			Ogre::RenderSystem *renderSystem = renderers.front();

			// user might pass back a null renderer, which would be bad!
			/*
			* TEST could we continue without a render system?
			if (renderSystem == 0)
				throw Exception();
			*/

			// configuration is setup automatically by ogre.cfg file
			if (renderSystem != 0)
			{
				renderSystem->setConfigOption("Full Screen", "No");
				m_root->setRenderSystem(renderSystem);
				// initialize without creating window
				m_root->saveConfig();
				m_root->initialise(false); // don't create a window
			}
			else
				qDebug() << "No render system!";
		}
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
		openMap(url, url == urls.last());
}

void Editor::openMap(const KUrl &url, bool switchTo)
{
	Map  *ptr = new Map(url);

	try
	{
		ptr->setSource(this, true);
	}
	catch (Exception &exception)
	{
		delete ptr;

		KMessageBox::error(this, i18n("Error while opening map \"%1\":\n\"%2\".", url.toEncoded().constData(), exception.what().c_str()));

		return;
	}

	// TODO set icon to w3m or w3x icon
	KAction *action = new KAction(tr("&%1 %2").arg(mapsActionCollection()->actions().size() + 1).arg(ptr->map()->name().c_str()), this);
	//action->setShortcut(KShortcut(i18n("F%1%", this->m_modulesActionCollection->actions().size() + 1)));
	action->setCheckable(true);
	mapsActionCollection()->addAction(tr("map%1").arg(mapsActionCollection()->actions().size() + 1), action);
	maps().append(ptr);
	mapActions().insert(ptr, action);
	emit openedMap(ptr);

	if (switchTo)
		switchToMap(ptr);
}

void Editor::switchToMap(class Map *map)
{
	if (currentMap() == 0)
		this->setMapActionsEnabled(true);

	m_currentMap = map;

	// TODO update all created modules
	emit switchedToMap(map);
}

void Editor::closeMap(class Map *map)
{
	// TODO switch to other open map!
	if (currentMap() == map)
		m_currentMap = 0;

	const int index = maps().indexOf(map);
	KAction *action = boost::polymorphic_cast<KAction*>(mapsActionCollection()->action(index));
	emit aboutToCloseMap(map);
	this->m_mapsActionCollection->removeAction(action);
	maps().removeAt(index);
	mapActions().remove(map);
	removeResource(map);
	// TODO update shortcuts of remaining actions

	if (!maps().isEmpty())
	{
		if (index - 1 >= 0)
			switchToMap(maps()[index - 1]);
		else
			switchToMap(maps()[index]);

		for (int i = index; i < maps().size(); ++i)
		{
			// TODO rename action internally in collection
			mapsActionCollection()->action(i)->setText(
			tr("&%1 %2").arg(mapsActionCollection()->actions().size() + 1).arg(map->map()->name().c_str()));
		}
	}
	else
		this->setMapActionsEnabled(false);
}

void Editor::closeMap()
{
	closeMap(currentMap());
}

void Editor::saveMap()
{
	currentMap()->save(currentMap()->url());
}

void Editor::saveMapAs()
{
	KUrl url = KFileDialog::getSaveUrl(KUrl(), mapFilter(), this, i18n("Save map"));

	if (url.isEmpty())
		return;

	currentMap()->save(url);
}

void Editor::showSourcesDialog()
{
	if (m_sourcesDialog == 0)
		m_sourcesDialog = new SourcesDialog(this, this);

	m_sourcesDialog->update();
	m_sourcesDialog->show();
}

void Editor::setMapActionsEnabled(bool enabled)
{
	this->actionCollection()->action("closemap")->setEnabled(enabled);
	this->actionCollection()->action("savemap")->setEnabled(enabled);
	this->actionCollection()->action("savemapas")->setEnabled(enabled);
	this->actionCollection()->action("savemapshadows")->setEnabled(enabled);
}

#include "moc_editor.cpp"

}

}
