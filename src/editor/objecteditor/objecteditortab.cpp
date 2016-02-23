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

#include <KMessageBox>
#include <KFileDialog>
#include <KAction>

#include <QtGui>

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

ObjectEditorTab::ObjectEditorTab(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor, QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f)
, m_source(source)
, m_tabIndex(0)
, m_filterSearchLine(0)
, m_tableFilterSearchLine(0)
, m_treeView(0)
, m_treeModel(0)
, m_tableView(0)
, m_tableModel(0)
, m_objectData(objectData)
, m_objectEditor(objectEditor)
, m_showRawData(false)
, m_idDialog(new ObjectIdDialog(this))
{
}

ObjectEditorTab::~ObjectEditorTab()
{
}

void ObjectEditorTab::setupUi()
{
	qDebug() << "Show tab " << this->name();

	/*
	 * Tree View
	 */
	m_filterSearchLine = new KFilterProxySearchLine(this);

	m_treeView = new ObjectTreeView(this);
	m_treeModel = createTreeModel();
	ObjectTreeSortFilterProxyModel *proxyModel = new ObjectTreeSortFilterProxyModel(this);
	proxyModel->setSourceModel(this->m_treeModel);
	treeView()->setModel(proxyModel);

	m_filterSearchLine->setProxy(this->proxyModel());

	/*
	 * Table View
	 */
	m_tableFilterSearchLine = new KFilterProxySearchLine(this);

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

	m_tableFilterSearchLine->setProxy(this->tableProxyModel());


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
	const QString customObjectsCollectionSuffix = "w3o";
	const QString mapSuffix = "w3m";
	const QString xmapSuffix = "w3x";

	const KUrl url = KFileDialog::getSaveUrl(KUrl(), QString("*\nCustom Units (*.%1)\nCustom Objects Collection (*.%2)\nMap (*.%3 *.%4)").arg(customUnitsSuffix).arg(customObjectsCollectionSuffix).arg(mapSuffix).arg(xmapSuffix), this, exportAllObjectsText());

	if (!url.isEmpty())
	{
		const QFileInfo fileInfo(url.toLocalFile());
		const QString suffix = fileInfo.suffix();

		// TODO support directory for meta data file list

		if (suffix == customUnitsSuffix && !this->objectData()->hasCustomUnits() && !this->objectData()->hasCustomObjects())
		{
			KMessageBox::error(this, tr("No custom units support."));

			return;
		}

		ofstream out(url.toLocalFile().toUtf8().constData());

		if (out)
		{
			try
			{
				if (suffix == customUnitsSuffix)
				{
					if (this->objectData()->hasCustomObjects())
					{
						qDebug() << "Exporting custom objects";
						this->objectData()->customObjects().write(out);
					}
					else if (this->objectData()->hasCustomUnits())
					{
						qDebug() << "Exporting custom units";

						this->objectData()->customUnits().write(out);
					}
				}
			}
			catch (Exception &e)
			{
				KMessageBox::error(this, tr("Error on exporting"), e.what());
			}
		}
	}
}

void ObjectEditorTab::importAllObjects()
{
	if (QMessageBox::question(this, tr("Import All"), tr("Importing all objects replaces all of your current modifications. Continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
	{
		const QString customObjectsSuffix = "w3u";
		const QString customObjectsCollectionSuffix = "w3o";
		const QString mapSuffix = "w3m";
		const QString xmapSuffix = "w3x";

		const KUrl url = KFileDialog::getOpenUrl(m_recentImportUrl, QString("*|All Files\n*.%1\nCustom Objects Collection *.%2\nMap (*.%3 *.%4)").arg(customObjectsSuffix).arg(customObjectsCollectionSuffix).arg(mapSuffix).arg(xmapSuffix), this, importAllObjectsText());

		if (!url.isEmpty())
		{
			const QString suffix = QFileInfo(url.toLocalFile()).suffix();
			qDebug() << "Suffix" << suffix;

			map::CustomUnits customUnits;

			if (suffix == customObjectsSuffix)
			{
				if (this->objectData()->hasCustomUnits())
				{
					ifstream in(url.toLocalFile().toUtf8().constData());

					try
					{
						customUnits.read(in);
						this->objectData()->importCustomUnits(customUnits);
						m_recentImportUrl = url;
					}
					catch (std::exception &e)
					{
						KMessageBox::error(this, tr("Error on importing"), e.what());
					}
				}
				else
				{
					KMessageBox::error(this, tr("Does not support custom units."));
				}
			}
			// TODO support custom object FILES
			else if (suffix == customObjectsCollectionSuffix)
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
									this->objectData()->importCustomUnits(*customObjectsCollection->units());
									m_recentImportUrl = url;
								}
								else
								{
									KMessageBox::error(this, tr("Collection has no units."));
								}
							}
						}
					}
					catch (std::exception &e)
					{
						KMessageBox::error(this, e.what());
					}
				}
				else
				{
					KMessageBox::error(this, tr("Does not support custom objects."));
				}
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

						m_recentImportUrl = url;
					}
					catch (std::exception &e)
					{
						KMessageBox::error(this, e.what());
					}
				}
				else
				{
					KMessageBox::error(this, tr("Does not support custom units."));
				}
			}
			// TODO support custom object FILES
			else if (suffix == xmapSuffix)
			{
				KMessageBox::error(this, tr("W3X is not supported yet."));
			}
			else
			{
				KMessageBox::error(this, tr("Unknown file type."));
			}
		}
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
					if (KMessageBox::questionYesNo(this, tr("Do you want to overwrite the existing custom object %1?").arg(customObjectId)) == KMessageBox::No)
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
			KMessageBox::error(this, e.what());
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
			this->objectData()->widgetize(KUrl::fromLocalFile(dir));
		}
		catch (Exception &e)
		{
			QMessageBox::critical(this, tr("Error on widgetizing all objects"), e.what());
		}
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
