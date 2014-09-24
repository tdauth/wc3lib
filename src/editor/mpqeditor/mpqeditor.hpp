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

#ifndef WC3LIB_EDITOR_MPQEDITOR_MPQEDITOR_HPP
#define WC3LIB_EDITOR_MPQEDITOR_MPQEDITOR_HPP

#include <kdemacros.h>
#include <KActionCollection>

#include "../module.hpp"
#include "../../mpq.hpp"
#include "ui_mpqeditor.h"
#include "../platform.hpp"
#include "archive.hpp"

namespace wc3lib
{

namespace editor
{

class ListfilesDialog;
class ArchiveInfoDialog;
class FileInfoDialog;

/**
 * \brief Simple file editor which can handle multiple MPQ archives.
 *
 * Moreover it allows you to easily open Warcraft III's default MPQ archives and all other MPQ entries from editor's MPQ priority list (\ref MpqPriorityList) plus all opened maps and campaigns in editor.
 *
 * \sa wc3lib::editor::MpqSlave wc3lib::editor::MpqPriorityList
 */
class KDE_EXPORT MpqEditor : public Module, protected Ui::MpqEditor
{
	Q_OBJECT

	public:
		/**
		 * All open MPQ archives are stored in a list.
		 */
		typedef boost::ptr_list<Archive> Archives;

		MpqEditor(wc3lib::editor::MpqPriorityList* source, QWidget* parent = 0, Qt::WindowFlags f = 0);
		virtual ~MpqEditor();

		/**
		 * \return Returns all open MPQ archives.
		 */
		const Archives& archives() const;

		/**
		 * \return Returns true if opening succeeded.
		 */
		bool openMpqArchive(const KUrl &url);

		ListfilesDialog* listfilesDialog() const;
		ArchiveInfoDialog* archiveInfoDialog() const;
		FileInfoDialog* fileInfoDialog() const;

	public slots:
		void newMpqArchive();
		void openMpqArchives();
		/**
		 * This slot can only be called by an action which is part of \ref m_archiveHistoryActions.
		 * Otherwise nothing happens.
		 */
		void openRecentArchive();
		void clearHistory();
		void saveMpqArchive();
		/**
		 * Closes only selected MPQ archives.
		 */
		void closeMpqArchives();
		void closeAllMpqArchives();
		void optimizeMpqArchives();

		void addFiles();
		/**
		 * Extracts all selected files (including directories) into a given target which is defined by user via file dialog.
		 */
		void extractFiles(); // selected files
		/**
		 * Removes all selected files (including directories) from the given MPQ archive if possible (archive may be opened in read-only mode).
		 */
		void removeFiles();
		/**
		 * Optimizes all selected files (including directories) which means that their compression and other flags will be defined trying to get the best compression results.
		 * \note If archive is open in read-only mode this can not be used on any of its files.
		 */
		void optimizeFiles();
		/**
		 * Creates a new directory with a user-defined name in the MPQ archive.
		 */
		void createDirectory();

		void openWar3();
		void openWar3X();
		void openWar3Patch();
		void openWar3XLocal();

		void showFileInfo();

	protected:
		virtual void createFileActions(KMenu *menu) override;
		virtual void createEditActions(KMenu *menu) override;
		virtual void createMenus(KMenuBar *menuBar) override;
		virtual void createWindowsActions(WindowsMenu *menu) override;
		virtual void createToolButtons(ModuleToolBar *toolBar) override;
		virtual SettingsInterface* settings() override;
		virtual void onSwitchToMap(Map *map) override;
		virtual QString actionName() const override;

		virtual void readSettings() override;
		virtual void writeSettings() override;

		/**
		 * For all tree widget items the corresponding archive must be stored.
		 */
		typedef QHash<QTreeWidgetItem*, Archive*> FileItems;

		/**
		 * \brief Constructs all tree items for one single archive.
		 *
		 * Constructs all tree widget items for \p entries which must be contained by \p archive.
		 * To ensure this you have to call \ref uniqueFiles() before.
		 * \param topItem The top level tree widgte item which is used for top level directory files in the archive as well as top level directories.
		 *
		 * \return Returns all created items.
		 */
		FileItems constructItems(const mpq::Listfile::Entries &entries, QTreeWidgetItem *topItem, Archive &archive);

		bool itemIsFolder(QTreeWidgetItem *item) const;

		QTreeWidgetItem* folderToItem(const QString &dirName, const QString &dirPath);
		/**
		 * Creates a tree widget item with all necessary column information for file \p path of \p archive.
		 *
		 * \return Returns 0 if the file is not part of the archive.
		 */
		QTreeWidgetItem* fileToItem(const boost::filesystem::path &path, Archive &archive);

		/**
		 * Extracts file of \p path from \p archive to local file \p target.
		 *
		 * \return Returns true if extraction succeeded.
		 */
		bool extractFile(const QString &path, mpq::Archive &archive, const QString &target);
		/**
		 * \return Returns all file path entries (without directories) from the files of dir item \p item recursively.
		 */
		mpq::Listfile::Entries dirEntries(QTreeWidgetItem *item) const;
		bool extractDir(const QString &path, mpq::Archive &archive, const QString &target, const mpq::Listfile::Entries &dirEntries);

		/**
		 * \return Returns the file name + extension of a listfile entry \p path.
		 */
		static QString fileName(const QString &path);
		/**
		 * \return Returns the base name (file name without extension) of a listfile entry \p path.
		 */
		static QString baseName(const QString &path);

		static QString dirname(const QString &path);

		/**
		 * Adds \p url to recent files history.
		 * If it does already exist it won't be added a second time.
		 */
		void addRecentAction(const KUrl &url);

		/**
		 * \return Returns all open MPQ archives.
		 */
		Archives& archives();

		/**
		 * Opens file \p filePath with the corresponding desktop application.
		 */
		void openFile(mpq::Archive &archive, const QString &filePath);

	private:
		Archives m_archives;
		FileItems m_archiveTopLevelItems;
		FileItems m_archiveFileItems;

		KUrl m_openUrl;
		KUrl m_saveUrl;
		KUrl m_addUrl;
		KUrl m_extractUrl;
		QList<KUrl> m_archiveHistory;

		KActionCollection *m_fileActions;
		KMenu *m_recentArchivesMenu;
		QAction *m_recentArchivesSeparator;
		KActionCollection *m_archiveHistoryActions;

		KAction *m_closeAction;
		KAction *m_closeAllAction;
		KAction *m_extractAction;
		KAction *m_infoAction;

		ListfilesDialog *m_listfilesDialog;
		ArchiveInfoDialog *m_archiveInfoDialog;
		FileInfoDialog *m_fileInfoDialog;

	private slots:
		void fileIsOpen(QTreeWidgetItem *item, int column);
		void contextMenu(QPoint point);
		void updateSelection();
		void expandItem(QTreeWidgetItem *item);
		void collapseItem(QTreeWidgetItem *item);
};

inline ListfilesDialog* MpqEditor::listfilesDialog() const
{
	return this->m_listfilesDialog;
}

inline ArchiveInfoDialog* MpqEditor::archiveInfoDialog() const
{
	return this->m_archiveInfoDialog;
}

inline FileInfoDialog* MpqEditor::fileInfoDialog() const
{
	return this->m_fileInfoDialog;
}

inline const MpqEditor::Archives& MpqEditor::archives() const
{
	return m_archives;
}

inline MpqEditor::Archives& MpqEditor::archives()
{
	return m_archives;
}

inline QString MpqEditor::actionName() const
{
	return "mpqeditor";
}

}

}

#endif
