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

#include "objecttablewidget.hpp"
#include "objecttablewidgetpair.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTableWidget::ObjectTableWidget(ObjectEditorTab *parent) : QTableWidget(parent->metaData()->metaData()->slk().rows() - 1, 2, parent), m_tab(parent)
{
	const QString name = parent->source()->sharedData()->tr("WESTRING_NAME");
	QTableWidgetItem *nameItem = new QTableWidgetItem(name);
	this->setHorizontalHeaderItem(0, nameItem);

	const QString value = parent->source()->sharedData()->tr("WESTRING_VALUE");
	QTableWidgetItem *valueItem = new QTableWidgetItem(value);
	this->setHorizontalHeaderItem(1, valueItem);

	this->verticalHeader()->setVisible(false);

	for (map::Slk::Table::size_type i = 0; i < tab()->metaData()->metaData()->slk().rows() - 1; ++i)
	{
		try
		{
			qDebug() << "Original value: " << tab()->metaData()->metaData()->value(i + 1, "\"ID\"");
			const QString fieldId = MetaData::fromSlkString(tab()->metaData()->metaData()->value(i + 1, "\"ID\""));
			ObjectTableWidgetPair *pair = new ObjectTableWidgetPair(this, parent, i, "", "", fieldId);
			m_pairs.insert(fieldId, pair);
			qDebug() << "pair " << i << ": " << fieldId;
		}
		catch (Exception &e)
		{
			qDebug() << "Exception:" << e.what();
		}
	}

	/*
	 * Hide columns initially and wait for activation of an object.
	 */
	this->hideColumn(0);
	this->hideColumn(1);

	m_contextMenu = new QMenu(this);
	const QString modifyField = parent->source()->sharedData()->tr("WESTRING_FIELDLIST_CM_MODIFY");
	const QString resetField = parent->source()->sharedData()->tr("WESTRING_FIELDLIST_CM_RESET");
	m_modifyField = new QAction(modifyField, this);
	m_resetField = new QAction(resetField, this);
	m_contextMenu->addAction(m_modifyField);
	m_contextMenu->addAction(m_resetField);

	connect(m_modifyField, SIGNAL(triggered()), this, SLOT(modifyField()));
	connect(m_resetField, SIGNAL(triggered()), this, SLOT(resetField()));

	connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(editItem(QTableWidgetItem*)));
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));

	this->horizontalHeader()->setStretchLastSection(true);
	this->setShowGrid(false);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	this->setEditTriggers(QTableWidget::NoEditTriggers);
	this->setSelectionBehavior(QTreeWidget::SelectRows);
}

void ObjectTableWidget::editItem(QTableWidgetItem *item)
{
	const QString fieldId = item->data(Qt::UserRole).toString();
	Pairs::iterator iterator = this->pairs().find(fieldId);

	if (iterator != this->pairs().end())
	{
		iterator.value()->edit();
	}
}

void ObjectTableWidget::customContextMenuRequested(QPoint pos)
{
	qDebug() << "custom context menu";
	m_contextMenu->popup(this->viewport()->mapToGlobal(pos));
}

void ObjectTableWidget::modifyField()
{
	QItemSelectionModel *select = this->selectionModel();

	foreach (QModelIndex index, select->selectedRows())
	{
		editItem(this->item(index.row(), 0));
	}
}

void ObjectTableWidget::resetField()
{
	QItemSelectionModel *select = this->selectionModel();

	foreach (QModelIndex index, select->selectedRows())
	{
		const QString fieldId = this->item(index.row(), 0)->data(Qt::UserRole).toString();
		Pairs::iterator iterator = this->pairs().find(fieldId);

		if (iterator != this->pairs().end())
		{
			iterator.value()->reset();
		}
	}
}

}

}
