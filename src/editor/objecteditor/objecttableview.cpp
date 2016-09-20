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
#include <QtWidgets>

#include "objecttableview.hpp"
#include "objectvaluedialog.hpp"
#include "objectlistdialog.hpp"
#include "objecttablemodel.hpp"
#include "objecteditortab.hpp"
#include "../objectdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTableView::ObjectTableView(ObjectEditorTab *parent) : QTableView(parent), m_tab(parent)
{
	m_contextMenu = new QMenu(this);

	if (this->tab()->hasObjectEditor())
	{
		m_contextMenu->addAction(tab()->objectEditor()->modifyFieldAction());
		m_contextMenu->addAction(tab()->objectEditor()->resetFieldAction());

		connect(tab()->objectEditor()->modifyFieldAction(), SIGNAL(triggered()), this, SLOT(modifyField()));
		connect(tab()->objectEditor()->resetFieldAction(), SIGNAL(triggered()), this, SLOT(resetField()));
	}

	connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(editItem(const QModelIndex&)));
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
	connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(orderBySection(int)));

	this->verticalHeader()->setVisible(false);
	this->horizontalHeader()->setVisible(true);
	/*
	 * The sections should be stretched to the text.
	 */
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->horizontalHeader()->setSortIndicatorShown(true);
	this->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
	this->setShowGrid(false);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	this->setEditTriggers(QTableWidget::NoEditTriggers);
	this->setSelectionBehavior(QTreeWidget::SelectRows);
}

void ObjectTableView::orderBySection(int logicalIndex)
{
	qDebug() << "Order by section" << logicalIndex;
	this->model()->sort(logicalIndex,  this->horizontalHeader()->sortIndicatorOrder());
}

void ObjectTableView::editItem(const QModelIndex &index)
{
	qDebug() << "Editing item with row" << index.row();
	const ObjectTableModel *model = this->tab()->tableModel();
	const QModelIndex mappedIndex = this->tab()->tableProxyModel()->mapToSource(index);
	const ObjectData::FieldId fieldId = model->fieldId(mappedIndex.row());
	const QString label = QString(tr("%1:")).arg(model->data(model->index(mappedIndex.row(), 0), Qt::DisplayRole).toString());
	const QString type = this->tab()->objectData()->metaData()->value(fieldId.fieldId(), "type");
	QString result;

	if (this->tab()->objectData()->fieldTypeIsList(type))
	{
		ObjectListDialog::getObjectIds(this->tableModel()->originalObjectId(), this->tableModel()->customObjectId(), fieldId.fieldId(), fieldId.level(), this->tab()->objectData(), this->tab()->source()->sharedData()->sharedObjectData().get(), label, this);
	}
	else if (ObjectValueDialog::getValue(result, this->tableModel()->originalObjectId(), this->tableModel()->customObjectId(), fieldId.fieldId(), fieldId.level(), this->tab()->objectData(), label, this->tab()) == QDialog::Accepted)
	{
		qDebug() << "New field value:" << this->tab()->objectData()->fieldValue(this->tableModel()->originalObjectId(), this->tableModel()->customObjectId(), fieldId.fieldId(), fieldId.level());
		qDebug() << "New readable field value:" << this->tab()->objectData()->fieldReadableValue(this->tableModel()->originalObjectId(), this->tableModel()->customObjectId(), fieldId.fieldId(), fieldId.level());
	}
}

void ObjectTableView::resetItem(const QModelIndex &index)
{
	const ObjectTableModel *model = this->tab()->tableModel();
	const QModelIndex mappedIndex = this->tab()->tableProxyModel()->mapToSource(index);

	const ObjectData::FieldId fieldId = model->fieldId(mappedIndex.row());
	qDebug() << "Resetting field " << fieldId.fieldId();
	this->tab()->objectData()->resetField(this->tableModel()->originalObjectId(), this->tableModel()->customObjectId(), fieldId.fieldId(), fieldId.level());
}

void ObjectTableView::customContextMenuRequested(QPoint pos)
{
	qDebug() << "custom context menu";
	m_contextMenu->popup(this->viewport()->mapToGlobal(pos));
}

void ObjectTableView::modifyField()
{
	QItemSelectionModel *select = this->selectionModel();

	foreach (QModelIndex index, select->selectedRows())
	{
		editItem(index);
	}
}

void ObjectTableView::resetField()
{
	QItemSelectionModel *select = this->selectionModel();

	foreach (QModelIndex index, select->selectedRows())
	{
		resetItem(index);
	}
}

void ObjectTableView::selectField(const QString &fieldId, int level)
{
	QItemSelectionModel *selection = new QItemSelectionModel(this->tableModel());
	ObjectData::FieldId fieldIdKey(fieldId, level);
	selection->select(this->tableModel()->index(this->tableModel()->row(fieldIdKey), 1), QItemSelectionModel::Rows);
	this->setSelectionModel(selection);
	// TODO inefficient deletion
	selection->deleteLater();
}

}

}
