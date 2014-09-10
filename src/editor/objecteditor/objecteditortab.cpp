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
#include "objecttablewidget.hpp"
#include "objectdata.hpp"
#include "objecttablewidgetpair.hpp"
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
, m_tableWidget(0)
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
	this->treeModel()->load(this->source(), this->objectData(), this);
	treeView()->setModel(m_treeModel);

	m_tableWidget = createTableWidget();

	QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
	layout->addWidget(m_filterLineEdit);
	layout->addWidget(m_treeView);
	splitter->addWidget(layoutWidget);
	splitter->addWidget(m_tableWidget);
	this->setLayout(new QHBoxLayout(this));
	this->layout()->addWidget(splitter);

	connect(m_filterLineEdit, SIGNAL(textChanged(QString)), this, SLOT(filterTreeWidget(const QString&)));
	connect(m_treeView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
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
	const QString originalObjectId = item->originalObjectId();
	/*
	 * Is folder item.
	 * Hide everything.
	 */
	if (item->isFolder())
	{
		this->tableWidget()->hideColumn(0);
		this->tableWidget()->hideColumn(1);
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

		this->tableWidget()->showColumn(0);
		this->tableWidget()->showColumn(1);

		for (ObjectTableWidget::Pairs::iterator iterator = this->tableWidget()->pairs().begin(); iterator != this->tableWidget()->pairs().end(); ++iterator)
		{
			fillTableRow(originalObjectId, customObjectId, iterator.key(), iterator.value());
		}

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
		QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
		proxyModel->setSourceModel(this->m_treeModel);
		proxyModel->setFilterRegExp(text);

		this->treeView()->setModel(proxyModel);
	}

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

#include "moc_objecteditortab.cpp"

}

}
