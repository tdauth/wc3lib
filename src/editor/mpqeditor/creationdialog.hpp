/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_MPQEDITOR_CREATIONDIALOG_HPP
#define WC3LIB_EDITOR_MPQEDITOR_CREATIONDIALOG_HPP

#include <QtGui/QDialog>

#include "../platform.hpp"
#include "../../mpq.hpp"

#include "ui_creationdialog.h"

namespace wc3lib
{

namespace editor
{

class CreationDialog : public QDialog, protected Ui::CreationDialog
{
	Q_OBJECT

	public:
		explicit CreationDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

		mpq::Archive::Format format() const;
		uint32 blockTableEntries() const;
		uint32 hashTableEntries() const;
		uint32 sectorSize() const;
		uint64 startOffset() const;
		bool createListfile() const;
};

inline mpq::Archive::Format CreationDialog::format() const
{
	return static_cast<mpq::Archive::Format>(m_formatComboBox->currentIndex());
}

inline uint32 CreationDialog::blockTableEntries() const
{
	return this->m_blockEntriesSpinBox->value();
}

inline uint32 CreationDialog::hashTableEntries() const
{
	return this->m_hashEntriesSpinBox->value();
}

inline uint32 CreationDialog::sectorSize() const
{
	return this->m_sectorSizeSpinBox->value();
}

inline uint64 CreationDialog::startOffset() const
{
	return this->m_startOffsetSpinBox->value();
}

inline bool CreationDialog::createListfile() const
{
	return this->m_createListfileCheckBox->isChecked();
}

}

}

#endif // WC3LIB_EDITOR_MPQEDITOR_CREATIONDIALOG_HPP
