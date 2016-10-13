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
#include <QtWidgets/QtWidgets>

#include "objecteditor.hpp"
#include "../sharedobjectdata.hpp"
#include "objecteditortab.hpp"
#include "uniteditor.hpp"
#include "itemeditor.hpp"
#include "abilityeditor.hpp"
#include "buffeditor.hpp"
#include "upgradeeditor.hpp"
#include "destructableeditor.hpp"
#include "doodadeditor.hpp"
#include "watereditor.hpp"
#include "weathereditor.hpp"
#include "misceditor.hpp"
#include "skineditor.hpp"
#include "objecttreemodel.hpp"
#include "../moduletoolbar.hpp"
#include "../upgradedata.hpp"

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
, m_skinEditor(0)
, m_newObjectAction(0)
, m_renameObjectAction(0)
, m_deleteObjectAction(0)
, m_resetObjectAction(0)
, m_resetAllObjectsAction(0)
, m_exportAllObjectsAction(0)
, m_importAllObjectsAction(0)
, m_exportAllAction(0)
, m_importAllAction(0)
, m_compressAllAction(0)
, m_widgetizeAllAction(0)
, m_copyObjectAction(0)
, m_pasteObjectAction(0)
, m_viewInPaletteAction(0)
, m_findAction(0)
, m_findNextAction(0)
, m_findPreviousAction(0)
, m_autoFillAction(0)
, m_modifyFieldAction(0)
, m_resetFieldAction(0)
, m_compressAction(0)
, m_widgetizeAction(0)
, m_viewMenu(0)
, m_rawDataAction(0)
, m_sortByNameAction(0)
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
		catch (const wc3lib::Exception &e)
		{
			QMessageBox::critical(this, tr("Error"), tr("Error when loading default files: %1").arg(e.what()));

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
	m_buffEditor = new BuffEditor(source(), source()->sharedData()->sharedObjectData()->buffData().get(), this, this);
	m_upgradeEditor = new UpgradeEditor(source(), source()->sharedData()->sharedObjectData()->upgradeData().get(), this, this);
	m_waterEditor = new WaterEditor(source(), source()->sharedData()->sharedObjectData()->waterData().get(), this, this);
	m_weatherEditor = new WeatherEditor(source(), source()->sharedData()->sharedObjectData()->weatherData().get(), this, this);
	m_miscEditor = new MiscEditor(source(), source()->sharedData()->sharedObjectData()->miscData().get(), this, this);
	m_skinEditor = new SkinEditor(source(), source()->sharedData()->sharedObjectData()->skinData().get(), this, this);

	tabWidget()->addTab(unitEditor(), unitEditor()->tabIcon(this), unitEditor()->name());
	tabWidget()->addTab(itemEditor(), itemEditor()->tabIcon(this), itemEditor()->name());
	tabWidget()->addTab(destructableEditor(), destructableEditor()->tabIcon(this), destructableEditor()->name());
	tabWidget()->addTab(doodadEditor(), doodadEditor()->tabIcon(this), doodadEditor()->name());
	tabWidget()->addTab(abilityEditor(), abilityEditor()->tabIcon(this), abilityEditor()->name());
	tabWidget()->addTab(buffEditor(), buffEditor()->tabIcon(this), buffEditor()->name());
	tabWidget()->addTab(upgradeEditor(), upgradeEditor()->tabIcon(this), upgradeEditor()->name());
	tabWidget()->addTab(waterEditor(), waterEditor()->tabIcon(this), waterEditor()->name());
	tabWidget()->addTab(weatherEditor(), weatherEditor()->tabIcon(this), weatherEditor()->name());
	tabWidget()->addTab(miscEditor(), miscEditor()->tabIcon(this), miscEditor()->name());
	tabWidget()->addTab(skinEditor(), skinEditor()->tabIcon(this), skinEditor()->name());

	// update current object data
	currentChanged(0);
	// connect signal and slot after adding actions and tabs first time!
	connect(tabWidget(), SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));

	retranslateUi();

	return true;
}

void ObjectEditor::retranslateUi()
{
	Module::retranslateUi();

	m_exportAllAction->setText(source()->sharedData()->tr("WESTRING_MENU_OE_EXPORTALL", "WorldEditStrings"));
	m_importAllAction->setText(source()->sharedData()->tr("WESTRING_MENU_OE_IMPORTALL", "WorldEditStrings"));

	m_compressAllAction->setText(tr("Compress all objects"));
	m_widgetizeAllAction->setText(tr("Widgetize all objects"));

	m_viewMenu->setTitle(source()->sharedData()->tr("WESTRING_MENU_VIEW"));
	m_rawDataAction->setText(this->source()->sharedData()->tr("WESTRING_MENU_OE_TOGGLERAWDATA"));
	m_sortByNameAction->setText(this->source()->sharedData()->tr("WESTRING_REGION_CM_SORT"));

	m_viewInPaletteAction->setText(source()->sharedData()->tr("WESTRING_MENU_VIEWINPALETTE", "WorldEditStrings"));
	m_findAction->setText(source()->sharedData()->tr("WESTRING_MENU_OE_FIND", "WorldEditStrings"));
	m_findNextAction->setText(source()->sharedData()->tr("WESTRING_MENU_OE_FINDNEXT", "WorldEditStrings"));
	m_findPreviousAction->setText(source()->sharedData()->tr("WESTRING_MENU_OE_FINDPREV", "WorldEditStrings"));
	m_autoFillAction->setText(source()->sharedData()->tr("WESTRING_MENU_OE_AUTOFILL", "WorldEditStrings"));

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

	if (collection.hasBuffs())
	{
		loadTabDataOnRequest(Tab::Buffs);
		this->buffEditor()->buffData()->importCustomObjects(*collection.buffs());
	}

	if (collection.hasUpgrades())
	{
		loadTabDataOnRequest(Tab::Upgrades);
		this->upgradeEditor()->upgradeData()->importCustomObjects(*collection.upgrades());
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

void ObjectEditor::exportCustomObjectsCollection(map::CustomObjectsCollection &collection)
{
	if (this->unitEditor()->unitData() != nullptr)
	{
		map::CustomObjectsCollection::CustomObjectsPtr units(new map::CustomObjects(unitEditor()->unitData()->customObjects()));
		collection.units().swap(units);
	}

	if (this->itemEditor()->itemData() != nullptr)
	{
		map::CustomObjectsCollection::CustomObjectsPtr items(new map::CustomObjects(itemEditor()->itemData()->customObjects()));
		collection.items().swap(items);
	}

	// TODO export all
}

void ObjectEditor::exportAll()
{
	// TODO collect all tab data (requires Frozen Throne)

	const QUrl url = QFileDialog::getSaveFileUrl(this, source()->sharedData()->tr("WESTRING_MENU_OE_EXPORTALL", "WorldEditStrings"), QUrl(), tr("All Files (*);;Custom Objects Collection (*.%1)").arg(objectsCollectionFilter()));

	if (!url.isEmpty())
	{
		map::CustomObjectsCollection collection;
		exportCustomObjectsCollection(collection);

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
						QMessageBox::critical(this, tr("Error"), tr("Unable to save file."));
					}
				}
				catch (Exception &e)
				{
					QMessageBox::critical(this, tr("Error"), e.what());
				}
			}
			else
			{
				QMessageBox::critical(this, tr("Error"), tr("Unable to open temporary file %1.").arg(file.fileName()));
			}
		}
		else
		{
			QMessageBox::critical(this, tr("Error"), tr("Unable to open temporary file %1.").arg(file.fileName()));
		}
	}
}

void ObjectEditor::importAll(const QUrl &url)
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
					QMessageBox::critical(this, tr("Error"), e.what());
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
					QMessageBox::critical(this, tr("Error"), e.what());
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
					QMessageBox::critical(this, tr("Error"), e.what());
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
					QMessageBox::critical(this, tr("Error"), e.what());
				}
			}
		}
		else
		{
			QMessageBox::critical(this, tr("Error"), tr("Unable to open downloaded file %1.").arg(file));
		}

		this->source()->removeTempFile(file);
	}
}

void ObjectEditor::importAll()
{
	if (QMessageBox::question(this, tr("Import All"), tr("Importing all objects replaces all of your current modifications. Continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
	{
		const QUrl url = QFileDialog::getOpenFileUrl(this, source()->sharedData()->tr("WESTRING_MENU_OE_IMPORTALL", "WorldEditStrings"), QUrl(), tr("All Files (*);;%1;;Custom Units (*.w3u);;Map (*.w3m *.w3x)").arg(objectsCollectionFilter()));

		if (!url.isEmpty())
		{
			this->importAll(url);
		}
	}
}

void ObjectEditor::compressAll()
{
	if (QMessageBox::question(this, tr("Compress all objects?"), tr("Are you sure you want to compress all objects of this tab? This might take while. Besides you don't want to lose modifications which might be used later when objects are changed again. All modifications are reset which have the same values as the defaults as well, so make sure you loaded the correct defaults from your sources."), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		int counter = 0;

		for (int i = 0; i < this->m_tabWidget->count(); ++i)
		{
			if (this->tab(i)->objectData() == nullptr)
			{
				loadTabDataOnRequest(i);
			}

			if (this->tab(i)->objectData() != nullptr)
			{
				counter += this->tab(i)->objectData()->compress();
			}
		}

		QMessageBox::information(this, tr("Compression done"), tr("Compressed %1 modifications of objects.").arg(counter));
	}
}


void ObjectEditor::widgetizeAll()
{
	const QString &dir = QFileDialog::getExistingDirectory(this, tr("Widgetize All Objects"));

	if (!dir.isEmpty())
	{
		try
		{
			for (int i = 0; i < this->m_tabWidget->count(); ++i)
			{
				if (this->tab(i)->objectData() == nullptr)
				{
					loadTabDataOnRequest(i);
				}

				this->tab(i)->objectData()->widgetize(QUrl::fromLocalFile(dir));
			}
		}
		catch (const Exception &e)
		{
			QMessageBox::critical(this, tr("Error on widgetizing all objects"), e.what());
		}
	}
}

void ObjectEditor::find()
{
	const QString findExpression = QInputDialog::getText(this, tr("Find"), tr("Text:"));

	if (!findExpression.isEmpty())
	{
		// TODO implement
	}
}

void ObjectEditor::findNext()
{
}

void ObjectEditor::findPrevious()
{
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

	m_exportAllAction = new QAction(this);
	menu->addAction(m_exportAllAction);
	connect(m_exportAllAction, SIGNAL(triggered()), this, SLOT(exportAll()));

	m_importAllAction = new QAction(this);
	menu->addAction(m_importAllAction);
	connect(m_importAllAction, SIGNAL(triggered()), this, SLOT(importAll()));

	menu->addSeparator();

	m_compressAllAction = new QAction(this);
	menu->addAction(m_compressAllAction);
	connect(m_compressAllAction, SIGNAL(triggered()), this, SLOT(compressAll()));

	m_widgetizeAllAction = new QAction(this);
	menu->addAction(m_widgetizeAllAction);
	connect(m_widgetizeAllAction, SIGNAL(triggered()), this, SLOT(widgetizeAll()));
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

	m_viewInPaletteAction = new QAction(this);
	menu->addAction(m_viewInPaletteAction);
	connect(m_viewInPaletteAction, SIGNAL(triggered()), this, SLOT(viewInPalette()));

	m_findAction = new QAction(this);
	menu->addAction(m_findAction);
	connect(m_findAction, SIGNAL(triggered()), this, SLOT(find()));

	m_findNextAction = new QAction(this);
	menu->addAction(m_findNextAction);
	connect(m_findNextAction, SIGNAL(triggered()), this, SLOT(findNext()));

	m_findPreviousAction = new QAction(this);
	menu->addAction(m_findPreviousAction);
	connect(m_findPreviousAction, SIGNAL(triggered()), this, SLOT(findPrevious()));

	menu->addSeparator();

	menu->addSeparator();

	m_modifyFieldAction = new QAction(this);
	m_resetFieldAction = new QAction(this);
	menu->addAction(m_modifyFieldAction);
	menu->addAction(m_resetFieldAction);

	connect(m_modifyFieldAction, SIGNAL(triggered()), this, SLOT(modifyField()));
	connect(m_resetFieldAction, SIGNAL(triggered()), this, SLOT(resetField()));

	m_autoFillAction = new QAction(this);
	menu->addAction(m_autoFillAction);
	connect(m_autoFillAction, SIGNAL(triggered()), this, SLOT(autoFill()));

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

	m_sortByNameAction = new QAction(this->source()->sharedData()->tr("WESTRING_REGION_CM_SORT"), this);
	m_sortByNameAction->setCheckable(true);
	connect(m_sortByNameAction, SIGNAL(triggered(bool)), this, SLOT(sortByName(bool)));
	m_viewMenu->addAction(m_sortByNameAction);

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

	qDebug() << "Current changed to " << index;

	removeCurrentActions();
	m_currentTab = tab(index);
	qDebug() << "Before adding actions";
	addCurrentActions();
	qDebug() << "After adding actions";
	setWindowTitle(tab(index)->name());
	m_rawDataAction->setChecked(this->currentTab()->treeModel()->showRawData());
	m_sortByNameAction->setChecked(this->currentTab()->sortByName());
	qDebug() << "After raw data";

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
			QMessageBox::critical(this, tr("Error"), e.what());
		}

		cursor = this->cursor();
		cursor.setShape(Qt::ArrowCursor);
		this->setCursor(cursor);

		qDebug() << "After loading object data";
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

void ObjectEditor::sortByName(bool sort)
{
	this->currentTab()->setSortByName(sort);
}

QIcon ObjectEditor::icon()
{
	return QIcon(this->source()->sharedData()->worldEditDataPixmap("ToolBarIcon_Module_ObjectEditor", "WorldEditArt", this));
}

#include "moc_objecteditor.cpp"

}

}
