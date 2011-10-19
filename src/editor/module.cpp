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

#include <KMenuBar>
#include <KToolBar>
#include <KActionCollection>
#include <KParts/Plugin>

#include "module.hpp"
#include "modulemenu.hpp"
#include "editor.hpp"
#include "settingsinterface.hpp"
#include "sourcesdialog.hpp"

namespace wc3lib
{

namespace editor
{

Module::Module(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : m_source(source), m_moduleMenu(0), m_menuBar(0), m_topLayout(new QVBoxLayout(this)), QWidget(parent, f | Qt::Window), KComponentData(), KXMLGUIClient() // each module should get its own window
{
	if (hasEditor())
	{
		connect(editor(), SIGNAL(switchedToMap(Map*)), this, SLOT(switchToMap(Map*)));
	}
}

Module::~Module()
{
}

bool Module::hasEditor() const
{
	// TODO typeid comparison doesn't work, dynamic_cast is working workaround!
	//qDebug() << "Source type " << typeid(source()).name() << "\nEditor type " << typeid(Editor*).name();
	//return (typeid(source()) == typeid(Editor*));
	return dynamic_cast<Editor*>(source()) != 0;
	/*
	try
	{
		return boost::polymorphic_cast<Editor*>(source());
	}
	catch (std::bad_cast &exception)
	{
		return false;
	}

	return true;
	*/
}

class Editor* Module::editor() const throw (std::bad_cast)
{
	return boost::polymorphic_cast<Editor*>(source());
}

void Module::showSourcesDialog()
{
	SourcesDialog *sourcesDialog = new SourcesDialog(source(), this);
	sourcesDialog->update();
	sourcesDialog->show();
}

void Module::setupUi()
{
	// TODO multi inheritance (KComponentData) leads to segmentation fault?
	//setAboutData(this->moduleAboutData());

	this->m_menuBar = new KMenuBar(this);
	topLayout()->addWidget(this->m_menuBar);

	this->m_fileMenu = new KMenu(tr("File"), this);
	this->menuBar()->addMenu(this->m_fileMenu);
	connect(this->m_fileMenu, SIGNAL(triggered(QAction *)), this, SLOT(triggered(QAction*)));

	// use actions from editor
	if (hasEditor())
	{
		this->m_fileMenu->addAction(this->editor()->actionCollection()->action("newmap"));
		this->m_fileMenu->addAction(this->editor()->actionCollection()->action("openmap"));
		this->m_fileMenu->addAction(this->editor()->actionCollection()->action("closemap"));
		this->m_fileMenu->addSeparator();
		this->m_fileMenu->addAction(this->editor()->actionCollection()->action("savemap"));
		this->m_fileMenu->addAction(this->editor()->actionCollection()->action("savemapas"));
		this->m_fileMenu->addAction(this->editor()->actionCollection()->action("savemapshadows"));
		this->m_fileMenu->addSeparator();
	}

	// create user-defined actions in file menu
	this->createFileActions(this->m_fileMenu);

	// use actions from editor
	this->m_fileMenu->addSeparator();

	if (hasEditor())
	{
		this->m_fileMenu->addAction(this->editor()->actionCollection()->action("testmap"));
		this->m_fileMenu->addSeparator();


		m_closeAction = this->editor()->actionCollection()->action("closemodule");
	}
	else
	{
		m_closeAction = new QAction(tr("Close"), this);
	}

	this->m_fileMenu->addAction(m_closeAction);

	this->m_sourcesAction = new KAction(tr("Sources"), this);
	connect(this->m_sourcesAction, SIGNAL(triggered()), this, SLOT(showSourcesDialog()));
	this->m_fileMenu->addAction(m_sourcesAction);

	this->m_editMenu = new KMenu(tr("Edit"), this);
	this->menuBar()->addMenu(this->m_editMenu);

	// create user-defined actions in edit menu
	this->createEditActions(this->m_editMenu);

	// module menu
	if (hasEditor())
	{
		this->m_moduleMenu = new ModuleMenu(this);
		this->menuBar()->addMenu(this->m_moduleMenu);
	}

	// create user-defined menus
	this->createMenus(this->menuBar());

	this->m_windowsMenu = new KMenu(tr("Windows"), this);
	this->menuBar()->addMenu(windowsMenu());

	// create user-defined actions in windows menu
	this->createWindowsActions(windowsMenu());

	// tool bar
	this->m_toolBar = new KToolBar(this);
	topLayout()->addWidget(toolBar());
	toolBar()->addSeparator();

	// user defined tool buttons
	this->createToolButtons(toolBar());

	toolBar()->addSeparator();

	if (hasEditor())
	{
		// modules tool buttons
		foreach (QAction *action, moduleMenu()->actions())
			toolBar()->addAction(action);

		// test map tool button
		toolBar()->addAction(this->editor()->actionCollection()->action("testmap"));
	}
}

KAboutData Module::moduleAboutData() const
{
	KAboutData aboutData(Editor::wc3libAboutData());
	aboutData.setAppName(actionName().toAscii());
	aboutData.setCatalogName(actionName().toAscii());

	return aboutData;
}

void Module::focusInEvent(QFocusEvent *event)
{
	if (hasEditor())
	{
		ModuleMenu::Actions::const_iterator iterator = moduleMenu()->actions().find(this);

		if (iterator != moduleMenu()->actions().end())
			(*iterator)->setChecked(true);
	}

	QWidget::focusInEvent(event);
}

void Module::focusOutEvent(QFocusEvent *event)
{
	if (hasEditor())
	{
		ModuleMenu::Actions::const_iterator iterator = moduleMenu()->actions().find(this);

		if (iterator != moduleMenu()->actions().end())
			(*iterator)->setChecked(false);
	}

	QWidget::focusOutEvent(event);
}

void Module::readSettings()
{
	SettingsInterface *settings = this->settings();
	KConfigGroup configGroup(KGlobal::config(), settings->groupName());
	settings->read(configGroup);
	delete settings;
}

void Module::writeSettings()
{
	SettingsInterface *settings = this->settings();
	KConfigGroup configGroup(KGlobal::config(), settings->groupName());
	settings->write(configGroup);
	delete settings;
}

void Module::triggered(QAction *action)
{
	if (this->isActiveWindow() && action == m_closeAction)
	{
		this->close();
	}
}

void Module::switchToMap(class Map *map)
{
	onSwitchToMap(map);
}

#include "moc_module.cpp"

}

}
