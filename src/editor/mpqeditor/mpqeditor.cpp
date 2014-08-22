/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#include <KFileDialog>
#include <KLocale>
#include <KAction>
#include <KMessageBox>

#include "mpqeditor.hpp"
#include "../listfilesdialog.hpp"

namespace wc3lib
{

namespace editor
{

MpqEditor::MpqEditor(MpqPriorityList* source, QWidget* parent, Qt::WindowFlags f) : Module(source, parent, f)
{
	Module::setupUi();
	QWidget *widget = new QWidget(this);
	Ui::MpqEditor::setupUi(widget);
	this->centerLayout()->addWidget(widget);
}

QTreeWidgetItem* MpqEditor::constructFile(const QString& path, QTreeWidgetItem *topItem)
{
	const QStringList list = path.split("\\");

	if (list.size() > 1)
	{

		const QString dirName = list[0];
		QList<QTreeWidgetItem*> children = topItem->takeChildren();
		QTreeWidgetItem *item = 0;

		foreach (QTreeWidgetItem *child, children)
		{
			if (child->data(0, Qt::UserRole) == dirName)
			{
				item = child;

				break;
			}
		}

		if (item == 0)
		{
			item = new QTreeWidgetItem(this->m_archivesTreeWidget);
			item->setText(0, dirName);
			item->setData(0, Qt::UserRole, dirName);
			topItem->addChild(item);
		}

		// has more directories
		if (list.size() > 2)
		{
			constructFile(path.left(dirName.size() + 1), item);
		}

		return item;
	}

	return 0;

}

QSet<QString> MpqEditor::uniqueDirs(const mpq::Listfile::Entries &entries) const
{
	QSet<QString> result;

	foreach (mpq::Listfile::Entries::const_reference ref, entries)
	{
		QString filePath = ref.c_str();
		/*
		 * Listfiles are not case sensitive so make to upper.
		 */
		filePath = filePath.toUpper();
		const int index = filePath.lastIndexOf("\\");

		if (index != -1)
		{
			const QString dirPath = filePath.left(index);
			//qDebug() << "Adding dir" << dirPath;
			result.insert(dirPath);
		}
	}

	return result;
}

void MpqEditor::openMpqArchive(const KUrl &url)
{
	ListfilesDialog *dialog = new ListfilesDialog(this);

	if (dialog->exec() == QDialog::Accepted)
	{
		mpq::Listfile::Entries listfileEntries = dialog->checkedEntries();

		qDebug() << "Listfile entries: " << listfileEntries.size();

		MpqPtr archive(new mpq::Archive());

		try
		{
			archive->open(url.toLocalFile().toUtf8().constData());
		}
		catch (Exception &e)
		{
			KMessageBox::error(this, i18n("Error on opening archive: %1", e.what()));

			return;
		}

		/*
		 * Add entries of the contained listfile if available.
		 */
		if (archive->containsListfileFile())
		{
			mpq::Listfile listfile = archive->listfileFile();

			if (listfile.isValid())
			{
				mpq::Listfile::Entries entries = listfile.entries();
				std::copy(entries.begin(), entries.end(), listfileEntries.begin());
			}
		}

		qDebug() << "Full size:" << listfileEntries.size();

		// TODO better performance when erasing duplicates?
		//std::sort(listfileEntries.begin(), listfileEntries.end());
		//listfileEntries.erase(std::unique(listfileEntries.begin(), listfileEntries.end()), listfileEntries.end());
		QSet<QString> dirs = this->uniqueDirs(listfileEntries);

		qDebug() << "Filtered size:" << dirs.size();

		QTreeWidgetItem *item = new QTreeWidgetItem(this->m_archivesTreeWidget);
		item->setData(0, Qt::UserRole, url);
		item->setText(0, url.fileName());

		foreach(QSet<QString>::const_reference ref, dirs)
		{
			mpq::File file = archive->findFile(ref.toUtf8().constData());

			if (file.isValid())
			{
				qDebug() << "Contains:" << ref;

				constructFile(ref, item);
			}
		}

		m_mpqArchives.push_back(archive);
	}
}

void MpqEditor::newMpqArchive()
{
}

void MpqEditor::openMpqArchives()
{
	KUrl::List urls = KFileDialog::getOpenUrls(m_openStartUrl, i18n("*.mpq|MPQ archives\n*"), this);

	foreach (const KUrl &url, urls)
	{
		openMpqArchive(url);
	}
}

void MpqEditor::saveMpqArchive()
{
	KUrl url = KFileDialog::getSaveUrl(m_saveStartUrl, i18n("*.mpq|MPQ archives\n*"), this);

	if (url.isEmpty())
		return;
}

void MpqEditor::closeMpqArchives()
{
	foreach (MpqPtr ptr, selection())
	{
		ptr->close();
		ptr.reset();
	}
}

void MpqEditor::optimizeMpqArchives()
{
}

void MpqEditor::addFiles()
{
}

void MpqEditor::extractFiles()
{
}

void MpqEditor::removeFiles()
{
}

void MpqEditor::optimizeFiles()
{
}

void MpqEditor::createDirectory()
{
}

void MpqEditor::openWar3()
{
	openMpqArchive(war3Url());
}

void MpqEditor::openWar3X()
{
	openMpqArchive(war3XUrl());
}

void MpqEditor::openWar3Patch()
{
	openMpqArchive(war3PatchUrl());
}

void MpqEditor::openWar3XLocal()
{
	openMpqArchive(war3XLocalUrl());
}

void MpqEditor::createFileActions(KMenu *menu)
{
	m_fileActions = new KActionCollection(this);

	KAction *action = new KAction(KIcon(":/actions/openarchives.png"), i18n("Open archives"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(openMpqArchives()));
	m_fileActions->addAction("openarchives", action);

	action = new KAction(KIcon(":/actions/opencustomtexttriggers.png"), i18n("Close archives"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(closeMpqArchives()));
	m_fileActions->addAction("closearchives", action);

	m_fileActions->associateWidget(menu);
}

void MpqEditor::createEditActions(class KMenu *menu)
{
}

void MpqEditor::createMenus(class KMenuBar *menuBar)
{
}

void MpqEditor::createWindowsActions(WindowsMenu *menu)
{
}

void MpqEditor::createToolButtons(ModuleToolBar *toolBar)
{
}

class SettingsInterface* MpqEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void MpqEditor::onSwitchToMap(Map *map)
{
}

#include "moc_mpqeditor.cpp"

}

}
