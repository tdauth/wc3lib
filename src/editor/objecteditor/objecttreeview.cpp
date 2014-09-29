/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#include "objecttreeview.hpp"
#include "objecteditortab.hpp"
#include "objecttreemodel.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTreeView::ObjectTreeView(ObjectEditorTab *tab, Qt::WindowFlags f) : QTreeView(tab), m_tab(tab), m_contextMenu(new QMenu(this))
{
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	this->setUniformRowHeights(true); // improves performance
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->setHeaderHidden(true);

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
	m_contextMenu->addAction(tab->objectEditor()->copyObjectAction());
	m_contextMenu->addAction(tab->objectEditor()->pasteObjectAction());
	// TODO add action "select in palette"
	m_contextMenu->addSeparator();
	m_contextMenu->addAction(tab->objectEditor()->newObjectAction());
	m_contextMenu->addAction(tab->objectEditor()->renameObjectAction());
	m_contextMenu->addAction(tab->objectEditor()->deleteObjectAction());
	m_contextMenu->addSeparator();
	m_contextMenu->addAction(tab->objectEditor()->resetObjectAction());

	//connect(this, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(updateActions(const QItemSelection &, const QItemSelection &)));
	connect(this, SIGNAL(expanded(const QModelIndex &)), this, SLOT(expandItem(const QModelIndex &)));
	connect(this, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(collapseItem(const QModelIndex &)));

}

void ObjectTreeView::customContextMenuRequested(QPoint pos)
{
	qDebug() << "custom context menu";

	m_contextMenu->popup(this->viewport()->mapToGlobal(pos));
}

void ObjectTreeView::updateActions(const QItemSelection &selected, const QItemSelection &deselected)
{
	const bool isFolder = onlyFoldersSelected();
	const bool isCustomObject = isCustomObjectSelected();

	this->tab()->objectEditor()->copyObjectAction()->setEnabled(!isFolder);
	this->tab()->objectEditor()->resetObjectAction()->setEnabled(isCustomObject);
	this->tab()->objectEditor()->deleteObjectAction()->setEnabled(isCustomObject);
	this->tab()->objectEditor()->renameObjectAction()->setEnabled(isCustomObject);
}

void ObjectTreeView::expandItem(const QModelIndex &index)
{
	ObjectTreeItem *item = this->tab()->treeModel()->item(this->tab()->proxyModel()->mapToSource(index));
	item->setExpanded(this->tab()->source(), this->tab());
}

void ObjectTreeView::collapseItem(const QModelIndex &index)
{
	ObjectTreeItem *item = this->tab()->treeModel()->item(this->tab()->proxyModel()->mapToSource(index));
	item->setCollapsed(this->tab()->source(), this->tab());
}

}

}
