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

#include "archiveinfodialog.hpp"

namespace wc3lib
{

namespace editor
{

ArchiveInfoDialog::ArchiveInfoDialog(QWidget *parent, Qt::WindowFlags f): QDialog(parent, f)
{
	setupUi(this);
}

void ArchiveInfoDialog::fill(mpq::Archive &archive)
{
	this->m_filePathLabel->setText(QString::fromStdString(archive.path().string()));

	this->m_formatLabel->setText(formatToString(archive.format()));
	this->m_hasListfileLabel->setText(boolToString(archive.containsListfileFile()));
	this->m_hasAttributesLabel->setText(boolToString(archive.containsAttributesFile()));
	this->m_hasSignatureLabel->setText(boolToString(archive.containsSignatureFile()));
	this->m_hasStrongSignatureLabel->setText(boolToString(archive.hasStrongDigitalSignature()));

	this->m_sizeLabel->setText(QString::fromStdString(sizeStringDecimal(archive.size())));
	this->m_offsetLabel->setText(QString::number(archive.startPosition()));
	this->m_sectorSizeLabel->setText(QString::fromStdString(sizeStringDecimal(archive.sectorSize())));
	this->m_blocksLabel->setText(QString::number(archive.blocks().size()));
	// TODO count hashes (multimap!)
	this->m_hashesLabel->setText(QString::number(archive.hashes().size()));
}

QString ArchiveInfoDialog::formatToString(mpq::Archive::Format format)
{
	switch (format)
	{
		case mpq::Archive::Format::Mpq1:
		{
			return tr("MPQ 1");
		}

		case mpq::Archive::Format::Mpq2:
		{
			return tr("MPQ 2");
		}
	}

	return QString();
}

QString ArchiveInfoDialog::boolToString(bool value)
{
	if (value)
	{
		return tr("Yes");
	}

	return tr("No");
}

}

}
