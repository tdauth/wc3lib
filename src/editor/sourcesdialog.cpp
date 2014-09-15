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
#include <KFileDialog>

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
	m_editListBox->clear();

	qDebug() << "Sources: " << const_cast<const MpqPriorityList*>(source())->sources().size();

	BOOST_REVERSE_FOREACH(MpqPriorityList::Sources::const_reference source, const_cast<const MpqPriorityList*>(this->source())->sources())
	{
		m_editListBox->insertItem(source.url().toEncoded());
	}
}

void SourcesDialog::addWc3Dir()
{
	const KUrl url = KFileDialog::getExistingDirectoryUrl(KUrl(), this, tr("Select Warcraft III Directory"));

	if (!url.isEmpty())
	{
		QMap<int, KUrl> urls;
		const QFileInfo fileInfo(url.toLocalFile());

		if (fileInfo.isDir())
		{
			qDebug() << "is dir" << fileInfo.filePath();
			const QDir dir = QDir(fileInfo.filePath());

			foreach (QFileInfo info, dir.entryInfoList())
			{
				if (info.isFile())
				{
					qDebug() << "filename" << info.fileName().toLower();

					if (info.fileName().toLower() == "war3patch.mpq")
					{
						urls.insert(3, KUrl(info.absoluteFilePath()));
					}
					else if (info.fileName().toLower() == "war3xlocal.mpq")
					{
						urls.insert(2, KUrl(info.absoluteFilePath()));
					}
					else if (info.fileName().toLower() == "war3x.mpq")
					{
						urls.insert(1, KUrl(info.absoluteFilePath()));
					}
					else if (info.fileName().toLower() == "war3.mpq")
					{
						urls.insert(0, KUrl(info.absoluteFilePath()));
					}
				}
			}
		}

		if (urls.isEmpty())
		{
			KMessageBox::error(this, tr("Invalid Warcraft III dir."));
		}
		else
		{
			QStringList items;

			for (QMap<int, KUrl>::iterator iterator = urls.begin(); iterator != urls.end(); ++iterator)
			{
				items.append(iterator.value().toLocalFile());
			}

			this->prepend(items);
		}
	}
}

void SourcesDialog::clear()
{
	this->m_editListBox->clear();
}

void SourcesDialog::prepend(const QStringList &items)
{
	QStringList newItems;

	foreach (QString item, items)
	{
		QString result;


		bool add = prepareItem(item, result);

		if (add)
		{
			newItems.append(result);
		}
	}

	QStringList oldItems = m_editListBox->items();

	foreach (QString item, newItems)
	{
		oldItems.prepend(item);
	}

	m_editListBox->setItems(oldItems);
}

SourcesDialog::SourcesDialog(MpqPriorityList *source, QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags)
, m_source(source)
{
	setupUi(this);

	//m_editListBox->
	// TODO use URL requester
	//KUrlRequester *requester = new KUrlRequester(this);
	//KUrlCompletion *urlCompletion = new KUrlCompletion(KUrlCompletion::DirCompletion);
	//m_editListBox->lineEdit()->setCompletionObject(urlCompletion);
	//m_editListBox->setCustomEditor(*requester);

	KUrlRequester *urlRequester = new KUrlRequester(m_editListBox);
	urlRequester->setMode(KFile::ExistingOnly | KFile::File | KFile::Directory);

	KMimeType::Ptr mpq(KMimeType::mimeType("application/x-mpq"));
	KMimeType::Ptr w3m(KMimeType::mimeType("application/x-w3m"));
	KMimeType::Ptr w3x(KMimeType::mimeType("application/x-w3x"));
	KMimeType::Ptr w3n(KMimeType::mimeType("application/x-w3n"));
	QString filter;

	if (mpq.isNull() || w3m.isNull() || w3x.isNull() || w3n.isNull())
	{
		filter = i18n("*|All Files\n*.mpq;*.w3m;*.w3x;*.w3n|Blizzard archives");
	}
	else
	{
		filter = i18n("all/allfiles application/x-mpq application/x-w3m application/x-w3x application/x-w3n");
	}

	urlRequester->setFilter(filter);
	KEditListWidget::CustomEditor customEditor(urlRequester, urlRequester->lineEdit());
	m_editListBox->setCustomEditor(customEditor);

	connect(m_dialogButtonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(ok()));
	connect(m_dialogButtonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
	connect(m_dialogButtonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));
	connect(m_editListBox, SIGNAL(added(QString)), this, SLOT(added(QString)));
	connect(m_wc3DirPushButton, SIGNAL(clicked()), this, SLOT(addWc3Dir()));
	connect(m_clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));
}

void SourcesDialog::added(const QString& text)
{
	QStringList items = m_editListBox->items();
	items.removeFirst();
	QString result;
	bool add = prepareItem(text, result);

	if (add)
	{
		items.prepend(result);
	}

	m_editListBox->setItems(items);
}

void SourcesDialog::ok()
{
	apply();
	hide();
}

void SourcesDialog::apply()
{
	QLinkedList<KUrl> valids;

	source()->clear(); // NOTE we always have to clear first since priorities of existing entries might have changed
	MpqPriorityListEntry::Priority priority = m_editListBox->items().size();

	foreach (const QString &item, m_editListBox->items())
	{
		if (!source()->addSource(item, priority))
		{
			KMessageBox::error(this, i18n("Invalid source \"%1\".", item));
		}
		else
		{
			qDebug() << "Successfully added item" << item << "with priority" << priority;
		}

		--priority;
	}

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

bool SourcesDialog::prepareItem(const QString &item, QString &result)
{
	// fix mpq entries automatically
	if (item.endsWith(".mpq") && !item.startsWith("mpq:/")) {
		QString protocol = "mpq:";

		if (!item.startsWith('/')) { // relative path
			protocol += '/';
		}

		result = protocol + item;

		return true;
	} else {
		const KUrl url(item);

		if (url.isLocalFile() && !item.startsWith("file://")) {
			result = "file://" + item;

			return true;
		}

		result = item;

		return true;
	}

	return false;
}

QString SourcesDialog::settingsGroup() const
{
	if (dynamic_cast<Editor*>(parentWidget()) != 0)
	{
		return static_cast<Editor*>(parentWidget())->aboutData().appName();
	}
	else if (dynamic_cast<Module*>(parentWidget()) != 0)
	{
		Module *module = static_cast<Module*>(parentWidget());

		return module->settingsGroup();
	}
	else
	{
		throw Exception(_("Sources can only have modules or editor as parent."));
	}

	return "";
}

}

}
