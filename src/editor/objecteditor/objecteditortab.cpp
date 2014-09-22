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
, m_filterLineEdit(0)
, m_treeView(0)
, m_treeModel(0)
, m_tableView(0)
, m_tableModel(0)
, m_objectData(objectData)
, m_showRawData(false)
{
	this->objectData()->load(this);
}

ObjectEditorTab::~ObjectEditorTab()
{
	delete this->m_objectData;
}

void ObjectEditorTab::setupUi()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QWidget *layoutWidget = new QWidget(this);
	layoutWidget->setLayout(layout);

	qDebug() << "Show tab " << this->name();

	m_filterLineEdit = new KLineEdit(this);
	m_filterLineEdit->setClearButtonShown(true);

	m_treeView = new ObjectTreeView(this);
	m_treeModel = createTreeModel();
	treeView()->setModel(m_treeModel);
	this->treeModel()->load(this->source(), this->objectData(), this);

	// TEST
	connect(m_treeModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));

	m_tableView = new ObjectTableView(this);
	m_tableModel = new ObjectTableModel(this);
	m_tableView->setModel(m_tableModel);

	QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
	layout->addWidget(m_filterLineEdit);
	layout->addWidget(m_treeView);
	splitter->addWidget(layoutWidget);
	splitter->addWidget(m_tableView);
	this->setLayout(new QHBoxLayout(this));
	this->layout()->addWidget(splitter);

	connect(m_filterLineEdit, SIGNAL(textChanged(QString)), this, SLOT(filterTreeWidget(const QString&)));
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
		ObjectTreeItem *item = this->treeModel()->item(index);
		table.push_back(new map::CustomObjects::Object(objectData()->customObject(item->originalObjectId(), item->customObjectId())));
	}

	return table;
}

void ObjectEditorTab::importAllObjects()
{
	onImportAllObjects();
}

void ObjectEditorTab::exportAllObjects()
{
	onExportAllObjects();
}

void ObjectEditorTab::itemClicked(QModelIndex index)
{
	ObjectTreeItem *item = (ObjectTreeItem*)(index.internalPointer());
	qDebug() << "Item:" << item;
	const QString originalObjectId = item->originalObjectId();
	/*
	 * Is folder item.
	 * Hide everything.
	 */
	if (item->isFolder())
	{
		this->tableView()->hideColumn(0);
		this->tableView()->hideColumn(1);
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

		tableModel()->load(objectData(), originalObjectId, customObjectId);

		this->tableView()->horizontalHeader()->setVisible(true);
		this->tableView()->showColumn(0);
		this->tableView()->showColumn(1);

		this->activateObject(item);
	}
}

void ObjectEditorTab::filterTreeWidget(const QString &text)
{
	if (text.isEmpty())
	{
		this->treeView()->setModel(this->m_treeModel);
	}
	else
	{
		QRegExp regExp = QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString);

		if (regExp.isValid())
		{
			// TODO by default if the parent doesn't match none of the children is shown, change behaviour!!!
			QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
			proxyModel->setSourceModel(this->m_treeModel);
			proxyModel->setFilterRegExp(regExp);
			qDebug() << "Filter fixed string" << text;

			this->treeView()->setModel(proxyModel);
		}
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
			ObjectTreeItem *item = this->treeModel()->item(index);

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
		ObjectTreeItem *item = this->treeModel()->item(index);

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

inline void ObjectEditorTab::pasteObject()
{
	for (std::size_t i = 0; i < this->m_clipboard.size(); ++i)
	{
		const map::CustomUnits::Unit &unit = this->m_clipboard[i];
		const map::CustomObjects::Object *object = boost::polymorphic_cast<const map::CustomObjects::Object*>(&unit);
		const QString customObjectId = this->objectData()->nextCustomObjectId();

		for (std::size_t j = 0; j < object->modifications().size(); ++j)
		{
			const map::CustomUnits::Modification &unitModification = object->modifications()[i];
			const map::CustomObjects::Modification *modification =  boost::polymorphic_cast<const map::CustomObjects::Modification*>(&unitModification);

			this->objectData()->modifyField(map::idToString(object->originalId()).c_str(), customObjectId, map::idToString(modification->valueId()).c_str(), *modification);
		}
	}

	onPasteObject();
}

#include "moc_objecteditortab.cpp"

}

}
