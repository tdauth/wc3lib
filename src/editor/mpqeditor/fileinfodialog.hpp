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

#ifndef WC3LIB_EDITOR_MPQEDITOR_FILEINFODIALOG_HPP
#define WC3LIB_EDITOR_MPQEDITOR_FILEINFODIALOG_HPP

#include <QDialog>

#include "../../mpq/file.hpp"
#include "../../mpq/archive.hpp"

#include "ui_fileinfodialog.h"

namespace wc3lib
{

namespace editor
{

/**
 * \brief Information dialog for one single file entry of an MPQ archive.
 *
 * It only lists all available internal information of the file.
 * The information should be copyable for the user.
 *
 * \note For directories this lists only the name since directories are no entries in archives. They are determined by the listfile entries only.
 *
 * \sa ArchiveInfoDialog
 */
class FileInfoDialog : public QDialog, public Ui::FileInfoDialog
{
	public:
		explicit FileInfoDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

		/**
		 * Fills the dialog labels with information about \p file.
		 *
		 * \param file The file about which the information is displayed.
		 *
		 * \todo The parameter file should be const. Do not use sectors() to calculate the number of sectors. Add another member function!
		 */
		void fill(mpq::Archive &archive, mpq::File &file);
		/**
		 * Fills the dialog labels with information about the directory with path \p dirPath.
		 */
		void fill(const QString &dirPath);

		static QString localeToString(mpq::File::Locale locale);
		static QString platformToString(mpq::File::Platform platform);
};

}

}

#endif // WC3LIB_EDITOR_MPQEDITOR_FILEINFODIALOG_HPP
