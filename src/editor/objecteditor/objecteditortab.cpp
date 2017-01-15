/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include "objecteditortab.hpp"
#include "../objectdata.hpp"
#include "objecttableview.hpp"
#include "objecttablemodel.hpp"
#include "objecttreeview.hpp"
#include "objecttreemodel.hpp"
#include "objecttreesortfilterproxymodel.hpp"
#include "objectiddialog.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectEditorTab::ObjectEditorTab(MpqPriorityList *source, ObjectData *objectData, const QString &groupName, ObjectEditor *objectEditor, QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f)
, m_source(source)
, m_filterSearchLine(0)
, m_tableFilterSearchLine(0)
, m_treeView(0)
, m_treeModel(0)
, m_tableView(0)
, m_tableModel(0)
, m_objectData(objectData)
, m_groupName(groupName)
, m_objectEditor(objectEditor)
, m_sortByName(false)
, m_showRawData(false)
, m_idDialog(new ObjectIdDialog(this))
{
	QSettings settings("wc3lib", "wc3lib");
	settings.beginGroup(this->groupName());
	this->m_recentExportUrl = settings.value("recentExportUrl").toUrl();
	this->m_recentImportUrl = settings.value("recentImportUrl").toUrl();
	settings.endGroup();
}

ObjectEditorTab::~ObjectEditorTab()
{
	QSettings settings("wc3lib", "wc3lib");
	settings.beginGroup(this->groupName());
	settings.setValue("recentExportUrl", this->m_recentExportUrl);
	settings.setValue("recentImportUrl", this->m_recentImportUrl);
	settings.endGroup();
}

void ObjectEditorTab::setupUi()
{
	qDebug() << "Show tab " << this->name();

	/*
	 * Tree View
	 */
	m_filterSearchLine = new QLineEdit(this);

	m_treeView = new ObjectTreeView(this);
	m_treeModel = createTreeModel();
	ObjectTreeSortFilterProxyModel *proxyModel = new ObjectTreeSortFilterProxyModel(this);
	proxyModel->setSourceModel(this->m_treeModel);
	treeView()->setModel(proxyModel);


	//m_filterSearchLine->setProxy(this->proxyModel());
	connect(m_filterSearchLine, &QLineEdit::textChanged, this, &ObjectEditorTab::filterObjects);

	/*
	 * Table View
	 */
	m_tableFilterSearchLine = new QLineEdit(this);

	m_tableView = new ObjectTableView(this);
	/*
	 * Hide columns initially and wait for activation of an object.
	 */
	m_tableView->horizontalHeader()->setVisible(false);
	m_tableView->hideColumn(0);
	m_tableView->hideColumn(1);

	m_tableModel = new ObjectTableModel(this);

	QSortFilterProxyModel *tableProxyModel = new QSortFilterProxyModel(this);
	tableProxyModel->setSourceModel(this->m_tableModel);
	m_tableView->setModel(tableProxyModel);

	// TODO set proxy
	//m_tableFilterSearchLine->setProxy(this->tableProxyModel());
	connect(m_tableFilterSearchLine, &QLineEdit::textChanged, this, &ObjectEditorTab::filterFields);


	QVBoxLayout *leftLayout = new QVBoxLayout(this);
	QWidget *leftLayoutWidget = new QWidget(this);
	leftLayoutWidget->setLayout(leftLayout);
	leftLayout->addWidget(m_filterSearchLine);
	leftLayout->addWidget(m_treeView);
	leftLayoutWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

	QVBoxLayout *rightLayout = new QVBoxLayout(this);
	QWidget *rightLayoutWidget = new QWidget(this);
	rightLayoutWidget->setLayout(rightLayout);
	rightLayout->addWidget(m_tableFilterSearchLine);
	rightLayout->addWidget(m_tableView);
	rightLayoutWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

	QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
	splitter->addWidget(leftLayoutWidget);
	splitter->addWidget(rightLayoutWidget);
	this->setLayout(new QHBoxLayout(this));
	this->layout()->addWidget(splitter);

	connect(m_treeView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

bool ObjectEditorTab::clipboardIsEmpty()
{
	return this->m_clipboard.empty();
}

map::CustomObjects::Table ObjectEditorTab::selection() const
{
	map::CustomObjects::Table table;

	QItemSelectionModel *selection = this->treeView()->selectionModel();

	foreach (QModelIndex index, selection->selectedIndexes())
	{
		ObjectTreeItem *item = this->treeModel()->item(proxyModel()->mapToSource(index));

		if (!item->isFolder())
		{
			table.push_back(new map::CustomObjects::Object(objectData()->customObject(item->originalObjectId(), item->customObjectId())));
		}
	}

	return table;
}

bool ObjectEditorTab::selectObject(const QString &originalObjectId, const QString &customObjectId)
{
	// store the selected field for later reselection
	const ObjectData::FieldId oldFieldId = selectedField();

	ObjectTreeItem *item = this->treeModel()->item(originalObjectId, customObjectId);

	if (item != 0)
	{
		QItemSelectionModel *selection = treeView()->selectionModel();
		const QModelIndex index = item->modelIndex(this->treeModel());

		if (!selection->isSelected(index))
		{
			selection->select(item->modelIndex(this->treeModel()), QItemSelectionModel::Rows);
			this->treeView()->setSelectionModel(selection);
		}

		tableModel()->load(objectData(), originalObjectId, customObjectId, this);

		this->tableView()->horizontalHeader()->setVisible(true);
		this->tableView()->showColumn(0);
		this->tableView()->showColumn(1);

		this->tableView()->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
		this->tableView()->model()->sort(0, Qt::AscendingOrder);

		if (this->hasObjectEditor())
		{
			this->objectEditor()->copyObjectAction()->setEnabled(true);
			this->objectEditor()->renameObjectAction()->setEnabled(true);
			this->objectEditor()->deleteObjectAction()->setEnabled(!customObjectId.isEmpty());
			this->objectEditor()->resetObjectAction()->setEnabled(true);
		}

		// Select the old field if possible. Sometimes field IDs are not shown for all objects. In this case it cannot be selected but in all other cases it will stay selected.
		this->selectField(oldFieldId.fieldId(), oldFieldId.level());

		return true;
	}

	return false;
}

bool ObjectEditorTab::selectField(const QString &fieldId, int level)
{
	if (tableModel()->originalObjectId().isEmpty())
	{
		return false;
	}

	const ObjectData::FieldId fieldIdKey(fieldId, level);
	const int row = this->tableModel()->row(fieldIdKey);

	if (row != -1)
	{
		QItemSelectionModel *selection = tableView()->selectionModel();
		const QModelIndex index = this->tableModel()->index(row, 0);

		if (!selection->isSelected(index))
		{
			selection->select(index, QItemSelectionModel::Rows);
			this->tableView()->setSelectionModel(selection);
		}

		return true;
	}

	return false;
}

ObjectData::FieldId ObjectEditorTab::selectedField() const
{
	QItemSelectionModel *selection = tableView()->selectionModel();

	if (selection == nullptr)
	{
		return ObjectData::FieldId("", 0);
	}

	return tableModel()->fieldId(selection->currentIndex().row());
}

bool ObjectEditorTab::modifyField(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId, int level)
{
	if (this->selectObject(originalObjectId, customObjectId))
	{
		this->tableView()->selectField(fieldId, level);

		return true;
	}

	return false;
}

void ObjectEditorTab::exportAllObjects()
{
	const QString customUnitsSuffix = "w3u";
	const QString customObjectsSuffix = this->objectData()->customObjectsExtension();
	const QString customObjectsCollectionSuffix = "w3o";
	const QString mapSuffix = "w3m";
	const QString xmapSuffix = "w3x";

	const QUrl url = QFileDialog::getSaveFileUrl(this, exportAllObjectsText(), QUrl(), QString("All files (*);;Custom Units (*.%1);;Custom Objects (*.%2);;Custom Objects Collection (*.%3);;Map (*.%4 *.%5)").arg(customUnitsSuffix).arg(customObjectsSuffix).arg(customObjectsCollectionSuffix).arg(mapSuffix).arg(xmapSuffix));

	if (!url.isEmpty())
	{
		const QFileInfo fileInfo(url.toLocalFile());
		const QString suffix = fileInfo.suffix();

		// TODO support directory for meta data file list

		if (suffix == customUnitsSuffix && !this->objectData()->hasCustomUnits())
		{
			QMessageBox::critical(this, tr("Error"), tr("No custom units support."));

			return;
		}
		else if (suffix == customObjectsSuffix && !this->objectData()->hasCustomObjects())
		{
			QMessageBox::critical(this, tr("Error"), tr("No custom objects support."));

			return;
		}

		ofstream out(url.toLocalFile().toUtf8().constData());

		if (out)
		{
			try
			{
				if (suffix == customObjectsSuffix)
				{
					if (this->objectData()->hasCustomObjects())
					{
						qDebug() << "Exporting custom objects";
						this->objectData()->customObjects().write(out);
						this->m_recentExportUrl = url;
					}
				}
				else if (suffix == customUnitsSuffix)
				{
					if (this->objectData()->hasCustomUnits())
					{
						qDebug() << "Exporting custom units";

						this->objectData()->customUnits().write(out);
						this->m_recentExportUrl = url;
					}
				}
			}
			catch (const Exception &e)
			{
				QMessageBox::critical(this, tr("Error"), tr("Error on exporting"), e.what());
			}
		}
	}
}

void ObjectEditorTab::updateImportUrlAndSort(const QUrl &url)
{
	m_recentImportUrl = url;
	this->treeView()->sortByColumn(0, Qt::AscendingOrder);
}

void ObjectEditorTab::importAllObjects()
{
	if (QMessageBox::question(this, tr("Import All"), tr("Importing all objects replaces all of your current modifications. Continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
	{
		const QString customObjectsSuffix = this->objectData()->customObjectsExtension();
		const QString customObjectsCollectionSuffix = "w3o";
		const QString mapSuffix = "w3m";
		const QString xmapSuffix = "w3x";

		const QUrl url = QFileDialog::getOpenFileUrl(this, importAllObjectsText(), m_recentImportUrl, QString("All Files (*);;Custom Objects (*.%1);;Custom Objects Collection (*.%2);;Map (*.%3 *.%4)").arg(customObjectsSuffix).arg(customObjectsCollectionSuffix).arg(mapSuffix).arg(xmapSuffix));

		if (!url.isEmpty())
		{
			const QString suffix = QFileInfo(url.toLocalFile()).suffix();
			qDebug() << "Suffix" << suffix;

			if (suffix == customObjectsSuffix)
			{
				if (this->objectData()->hasCustomObjects())
				{
					ifstream in(url.toLocalFile().toUtf8().constData());

					try
					{
						std::unique_ptr<map::CustomObjects> customObjects(new map::CustomObjects(this->objectData()->type()));
						customObjects->read(in);
						this->objectData()->importCustomObjects(*customObjects);
						this->updateImportUrlAndSort(url);
					}
					catch (const std::exception &e)
					{
						QMessageBox::critical(this, tr("Error"), e.what());
					}
				}
				else if (this->objectData()->hasCustomUnits())
				{
					ifstream in(url.toLocalFile().toUtf8().constData());

					try
					{
						map::CustomUnits customUnits;
						customUnits.read(in);
						this->objectData()->importCustomUnits(customUnits);
						this->updateImportUrlAndSort(url);
					}
					catch (std::exception &e)
					{
						QMessageBox::critical(this, tr("Error"), tr("Error on importing"), e.what());
					}
				}
				else
				{
					QMessageBox::critical(this, tr("Error"), tr("Does not support custom objects or custom units."));
				}
			}
			else if (suffix == customObjectsCollectionSuffix)
			{
				this->importAllObjectsFromCustomObjectsCollection(url);
			}
			else if (suffix == mapSuffix)
			{
				if (this->objectData()->hasCustomUnits())
				{
					try
					{
						std::unique_ptr<map::W3m> map(new map::W3m());
						map->open(url.toLocalFile().toUtf8().constData());
						std::streamsize size = map->readFileFormat(map->customUnits().get());
						qDebug() << "Size of custom units:" << size;
						this->objectData()->importCustomUnits(*map->customUnits());
						this->objectData()->applyMapStrings(*map);

						this->updateImportUrlAndSort(url);
					}
					catch (std::exception &e)
					{
						QMessageBox::critical(this, tr("Error"), e.what());
					}
				}
				else
				{
					QMessageBox::critical(this, tr("Error"), tr("Does not support custom units."));
				}
			}
			// TODO support custom object FILES
			else if (suffix == xmapSuffix)
			{
				QMessageBox::critical(this, tr("Error"), tr("W3X is not supported yet."));
			}
			else
			{
				QMessageBox::critical(this, tr("Error"), tr("Unknown file type."));
			}
		}
	}
}

void ObjectEditorTab::importAllObjectsFromCustomObjectsCollection(const QUrl &url)
{
	if (this->objectData()->hasCustomObjects())
	{
		ifstream in(url.toLocalFile().toUtf8().constData());

		try
		{
			std::unique_ptr<map::CustomObjectsCollection> customObjectsCollection(new map::CustomObjectsCollection());
			customObjectsCollection->read(in);

			switch (this->objectData()->type())
			{
				case map::CustomObjects::Type::Units:
				{
					if (customObjectsCollection->hasUnits())
					{
						this->objectData()->importCustomObjects(*customObjectsCollection->units());
						this->updateImportUrlAndSort(url);
					}
					else
					{
						QMessageBox::critical(this, tr("Error"), tr("Collection has no units."));
					}
				}

				case map::CustomObjects::Type::Items:
				{
					if (customObjectsCollection->hasItems())
					{
						this->objectData()->importCustomObjects(*customObjectsCollection->items());
						this->updateImportUrlAndSort(url);
					}
					else
					{
						QMessageBox::critical(this, tr("Error"), tr("Collection has no items."));
					}
				}

				case map::CustomObjects::Type::Abilities:
				{
					if (customObjectsCollection->hasAbilities())
					{
						this->objectData()->importCustomObjects(*customObjectsCollection->abilities());
						this->updateImportUrlAndSort(url);
					}
					else
					{
						QMessageBox::critical(this, tr("Error"), tr("Collection has no abilities."));
					}
				}

				case map::CustomObjects::Type::Buffs:
				{
					if (customObjectsCollection->hasBuffs())
					{
						this->objectData()->importCustomObjects(*customObjectsCollection->buffs());
						this->updateImportUrlAndSort(url);
					}
					else
					{
						QMessageBox::critical(this, tr("Error"), tr("Collection has no buffs."));
					}
				}

				case map::CustomObjects::Type::Doodads:
				{
					if (customObjectsCollection->hasDoodads())
					{
						this->objectData()->importCustomObjects(*customObjectsCollection->doodads());
						this->updateImportUrlAndSort(url);
					}
					else
					{
						QMessageBox::critical(this, tr("Error"), tr("Collection has no doodads."));
					}
				}

				case map::CustomObjects::Type::Destructibles:
				{
					if (customObjectsCollection->hasDestructibles())
					{
						this->objectData()->importCustomObjects(*customObjectsCollection->destructibles());
						this->updateImportUrlAndSort(url);
					}
					else
					{
						QMessageBox::critical(this, tr("Error"), tr("Collection has no destructibles."));
					}
				}

				case map::CustomObjects::Type::Upgrades:
				{
					if (customObjectsCollection->hasUpgrades())
					{
						this->objectData()->importCustomObjects(*customObjectsCollection->upgrades());
						this->updateImportUrlAndSort(url);
					}
					else
					{
						QMessageBox::critical(this, tr("Error"), tr("Collection has no upgrades."));
					}
				}
			}
		}
		catch (const std::exception &e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}
	else
	{
		QMessageBox::critical(this, tr("Error"), tr("Does not support custom objects."));
	}
}

void ObjectEditorTab::itemClicked(QModelIndex index)
{
	ObjectTreeItem *item = this->treeModel()->item(this->proxyModel()->mapToSource(index));
	qDebug() << "Item:" << item;
	const QString originalObjectId = item->originalObjectId();
	/*
	 * Is folder item.
	 * Hide everything.
	 */
	if (item->isFolder())
	{
		this->tableView()->horizontalHeader()->setVisible(false);
		this->tableView()->hideColumn(0);
		this->tableView()->hideColumn(1);

		if (this->hasObjectEditor())
		{
			this->objectEditor()->copyObjectAction()->setEnabled(false);
			this->objectEditor()->renameObjectAction()->setEnabled(false);
			this->objectEditor()->deleteObjectAction()->setEnabled(false);
			this->objectEditor()->resetObjectAction()->setEnabled(false);
		}
	}
	/*
	 * Is object item.
	 * TODO Update table widget.
	 */
	else
	{
		qDebug() << "activating object" << originalObjectId;
		const QString customObjectId = item->customObjectId();

		this->selectObject(originalObjectId, customObjectId);
	}
}

void ObjectEditorTab::deleteObject()
{
	const QItemSelectionModel *selection = this->treeView()->selectionModel();

	if (!selection->selectedIndexes().isEmpty())
	{
		foreach (QModelIndex index, selection->selectedIndexes())
		{
			ObjectTreeItem *item = this->treeModel()->item(this->proxyModel()->mapToSource(index));

			/*
			 * Only custom untis can be deleted.
			 */
			if (!item->isFolder() && !item->customObjectId().isEmpty())
			{
				/*
				 * Make sure one cannot edit any values of deleted objects.
				 */
				this->tableView()->hideColumn(0);
				this->tableView()->hideColumn(1);

				this->objectData()->deleteObject(item->originalObjectId(), item->customObjectId());
			}
		}
	}
}

void ObjectEditorTab::resetObject()
{
	const QItemSelectionModel *selection = this->treeView()->selectionModel();

	foreach (QModelIndex index, selection->selectedIndexes())
	{
		ObjectTreeItem *item = this->treeModel()->item(proxyModel()->mapToSource(index));

		if (!item->isFolder())
		{
			this->objectData()->resetObject(item->originalObjectId(), item->customObjectId());
		}
	}
}

void ObjectEditorTab::resetAllObjects()
{
	for (ObjectTreeModel::Items::iterator iterator = this->treeModel()->standardItems().begin(); iterator != this->treeModel()->standardItems().end(); ++iterator)
	{
		ObjectTreeItem *item = iterator.value();
		this->objectData()->resetObject(item->originalObjectId(), item->customObjectId());
	}
}

void ObjectEditorTab::setSortByName(bool sort)
{
	this->m_sortByName = sort;
	//this->treeModel()->setSortByName(sort);
	//this->tableModel()->setSortByName(sort);
}

bool ObjectEditorTab::sortByName() const
{
	return this->m_sortByName;
}

void ObjectEditorTab::setShowRawData(bool show)
{
	this->m_showRawData = show;
	this->treeModel()->setShowRawData(show);
	this->tableModel()->setShowRawData(show);

	//QList<QTreeWidgetItem*> selection = treeWidget()->selectedItems();

	/*
	 * Reactivating the selected object.should update the field descriptions with raw data.
	 * TODO updating description items only is faster.
	 */
	/*
	if (!selection.isEmpty())
	{
		itemClicked(selection.first(), 0);
	}
	*/
}

void ObjectEditorTab::copyObject()
{
	this->m_clipboard = this->selection();

	if (this->hasObjectEditor())
	{
		this->objectEditor()->pasteObjectAction()->setEnabled(true);
	}
}

void ObjectEditorTab::pasteObject()
{
	for (std::size_t i = 0; i < this->m_clipboard.size(); ++i)
	{
		try
		{
			const map::CustomUnits::Unit &unit = this->m_clipboard[i];
			const map::CustomObjects::Object *object = boost::polymorphic_cast<const map::CustomObjects::Object*>(&unit);
			const QString originalObjectId = map::idToString(object->originalId()).c_str();
			const QString nextId = this->objectData()->nextCustomObjectId(originalObjectId);
			qDebug() << "Next ID" << nextId;
			this->idDialog()->setId(nextId);

			if (this->idDialog()->exec() == QDialog::Accepted)
			{
				const QString customObjectId = this->idDialog()->id();

				/*
				 * Custom ID is already in use, so ask the user if he wants to overwrite an existing object.
				 */
				if (this->objectData()->isObjectModified(originalObjectId, customObjectId))
				{
					if (QMessageBox::question(this, tr("Overwrite?"), tr("Do you want to overwrite the existing custom object %1?").arg(customObjectId)) == QMessageBox::No)
					{
						continue;
					}
				}

				if (!object->modifications().empty())
				{
					for (std::size_t j = 0; j < object->modifications().size(); ++j)
					{
						const map::CustomUnits::Modification &unitModification = object->modifications()[i];
						const map::CustomObjects::Modification *modification =  boost::polymorphic_cast<const map::CustomObjects::Modification*>(&unitModification);

						this->objectData()->modifyField(originalObjectId, customObjectId, map::idToString(modification->valueId()).c_str(), *modification);
					}
				}
				/*
				* If no modifications are done at least add the object's name that a new object is created at all.
				*/
				else
				{
					this->objectData()->createObject(originalObjectId, customObjectId);
				}
			}
		}
		catch (Exception &e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}
}

void ObjectEditorTab::widgetizeAllObjects()
{
	const QString &dir = QFileDialog::getExistingDirectory(this, tr("Widgetize All Objects"));

	if (!dir.isEmpty())
	{
		try
		{
			this->objectData()->widgetize(QUrl::fromLocalFile(dir));
		}
		catch (Exception &e)
		{
			QMessageBox::critical(this, tr("Error on widgetizing all objects"), e.what());
		}
	}
}

void ObjectEditorTab::filterObjects(const QString &filter)
{
	const QRegExp regExp = QRegExp(filter);

	if (regExp.isValid())
	{
		this->proxyModel()->setFilterRegExp(regExp);
	}
}

void ObjectEditorTab::filterFields(const QString &filter)
{
	const QRegExp regExp = QRegExp(filter);

	if (regExp.isValid())
	{
		this->tableProxyModel()->setFilterRegExp(regExp);
	}
}

QSortFilterProxyModel* ObjectEditorTab::proxyModel() const
{
	return boost::polymorphic_cast<QSortFilterProxyModel*>(this->m_treeView->model());
}

QSortFilterProxyModel* ObjectEditorTab::tableProxyModel() const
{
	return boost::polymorphic_cast<QSortFilterProxyModel*>(this->m_tableView->model());
}

void ObjectEditorTab::renameObject()
{
	QItemSelectionModel *selection = this->treeView()->selectionModel();

	foreach (QModelIndex index, selection->selectedIndexes())
	{
		ObjectTreeItem *item = this->treeModel()->item(proxyModel()->mapToSource(index));

		if (item != 0 && !item->isFolder())
		{
			// TODO get name field if available
			//this->modifyField(item->originalObjectId(), item->customObjectId(), this->objectData()->objectNameFieldId());
		}
	}
}

#include "moc_objecteditortab.cpp"

}

}
