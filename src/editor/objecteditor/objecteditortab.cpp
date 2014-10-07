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
#include "objectdata.hpp"
#include "objecttableview.hpp"
#include "objecttablemodel.hpp"
#include "objecttreeview.hpp"
#include "objecttreemodel.hpp"
#include "iddialog.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectEditorTab::ObjectEditorTab(MpqPriorityList *source, ObjectData *objectData, QWidget *parent, Qt::WindowFlags f)
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
, m_showRawData(false)
, m_idDialog(new IdDialog(this))
{
	try
	{
		this->objectData()->load(this);
	}
	catch (const Exception &e)
	{
		KMessageBox::error(this, e.what());
	}
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
	QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(this->m_treeModel);
	treeView()->setModel(proxyModel);
	this->treeModel()->load(this->source(), this->objectData(), this);

	m_filterSearchLine->setProxy(this->proxyModel());

	// TEST
	connect(m_treeModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));

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

	QVBoxLayout *rightLayout = new QVBoxLayout(this);
	QWidget *rightLayoutWidget = new QWidget(this);
	rightLayoutWidget->setLayout(rightLayout);
	rightLayout->addWidget(m_tableFilterSearchLine);
	rightLayout->addWidget(m_tableView);

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

bool ObjectEditorTab::selectObject(const QString& originalObjectId, const QString& customObjectId)
{
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

		tableModel()->load(objectData(), originalObjectId, customObjectId);

		this->tableView()->horizontalHeader()->setVisible(true);
		this->tableView()->showColumn(0);
		this->tableView()->showColumn(1);

		this->tableView()->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
		this->tableView()->model()->sort(0, Qt::AscendingOrder);

		this->objectEditor()->copyObjectAction()->setEnabled(true);
		this->objectEditor()->renameObjectAction()->setEnabled(true);
		this->objectEditor()->deleteObjectAction()->setEnabled(true);
		this->objectEditor()->resetObjectAction()->setEnabled(true);

		this->activateObject(item);

		return true;
	}

	return false;
}

bool ObjectEditorTab::modifyField(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId)
{
	if (this->selectObject(originalObjectId, customObjectId))
	{
		this->tableView()->selectField(fieldId);

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
	const QString slkSuffix = "slk";

	const KUrl url = KFileDialog::getSaveUrl(KUrl(), QString("*\n*.%1\nCustom Objects Collection *.%2\nMap (*.%3 *.%4)\nSlk (*.%5)").arg(customUnitsSuffix).arg(customObjectsCollectionSuffix).arg(mapSuffix).arg(xmapSuffix).arg(slkSuffix), this, exportAllObjectsText());

	if (!url.isEmpty())
	{
		const QFileInfo fileInfo(url.toLocalFile());
		const QString suffix = fileInfo.suffix();

		if (suffix == slkSuffix && !this->objectData()->hasSlks())
		{
			KMessageBox::error(this, tr("No SLK support."));

			return;
		}

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
				else if (suffix == slkSuffix)
				{
					if (this->objectData()->hasSlks())
					{
						qDebug() << "Export SLKs";

						const ObjectData::Slks slks = this->objectData()->slks();
						int i = 0;

						BOOST_FOREACH(ObjectData::Slks::const_reference ref, slks)
						{
							const QDir dir = fileInfo.dir();
							const QString file = dir.absoluteFilePath(tr("out%1.slk").arg(i));
							ofstream out(file.toUtf8().constData());
							ref.write(out);

							++i;
						}
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
	const QString customObjectsSuffix = "w3u";
	const QString customObjectsCollectionSuffix = "w3o";
	const QString mapSuffix = "w3m";
	const QString xmapSuffix = "w3x";

	const KUrl url = KFileDialog::getOpenUrl(KUrl(), QString("*\n*.%1\nCustom Objects Collection *.%2\nMap (*.%3 *.%4)").arg(customObjectsSuffix).arg(customObjectsCollectionSuffix).arg(mapSuffix).arg(xmapSuffix), this, importAllObjectsText());

	if (!url.isEmpty())
	{
		const QString suffix = QFileInfo(url.toLocalFile()).suffix();
		qDebug() << "Suffix" << suffix;

		map::CustomUnits customUnits;

		if (suffix == customObjectsSuffix)
		{
			ifstream in(url.toLocalFile().toUtf8().constData());

			try
			{
				customUnits.read(in);
				this->objectData()->importCustomUnits(customUnits);
				// TODO refresh tree widget by using object data not custom units
				treeModel()->load(this->source(), this->objectData(), this);
			}
			catch (std::exception &e)
			{
				KMessageBox::error(this, tr("Error on importing"), e.what());
			}
		}
		else if (suffix == customObjectsCollectionSuffix)
		{
			ifstream in(url.toLocalFile().toUtf8().constData());

			try
			{
				std::unique_ptr<map::CustomObjectsCollection> customObjectsCollection(new map::CustomObjectsCollection());
				customObjectsCollection->read(in);

				if (customObjectsCollection->hasUnits())
				{
					this->objectData()->importCustomUnits(*customObjectsCollection->units());
					// TODO refresh tree widget by using object data not custom units
					treeModel()->load(this->source(), this->objectData(), this);
				}
				else
				{
					KMessageBox::error(this, tr("Collection has no units."));
				}
			}
			catch (std::exception &e)
			{
				KMessageBox::error(this, e.what());
			}
		}
		else if (suffix == mapSuffix)
		{
			try
			{
				std::unique_ptr<map::W3m> map(new map::W3m());
				map->open(url.toLocalFile().toUtf8().constData());
				std::streamsize size = map->readFileFormat(map->customUnits().get());
				qDebug() << "Size of custom units:" << size;
				this->objectData()->importCustomUnits(*map->customUnits());
				// TODO refresh tree widget by using object data not custom units
				treeModel()->load(this->source(), this->objectData(), this);
			}
			catch (std::exception &e)
			{
				KMessageBox::error(this, e.what());
			}
		}
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
		this->objectEditor()->copyObjectAction()->setEnabled(false);
		this->objectEditor()->renameObjectAction()->setEnabled(false);
		this->objectEditor()->deleteObjectAction()->setEnabled(false);
		this->objectEditor()->resetObjectAction()->setEnabled(false);
		this->activateFolder(item);
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

void ObjectEditorTab::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
	qDebug() << "Data changed";
}

void ObjectEditorTab::deleteObject()
{
	const QItemSelectionModel *selection = this->treeView()->selectionModel();

	if (!selection->selectedIndexes().isEmpty())
	{
		/*
		 * Make sure one cannot edit any values of deleted objects.
		 */
		this->tableView()->hideColumn(0);
		this->tableView()->hideColumn(1);

		foreach (QModelIndex index, selection->selectedIndexes())
		{
			ObjectTreeItem *item = this->treeModel()->item(this->proxyModel()->mapToSource(index));

			/*
			 * Only custom untis can be deleted.
			 */
			if (!item->isFolder() && !item->customObjectId().isEmpty())
			{
				this->objectData()->deleteObject(item->originalObjectId(), item->customObjectId());
			}
		}
	}

	onDeleteObject();
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

	onResetObject();
}

void ObjectEditorTab::resetAllObjects()
{
	for (ObjectTreeModel::Items::iterator iterator = this->treeModel()->standardItems().begin(); iterator != this->treeModel()->standardItems().end(); ++iterator)
	{
		ObjectTreeItem *item = iterator.value();
		this->objectData()->resetObject(item->originalObjectId(), item->customObjectId());
	}

	onResetAllObjects();
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

	onShowRawData(show);
}

void ObjectEditorTab::copyObject()
{
	this->m_clipboard = this->selection();
	this->objectEditor()->pasteObjectAction()->setEnabled(true);

	onCopyObject();
}

void ObjectEditorTab::pasteObject()
{
	for (std::size_t i = 0; i < this->m_clipboard.size(); ++i)
	{
		try
		{
			const QString nextId = this->objectData()->nextCustomObjectId();
			qDebug() << "Next ID" << nextId;
			this->idDialog()->setId(nextId);

			if (this->idDialog()->exec() == QDialog::Accepted)
			{
				const QString customObjectId = this->idDialog()->id();
				const map::CustomUnits::Unit &unit = this->m_clipboard[i];
				const map::CustomObjects::Object *object = boost::polymorphic_cast<const map::CustomObjects::Object*>(&unit);
				const QString originalObjectId = map::idToString(object->originalId()).c_str();

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
					this->objectData()->modifyField(originalObjectId, customObjectId, this->objectData()->objectNameFieldId(), this->objectData()->fieldValue(originalObjectId, customObjectId, this->objectData()->objectNameFieldId()));
				}
			}
		}
		catch (Exception &e)
		{
			KMessageBox::error(this, e.what());
		}
	}

	onPasteObject();
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
			this->modifyField(item->originalObjectId(), item->customObjectId(), this->objectData()->objectNameFieldId());
		}
	}

	onRenameObject();
}

#include "moc_objecteditortab.cpp"

}

}
