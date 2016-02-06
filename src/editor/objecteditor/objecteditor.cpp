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

#include <KFileDialog>
#include <KMessageBox>

#include "objecteditor.hpp"
#include "../sharedobjectdata.hpp"
#include "objecteditortab.hpp"
#include "editor.hpp"
#include "uniteditor.hpp"
#include "itemeditor.hpp"
#include "abilityeditor.hpp"
#include "destructableeditor.hpp"
#include "doodadeditor.hpp"
#include "watereditor.hpp"
#include "weathereditor.hpp"
#include "misceditor.hpp"
#include "objecttreemodel.hpp"
#include "../moduletoolbar.hpp"

namespace wc3lib
{

namespace editor
{

ObjectEditor::ObjectEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : Module(source, parent, f)
, m_tabWidget(new QTabWidget(this))
, m_currentTab(0)
, m_unitEditor(0)
, m_doodadEditor(0)
, m_destructableEditor(0)
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
, m_waterEditor(0)
, m_weatherEditor(0)
, m_soundEntryEditor(0)
, m_miscEditor(0)
, m_newObjectAction(0)
, m_renameObjectAction(0)
, m_deleteObjectAction(0)
, m_resetObjectAction(0)
, m_resetAllObjectsAction(0)
, m_exportAllObjectsAction(0)
, m_importAllObjectsAction(0)
, m_copyObjectAction(0)
, m_pasteObjectAction(0)
, m_modifyFieldAction(0)
, m_resetFieldAction(0)
, m_compressAction(0)
, m_widgetizeAction(0)
, m_viewMenu(0)
, m_rawDataAction(0)
{
	Module::setupUi();
	//Ui::ObjectEditor::setupUi(this);
	topLayout()->addWidget(tabWidget());
}

ObjectEditor::~ObjectEditor()
{
}

bool ObjectEditor::configure()
{
	readSettings();

	// Update required files if started as stand-alone module
	if (!hasEditor())
	{
		if (!source()->configure(this))
		{
			return false;
		}

		try
		{
			source()->sharedData()->refreshWorldEditorStrings(this);
			source()->sharedData()->refreshWorldEditorGameStrings(this);
			source()->sharedData()->refreshWorldEditData(this);
			source()->sharedData()->sharedObjectData()->unitEditorData()->setSource(source());
			source()->sharedData()->sharedObjectData()->unitEditorData()->load();
		}
		catch (wc3lib::Exception &e)
		{
			KMessageBox::error(0, i18n("Error when loading default files: %1", e.what()));

			return false;
		}
	}

	/*
	 * Create all tabs after the actions have been created.
	 */
	m_unitEditor = new UnitEditor(source(), source()->sharedData()->sharedObjectData()->unitData().get(), this, this);
	m_itemEditor = new ItemEditor(source(), source()->sharedData()->sharedObjectData()->itemData().get(), this, this);
	m_destructableEditor = new DestructableEditor(source(), source()->sharedData()->sharedObjectData()->destructableData().get(), this, this);
	m_doodadEditor = new DoodadEditor(source(), source()->sharedData()->sharedObjectData()->doodadData().get(), this, this);
	m_abilityEditor = new AbilityEditor(source(), source()->sharedData()->sharedObjectData()->abilityData().get(), this, this);
	m_waterEditor = new WaterEditor(source(), source()->sharedData()->sharedObjectData()->waterData().get(), this, this);
	m_weatherEditor = new WeatherEditor(source(), source()->sharedData()->sharedObjectData()->weatherData().get(), this, this);
	m_miscEditor = new MiscEditor(source(), source()->sharedData()->sharedObjectData()->miscData().get(), this, this);

	tabWidget()->addTab(unitEditor(), unitEditor()->tabIcon(this), unitEditor()->name());
	tabWidget()->addTab(itemEditor(), itemEditor()->tabIcon(this), itemEditor()->name());
	tabWidget()->addTab(destructableEditor(), destructableEditor()->tabIcon(this), destructableEditor()->name());
	tabWidget()->addTab(doodadEditor(), doodadEditor()->tabIcon(this), doodadEditor()->name());
	tabWidget()->addTab(abilityEditor(), abilityEditor()->tabIcon(this), abilityEditor()->name());
	tabWidget()->addTab(waterEditor(), waterEditor()->tabIcon(this), waterEditor()->name());
	tabWidget()->addTab(weatherEditor(), weatherEditor()->tabIcon(this), weatherEditor()->name());
	tabWidget()->addTab(miscEditor(), miscEditor()->tabIcon(this), miscEditor()->name());

	currentChanged(0);
	// connect signal and slot after adding actions and tabs first time!
	connect(tabWidget(), SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));

	retranslateUi();

	return true;
}

void ObjectEditor::retranslateUi()
{
	Module::retranslateUi();

	m_viewMenu->setTitle(source()->sharedData()->tr("WESTRING_MENU_VIEW"));
	m_rawDataAction->setText(this->source()->sharedData()->tr("WESTRING_MENU_OE_TOGGLERAWDATA"));

	const QString modifyField = this->source()->sharedData()->tr("WESTRING_FIELDLIST_CM_MODIFY");
	const QString resetField = this->source()->sharedData()->tr("WESTRING_FIELDLIST_CM_RESET");
	m_modifyFieldAction->setText(modifyField);
	m_resetFieldAction->setText(resetField);
}

ObjectEditorTab* ObjectEditor::tab(int index) const
{
	return boost::polymorphic_cast<ObjectEditorTab*>(this->tabWidget()->widget(index));
}

void ObjectEditor::importCustomObjectsCollection(const map::CustomObjectsCollection& collection)
{
	if (collection.hasUnits())
	{
		loadTabDataOnRequest(Tab::Units);
		this->unitEditor()->unitData()->importCustomObjects(*collection.units());
	}

	if (collection.hasItems())
	{
		loadTabDataOnRequest(Tab::Items);
		this->itemEditor()->itemData()->importCustomObjects(*collection.items());
	}

	if (collection.hasDestructibles())
	{
		loadTabDataOnRequest(Tab::Destructibles);
		this->destructableEditor()->destructableData()->importCustomObjects(*collection.destructibles());
	}

	if (collection.hasDoodads())
	{
		loadTabDataOnRequest(Tab::Doodads);
		this->doodadEditor()->doodadData()->importCustomObjects(*collection.doodads());
	}

	if (collection.hasAbilities())
	{
		loadTabDataOnRequest(Tab::Abilities);
		this->abilityEditor()->abilityData()->importCustomObjects(*collection.abilities());
	}

	// TODO further stuff
}

void ObjectEditor::importCustomUnits(const map::CustomUnits& customUnits)
{
	loadTabDataOnRequest(0);
	this->unitEditor()->unitData()->importCustomUnits(customUnits);
	loadTabDataOnRequest(1);
	this->itemEditor()->itemData()->importCustomUnits(customUnits);
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
	QString file;

	if (this->source()->download(url, file, this))
	{
		ifstream in(file.toUtf8().constData());

		if (in)
		{
			QFileInfo fileInfo(url.toLocalFile());

			if (fileInfo.suffix() == "w3o")
			{
				try
				{
					map::CustomObjectsCollection collection;
					collection.read(in);

					importCustomObjectsCollection(collection);
				}
				catch (const Exception &e)
				{
					KMessageBox::error(this, e.what());
				}
			}
			// TODO support custom object FILES
			else if (fileInfo.suffix() == "w3u")
			{
				try
				{
					map::CustomUnits customUnits;
					customUnits.read(in);

					importCustomUnits(customUnits);
				}
				catch (const Exception &e)
				{
					KMessageBox::error(this, e.what());
				}
			}
			else if (fileInfo.suffix() == "w3m")
			{
				try
				{
					map::W3m map;
					map.open(file.toUtf8().constData());

					if (map.customUnits().get() != 0)
					{
						map.readFileFormat(map.customUnits().get());

						importCustomUnits(*map.customUnits());

						this->unitEditor()->unitData()->applyMapStrings(map);
						this->itemEditor()->itemData()->applyMapStrings(map);
					}
				}
				catch (const Exception &e)
				{
					KMessageBox::error(this, e.what());
				}
			}
			else if (fileInfo.suffix() == "w3x")
			{
				try
				{
					map::W3x map;
					map.open(file.toUtf8().constData());

					if (map.customUnits().get() != 0)
					{
						map.readFileFormat(map.customUnits().get());
						this->unitEditor()->unitData()->importCustomUnits(*map.customUnits());
						this->unitEditor()->unitData()->applyMapStrings(map);
						this->itemEditor()->itemData()->importCustomUnits(*map.customUnits());
						this->itemEditor()->itemData()->applyMapStrings(map);
					}
					else
					{
						// TODO get custom objects collection
					}
				}
				catch (const Exception &e)
				{
					KMessageBox::error(this, e.what());
				}
			}
		}
		else
		{
			KMessageBox::error(this, tr("Unable to open downloaded file %1.").arg(file));
		}

		this->source()->removeTempFile(file);
	}
}

void ObjectEditor::importAll()
{
	const KUrl url = KFileDialog::getOpenUrl(KUrl(), tr("*|All Files\n%1\nCustom Units (*.w3u)\nMap (*.w3m *.w3x)").arg(objectsCollectionFilter()), this, source()->sharedData()->tr("WESTRING_MENU_OE_IMPORTALL", "WorldEditStrings"));

	if (!url.isEmpty())
	{
		this->importAll(url);
	}
}

void ObjectEditor::compress()
{
	if (QMessageBox::question(this, tr("Compress all objects?"), tr("Are you sure you want to compress all objects of this tab? This might take while. Besides you don't want to lose modifications which might be used later when objects are changed again. All modifications are reset which have the same values as the defaults as well, so make sure you loaded the correct defaults from your sources."), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		const int counter = this->currentTab()->objectData()->compress();

		QMessageBox::information(this, tr("Compression done"), tr("Compressed %1 modifications of objects.").arg(counter));
	}
}

void ObjectEditor::widgetize()
{
	this->currentTab()->widgetizeAllObjects();
}

void ObjectEditor::createFileActions(QMenu *menu)
{
	m_newObjectAction = new QAction(this);
	menu->addAction(newObjectAction());

	m_renameObjectAction = new QAction(this);
	menu->addAction(renameObjectAction());

	m_deleteObjectAction = new QAction(this);
	menu->addAction(deleteObjectAction());

	menu->addSeparator();

	m_resetObjectAction = new QAction(this);
	menu->addAction(resetObjectAction());

	m_resetAllObjectsAction = new QAction(this);
	menu->addAction(resetAllObjectsAction());

	menu->addSeparator();

	m_exportAllObjectsAction = new QAction(this);
	menu->addAction(exportAllObjectsAction());

	m_importAllObjectsAction = new QAction(this);
	menu->addAction(importAllObjectsAction());

	QAction *action = new QAction(source()->sharedData()->tr("WESTRING_MENU_OE_EXPORTALL", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(exportAll()));

	action = new QAction(source()->sharedData()->tr("WESTRING_MENU_OE_IMPORTALL", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(importAll()));
}

void ObjectEditor::createEditActions(QMenu *menu)
{
	m_copyObjectAction = new QAction(this);
	menu->addAction(copyObjectAction());

	m_pasteObjectAction = new QAction(this);
	menu->addAction(pasteObjectAction());

	menu->addSeparator();

	// TODO add action copy field and paste field

	menu->addSeparator();

	QAction *action = new QAction(source()->sharedData()->tr("WESTRING_MENU_VIEWINPALETTE", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(viewInPalette()));

	action = new QAction(source()->sharedData()->tr("WESTRING_MENU_OE_FIND", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(find()));

	action = new QAction(source()->sharedData()->tr("WESTRING_MENU_OE_FINDNEXT", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(findNext()));

	action = new QAction(source()->sharedData()->tr("WESTRING_MENU_OE_FINDPREV", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(findPrevious()));

	menu->addSeparator();

	menu->addSeparator();

	m_modifyFieldAction = new QAction(this);
	m_resetFieldAction = new QAction(this);
	menu->addAction(m_modifyFieldAction);
	menu->addAction(m_resetFieldAction);

	connect(m_modifyFieldAction, SIGNAL(triggered()), this, SLOT(modifyField()));
	connect(m_resetFieldAction, SIGNAL(triggered()), this, SLOT(resetField()));

	action = new QAction(source()->sharedData()->tr("WESTRING_MENU_OE_AUTOFILL", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(autoFill()));

	menu->addSeparator();

	m_compressAction = new QAction(tr("Compress all objects"), this);
	menu->addAction(m_compressAction);
	connect(m_compressAction, SIGNAL(triggered()), this, SLOT(compress()));

	m_widgetizeAction = new QAction(tr("Widgetize all objects"), this);
	menu->addAction(m_widgetizeAction);
	connect(m_widgetizeAction, SIGNAL(triggered()), this, SLOT(widgetize()));
}

void ObjectEditor::createMenus(QMenuBar *menuBar)
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

	loadTabDataOnRequest(index);
}

void ObjectEditor::loadTabDataOnRequest(int index)
{
	ObjectEditorTab *tab = this->tab(index);
	/*
	 * Load data on request when the tab is shown for the first time.
	 */
	if (tab->objectData() != 0)
	{
		qDebug() << "Show" << tab->name() << "first time";
		/*
		 * Indicate loading by changing the cursor to busy.
		 * The process of loading object data might take quite some time.
		 */
		tab->objectData()->loadOnRequest(this);
	}

	/*
	 * Load data on request when the tab is shown for the first time.
	 */
	if (tab->objectData() != 0 && tab->treeModel()->objectData() == 0)
	{
		qDebug() << "Show" << tab->name() << "first time";
		/*
		 * Indicate loading by changing the cursor to busy.
		 * The process of loading object data might take quite some time.
		 */
		QCursor cursor = this->cursor();
		cursor.setShape(Qt::BusyCursor);
		this->setCursor(cursor);

		try
		{
			tab->treeModel()->load(tab->source(), tab->objectData(), tab);
		}
		catch (const Exception &e)
		{
			KMessageBox::error(this, e.what());
		}

		cursor = this->cursor();
		cursor.setShape(Qt::ArrowCursor);
		this->setCursor(cursor);
	}
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

QIcon ObjectEditor::icon()
{
	return QIcon(this->source()->sharedData()->worldEditDataPixmap("ToolBarIcon_Module_ObjectEditor", "WorldEditArt", this));
}

#include "moc_objecteditor.cpp"

}

}
