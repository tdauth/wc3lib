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
#include "fileinfodialog.hpp"

namespace wc3lib
{

namespace editor
{

MpqEditor::MpqEditor(MpqPriorityList* source, QWidget* parent, Qt::WindowFlags f)
: Module(source, parent, f)
, m_listfilesDialog(new ListfilesDialog(this))
, m_fileInfoDialog(new FileInfoDialog(this))
{
	Module::setupUi();
	QWidget *widget = new QWidget(this);
	Ui::MpqEditor::setupUi(widget);
	this->centerLayout()->addWidget(widget);

	connect(this->m_archivesTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
	connect(this->m_archivesTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(fileIsOpen(QTreeWidgetItem*,int)));
	connect(this->m_archivesTreeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));

	readSettings(); // fill sources first
}

MpqEditor::~MpqEditor()
{
	writeSettings();

	closeMpqArchives();
}

void MpqEditor::updateSelection()
{
	QList<QTreeWidgetItem*> items = this->m_archivesTreeWidget->selectedItems();

	m_extractAction->setEnabled(!items.empty());
}

void MpqEditor::fileIsOpen(QTreeWidgetItem *item, int column)
{
	qDebug() << "file is open";

	FileItems::iterator iterator = m_archiveFileItems.find(item);

	if (iterator != m_archiveFileItems.end())
	{
		qDebug() << "file is found in items";
		const QString filePath = item->data(0, Qt::UserRole).toString();
		mpq::Archive *archive = iterator.value();
		qDebug() << "path:" << filePath;

		/*
		 * Is regular file.
		 */
		if (!filePath.endsWith('\\'))
		{
			qDebug() << "regular file";
			mpq::File file = archive->findFile(filePath.toUtf8().constData());

			if (file.isValid())
			{
				qDebug() << "valid";
				this->m_fileInfoDialog->fill(*archive, file);
				this->m_fileInfoDialog->show();
			}
			else
			{
				KMessageBox::error(this, i18n("File %1 not found in archive.", filePath));
			}
		}
		/*
		 * Is directory.
		 */
		else
		{
			qDebug() << "dir";
			this->m_fileInfoDialog->fill(filePath);
			this->m_fileInfoDialog->show();
		}
	}
}

void MpqEditor::contextMenu(QPoint point)
{
	//QList<QTreeWidgetItem*> items = this->m_archivesTreeWidget->selectedItems();

	KMenu *contextMenu = new KMenu(this);
	contextMenu->addAction(m_extractAction);

	contextMenu->popup(m_archivesTreeWidget->viewport()->mapToGlobal(point));
}

mpq::Listfile::Entries MpqEditor::uniqueFiles(const mpq::Listfile::Entries &entries, mpq::Archive &archive) const
{
	QMap<QString, QString> map;
	mpq::Listfile::Entries result;

	foreach (mpq::Listfile::Entries::const_reference ref, entries)
	{
		QString filePath = ref.c_str();

		/*
		 * If file is not already in the result we check if the file
		 * does exist in the archive and if it does we add it.
		 *
		 * Listfiles are not case sensitive so make to upper.
		 */
		if (!map.contains(filePath.toUpper()))
		{
			mpq::File file = archive.findFile(ref.c_str());

			if (file.isValid())
			{
				map.insert(filePath.toUpper(), filePath);
				result.push_back(filePath.toUtf8().constData());
			}
		}
	}

	return result;
}

MpqEditor::FileItems MpqEditor::constructItems(const mpq::Listfile::Entries &entries, QTreeWidgetItem *topItem, mpq::Archive &archive)
{
	/*
	 * Use this map to store all directory items for fast access when adding childs.
	 */
	QMap<QString, QTreeWidgetItem*> map;
	/*
	 * Return all created items in the end so that they can be associated with the archive.
	 */
	FileItems result;

	for (mpq::Listfile::Entries::const_iterator iterator = entries.begin(); iterator != entries.end(); ++iterator)
	{
		const QString file = iterator->c_str();
		const int lastIndex = file.lastIndexOf('\\');

		if (lastIndex != -1)
		{
			const QString dirs = file.left(lastIndex + 1);
			int i = 0;
			QTreeWidgetItem *last = 0;

			for (int index = dirs.indexOf('\\'); index != -1; index = dirs.indexOf('\\', index + 1))
			{
				const QString dirPath = dirs.left(index + 1);
				QString dirName = dirPath;
				dirName.chop(1);
				dirName = this->fileName(dirName);
				const QString upperDirPath = dirPath.toUpper();
				QMap<QString, QTreeWidgetItem*>::iterator iterator = map.find(upperDirPath);

				/*
				 * Directory item does not exist already, so create the tree widget item.
				 */
				if (iterator == map.end())
				{
					QTreeWidgetItem *item = new QTreeWidgetItem();
					/*
					 * Show only dir name.
					 */
					item->setText(0, dirName);
					/*
					 * Store the whole dir path with / that the program knows it is a directory.
					 */
					item->setData(0, Qt::UserRole, upperDirPath);

					if (i == 0)
					{
						topItem->addChild(item);
					}
					else
					{
						last->addChild(item);
					}

					last = item;
					map.insert(upperDirPath, item);
					result.insert(item, &archive);
				}
				/*
				 * Otherwise store item of parent directory for adding the new item as child.
				 */
				else
				{
					last = iterator.value();
				}

				++i;
			}
		}
		/*
		 * Otherwise it is a top level file and can directly be added to topItem.
		 */
		else
		{
			QTreeWidgetItem *item = fileToItem(*iterator, archive);
			topItem->addChild(item);
			result.insert(item, &archive);
		}
	}

	for (QMap<QString, QTreeWidgetItem*>::iterator iterator = map.begin(); iterator != map.end(); ++iterator)
	{
		const mpq::Listfile::Entries files = mpq::Listfile::dirEntries(entries, iterator.key().toUtf8().constData(), false, false);

		foreach (mpq::Listfile::Entries::const_reference file, files)
		{
			const QString fileName = file.c_str();
			QTreeWidgetItem *item = fileToItem(file, archive);
			iterator.value()->addChild(item);
			result.insert(item, &archive);
		}
	}

	return result;
}

QTreeWidgetItem* MpqEditor::fileToItem(const boost::filesystem::path &path, mpq::Archive &archive)
{
	mpq::File file = archive.findFile(path);

	if (file.isValid())
	{
		const QString filePath = path.c_str();
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, this->fileName(filePath));
		item->setData(0, Qt::UserRole, filePath.toUpper());

		item->setText(1, sizeStringDecimal(file.size()).c_str());

		item->setText(2, sizeStringDecimal(file.compressedSize()).c_str());

		return item;
	}

	return 0;
}

void MpqEditor::addRecentAction(const KUrl& url)
{
	/*
	 * Do not add entries twice in a history.
	 *
	 * TODO move action up.
	 */
	if (m_archiveHistory.indexOf(url) != -1)
	{
		return;
	}

	const int i = m_archiveHistory.size();
	KAction *action = new KAction(url.toLocalFile(), this);
	connect(action, SIGNAL(triggered()), this, SLOT(openRecentArchive()));
	m_recentArchivesMenu->insertAction(m_recentArchivesSeparator, action);
	m_archiveHistoryActions->addAction(QString("recent%1").arg(i), action);
	m_archiveHistory.push_back(url);
}

bool MpqEditor::openMpqArchive(const KUrl &url)
{
	if (listfilesDialog()->exec() == QDialog::Accepted)
	{
		mpq::Listfile::Entries listfileEntries = listfilesDialog()->checkedEntries();

		qDebug() << "Listfile entries: " << listfileEntries.size();

		MpqPtr archive(new mpq::Archive());

		try
		{
			archive->open(url.toLocalFile().toUtf8().constData());
		}
		catch (Exception &e)
		{
			KMessageBox::error(this, i18n("Error on opening archive: %1", e.what()));

			return false;
		}

		/*
		 * Make sure extra files are listed if available.
		 */
		listfileEntries.push_back("(listfile)");
		listfileEntries.push_back("(attributes");
		listfileEntries.push_back("(signature");

		/*
		 * Add entries of the contained listfile if available.
		 */
		if (archive->containsListfileFile())
		{
			qDebug() << "Has custom listfile";

			mpq::Listfile listfile = archive->listfileFile();

			if (listfile.isValid())
			{
				qDebug() << "Listfile is valid.";

				mpq::Listfile::Entries entries = listfile.entries();

				/*
				 * Still use user selected listfiles even if it has own entries.
				 * This should be the expected behaviour.
				 */
				if (!entries.empty())
				{
					foreach (mpq::Listfile::Entries::const_reference ref, entries)
					{
						listfileEntries.push_back(ref);
					}
				}
				else
				{
					qDebug() << "Listfile has no entries.";
				}
			}
			else
			{
				qDebug() << "Listfile is invalid.";
			}
		}

		qDebug() << "Full size:" << listfileEntries.size();

		mpq::Listfile::Entries files = this->uniqueFiles(listfileEntries, *archive);

		qDebug() << "Filtered size:" << files.size();

		QTreeWidgetItem *item = new QTreeWidgetItem(this->m_archivesTreeWidget);
		item->setData(0, Qt::UserRole, url);
		item->setText(0, url.fileName());

		FileItems items = constructItems(files, item, *archive);

		for (FileItems::iterator iterator = items.begin(); iterator != items.end(); ++iterator)
		{
			m_archiveFileItems.insert(iterator.key(), iterator.value());
		}

		m_mpqArchives.push_back(archive);
		addRecentAction(url);

		return true;
	}

	return false;
}

void MpqEditor::newMpqArchive()
{
}

void MpqEditor::openMpqArchives()
{
	KUrl::List urls = KFileDialog::getOpenUrls(m_openUrl, i18n("*.mpq|MPQ archives\n*"), this);

	foreach (const KUrl &url, urls)
	{
		if (openMpqArchive(url))
		{
			m_openUrl = url.directory();
		}
	}
}

void MpqEditor::openRecentArchive()
{
	KAction *action = dynamic_cast<KAction*>(QObject::sender());

	if (action != 0)
	{
		const int index = this->m_archiveHistoryActions->actions().indexOf(action);

		if (index != -1)
		{
			openMpqArchive(this->m_archiveHistory[index]);
		}
	}
}

void MpqEditor::clearHistory()
{
	this->m_archiveHistoryActions->clear();
	this->m_archiveHistory.clear();
}

void MpqEditor::saveMpqArchive()
{
	KUrl url = KFileDialog::getSaveUrl(m_saveUrl, i18n("*.mpq|MPQ archives\n*"), this);

	if (url.isEmpty())
	{
		return;
	}
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
	QList<QTreeWidgetItem*> items = this->m_archivesTreeWidget->selectedItems();

	if (items.size() == 1)
	{
		QTreeWidgetItem *item = items.first();
		KUrl url = KFileDialog::getSaveUrl(KUrl(), "*", this);

		if (!url.isEmpty())
		{
			const QString filePath = item->data(0, Qt::UserRole).toString();
			const QString fileName = this->fileName(filePath);
			FileItems::iterator iterator = m_archiveFileItems.find(item);

			if (iterator != m_archiveFileItems.end())
			{
				mpq::Archive *archive = iterator.value();

				extractFile(filePath, *archive, url.toLocalFile());
			}
			else
			{
				KMessageBox::error(this, i18n("File %1 is not part of any archive.", filePath));
			}
		}
	}
	else
	{
		QString dir = KFileDialog::getExistingDirectory(KUrl(), this);

		if (!dir.isEmpty())
		{
			foreach (QTreeWidgetItem *item, items)
			{
				const QString filePath = item->data(0, Qt::UserRole).toString();
				const QString fileName = this->fileName(filePath);
				FileItems::iterator iterator = m_archiveFileItems.find(item);

				if (iterator != m_archiveFileItems.end())
				{
					mpq::Archive *archive = iterator.value();

					extractFile(filePath, *archive, dir + fileName);
				}
				else
				{
					KMessageBox::error(this, i18n("File %1 is not part of any archive.", filePath));
				}
			}
		}
	}
}

QString MpqEditor::fileName(const QString &path)
{
	const int lastIndex = path.lastIndexOf('\\');

	if (lastIndex != -1)
	{
		if (path.size() > lastIndex)
		{
			return path.mid(lastIndex + 1);
		}

		return QString();
	}

	return path;
}

QString MpqEditor::baseName(const QString& path)
{
	QString fileName = this->fileName(path);

	const int lastIndex = fileName.lastIndexOf('.');

	if (lastIndex != -1)
	{
		if (fileName.size() > lastIndex)
		{
			return fileName.mid(lastIndex + 1);
		}

		return QString();
	}

	return fileName;
}

bool MpqEditor::extractFile(const QString &path, mpq::Archive& archive, const QString& target)
{
	mpq::File file = archive.findFile(path.toUtf8().constData());

	if (file.isValid())
	{
		QFile outputFile(target);

		if (outputFile.isWritable() || !outputFile.exists())
		{
			if ((outputFile.exists() && KMessageBox::questionYesNo(this, i18n("Overwrite existing file %1?", outputFile.fileName())) == KMessageBox::Yes)
				|| !outputFile.exists())
			{
				ofstream out(target.toUtf8().constData(), std::ios::out | std::ios::binary);

				if (out)
				{

					try
					{
						file.writeData(out);
						out.close();

						return true;
					}
					catch (Exception &e)
					{
						KMessageBox::error(this, e.what());
					}
				}
				else
				{
					KMessageBox::error(this, i18n("Error on opening file %1 for writing.", target));
				}
			}
		}
		else
		{
			KMessageBox::error(this, i18n("No writing permission for file %1.", outputFile.fileName()));
		}
	}
	else
	{
		KMessageBox::error(this, i18n("File %1 is not part of archive %2.", path, archive.path().c_str()));
	}

	return false;
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

	m_recentArchivesMenu = new KMenu(i18n("Open recent"), this);
	m_fileActions->addAction("openrecentarchives", m_recentArchivesMenu->menuAction());
	m_archiveHistoryActions = new KActionCollection(this);

	m_recentArchivesSeparator = m_recentArchivesMenu->addSeparator();
	action = new KAction(i18n("Clear list"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(clearHistory()));
	m_recentArchivesMenu->addAction(action);

	action = new KAction(KIcon(":/actions/closearchives.png"), i18n("Close archives"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(closeMpqArchives()));
	m_fileActions->addAction("closearchives", action);

	m_fileActions->associateWidget(menu);
}

void MpqEditor::createEditActions(KMenu *menu)
{
	m_extractAction = new KAction(KIcon(":/actions/extract.png"), i18n("Extract files"), this);
	connect(m_extractAction, SIGNAL(triggered()), this, SLOT(extractFiles()));
	menu->addAction(m_extractAction);
}

void MpqEditor::createMenus(KMenuBar *menuBar)
{
}

void MpqEditor::createWindowsActions(WindowsMenu *menu)
{
}

void MpqEditor::createToolButtons(ModuleToolBar *toolBar)
{
}

SettingsInterface* MpqEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void MpqEditor::onSwitchToMap(Map *map)
{
}

void MpqEditor::readSettings()
{
	Module::readSettings();

	QSettings settings("wc3lib", "mpqeditor");

	qDebug() << "MPQ editor settings:" << settings.fileName();

	settings.beginGroup("history");
	this->m_openUrl = settings.value("openurl").toByteArray();
	qDebug() << "open url" << this->m_openUrl.toEncoded();
	this->m_saveUrl = settings.value("saveurl").toByteArray();
	this->m_addUrl = settings.value("addurl").toByteArray();
	this->m_extractUrl = settings.value("extracturl").toByteArray();

	const int max = settings.beginReadArray("history");

	for (int i = 0; i < max; ++i)
	{
		settings.setArrayIndex(i);
		const KUrl url = KUrl(settings.value("url").toByteArray());
		qDebug() << "Url " << i << ":" << url;
		addRecentAction(url);
	}

	settings.endArray();

	listfilesDialog()->setAddUrl(KUrl(settings.value("listfilesurl").toByteArray()));

	settings.endGroup();
}

void MpqEditor::writeSettings()
{
	Module::writeSettings();

	QSettings settings("wc3lib", "mpqeditor");

	qDebug() << "MPQ editor settings:" << settings.fileName();

	if (!settings.isWritable())
	{
		qDebug() << "Settings cannot be written.";
	}

	settings.beginGroup("history");
	settings.setValue("openurl", this->m_openUrl.toEncoded());
	settings.setValue("saveurl", this->m_saveUrl.toEncoded());
	settings.setValue("addurl", this->m_addUrl.toEncoded());
	settings.setValue("extracturl", this->m_extractUrl.toEncoded());

	settings.beginWriteArray("history", m_archiveHistory.size());

	for (int i = 0; i < this->m_archiveHistory.size(); ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue("url", this->m_archiveHistory[i].toEncoded());
	}

	settings.endArray();

	settings.setValue("listfilesurl", listfilesDialog()->addUrl().toEncoded());

	settings.endGroup();

	settings.sync();

	if (settings.status() != QSettings::NoError)
	{
		qDebug() << "Error on writing settings.";
	}
}

#include "moc_mpqeditor.cpp"

}

}
