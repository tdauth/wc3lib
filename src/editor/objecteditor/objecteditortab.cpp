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

#include <QtGui>

#include "objecteditortab.hpp"
#include "objecttreewidget.hpp"
#include "objecttablewidget.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectEditorTab::ObjectEditorTab(class MpqPriorityList *source, MetaData *metaData, QWidget *parent, Qt::WindowFlags f) : m_source(source), m_metaData(metaData), m_tabIndex(0), m_treeWidget(0), m_tableWidget(0), m_showRawData(false), QWidget(parent, f)
{
	this->metaData()->setSource(this->source());

	try
	{
		this->metaData()->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(this, i18n("Error on loading file \"%1\": %2", this->metaData()->url().toEncoded().constData(), e.what()));
	}
}

void ObjectEditorTab::setupUi()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	qDebug() << "Show tab " << this->name();

	QHBoxLayout *horizontalLayout = new QHBoxLayout();
	layout->addLayout(horizontalLayout);

	m_treeWidget = createTreeWidget();
	m_tableWidget = createTableWidget();

	QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
	splitter->addWidget(m_treeWidget);
	splitter->addWidget(m_tableWidget);
	horizontalLayout->addWidget(splitter);

	connect(m_treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*,int)));
}

void ObjectEditorTab::onUpdateCollection(const map::CustomObjects& objects)
{
}

void ObjectEditorTab::itemClicked(QTreeWidgetItem* item, int column)
{
	this->activateObject(item, column, item->data(0, Qt::UserRole).toString());
}

#include "moc_objecteditortab.cpp"

}

}
