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

#ifndef WC3LIB_EDITOR_MPQEDITOR_ARCHIVEINFODIALOG_HPP
#define WC3LIB_EDITOR_MPQEDITOR_ARCHIVEINFODIALOG_HPP

#include <QDialog>

#include "../../mpq.hpp"
#include "ui_archiveinfodialog.h"

namespace wc3lib
{

namespace editor
{

/**
 * \brief Information dialog for MPQ archives.
 *
 * \sa FileInfoDialog
 */
class ArchiveInfoDialog : public QDialog, protected Ui::ArchiveInfoDialog
{
	public:
		explicit ArchiveInfoDialog(QWidget* parent = nullptr);

		/**
		 * Fills the dialog labels with information about \p archive.
		 */
		void fill(mpq::Archive &archive);

		static QString formatToString(mpq::Archive::Format format);
		static QString boolToString(bool value);
};

}

}

#endif // WC3LIB_EDITOR_MPQEDITOR_ARCHIVEINFODIALOG_HPP
