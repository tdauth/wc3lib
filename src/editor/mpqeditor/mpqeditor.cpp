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
#include "mpqtreeitem.hpp"

namespace wc3lib
{

namespace editor
{

const int MpqEditor::maxRecentActions = 5;

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
	//MpqTreeProxyModel *proxyModel = new MpqTreeProxyModel(this);
	MpqTreeModel *treeModel = new MpqTreeModel(this->source(), this);
	MpqTreeProxyModel *proxyModel = new MpqTreeProxyModel(this);
	proxyModel->setSourceModel(treeModel);
	m_archivesTreeView->setModel(proxyModel);
	this->m_filterProxySearchLine->setProxy(proxyModel);
	this->m_archivesTreeView->header()->setSortIndicator(0, Qt::AscendingOrder);

	setWindowTitle(tr("MPQ Editor"));

	connect(this->m_archivesTreeView, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
	connect(this->m_archivesTreeView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickItem(const QModelIndex &)));
	connect(this->m_archivesTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
	connect(this->m_archivesTreeView, SIGNAL(expanded(const QModelIndex&)), this, SLOT(expandItem(QTreeWidgetItem*)));
	connect(this->m_archivesTreeView, SIGNAL(collapsed(const QModelIndex&)), this, SLOT(collapseItem(QTreeWidgetItem*)));
	connect(this->m_archivesTreeView->header(), SIGNAL(sectionClicked(int)), this, SLOT(orderBySection(int)));

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
	const bool hasSelection = this->m_archivesTreeView->selectionModel()->hasSelection();

	m_extractAction->setEnabled(hasSelection);
	m_infoAction->setEnabled(hasSelection);
}

void MpqEditor::expandItem(const QModelIndex &item)
{
	MpqTreeItem *treeItem = this->treeModel()->item(sortFilterModel()->mapToSource(item));

	if (treeItem->isFolder())
	{
		if (this->source()->sharedData()->worldEditData().get() != nullptr)
		{
			const QIcon folderIcon = this->source()->sharedData()->worldEditDataIcon("OMIcon_FolderOpen", "WorldEditArt", this);
			treeItem->setIcon(folderIcon);
		}
	}
}

void MpqEditor::collapseItem(const QModelIndex &item)
{
	MpqTreeItem *treeItem = this->treeModel()->item(sortFilterModel()->mapToSource(item));

	if (treeItem->isFolder())
	{
		if (this->source()->sharedData()->worldEditData().get() != nullptr)
		{
			const QIcon folderIcon = this->source()->sharedData()->worldEditDataIcon("OMIcon_Folder", "WorldEditArt", this);
			treeItem->setIcon(folderIcon);
		}
	}
}

void MpqEditor::orderBySection(int logicalIndex)
{
	qDebug() << "Order by section" << logicalIndex;
	sortFilterModel()->sort(logicalIndex,  m_archivesTreeView->header()->sortIndicatorOrder());
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

		/*
		 * Keep the file extension that it will be opened withe the correct program.
		 */
		const QFileInfo fileInfo(filePath);

		if (!fileInfo.suffix().isEmpty())
		{
			tempFile.setFileTemplate("XXXXXX." + fileInfo.suffix());
		}

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

void MpqEditor::doubleClickItem(const QModelIndex &index)
{
	MpqTreeItem *item = this->treeModel()->item(sortFilterModel()->mapToSource(index));

	if (item->isFile())
	{
		openFile(*item->archive(), item->filePath());
		//this->m_fileInfoDialog->fill(archive, file);
		//this->m_fileInfoDialog->show();
	}
	else if (item->isFolder())
	{
		qDebug() << "dir";
		this->m_fileInfoDialog->fill(item->filePath());
		this->m_fileInfoDialog->show();
	}
	else if (item->isArchive())
	{
		this->m_archiveInfoDialog->fill(*item->archive());
		this->m_archiveInfoDialog->show();
	}
}

void MpqEditor::showFileInfo()
{
	foreach (QModelIndex index, this->m_archivesTreeView->selectionModel()->selectedIndexes())
	{
		MpqTreeItem *item = this->treeModel()->item(sortFilterModel()->mapToSource(index));

		if (item->isFile())
		{
			mpq::File file = item->archive()->findFile(item->filePath().toUtf8().constData());

			if (file.isValid())
			{
				this->m_fileInfoDialog->fill(*item->archive(), file);
				this->m_fileInfoDialog->show();
			}
		}
		else if (item->isFolder())
		{
			qDebug() << "dir";
			this->m_fileInfoDialog->fill(item->filePath());
			this->m_fileInfoDialog->show();
		}
		else if (item->isArchive())
		{
			this->m_archiveInfoDialog->fill(*item->archive());
			this->m_archiveInfoDialog->show();
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

	contextMenu->popup(m_archivesTreeView->viewport()->mapToGlobal(point));
}

void MpqEditor::addRecentAction(const KUrl &url)
{
	/*
	 * Do not add entries twice in a history.
	 */
	m_archiveHistory.removeAll(url);

	/*
	 * Clear recent actions.
	 */
	foreach (QAction *action, m_archiveHistoryActions->actions())
	{
		m_archiveHistoryActions->removeAction(action);
		m_recentArchivesMenu->removeAction(action);

		delete action;
	}

	/*
	 * Add new recent URL.
	 */
	m_archiveHistory.prepend(url);


	while (m_archiveHistory.size() > maxRecentActions)
	{
		m_archiveHistory.removeLast();
	}

	/*
	 * The action which comes before is below the current action.
	 * Since the recent action always has to be added on top either use the separator if there is no other recent URLs or use the first one.
	 */
	QAction *before = m_recentArchivesSeparator;

	for (int i = m_archiveHistory.size(); i > 0; --i)
	{
		const int index = i - 1;
		KAction *action = new KAction(m_archiveHistory[index].toString(), this);
		action->setData(m_archiveHistory[index].toEncoded());
		connect(action, SIGNAL(triggered()), this, SLOT(openRecentArchive()));
		m_archiveHistoryActions->addAction(action);
		m_recentArchivesMenu->insertAction(before, action);

		before = action;
	}
}

bool MpqEditor::openMpqArchive(const KUrl &url)
{
	// Do not open an archive twice.
	Archives::iterator iterator = this->archives().find(url);

	if (iterator != this->archives().end())
	{
		MpqTreeItem *item = this->treeModel()->topLevelItem(iterator->second);

		if (item != 0)
		{
			this->m_archivesTreeView->setCurrentIndex(this->sortFilterModel()->mapFromSource(item->index(this->treeModel())));
		}
		else
		{
			qDebug() << "Missing archive item" << url;
		}

		return true;
	}

	if (listfilesDialog()->exec() == QDialog::Accepted)
	{
		mpq::Listfile::Entries listfileEntries = listfilesDialog()->checkedEntries();

		qDebug() << "Listfile entries: " << listfileEntries.size();

		std::auto_ptr<mpq::Archive> archive(new mpq::Archive());

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
		else if (listfileEntries.empty())
		{
			QMessageBox::warning(this, tr("No (listfile) file"), tr("Archive does not contain any (listfile) file and you selected none. Therefore no files will be listed."));
		}

		/*
		 * Make sure extra files are listed if available.
		 */
		listfileEntries.push_back("(listfile)");
		listfileEntries.push_back("(attributes");
		listfileEntries.push_back("(signature");


		qDebug() << "Full size:" << listfileEntries.size();

		const mpq::Listfile::Entries files = mpq::Listfile::existingEntries(listfileEntries, *archive);

		qDebug() << "Filtered size:" << files.size();

		this->treeModel()->addArchive(archive.get(), files);
		m_archives.insert(url, archive);
		addRecentAction(url);

		// call after inserting data for better performance
		this->m_archivesTreeView->setSortingEnabled(true);

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
	QAction *action = boost::polymorphic_cast<QAction*>(QObject::sender());
	openMpqArchive(KUrl::fromEncoded(action->data().toByteArray()));
}

void MpqEditor::clearHistory()
{
	foreach (QAction *action, this->m_archiveHistoryActions->actions())
	{
		this->m_archiveHistoryActions->removeAction(action);
		this->m_recentArchivesMenu->removeAction(action);

		delete action;
	}

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
	QItemSelectionModel *selection = this->m_archivesTreeView->selectionModel();

	foreach (QModelIndex index, selection->selectedIndexes())
	{
		MpqTreeItem *item = this->treeModel()->item(this->sortFilterModel()->mapToSource(index));
		mpq::Archive *archive = item->archive();
		Archives::iterator iterator = std::find_if(this->m_archives.begin(), this->m_archives.end(), [&archive](Archives::reference value)->bool { return value.second == archive; });

		if (iterator != this->m_archives.end())
		{
			this->treeModel()->removeArchive(archive);
			this->m_archives.erase(iterator);
		}
	}
}

void MpqEditor::closeAllMpqArchives()
{
	for (Archives::iterator iterator = this->m_archives.begin(); iterator != this->m_archives.end(); iterator = this->m_archives.erase(iterator))
	{
		this->treeModel()->removeArchive(iterator->second);
	}

	this->m_archives.clear();
}

void MpqEditor::optimizeMpqArchives()
{
}

void MpqEditor::addFiles()
{
}

void MpqEditor::extractFiles()
{
	const QModelIndexList items = this->m_archivesTreeView->selectionModel()->selectedIndexes();

	if (items.size() == 1)
	{
		MpqTreeItem *item = this->treeModel()->item(sortFilterModel()->mapToSource(items.first()));

		if (item != 0)
		{
			const QString filePath = item->filePath();
			KUrl url;

			if (item->isFolder())
			{
				url = KFileDialog::getExistingDirectoryUrl(m_extractUrl, this);
			}
			else
			{
				m_extractUrl.setFileName(this->fileName(filePath));
				url = KFileDialog::getSaveUrl(m_extractUrl, "*", this);
			}

			if (!url.isEmpty())
			{
				mpq::Archive *archive = item->archive();
				MpqTreeItem *archiveItem = this->treeModel()->topLevelItem(archive);

				if (archiveItem != 0)
				{
					bool result = false;

					if (item->isFolder())
					{
						// TODO valid dir entries, use entries of archvie
						const QString prefix = item->filePath() + '\\';
						const mpq::Listfile::Entries fileEntries = mpq::Listfile::caseSensitiveFileEntries(archiveItem->entries(), prefix.toUtf8().constData(), true);
						result = extractDir(filePath, *archive, url.toLocalFile(), fileEntries);
					}
					else
					{
						result = extractFile(filePath, *archive, url.toLocalFile());
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
					KMessageBox::error(this, i18n("Missing archive item."));
				}
			}
		}
	}
	else if (items.size() > 1)
	{
		const QString dir = KFileDialog::getExistingDirectory(m_extractUrl, this);

		if (!dir.isEmpty())
		{
			bool success = true;

			foreach (QModelIndex index, items)
			{
				MpqTreeItem *item = this->treeModel()->item(sortFilterModel()->mapToSource(index));

				/*
				 * Is the item of an open MPQ archive.
				 */
				if (item->isArchive())
				{
					qDebug() << "Support extracting archives!";
				}
				/*
				 * Is a file or directory item.
				 */
				else if (item->isFile())
				{
					const QString filePath = item->filePath();
					const QString fileName = this->fileName(filePath);

					mpq::Archive *archive = item->archive();
					KUrl url;
					url.setDirectory(dir);
					url.setFileName(fileName);

					extractFile(filePath, *archive, url.toLocalFile());
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

bool MpqEditor::extractDir(const QString &path, mpq::Archive &archive, const QString &target, const mpq::Listfile::Entries &dirEntries)
{

	QDir outputDir(target);

	if (outputDir.exists())
	{
		const QString dirname = this->baseName(path);
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

					qDebug() << "relative file path 1:" << relativeFilePath.c_str();

					// cut directory path to get relative path.
					relativeFilePath = relativeFilePath.substr(path.size());

					qDebug() << "relative file path 2:" << relativeFilePath.c_str();

					boost::filesystem::path fileTarget = target.toUtf8().constData();
					fileTarget /= dirname.toUtf8().constData();
					fileTarget /= relativeFilePath;

					boost::filesystem::path relativeDirPath = relativeFilePath;
					relativeDirPath.remove_filename();

					qDebug() << "relative file path:" << relativeFilePath.c_str();

					if (parentDir.mkpath(relativeDirPath.c_str()))
					{
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

bool MpqEditor::extractFile(const QString &path, mpq::Archive &archive, const QString &target)
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
	m_archiveHistoryActions = new QActionGroup(this);

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

		if (url.isValid())
		{
			qDebug() << "Url " << i << ":" << url;
			addRecentAction(url);
		}
		else
		{
			qDebug() << "Invalid URL" << i << url;
		}
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
