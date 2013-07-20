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
#include "metadata.hpp"

namespace wc3lib
{

namespace editor
{

ObjectEditorTab::ObjectEditorTab(class MpqPriorityList *source, const KUrl &metaDataUrl, QWidget *parent, Qt::WindowFlags f) : m_source(source), m_metaData(new MetaData(metaDataUrl)), m_tabIndex(0), m_metaDataUrlRequester(0), m_treeWidget(0), m_tableWidget(0), QWidget(parent, f)
{
	try
	{
		m_metaData->setSource(source, true);
	}
	catch (Exception &exception)
	{
		KMessageBox::error(this, i18n("Error while loading meta data from \"%1\":\n\"%2\".", m_metaData->url().toEncoded().constData(), exception.what().c_str()));
	}
}

void ObjectEditorTab::setMetaDataUrl(const KUrl& url)
{
	delete this->metaData();
	this->m_metaData = new MetaData(url);

	try
	{
		m_metaData->setSource(this->source(), true);
	}
	catch (Exception &exception)
	{
		KMessageBox::error(this, i18n("Error while loading meta data from \"%1\":\n\"%2\".", m_metaData->url().toEncoded().constData(), exception.what().c_str()));
	}
}

void ObjectEditorTab::setupUi()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	qDebug() << "Show tab " << this->name();
	m_metaDataUrlRequester = new KUrlRequester(metaData()->url(), this);
	connect(m_metaDataUrlRequester, SIGNAL(urlSelected(KUrl)), this, SLOT(setMetaDataUrl(KUrl)));
	layout->addWidget(m_metaDataUrlRequester);

	QHBoxLayout *horizontalLayout = new QHBoxLayout();
	layout->addLayout(horizontalLayout);
	m_treeWidget = createTreeWidget();
	horizontalLayout->addWidget(m_treeWidget);
	m_tableWidget = createTableWidget();
	horizontalLayout->addWidget(m_tableWidget);
}

#include "moc_objecteditortab.cpp"

}

}
