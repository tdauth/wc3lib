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
#include <KMenuBar>
#include <KAction>
#include <KToolBar>
#include <KFileDialog>
#include <KMessageBox>

#include "objecteditor.hpp"
#include "objecteditortab.hpp"
#include "editor.hpp"
#include "uniteditor.hpp"
#include "itemeditor.hpp"
#include "weathereditor.hpp"
#include "objecttreemodel.hpp"
#include "../moduletoolbar.hpp"

namespace wc3lib
{

namespace editor
{

ObjectEditor::ObjectEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : Module(source, parent, f)
, m_tabWidget(new KTabWidget(this))
, m_currentTab(0)
, m_unitEditor(0)
, m_doodadEditor(0)
, m_destructibleEditor(0)
, m_itemEditor(0)
, m_abilityEditor(0)
, m_buffEditor(0)
, m_upgradeEditor(0)
// newly supported
, m_modelEntryEditor(0)
, m_skinEntryEditor(0)
, m_ubersplatEntryEditor(0)
, m_splatEntryEditor(0)
, m_spawnEntryEditor(0)
, m_lightningEffectEntryEditor(0)
, m_cliffTypeEntryEditor(0)
, m_tilesetEntryEditor(0)
, m_waterEntryEditor(0)
, m_weatherEditor(0)
, m_soundEntryEditor(0)
, m_copyObjectAction(0)
, m_pasteObjectAction(0)
, m_modifyFieldAction(0)
, m_resetFieldAction(0)
, m_rawDataAction(0)
{
	readSettings();

	// Update required files if started as stand-alone module
	if (!hasEditor())
	{
		try
		{
			source->sharedData()->refreshWorldEditorStrings(this);
			source->sharedData()->refreshWorldEditorGameStrings(this);
			source->sharedData()->refreshWorldEditData(this);
		}
		catch (wc3lib::Exception &e)
		{
			KMessageBox::error(0, i18n("Error when loading default files: %1", e.what()));
		}
	}

	Module::setupUi();
	//Ui::ObjectEditor::setupUi(this);
	topLayout()->addWidget(tabWidget());
	setMinimumSize(QSize(200, 200)); // TEST

	/*
	 * Create all tabs after the actions have been created.
	 */
	m_unitEditor = new UnitEditor(source, this, f);
	m_itemEditor = new ItemEditor(source, this, f);
	m_weatherEditor = new WeatherEditor(source, this, f);

	tabWidget()->addTab(unitEditor(), unitEditor()->name());
	tabWidget()->addTab(itemEditor(), itemEditor()->name());
	tabWidget()->addTab(weatherEditor(), weatherEditor()->name());

	m_currentTab = tab(0);
	setWindowTitle(currentTab()->name());
	addCurrentActions();
	// connect signal and slot after adding actions and tabs first time!
	connect(tabWidget(), SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
}

ObjectEditor::~ObjectEditor()
{
}

ObjectEditorTab* ObjectEditor::tab(int index) const
{
	return boost::polymorphic_cast<ObjectEditorTab*>(this->tabWidget()->widget(index));
}

void ObjectEditor::exportAll()
{
	// TODO collect all tab data (requires Frozen Throne)

	const KUrl url = KFileDialog::getSaveUrl(KUrl(), tr("*|All Files\n%1").arg(objectsCollectionFilter()), this, source()->sharedData()->tr("WESTRING_MENU_OE_EXPORTALL", "WorldEditStrings"));

	if (!url.isEmpty())
	{
		map::CustomObjectsCollection collection;
		map::CustomObjectsCollection::CustomObjectsPtr units(new map::CustomObjects(unitEditor()->unitData()->customObjects()));
		collection.units().swap(units);

		QTemporaryFile file;

		if (file.open())
		{
			ofstream out(file.fileName().toUtf8().constData());

			if (out)
			{
				try
				{
					collection.write(out);
					out.close();
					file.close();

					if (!this->source()->upload(file.fileName(), url, this))
					{
						KMessageBox::error(this, tr("Unable to save file."));
					}
				}
				catch (Exception &e)
				{
					KMessageBox::error(this, e.what());
				}
			}
			else
			{
				KMessageBox::error(this, tr("Unable to open temporary file %1.").arg(file.fileName()));
			}
		}
		else
		{
			KMessageBox::error(this, tr("Unable to open temporary file %1.").arg(file.fileName()));
		}
	}
}

void ObjectEditor::importAll(const KUrl &url)
{
	// TODO import all collection or a collection from a map (requires Frozen Throne)
	// TODO distribute to the tabs

	QString file;

	if (this->source()->download(url, file, this))
	{
		ifstream in(file.toUtf8().constData());

		if (in)
		{
			try
			{
				map::CustomObjectsCollection collection;
				collection.read(in);

				if (collection.hasUnits())
				{
					this->unitEditor()->unitData()->importCustomObjects(*collection.units());
				}
			}
			catch (Exception &e)
			{
				KMessageBox::error(this, e.what());
			}
		}
		else
		{
			KMessageBox::error(this, tr("Unable to open downloaded file %1.").arg(file));
		}
	}
}

void ObjectEditor::importAll()
{
	const KUrl url = KFileDialog::getOpenUrl(KUrl(), tr("*|All Files\n%1").arg(objectsCollectionFilter()), this, source()->sharedData()->tr("WESTRING_MENU_OE_IMPORTALL", "WorldEditStrings"));

	if (!url.isEmpty())
	{
		this->importAll(url);
	}
}

void ObjectEditor::createFileActions(KMenu *menu)
{
	m_newObjectAction = new KAction(this);
	menu->addAction(newObjectAction());

	m_renameObjectAction = new KAction(this);
	menu->addAction(renameObjectAction());

	m_deleteObjectAction = new KAction(this);
	menu->addAction(deleteObjectAction());

	menu->addSeparator();

	m_resetObjectAction = new KAction(this);
	menu->addAction(resetObjectAction());

	m_resetAllObjectsAction = new KAction(this);
	menu->addAction(resetAllObjectsAction());

	menu->addSeparator();

	m_exportAllObjectsAction = new KAction(this);
	menu->addAction(exportAllObjectsAction());

	m_importAllObjectsAction = new KAction(this);
	menu->addAction(importAllObjectsAction());

	KAction *action = new KAction(source()->sharedData()->tr("WESTRING_MENU_OE_EXPORTALL", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(exportAll()));

	action = new KAction(source()->sharedData()->tr("WESTRING_MENU_OE_IMPORTALL", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(importAll()));
}

void ObjectEditor::createEditActions(KMenu *menu)
{
	m_copyObjectAction = new KAction(this);
	menu->addAction(copyObjectAction());

	m_pasteObjectAction = new KAction(this);
	menu->addAction(pasteObjectAction());

	menu->addSeparator();

	// TODO add action copy field and paste field

	menu->addSeparator();

	KAction *action = new KAction(source()->sharedData()->tr("WESTRING_MENU_VIEWINPALETTE", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(viewInPalette()));

	action = new KAction(source()->sharedData()->tr("WESTRING_MENU_OE_FIND", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(find()));

	action = new KAction(source()->sharedData()->tr("WESTRING_MENU_OE_FINDNEXT", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(findNext()));

	action = new KAction(source()->sharedData()->tr("WESTRING_MENU_OE_FINDPREV", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(findPrevious()));

	menu->addSeparator();

	menu->addSeparator();

	const QString modifyField = this->source()->sharedData()->tr("WESTRING_FIELDLIST_CM_MODIFY");
	const QString resetField = this->source()->sharedData()->tr("WESTRING_FIELDLIST_CM_RESET");
	m_modifyFieldAction = new KAction(modifyField, this);
	m_resetFieldAction = new KAction(resetField, this);
	menu->addAction(m_modifyFieldAction);
	menu->addAction(m_resetFieldAction);

	connect(m_modifyFieldAction, SIGNAL(triggered()), this, SLOT(modifyField()));
	connect(m_resetFieldAction, SIGNAL(triggered()), this, SLOT(resetField()));

	action = new KAction(source()->sharedData()->tr("WESTRING_MENU_OE_AUTOFILL", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(autoFill()));
}

void ObjectEditor::createMenus(KMenuBar *menuBar)
{
	/// \todo Create menu "view" with meta data categories of current tab and "raw data" and sort by names actions
	m_viewMenu = new QMenu(source()->sharedData()->tr("WESTRING_MENU_VIEW"), this);

	// TODO is Frozen Throne
	m_rawDataAction = new QAction(this->source()->sharedData()->tr("WESTRING_MENU_OE_TOGGLERAWDATA"), this);
	m_rawDataAction->setCheckable(true);
	connect(m_rawDataAction, SIGNAL(triggered(bool)), this, SLOT(showRawData(bool)));
	m_viewMenu->addAction(m_rawDataAction);

	menuBar->addMenu(m_viewMenu);
}

void ObjectEditor::createWindowsActions(WindowsMenu *menu)
{
}

void ObjectEditor::createToolButtons(ModuleToolBar *toolBar)
{
	toolBar->addCustomAction(copyObjectAction());
	toolBar->addCustomAction(pasteObjectAction());
	toolBar->addCustomAction(newObjectAction());
	toolBar->addCustomSeparator();
}

SettingsInterface* ObjectEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void ObjectEditor::onSwitchToMap(Map *map)
{
	for (int i = 0; i < this->tabWidget()->count(); ++i)
	{
		tab(i)->onSwitchToMap(map);
	}
}

void ObjectEditor::currentChanged(int index)
{
	/*
	// refresh file actions
	foreach (QAction *action, m_currentActions)
		delete action;

	m_currentActions.clear();

	ObjectEditorTab *t = tab(index);
	/// \todo ObjectEditorTab actions have to be placed before
	m_currentActions << t->createFileActions(fileMenu());
	m_currentActions << t->createEditActions(editMenu());
	t->createToolButtons(toolBar());
	*/
	removeCurrentActions();
	m_currentTab = tab(index);
	addCurrentActions();
	setWindowTitle(tab(index)->name());
	m_rawDataAction->setChecked(this->currentTab()->treeModel()->showRawData());
}

void ObjectEditor::removeCurrentActions()
{
	disconnect(newObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(newObject()));
	disconnect(renameObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(renameObject()));
	disconnect(deleteObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(deleteObject()));
	disconnect(resetObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(resetObject()));
	disconnect(resetAllObjectsAction(), SIGNAL(triggered()), currentTab(), SLOT(resetAllObjects()));
	disconnect(exportAllObjectsAction(), SIGNAL(triggered()), currentTab(), SLOT(exportAllObjects()));
	disconnect(importAllObjectsAction(), SIGNAL(triggered()), currentTab(), SLOT(importAllObjects()));
	disconnect(copyObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(copyObject()));
	disconnect(pasteObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(pasteObject()));
}

void ObjectEditor::addCurrentActions()
{
	// new object

	newObjectAction()->setText(currentTab()->newObjectText());
	newObjectAction()->setIcon(source()->sharedData()->icon(currentTab()->newObjectIconUrl(), this));

	connect(newObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(newObject()));

	// rename object

	renameObjectAction()->setText(currentTab()->renameObjectText());
	connect(renameObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(renameObject()));

	// delete object

	deleteObjectAction()->setText(currentTab()->deleteObjectText());
	connect(deleteObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(deleteObject()));


	// reset object

	resetObjectAction()->setText(currentTab()->resetObjectText());
	connect(resetObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(resetObject()));

	resetAllObjectsAction()->setText(currentTab()->resetAllObjectsText());
	connect(resetAllObjectsAction(), SIGNAL(triggered()), currentTab(), SLOT(resetAllObjects()));;

	// export object

	exportAllObjectsAction()->setText(currentTab()->exportAllObjectsText());
	connect(exportAllObjectsAction(), SIGNAL(triggered()), currentTab(), SLOT(exportAllObjects()));

	importAllObjectsAction()->setText(currentTab()->importAllObjectsText());
	connect(importAllObjectsAction(), SIGNAL(triggered()), currentTab(), SLOT(importAllObjects()));

	// copy object

	copyObjectAction()->setText(currentTab()->copyObjectText());
	copyObjectAction()->setIcon(source()->sharedData()->icon(currentTab()->copyObjectIconUrl(), this));
	connect(copyObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(copyObject()));

	pasteObjectAction()->setText(currentTab()->pasteObjectText());
	pasteObjectAction()->setIcon(source()->sharedData()->icon(currentTab()->pasteObjectIconUrl(), this));
	connect(pasteObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(pasteObject()));

	pasteObjectAction()->setEnabled(!currentTab()->clipboardIsEmpty());
}

void ObjectEditor::showRawData(bool checked)
{
	this->currentTab()->setShowRawData(checked);
}

#include "moc_objecteditor.cpp"

}

}
