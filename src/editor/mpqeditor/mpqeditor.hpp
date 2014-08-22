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

namespace wc3lib
{

namespace editor
{

class ListfilesDialog;
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
		typedef boost::shared_ptr<mpq::Archive> MpqPtr;
		typedef std::list<MpqPtr> List;

		MpqEditor(wc3lib::editor::MpqPriorityList* source, QWidget* parent = 0, Qt::WindowFlags f = 0);
		virtual ~MpqEditor();

		const List& mpqArchives() const;
		const List& selection() const;

		/**
		 * \return Returns true if opening succeeded.
		 */
		bool openMpqArchive(const KUrl &url);

		ListfilesDialog* listfilesDialog() const;

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
		void closeMpqArchives();
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
		typedef QHash<QTreeWidgetItem*, mpq::Archive*> FileItems;

		/**
		 * \return Returns only entries which are contained by \p archive.
		 */
		mpq::Listfile::Entries uniqueFiles(const mpq::Listfile::Entries &entries, mpq::Archive &archive) const;

		/**
		 * \brief Constructs all tree items for one single archive.
		 *
		 * Constructs all tree widget items for \p entries which must be contained by \p archive.
		 * To ensure this you have to call \ref uniqueFiles() before.
		 * \param topItem The top level tree widgte item which is used for top level directory files in the archive as well as top level directories.
		 *
		 * \return Returns all created items.
		 */
		FileItems constructItems(const mpq::Listfile::Entries &entries, QTreeWidgetItem *topItem, mpq::Archive &archive);

		/**
		 * Creates a tree widget item with all necessary column information for file \p path of \p archive.
		 *
		 * \return Returns 0 if the file is not part of the archive.
		 */
		QTreeWidgetItem* fileToItem(const boost::filesystem::path &path, mpq::Archive &archive);

		bool extractFile(const QString &path, mpq::Archive &archive, const QString &target);

		QString fileName(const QString &path);
		QString baseName(const QString &path);

		void addRecentAction(const KUrl &url);

		List& mpqArchives();
		List& selection();

		List m_mpqArchives;
		FileItems m_archiveFileItems;
		List m_selection;

		KUrl m_openUrl;
		KUrl m_saveUrl;
		KUrl m_addUrl;
		KUrl m_extractUrl;
		QList<KUrl> m_archiveHistory;

		KActionCollection *m_fileActions;
		KMenu *m_recentArchivesMenu;
		QAction *m_recentArchivesSeparator;
		KActionCollection *m_archiveHistoryActions;

		KAction *m_extractAction;

		ListfilesDialog *m_listfilesDialog;
		FileInfoDialog *m_fileInfoDialog;

	private slots:
		void fileIsOpen(QTreeWidgetItem *item, int column);
		void contextMenu(QPoint point);
		void updateSelection();
};

inline ListfilesDialog* MpqEditor::listfilesDialog() const
{
	return this->m_listfilesDialog;
}

inline const MpqEditor::List& MpqEditor::mpqArchives() const
{
	return m_mpqArchives;
}

inline const MpqEditor::List& MpqEditor::selection() const
{
	return m_selection;
}

inline MpqEditor::List& MpqEditor::mpqArchives()
{
	return m_mpqArchives;
}

inline MpqEditor::List& MpqEditor::selection()
{
	return m_selection;
}

inline QString MpqEditor::actionName() const
{
	return "mpqeditor";
}

}

}

#endif
