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
#include "../mpqprioritylist.hpp"
#include "../platform.hpp"
#include "archiveinfodialog.hpp"
#include "fileinfodialog.hpp"

namespace wc3lib
{

namespace editor
{

MpqEditor::MpqEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f)
: Module(source, parent, f)
, m_extractAction(0)
, m_infoAction(0)
, m_listfilesDialog(new ListfilesDialog(this))
, m_archiveInfoDialog(new ArchiveInfoDialog(this))
, m_fileInfoDialog(new FileInfoDialog(this))
{
	Module::readSettings(); // fill sources first

	// Update required files if started as stand-alone module
	if (!hasEditor())
	{
		try
		{
			source->sharedData()->refreshWorldEditorStrings(this);
			source->sharedData()->refreshWorldEditData(this);
		}
		catch (wc3lib::Exception &e)
		{
			KMessageBox::error(0, i18n("Error when loading default files: %1", e.what()));
		}
	}

	Module::setupUi();
	QWidget *widget = new QWidget(this);
	Ui::MpqEditor::setupUi(widget);
	this->centerLayout()->addWidget(widget);

	setWindowTitle(tr("MPQ Editor"));
	m_archivesTreeWidget->sortByColumn(0, Qt::AscendingOrder);

	connect(this->m_archivesTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
	connect(this->m_archivesTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(fileIsOpen(QTreeWidgetItem*,int)));
	connect(this->m_archivesTreeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
	connect(this->m_archivesTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(expandItem(QTreeWidgetItem*)));
	connect(this->m_archivesTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(collapseItem(QTreeWidgetItem*)));

	// read GUI settings
	readSettings();

}

MpqEditor::~MpqEditor()
{
	writeSettings();

	closeAllMpqArchives();
}

void MpqEditor::updateSelection()
{
	QList<QTreeWidgetItem*> items = this->m_archivesTreeWidget->selectedItems();

	m_extractAction->setEnabled(!items.empty());
	m_infoAction->setEnabled(!items.empty());
}

void MpqEditor::expandItem(QTreeWidgetItem *item)
{
	if (!m_archiveTopLevelItems.contains(item))
	{
		if (this->source()->sharedData()->worldEditData().get() != nullptr)
		{
			const QIcon folderIcon = this->source()->sharedData()->worldEditDataIcon("OMIcon_FolderOpen", "WorldEditArt", this);
			item->setIcon(0, folderIcon);
		}
	}
}

void MpqEditor::collapseItem(QTreeWidgetItem *item)
{
	if (!m_archiveTopLevelItems.contains(item))
	{
		if (this->source()->sharedData()->worldEditData().get() != nullptr)
		{
			const QIcon folderIcon = this->source()->sharedData()->worldEditDataIcon("OMIcon_Folder", "WorldEditArt", this);
			item->setIcon(0, folderIcon);
		}
	}
}

void MpqEditor::openFile(mpq::Archive& archive, const QString& filePath)
{
	mpq::File file = archive.findFile(filePath.toUtf8().constData());

	if (file.isValid())
	{
		qDebug() << "valid";

		// TODO performance might be poor with stringstream and string
		stringstream sstream;
		file.writeData(sstream);
		const string data = sstream.str();

		QTemporaryFile tempFile;
		/*
		 * Otherwise calling the service asynchronously won't find the file anymore.
		 * TODO is there any other way? It may create many many temporary files!
		 */
		tempFile.setAutoRemove(false);

		if (tempFile.open())
		{
			tempFile.write(data.c_str(), data.size());
			tempFile.close();

			QDesktopServices::openUrl(tempFile.fileName());
		}
	}
	else
	{
		KMessageBox::error(this, i18n("File %1 not found in archive.", filePath));
	}

}

void MpqEditor::fileIsOpen(QTreeWidgetItem *item, int column)
{
	qDebug() << "file is open";

	FileItems::iterator iterator = m_archiveFileItems.find(item);

	if (iterator != m_archiveFileItems.end())
	{
		qDebug() << "file is found in items";
		const QString filePath = item->data(0, Qt::UserRole).toString();
		mpq::Archive &archive = iterator.value()->archive();
		qDebug() << "path:" << filePath;

		/*
		 * Is regular file.
		 */
		if (!filePath.endsWith('\\'))
		{
			qDebug() << "regular file";
			openFile(archive, filePath);
			//this->m_fileInfoDialog->fill(archive, file);
			//this->m_fileInfoDialog->show();

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
	/*
	 * Archive item:
	 */
	else
	{
		FileItems::iterator iterator = m_archiveTopLevelItems.find(item);

		if (iterator != m_archiveTopLevelItems.end())
		{
			this->m_archiveInfoDialog->fill(iterator.value()->archive());
			this->m_archiveInfoDialog->show();
		}
	}
}

void MpqEditor::showFileInfo()
{

	foreach (QTreeWidgetItem *item, this->m_archivesTreeWidget->selectedItems())
	{
		FileItems::iterator iterator = m_archiveFileItems.find(item);

		if (iterator != m_archiveFileItems.end())
		{
			qDebug() << "file is found in items";
			const QString filePath = item->data(0, Qt::UserRole).toString();
			mpq::Archive &archive = iterator.value()->archive();
			qDebug() << "path:" << filePath;

			/*
			* Is regular file.
			*/
			if (!filePath.endsWith('\\'))
			{
				qDebug() << "regular file";
				mpq::File file = archive.findFile(filePath.toUtf8().constData());

				if (file.isValid())
				{
					this->m_fileInfoDialog->fill(archive, file);
					this->m_fileInfoDialog->show();
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
		/*
		* Archive item:
		*/
		else
		{
			FileItems::iterator iterator = m_archiveTopLevelItems.find(item);

			if (iterator != m_archiveTopLevelItems.end())
			{
				this->m_archiveInfoDialog->fill(iterator.value()->archive());
				this->m_archiveInfoDialog->show();
			}
		}
	}
}

void MpqEditor::contextMenu(QPoint point)
{
	//QList<QTreeWidgetItem*> items = this->m_archivesTreeWidget->selectedItems();

	KMenu *contextMenu = new KMenu(this);
	contextMenu->addAction(m_extractAction);
	contextMenu->addAction(m_infoAction);
	contextMenu->addAction(m_closeAction);

	contextMenu->popup(m_archivesTreeWidget->viewport()->mapToGlobal(point));
}

MpqEditor::FileItems MpqEditor::constructItems(const mpq::Listfile::Entries &entries, QTreeWidgetItem *topItem, Archive &archive)
{
	/*
	 * Use this map to store all directory items for fast access when adding childs.
	 */
	typedef QMap<QString, QTreeWidgetItem*> Map;
	Map map;
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
				Map::iterator iterator = map.find(upperDirPath);

				/*
				 * Directory item does not exist already, so create the tree widget item.
				 */
				if (iterator == map.end())
				{
					QTreeWidgetItem *item = folderToItem(dirName, dirPath);

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

			if (item != 0)
			{
				result.insert(item, &archive);
				topItem->addChild(item);
			}
		}
	}

	/*
	 * Now create all files of sub directories stored in the map.
	 */
	for (QMap<QString, QTreeWidgetItem*>::iterator iterator = map.begin(); iterator != map.end(); ++iterator)
	{
		const QString dirPath = iterator.key();
		/*
		 * Only get the files of the current directory level (not recursively).
		 * Add file items to the current directory.
		 */
		const mpq::Listfile::Entries files = mpq::Listfile::caseSensitiveEntries(entries, dirPath.toUtf8().constData(), false);
		/*
		 * Set size column to number of entries.
		 */
		iterator.value()->setText(1, QString(tr("%1 files")).arg(files.size()));
		iterator.value()->setData(1, Qt::UserRole, (quint32)files.size());

		foreach (mpq::Listfile::Entries::const_reference file, files)
		{
			const QString fileName = file.c_str();
			QTreeWidgetItem *item = fileToItem(file, archive);

			if (item != 0)
			{
				iterator.value()->addChild(item);
				result.insert(item, &archive);
			}
		}
	}

	return result;
}

bool MpqEditor::itemIsFolder(QTreeWidgetItem* item) const
{
	return item->data(0, Qt::UserRole).toString().endsWith('\\');
}

QTreeWidgetItem* MpqEditor::folderToItem(const QString& dirName, const QString &dirPath)
{
	QTreeWidgetItem *item = new QTreeWidgetItem();
	/*
	 * Show only dir name.
	 */
	item->setText(0, dirName);
	/*
	 * Store the whole dir path with / that the program knows it is a directory.
	 */
	item->setData(0, Qt::UserRole, dirPath);

	if (this->source()->sharedData()->worldEditData().get() != nullptr)
	{
		const QIcon folderIcon = this->source()->sharedData()->worldEditDataIcon("OMIcon_Folder", "WorldEditArt", this);
		item->setIcon(0, folderIcon);
	}

	return item;
}

QTreeWidgetItem* MpqEditor::fileToItem(const boost::filesystem::path &path, Archive &archive)
{
	mpq::File file = archive.archive().findFile(path);

	if (file.isValid())
	{
		const QString filePath = path.c_str();
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, this->fileName(filePath));
		item->setData(0, Qt::UserRole, filePath);

		item->setText(1, sizeStringDecimal(file.size()).c_str());

		item->setText(2, sizeStringDecimal(file.compressedSize()).c_str());

		KMimeType::Ptr mimeType = KMimeType::findByPath(filePath);

		if (!mimeType.isNull())
		{
			const QIcon fileTypeIcon = QIcon(mimeType->iconName());
			item->setIcon(0, fileTypeIcon);
		}

		return item;
	}
	else
	{
		qDebug() << "Error: file does not exist" << path.c_str();
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

		std::auto_ptr<Archive> archive(new Archive());

		try
		{
			archive->archive().open(url.toLocalFile().toStdString());
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
		if (archive->archive().containsListfileFile())
		{
			qDebug() << "Has custom listfile";

			mpq::Listfile listfile = archive->archive().listfileFile();

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

		mpq::Listfile::Entries files = mpq::Listfile::caseSensitiveEntries(mpq::Listfile::existingEntries(listfileEntries, archive->archive()));

		qDebug() << "Filtered size:" << files.size();

		QFile file("tmplistfile.txt");
		QTextStream stream(&file);

		if (file.open(QIODevice::WriteOnly))
		{
			BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, files)
			{
				stream << ref.c_str() << '\n';
			}

		}

		file.close();

		QTreeWidgetItem *item = new QTreeWidgetItem(this->m_archivesTreeWidget);
		item->setText(0, url.fileName());
		item->setData(0, Qt::UserRole, url);
		item->setText(1, tr("%1 files").arg(files.size()));
		item->setData(1, Qt::UserRole, (quint32)files.size());

		FileItems items = constructItems(files, item, *archive);

		for (FileItems::iterator iterator = items.begin(); iterator != items.end(); ++iterator)
		{
			m_archiveFileItems.insert(iterator.key(), iterator.value());
		}

		archive->setItem(item);
		archive->setEntries(files);
		m_archiveTopLevelItems.insert(item, archive.get());
		m_archives.push_back(archive);
		addRecentAction(url);

		item->setExpanded(true);

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
	QList<QTreeWidgetItem*> selection = this->m_archivesTreeWidget->selectedItems();

	foreach (QTreeWidgetItem *item, selection)
	{
		FileItems::iterator iterator = this->m_archiveTopLevelItems.find(item);

		if (iterator != this->m_archiveTopLevelItems.end())
		{
			iterator.value()->deleteItem();
			iterator.value()->archive().close();
			this->m_archiveTopLevelItems.erase(iterator);
		}
	}
}

void MpqEditor::closeAllMpqArchives()
{
	for (Archives::iterator iterator = this->m_archives.begin(); iterator != this->m_archives.end(); iterator = this->m_archives.erase(iterator))
	{
		iterator->deleteItem();
		iterator->archive().close();
	}

	this->m_archiveTopLevelItems.clear();
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
		FileItems::iterator iterator = m_archiveTopLevelItems.find(item);

		/*
		 * Is the item of an open MPQ archive.
		 */
		if (iterator != m_archiveTopLevelItems.end())
		{
			qDebug() << "Support extracting archives!";
		}
		/*
		 * Is the item of a file or directory.
		 */
		else
		{
			const QString filePath = item->data(0, Qt::UserRole).toString();
			QString fileName;
			KUrl url;

			if (itemIsFolder(item))
			{
				fileName = this->dirname(filePath);
				url = KFileDialog::getExistingDirectoryUrl(m_extractUrl, this);
			}
			else
			{
				fileName = this->fileName(filePath);
				m_extractUrl.setFileName(fileName);
				url = KFileDialog::getSaveUrl(m_extractUrl, "*", this);
			}

			if (!url.isEmpty())
			{
				FileItems::iterator iterator = m_archiveFileItems.find(item);

				if (iterator != m_archiveFileItems.end())
				{
					Archive *archive = iterator.value();
					bool result = false;

					if (itemIsFolder(item))
					{
						mpq::Listfile::Entries dirEntries = this->dirEntries(item);
						result = extractDir(filePath, archive->archive(), url.toLocalFile(), dirEntries);
					}
					else
					{
						result = extractFile(filePath, archive->archive(), url.toLocalFile());
					}

					/*
					 * Update extraction URL path on success.
					 */
					if (result)
					{
						m_extractUrl = url.directory();
					}
				}
				else
				{
					KMessageBox::error(this, i18n("File %1 is not part of any archive.", filePath));
				}
			}
		}
	}
	else
	{
		QString dir = KFileDialog::getExistingDirectory(m_extractUrl, this);

		if (!dir.isEmpty())
		{
			bool success = true;

			foreach (QTreeWidgetItem *item, items)
			{
				FileItems::iterator iterator = m_archiveTopLevelItems.find(item);

				/*
				 * Is the item of an open MPQ archive.
				 */
				if (iterator != m_archiveTopLevelItems.end())
				{
					qDebug() << "Support extracting archives!";
				}
				/*
				 * Is a file or directory item.
				 */
				else
				{
					const QString filePath = item->data(0, Qt::UserRole).toString();
					const QString fileName = this->fileName(filePath);
					FileItems::iterator iterator = m_archiveFileItems.find(item);

					if (iterator != m_archiveFileItems.end())
					{
						Archive *archive = iterator.value();
						KUrl url;
						url.setDirectory(dir);
						url.setFileName(fileName);

						extractFile(filePath, archive->archive(), url.toLocalFile());
					}
					else
					{
						success = false;
						KMessageBox::error(this, i18n("File %1 is not part of any archive.", filePath));
					}
				}
			}

			/*
			 * Update extraction URL path on success.
			 */
			if (success)
			{
				m_extractUrl = dir;
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
	QString fileName = MpqEditor::fileName(path);

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

QString MpqEditor::dirname(const QString &path)
{
	const int lastIndex = path.lastIndexOf('\\');

	if (lastIndex != -1 && lastIndex > 0)
	{
		const QString result = path.mid(0, lastIndex);
		const int secondLastIndex = result.lastIndexOf('\\');

		if (secondLastIndex != -1 && secondLastIndex < result.size() - 1)
		{
			return result.mid(secondLastIndex + 1);
		}
		// top level dir
		else
		{
			return result;
		}
	}

	return QString();
}

mpq::Listfile::Entries MpqEditor::dirEntries(QTreeWidgetItem* item) const
{
	const QString dirPath = item->data(0, Qt::UserRole).toString();
	mpq::Listfile::Entries result;
	QStack<QTreeWidgetItem*> children;

	for (int i = 0; i < item->childCount(); ++i)
	{
		children.push(item->child(i));
	}

	while (!children.isEmpty())
	{
		QTreeWidgetItem *child = children.pop();

		if (!itemIsFolder(child))
		{
			const QString filePath = child->data(0, Qt::UserRole).toString();

			result.push_back(filePath.toUtf8().constData());
		}
		else
		{
			for (int i = 0; i < child->childCount(); ++i)
			{
				children.push(child->child(i));
			}
		}
	}

	return result;
}


bool MpqEditor::extractDir(const QString &path, mpq::Archive &archive, const QString &target, const mpq::Listfile::Entries &dirEntries)
{

	QDir outputDir(target);

	if (outputDir.exists())
	{
		QString dirname = this->dirname(path);
		/*
		 * Parent directory of the extraction.
		 */
		const boost::filesystem::path parentDirPath = boost::filesystem::path(target.toUtf8().constData()) / dirname.toUtf8().constData();
		QDir parentDir = QDir(parentDirPath.c_str());

		qDebug() << "Making dir" << parentDir;

		if ((parentDir.exists() && KMessageBox::questionYesNo(this, i18n("Overwrite existing file %1?", parentDir.path())) == KMessageBox::Yes) || !parentDir.exists())
		{
			/*
			 * Existing directory has to be removed first.
			 */
			if (((parentDir.exists() && removeDirRecursively(parentDir.path())) || !parentDir.exists()) && outputDir.mkdir(dirname))
			{
				bool result = true;

				foreach (mpq::Listfile::Entries::const_reference ref, dirEntries)
				{
					string relativeFilePath = ref;
					mpq::Listfile::toNativePath(relativeFilePath);
					// cut directory path to get relative path.
					relativeFilePath = relativeFilePath.substr(path.size());

					boost::filesystem::path fileTarget = target.toUtf8().constData();
					fileTarget /= dirname.toUtf8().constData();
					fileTarget /= relativeFilePath;

					boost::filesystem::path relativeDirPath = relativeFilePath;
					relativeDirPath.remove_filename();

					if (parentDir.mkpath(relativeDirPath.c_str()))
					{
						qDebug() << "relative file path:" << relativeFilePath.c_str();

						qDebug() << "Extract entry:" << ref.c_str();
						qDebug() << "Extract target:" << fileTarget.c_str();

						if (!extractFile(ref.c_str(), archive, fileTarget.c_str()))
						{
							result = false;

							break;
						}
					}
					else
					{
						qDebug() << "Error on extracting directory:" << relativeDirPath.c_str();

						result = false;

						break;
					}
				}

				return result;
			}
			else
			{
				KMessageBox::error(this, i18n("Error on creating directory %1.", dirname));
			}
		}
	}
	else
	{
		KMessageBox::error(this, i18n("Directory %1 does not exist.", outputDir.absolutePath()));
	}

	return false;
}

bool MpqEditor::extractFile(const QString &path, mpq::Archive &archive, const QString& target)
{
	mpq::File file = archive.findFile(path.toStdString());

	if (file.isValid())
	{
		QFile outputFile(target);

		if (outputFile.isWritable() || !outputFile.exists())
		{
			if ((outputFile.exists() && KMessageBox::questionYesNo(this, i18n("Overwrite existing file %1?", outputFile.fileName())) == KMessageBox::Yes)
				|| !outputFile.exists())
			{
				ofstream out(target.toStdString(), std::ios::out | std::ios::binary);

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

	m_closeAction = new KAction(KIcon(":/actions/closearchives.png"), i18n("Close archives"), this);
	connect(m_closeAction, SIGNAL(triggered()), this, SLOT(closeMpqArchives()));
	m_fileActions->addAction("closearchives", m_closeAction);

	m_closeAllAction = new KAction(KIcon(":/actions/closearchives.png"), i18n("Close all archives"), this);
	connect(m_closeAllAction, SIGNAL(triggered()), this, SLOT(closeAllMpqArchives()));
	m_fileActions->addAction("closearchives", m_closeAllAction);

	m_fileActions->associateWidget(menu);
}

void MpqEditor::createEditActions(KMenu *menu)
{
	m_extractAction = new KAction(KIcon(":/actions/extract.png"), i18n("Extract files"), this);
	connect(m_extractAction, SIGNAL(triggered()), this, SLOT(extractFiles()));
	menu->addAction(m_extractAction);

	m_infoAction = new KAction(KIcon(":/actions/showfileinfo.png"), i18n("File info"), this);
	connect(m_infoAction, SIGNAL(triggered()), this, SLOT(showFileInfo()));
	menu->addAction(m_infoAction);
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
	// only read GUI settings
	// sources have to be read manually before
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
