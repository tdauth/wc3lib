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

#include <QMessageBox>

#include "objecttableview.hpp"
#include "objectvaluedialog.hpp"
#include "objecttablemodel.hpp"
#include "objecteditortab.hpp"
#include "objectdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTableView::ObjectTableView(ObjectEditorTab *parent) : QTableView(parent), m_tab(parent)
{
	m_contextMenu = new QMenu(this);
	m_contextMenu->addAction(tab()->objectEditor()->modifyFieldAction());
	m_contextMenu->addAction(tab()->objectEditor()->resetFieldAction());

	connect(tab()->objectEditor()->modifyFieldAction(), SIGNAL(triggered()), this, SLOT(modifyField()));
	connect(tab()->objectEditor()->resetFieldAction(), SIGNAL(triggered()), this, SLOT(resetField()));

	connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(editItem(const QModelIndex&)));
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));

	this->verticalHeader()->setVisible(false);
	this->horizontalHeader()->setVisible(true);
	this->horizontalHeader()->setStretchLastSection(true);
	this->horizontalHeader()->setSortIndicatorShown(true);
	this->setShowGrid(false);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	this->setEditTriggers(QTableWidget::NoEditTriggers);
	this->setSelectionBehavior(QTreeWidget::SelectRows);
}

void ObjectTableView::editItem(const QModelIndex &index)
{
	const QString fieldId = model()->data(index, Qt::UserRole).toString();
	const QString label = QString(tr("%1:")).arg(model()->data(model()->index(index.row(), 0), Qt::DisplayRole).toString());
	QString result;

	if (ObjectValueDialog::show(result, this->tableModel()->originalObjectId(), this->tableModel()->customObjectId(), fieldId, this->tab()->objectData(), label, this->tab()) == QDialog::Accepted)
	{
	}
}

void ObjectTableView::resetItem(const QModelIndex& index)
{
	const QString fieldId = model()->data(index, Qt::UserRole).toString();
	this->tab()->objectData()->resetField(this->tableModel()->originalObjectId(), this->tableModel()->customObjectId(), fieldId);
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

}

}
