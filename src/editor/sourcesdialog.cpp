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
#include <QtWidgets>

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
		this->addUrl(source.url());
	}
}

void SourcesDialog::addWc3libDir()
{
#ifdef Q_OS_UNIX
	const QUrl url = QUrl::fromLocalFile("/usr/share/wc3lib/war3");
	Urls items;
	items.push_back(url);
	this->prepend(items);
#else
#warning Fix wc3lib directory on this platform.
#endif
}

void SourcesDialog::addWc3Dir()
{
	const QUrl url = QFileDialog::getExistingDirectoryUrl(this, tr("Select Warcraft III Directory"));

	if (!url.isEmpty())
	{
		QMap<int, QUrl> urls;
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
						urls.insert(3, QUrl::fromLocalFile(info.absoluteFilePath()));
					}
					else if (info.fileName().toLower() == "war3xlocal.mpq")
					{
						urls.insert(2, QUrl::fromLocalFile(info.absoluteFilePath()));
					}
					else if (info.fileName().toLower() == "war3x.mpq")
					{
						urls.insert(1, QUrl::fromLocalFile(info.absoluteFilePath()));
					}
					else if (info.fileName().toLower() == "war3.mpq")
					{
						urls.insert(0, QUrl::fromLocalFile(info.absoluteFilePath()));
					}
				}
			}
		}

		if (urls.isEmpty())
		{
			QMessageBox::critical(this, tr("Error"), tr("Invalid Warcraft III dir."));
		}
		else
		{
			Urls items;

			for (QMap<int, QUrl>::iterator iterator = urls.begin(); iterator != urls.end(); ++iterator)
			{
				items.append(iterator.value());
			}

			this->prepend(items);
		}
	}
}

void SourcesDialog::clear()
{
	this->m_editListBox->clear();
}

void SourcesDialog::prepend(const Urls &items)
{
	Urls newItems;

	foreach (QUrl item, items)
	{
		QUrl result;


		bool add = prepareItem(item, result);

		if (add)
		{
			newItems.append(result);
		}
	}

	Urls oldItems;

	for (int i = 0; i < m_editListBox->count(); ++i)
	{
		const QListWidgetItem *item = m_editListBox->item(i);
		oldItems.push_back(item->data(Qt::UserRole).toUrl());
	}

	foreach (QUrl item, newItems)
	{
		oldItems.prepend(item);
	}

	m_editListBox->clear();

	foreach (QUrl url, oldItems)
	{
		this->addUrl(url);
	}
}

SourcesDialog::SourcesDialog(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
, m_source(source)
{
	setupUi(this);

	QMimeDatabase db;
	const QMimeType mpq = db.mimeTypeForName("application/x-mpq");
	const QMimeType w3m = db.mimeTypeForName("application/x-w3m");
	const QMimeType w3x = db.mimeTypeForName("application/x-w3x");
	const QMimeType w3n = db.mimeTypeForName("application/x-w3n");
	QString filter;

	if (!mpq.isValid() || !w3m.isValid() || !w3x.isValid() || !w3n.isValid())
	{
		filter = tr("*|All Files\n*.mpq;*.w3m;*.w3x;*.w3n|Blizzard archives");
	}
	else
	{
		filter = tr("all/allfiles application/x-mpq application/x-w3m application/x-w3x application/x-w3n");
	}

	connect(m_dialogButtonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(ok()));
	connect(m_dialogButtonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
	connect(m_dialogButtonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));
	connect(m_addUrlPushButton, SIGNAL(clicked()), this, SLOT(addUrlSlot()));
	connect(m_wc3libDirPushButton, SIGNAL(clicked()), this, SLOT(addWc3libDir()));
	connect(m_wc3DirPushButton, SIGNAL(clicked()), this, SLOT(addWc3Dir()));
	connect(m_clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));
}

void SourcesDialog::addUrlSlot()
{
	const QUrl url = QFileDialog::getOpenFileUrl(this, tr("Add MPQ Archive or directory"));

	if (!url.isEmpty())
	{
		QUrl result;
		bool add = prepareItem(url, result);

		if (add)
		{
			this->addUrl(result);
		}
	}
}

void SourcesDialog::ok()
{
	apply();
	hide();
}

void SourcesDialog::apply()
{
	source()->clear(); // NOTE we always have to clear first since priorities of existing entries might have changed
	MpqPriorityListEntry::Priority priority = m_editListBox->count();

	for (int i = 0; i < m_editListBox->count(); ++i)
	{
		const QListWidgetItem *item = m_editListBox->item(i);
		const QUrl url = item->data(Qt::UserRole).toUrl();

		if (!source()->addSource(url, priority))
		{
			QMessageBox::critical(this, tr("Error"), tr("Invalid source \"%1\".").arg(url.toString()));
		}
		else
		{
			qDebug() << "Successfully added item" << url << "with priority" << priority;
		}

		--priority;
	}

	this->source()->writeSettings(settingsGroup());
}

void SourcesDialog::restoreDefaults()
{
	m_editListBox->clear();

	if (!war3Url().isEmpty())
	{
		this->addUrl(war3Url());
	}

	if (!war3XUrl().isEmpty())
	{
		this->addUrl(war3XUrl());
	}

	if (!war3XLocalUrl().isEmpty())
	{
		this->addUrl(war3XLocalUrl());
	}

	if (!war3PatchUrl().isEmpty())
	{
		this->addUrl(war3PatchUrl());
	}
}

void SourcesDialog::showEvent(QShowEvent *e)
{
	update();
	QDialog::showEvent(e);
}

bool SourcesDialog::prepareItem(const QUrl &item, QUrl &result)
{
	result = item;

	// fix mpq entries automatically
	if (result.toString().endsWith(".mpq") && result.scheme() != "mpq")
	{
		result.setScheme("mpq");

		return true;
	}
	else
	{
		if (result.isLocalFile() && result.scheme() != "file")
		{
			result.setScheme("file");
		}

		return true;
	}

	return false;
}

QString SourcesDialog::settingsGroup() const
{
	if (dynamic_cast<Editor*>(source()) != 0)
	{
		return "WorldEditor";
	}
	else if (dynamic_cast<Module*>(parentWidget()) != 0)
	{
		Module *module = static_cast<Module*>(parentWidget());

		return module->settingsGroup();
	}

	qDebug() << "Warning: Using object name as settings group:" << parent()->objectName();

	return parent()->objectName();
}

void SourcesDialog::addUrl(const QUrl &url)
{
	QListWidgetItem *item = new QListWidgetItem(url.toString());
	item->setData(Qt::UserRole, url);
	m_editListBox->addItem(item);
}

}

}
