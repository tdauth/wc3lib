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

#include <QtWidgets/QtWidgets>

#include "config.h"
#include "module.hpp"
#include "modulemenu.hpp"
#include "moduletoolbar.hpp"
#include "windowsmenu.hpp"
#include "mpqprioritylist.hpp"
#include "editor.hpp"
#include "settingsinterface.hpp"
#include "sourcesdialog.hpp"
#include "resourcesdialog.hpp"

namespace wc3lib
{

namespace editor
{

Module::Module(MpqPriorityList *source, const QString &organization, const QString &applicationName, QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f | Qt::Window)
, m_source(source)
, m_organization(organization)
, m_applicationName(applicationName)
, m_moduleMenu(0)
, m_menuBar(0)
, m_topLayout(new QVBoxLayout())
, m_centerLayout(new QGridLayout())
, m_sourcesDialog(0)
, m_resourcesDialog(0)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	setLayout(layout);
	topLayout()->setAlignment(Qt::AlignTop);
	layout->addLayout(topLayout());
	layout->addLayout(centerLayout());

	if (hasEditor())
	{
		connect(editor(), SIGNAL(switchedToMap(Map*)), this, SLOT(switchToMap(Map*)));
	}

	/*
	 * Make sure that qblp is loaded when the plugin is not installed.
	 */
#ifdef DEBUG
	QPluginLoader loader("../qblp/qblp.so");

	if (loader.instance())
	{
		qDebug() << "Ok";
		qDebug() << "Supported image formats:" << QImageReader::supportedImageFormats();
	}
	else
	{
		qDebug() << "fail" << loader.errorString();
	}
#endif
}

Module::~Module()
{
}

bool Module::configure()
{
	readSettings();

	// Configure source if started as stand-alone module.
	if (!hasEditor())
	{
		if (!source()->configure(this, m_organization, m_applicationName))
		{
			return false;
		}
	}

	retranslateUi();

	return true;
}

void Module::retranslateUi()
{
	this->m_fileMenu->setTitle(this->source()->sharedData()->tr("WESTRING_MENU_FILE"));

	if (!hasEditor())
	{
		m_closeAction->setText(source()->sharedData()->tr("WESTRING_MENU_CLOSEMODULE"));
	}

	this->m_sourcesAction->setText(tr("Sources"));
	this->m_editMenu->setTitle(source()->sharedData()->tr("WESTRING_MENU_EDIT"));
	this->m_windowsMenu->retranslateUi();
	this->m_helpMenu->setTitle(tr("Help"));
}

bool Module::hasEditor() const
{
	return dynamic_cast<Editor*>(source()) != 0;
}

Editor* Module::editor() const
{
	return boost::polymorphic_cast<Editor*>(source());
}

QString Module::settingsGroup() const
{
	if (this->hasEditor())
	{
		return "WorldEditor";
	}
	else
	{
		return this->objectName(); // TODO set app name
	}
}

void Module::showSourcesDialog()
{
	if (hasEditor())
	{
		editor()->showSourcesDialog();
	}
	else
	{
		if (m_sourcesDialog == 0)
		{
			m_sourcesDialog = new SourcesDialog(source(), m_organization, m_applicationName, this);
		}

		m_sourcesDialog->update();
		m_sourcesDialog->show();
	}
}

void Module::showResourcesDialog()
{
	if (m_resourcesDialog == 0)
	{
		m_resourcesDialog = new ResourcesDialog(this);
	}

	m_resourcesDialog->setSources(source());
	m_resourcesDialog->show();
}

void Module::aboutWc3lib()
{
	QMessageBox::about(this, tr("About wc3lib"), tr("wc3lib has been created by Tamino Dauth"));
}

void Module::aboutQt()
{
	QMessageBox::aboutQt(this);
}

void Module::setupUi()
{
	this->m_menuBar = new QMenuBar(this);
	topLayout()->addWidget(this->m_menuBar);

	this->m_fileMenu = new QMenu(this);
	this->menuBar()->addMenu(this->m_fileMenu);
	connect(this->m_fileMenu, SIGNAL(triggered(QAction *)), this, SLOT(triggered(QAction*)));

	// use actions from editor
	if (hasEditor())
	{
	}

	// create user-defined actions in file menu
	this->createFileActions(this->m_fileMenu);

	// use actions from editor
	this->m_fileMenu->addSeparator();

	if (hasEditor())
	{
	}
	else
	{
		m_closeAction = new QAction(this);
	}

	this->m_fileMenu->addAction(m_closeAction);

	this->m_sourcesAction = new QAction(this);
	connect(this->m_sourcesAction, SIGNAL(triggered()), this, SLOT(showSourcesDialog()));
	this->m_fileMenu->addAction(m_sourcesAction);

	this->m_editMenu = new QMenu(this);
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

	this->m_windowsMenu = new WindowsMenu(this);
	this->menuBar()->addMenu(windowsMenu());

	// create user-defined actions in windows menu
	this->createWindowsActions(windowsMenu());

	this->m_helpMenu = new QMenu(this);
	this->menuBar()->addMenu(this->m_helpMenu);

	this->m_resourcesAction = new QAction(tr("Resources"), this);
	connect(this->m_resourcesAction, SIGNAL(triggered()), this, SLOT(showResourcesDialog()));
	this->m_helpMenu->addAction(this->m_resourcesAction);
	this->m_aboutWc3libAction = new QAction(tr("About wc3lib"), this);
	connect(this->m_aboutWc3libAction, SIGNAL(triggered()), this, SLOT(aboutWc3lib()));
	this->m_helpMenu->addAction(this->m_aboutWc3libAction);
	this->m_aboutQtAction = new QAction(tr("About Qt"), this);
	connect(this->m_aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));
	this->m_helpMenu->addAction(this->m_aboutQtAction);

	// tool bar
	this->m_toolBar = new ModuleToolBar(this);
	this->topLayout()->addWidget(m_toolBar);

	// user defined tool buttons
	this->createToolButtons(toolBar());
}

void Module::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);

	if (event->type() == QEvent::ActivationChange && hasEditor())
	{
		const_cast<const Editor*>(editor())->modulesActions()[this]->setChecked(this->isActiveWindow());
	}
	else if (event->type() == QEvent::LanguageChange)
	{
		this->retranslateUi();
	}
}

void Module::readSettings()
{
	if (!this->hasEditor())
	{
		QSettings settings(organization(), applicationName());
		this->source()->readSettings(settings, settingsGroup());
	}
}

void Module::writeSettings()
{
	if (!this->hasEditor())
	{
		QSettings settings(organization(), applicationName());
		this->source()->writeSettings(settings, settingsGroup());
	}
}

void Module::onEditorActionTrigger()
{
	this->show();
	this->activateWindow();
	this->raise();
}

void Module::triggered(QAction *action)
{
	if (this->isActiveWindow() && action == m_closeAction)
	{
		this->close();
	}
}

void Module::switchToMap(Map *map)
{
	onSwitchToMap(map);
}

#include "moc_module.cpp"

}

}
