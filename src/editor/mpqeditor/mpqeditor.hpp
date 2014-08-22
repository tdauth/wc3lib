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

#ifndef WC3LIB_EDITOR_MPQEDITOR_HPP
#define WC3LIB_EDITOR_MPQEDITOR_HPP

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

		const List& mpqArchives() const;
		const List& selection() const;

		void openMpqArchive(const KUrl &url);

	public slots:
		void newMpqArchive();
		void openMpqArchives();
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

		QTreeWidgetItem* constructFile(const QString &path, QTreeWidgetItem *topItem);
		QSet<QString> uniqueDirs(const mpq::Listfile::Entries &entries) const;

		List& mpqArchives();
		List& selection();

		List m_mpqArchives;
		mpq::Listfile::Entries m_listfileEntries;
		List m_selection;

		KUrl m_openStartUrl;
		KUrl m_saveStartUrl;
		KUrl m_addStartUrl;
		KUrl m_extractStartUrl;

		KActionCollection *m_fileActions;
};

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
