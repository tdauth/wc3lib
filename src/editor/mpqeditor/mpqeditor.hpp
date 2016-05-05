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

#include <boost/ptr_container/ptr_map.hpp>

#include <kdemacros.h>
#include <KUrl>

#include "../module.hpp"
#include "../../mpq.hpp"
#include "ui_mpqeditor.h"
#include "../platform.hpp"
#include "mpqtreemodel.hpp"
#include "mpqtreeproxymodel.hpp"

namespace wc3lib
{

namespace editor
{

class CreationDialog;
class ListfilesDialog;
class ArchiveInfoDialog;
class FileInfoDialog;

/**
 * \page mpqeditorsection MPQ Editor
 * The MPQ Editor provides an interface for maintaining multiple MPQ archives at the same time.
 * It allows the user to open multiple archives, to search for and export files, directories or archives and to get all necessary information.
 */

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
		 * The maximum number of recent file URL entries stored in the file menu.
		 */
		static const int maxRecentActions;

		/**
		 * All open MPQ archives are stored in a vector.
		 */
		typedef boost::ptr_map<KUrl, mpq::Archive> Archives;

		MpqEditor(wc3lib::editor::MpqPriorityList* source, QWidget* parent = 0, Qt::WindowFlags f = 0);
		virtual ~MpqEditor();

		virtual bool configure() override;
		virtual void retranslateUi() override;

		/**
		 * The MPQ archives which are currently open in the MPQ editor.
		 *
		 * \return Returns the open MPQ archives.
		 * @{
		 */
		Archives& archives();
		const Archives& archives() const;
		/**
		 * @}
		 */

		/**
		 * \return Returns the MPQ archive of the first selected item.
		 */
		mpq::Archive* selectedArchive() const;
		/**
		 * \return Returns the directory path of the first selected item.
		 */
		QString currentDirectory() const;

		/**
		 * Opens an MPQ archive and lists its files in the MPQ editor.
		 * \param url The URL of the MPQ archive.
		 * \return Returns true if opening succeeded.
		 */
		bool openMpqArchive(const KUrl &url);

		CreationDialog* creationDialog() const;
		/**
		 * The dialog for selecting predefined (listfile) files.
		 * If the MPQ archive does not contain any (lisfile) file this will help to list all contained files.
		 * \return Returns the selection dialog for predefined (listfile) files.
		 */
		ListfilesDialog* listfilesDialog() const;
		ArchiveInfoDialog* archiveInfoDialog() const;
		FileInfoDialog* fileInfoDialog() const;

		/**
		 * \return Returns the file name + extension of a listfile entry \p path.
		 */
		static QString fileName(const QString &path);
		/**
		 * \return Returns the base name (file name without extension) of a listfile entry \p path.
		 */
		static QString baseName(const QString &path);

		static QString dirname(const QString &path);

	public slots:
		/**
		 * Opens a dialog for selecting options to create a new MPQ archive.
		 */
		void newMpqArchive();
		/**
		 * Opens one or several selected files as MPQ archives and shows them in the MPQ Editor.
		 */
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

		/**
		 * Does some defragmentation of all blocks etc.
		 * TODO Implement
		 */
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
		virtual void createFileActions(QMenu *menu) override;
		virtual void createEditActions(QMenu *menu) override;
		virtual void createMenus(QMenuBar *menuBar) override;
		virtual void createWindowsActions(WindowsMenu *menu) override;
		virtual void createToolButtons(ModuleToolBar *toolBar) override;
		virtual SettingsInterface* settings() override;
		virtual void onSwitchToMap(Map *map) override;
		virtual QString actionName() const override;
		virtual QIcon icon() override;

		virtual void readSettings() override;
		virtual void writeSettings() override;


		/**
		 * Extracts file of \p path from \p archive to local file \p target.
		 *
		 * \return Returns true if extraction succeeded.
		 */
		bool extractFile(const QString &path, mpq::Archive &archive, const QString &target);
		bool extractDir(const QString &path, mpq::Archive &archive, const QString &target, const mpq::Listfile::Entries &dirEntries);

		/**
		 * Adds \p url to recent files history.
		 * If it does already exist it won't be added a second time.
		 */
		void addRecentAction(const KUrl &url);

		/**
		 * Opens file \p filePath with the corresponding desktop application.
		 */
		void openFile(mpq::Archive &archive, const QString &filePath);

		MpqTreeProxyModel* sortFilterModel() const;
		MpqTreeModel* treeModel() const;

	private:
		void openArchive(mpq::Archive &archive, const KUrl &url, mpq::Listfile::Entries &listfileEntries);

		Archives m_archives;

		KUrl m_openUrl;
		KUrl m_saveUrl;
		KUrl m_addUrl;
		KUrl m_extractUrl;
		QList<QUrl> m_archiveHistory;

		QMenu *m_recentArchivesMenu;
		QAction *m_recentArchivesSeparator;
		QActionGroup *m_archiveHistoryActions;

		QAction *m_closeAction;
		QAction *m_closeAllAction;
		QAction *m_addAction;
		QAction *m_extractAction;
		QAction *m_infoAction;

		CreationDialog *m_creationDialog;
		ListfilesDialog *m_listfilesDialog;
		ArchiveInfoDialog *m_archiveInfoDialog;
		FileInfoDialog *m_fileInfoDialog;

	private slots:
		void doubleClickItem(const QModelIndex &index);
		void contextMenu(QPoint point);
		void updateSelection(const QModelIndex &previous, const QModelIndex &next);
		void expandItem(const QModelIndex &item);
		void collapseItem(const QModelIndex &item);
		void orderBySection(int logicalIndex);
};

inline MpqTreeProxyModel* MpqEditor::sortFilterModel() const
{
	return boost::polymorphic_cast<MpqTreeProxyModel*>(this->m_archivesTreeView->model());
}

inline MpqTreeModel* MpqEditor::treeModel() const
{
	//return boost::polymorphic_cast<MpqTreeModel*>(boost::polymorphic_cast<MpqTreeProxyModel*>(this->m_archivesTreeView->model())->sourceModel());
	return boost::polymorphic_cast<MpqTreeModel*>(sortFilterModel()->sourceModel());
}

inline CreationDialog* MpqEditor::creationDialog() const
{
	return this->m_creationDialog;
}

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
