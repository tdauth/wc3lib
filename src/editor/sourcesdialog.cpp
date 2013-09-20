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

#include <QtCore>
#include <QtGui>

#include <KLineEdit>
#include <KUrlCompletion>
#include <KUrlRequester>
#include <KMessageBox>

#include "sourcesdialog.hpp"
#include "mpqprioritylist.hpp"
#include "editor.hpp"
#include "module.hpp"

namespace wc3lib
{

namespace editor
{

void SourcesDialog::update()
{
	foreach (MpqPriorityList::Source source, const_cast<const MpqPriorityList*>(source())->sources())
		m_editListBox->insertItem(source->url().toEncoded());
}

SourcesDialog::SourcesDialog(class MpqPriorityList *source, QWidget *parent, Qt::WFlags flags): m_source(source), QDialog(parent, flags)
{
	setupUi(this);

	//m_editListBox->
	// TODO use URL requester
	//KUrlRequester *requester = new KUrlRequester(this);
	//KUrlCompletion *urlCompletion = new KUrlCompletion(KUrlCompletion::DirCompletion);
	//m_editListBox->lineEdit()->setCompletionObject(urlCompletion);
	//m_editListBox->setCustomEditor(*requester);
	KUrlRequester *urlRequester = new KUrlRequester(m_editListBox);
	urlRequester->setMode(KFile::ExistingOnly | KFile::Files | KFile::Directory);

	KMimeType::Ptr mpq(KMimeType::mimeType("application/x-mpq"));
	KMimeType::Ptr w3m(KMimeType::mimeType("application/x-w3m"));
	KMimeType::Ptr w3x(KMimeType::mimeType("application/x-w3x"));
	KMimeType::Ptr w3n(KMimeType::mimeType("application/x-w3n"));
	QString filter;

	if (mpq.isNull() || w3m.isNull() || w3x.isNull() || w3n.isNull())
		filter = i18n("*|All Files\n*.mpq;*.w3m;*.w3x;*.w3n|Blizzard archives");
	else
		filter = i18n("all/allfiles application/x-mpq application/x-w3m application/x-w3x application/x-w3n");

	urlRequester->setFilter(filter);
	KEditListWidget::CustomEditor customEditor(urlRequester, urlRequester->lineEdit());
	m_editListBox->setCustomEditor(customEditor);

	connect(m_dialogButtonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(ok()));
	connect(m_dialogButtonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
	connect(m_dialogButtonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));
	connect(m_editListBox, SIGNAL(added(QString)), this, SLOT(added(QString)));
}

void SourcesDialog::added(const QString& text)
{
	// fix mpq entries automatically
	if (text.endsWith(".mpq") && !text.startsWith("mpq:/")) {
		QStringList items = m_editListBox->items();
		items.removeFirst();
		items.push_front("mpq:/" + text);
		m_editListBox->setItems(items);
	}
}

void SourcesDialog::ok()
{
	apply();
	hide();
}

void SourcesDialog::apply()
{
	foreach (const QString &item, m_editListBox->items())
	{
		if (const_cast<const MpqPriorityList*>(source())->sources().get<KUrl>().find(KUrl(item)) == const_cast<const MpqPriorityList*>(source())->sources().get<KUrl>().end())
		{
			if (!source()->addSource(item))
				KMessageBox::error(this, i18n("Invalid source \"%1\".", item));
		}
	}
	QLinkedList<KUrl> invalidUrls;

	foreach (MpqPriorityList::Source source, const_cast<const MpqPriorityList*>(source())->sources())
	{
		if (!m_editListBox->items().contains(source->url().toEncoded()))
			invalidUrls.push_back(source->url());
	}

	foreach (const KUrl &url, invalidUrls)
		source()->removeSource(url);

	this->source()->writeSettings(settingsGroup());
}

void SourcesDialog::restoreDefaults()
{
	m_editListBox->clear();

	if (!war3Url().isEmpty())
		m_editListBox->insertItem(war3Url().toEncoded());

	if (!war3XUrl().isEmpty())
		m_editListBox->insertItem(war3XUrl().toEncoded());

	if (!war3XLocalUrl().isEmpty())
		m_editListBox->insertItem(war3XLocalUrl().toEncoded());

	if (!war3PatchUrl().isEmpty())
		m_editListBox->insertItem(war3PatchUrl().toEncoded());
}

void SourcesDialog::showEvent(QShowEvent *e)
{
	update();
	QDialog::showEvent(e);
}

QString SourcesDialog::settingsGroup() const
{
	if (dynamic_cast<Editor*>(parentWidget()) != 0)
		return static_cast<Editor*>(parentWidget())->aboutData().appName();
	else if (dynamic_cast<Module*>(parentWidget()) != 0)
	{
		Module *module = static_cast<Module*>(parentWidget());

		if (module->hasEditor())
			return module->editor()->aboutData().appName();
		else
			return module->componentData().aboutData()->appName();
	}
	else
		throw Exception(_("Sources can only have modules or editor as parent."));

	return "";
}

}

}
