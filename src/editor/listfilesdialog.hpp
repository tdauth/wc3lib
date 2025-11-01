/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_LISTFILESDIALOG_HPP
#define WC3LIB_EDITOR_LISTFILESDIALOG_HPP

#include <QDialog>
#include <QDir>
#include <QList>
#include <QCheckBox>
#include <QUrl>

#include "../mpq/listfile.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief A simple dialog which allows you to select listfiles when openeing an MPQ archive.
 *
 * The default listfiles are determined by the wc3lib installation and are found in "/usr/share/wc3lib/listfiles/".
 * Additionally listfiles can be added.
 */
class ListfilesDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit ListfilesDialog(QWidget* parent = nullptr);

		/**
		 * \return Returns only files which are checked by the user.
		 */
		QFileInfoList checkedFiles() const;
		const QFileInfoList& files() const;

		/**
		 * \return Returns all (listfile) entries from all checked files only.
		 */
		mpq::Listfile::Entries checkedEntries() const;

		/**
		 * Sets the URL where the add files dialog starts.
		 */
		void setAddUrl(const QUrl &url);
		const QUrl& addUrl();

		/**
		 * Shows a Listfile dialog which blocks until it is accepted or rejected.
		 * \param entries The entries which are selected.
		 * \return Returns the result of the shown dialog.
		 */
		static int show(mpq::Listfile::Entries &entries, QWidget *parent = 0);

	public slots:
		/**
		 * Opens file dialog to select multiple URLs of files which should be added to the list
		 * of available listfiles.
		 */
		void addFiles();
		/**
		 * Adds a new file to the checkable list which can be used as listfile.
		 */
		void addFile(const QFileInfo &fileInfo);
		void checkAll(bool check);

	private:
		void fill();

		QLayout *m_fileListLayout;
		QFileInfoList m_files;
		QList<QCheckBox*> m_checkBoxes;
		QUrl m_addUrl;
		static ListfilesDialog *m_dialog;
};

inline const QFileInfoList& ListfilesDialog::files() const
{
	return this->m_files;
}

inline void ListfilesDialog::setAddUrl(const QUrl& url)
{
	this->m_addUrl = url;
}

inline const QUrl& ListfilesDialog::addUrl()
{
	return this->m_addUrl;
}

}

}

#endif // LISTFILESDIALOG_H
