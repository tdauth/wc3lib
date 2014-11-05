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

#include "module.hpp"
#include "modulemenu.hpp"
#include "moduletoolbar.hpp"
#include "windowsmenu.hpp"
#include "mpqprioritylist.hpp"
#include "editor.hpp"
#include "settingsinterface.hpp"
#include "sourcesdialog.hpp"

namespace wc3lib
{

namespace editor
{

Module::Module(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f | Qt::Window)
, m_source(source)
, m_moduleMenu(0)
, m_menuBar(0)
, m_topLayout(new QVBoxLayout())
, m_centerLayout(new QGridLayout())
, m_sourcesDialog(0)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	setLayout(layout);
	topLayout()->setAlignment(Qt::AlignTop);
	//topLayout()->widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	//topLayout()->setSizeConstraint( Qt::Horizontal);
	layout->addLayout(topLayout());
	layout->addLayout(centerLayout());

	if (hasEditor())
	{
		connect(editor(), SIGNAL(switchedToMap(Map*)), this, SLOT(switchToMap(Map*)));
	}
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
		if (!source()->configure(this))
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

Editor* Module::editor() const
{
	return boost::polymorphic_cast<Editor*>(source());
}

QString Module::settingsGroup() const
{
	if (this->hasEditor())
	{
		return this->editor()->aboutData().appName();
	}
	else
	{
		return this->componentData().aboutData()->appName();
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
			m_sourcesDialog = new SourcesDialog(source(), this);

		m_sourcesDialog->update();
		m_sourcesDialog->show();
	}
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

	// tool bar
	this->m_toolBar = new ModuleToolBar(this);
	this->topLayout()->addWidget(m_toolBar);

	// user defined tool buttons
	this->createToolButtons(toolBar());
}

KAboutData Module::moduleAboutData() const
{
	KAboutData aboutData(Editor::wc3libAboutData());
	aboutData.setAppName(actionName().toAscii());
	aboutData.setCatalogName(actionName().toAscii());

	return aboutData;
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
	/*
	KSharedConfigPtr config(new KSharedConfig(componentData()
	SettingsInterface *settings = this->settings();
	KConfigGroup configGroup(KGlobal::config(), settings->groupName());
	settings->read(configGroup);
	delete settings;
	*/
	//QSettings settings("wc3editor", settingsGroup());
	if (!this->hasEditor())
	{
		this->source()->readSettings(settingsGroup());
	}
}

void Module::writeSettings()
{
	/*
	SettingsInterface *settings = this->settings();
	KConfigGroup configGroup(KGlobal::config(), settings->groupName());
	settings->write(configGroup);
	delete settings;
	*/
	if (!this->hasEditor())
	{
		this->source()->writeSettings(settingsGroup());
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
