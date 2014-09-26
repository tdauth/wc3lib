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

#include "fileinfodialog.hpp"
#include "../../mpq.hpp"

namespace wc3lib
{

namespace editor
{

FileInfoDialog::FileInfoDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	setupUi(this);
}

void FileInfoDialog::fill(mpq::Archive &archive, mpq::File &file)
{
	this->m_filePathLabel->setText(file.path().c_str());

	/*
	 * Hash data:
	 */
	this->m_fileHashALabel->setText(QString::number(file.hash()->cHashData().filePathHashA()));
	this->m_fileHashBLabel->setText(QString::number(file.hash()->cHashData().filePathHashB()));
	this->m_localeLabel->setText(localeToString(file.locale()));
	this->m_platformLabel->setText(platformToString(file.platform()));
	this->m_hashValueLabel->setText(QString::number(mpq::hash_value(file.hash()->cHashData())));

	/*
	 * Read extended attributes from file if available.
	 */
	if (archive.containsAttributesFile())
	{
		mpq::Attributes attributes = archive.attributesFile();

		if (attributes.isValid())
		{
			int32 version = 0;
			mpq::Attributes::ExtendedAttributes extendedAttributes = mpq::Attributes::ExtendedAttributes::None;
			mpq::Attributes::Crc32s crcs;
			mpq::Attributes::Md5s md5s;
			mpq::Attributes::FileTimes fileTimes;
			attributes.attributes(version, extendedAttributes,crcs, fileTimes, md5s);

			if ((extendedAttributes & mpq::Attributes::ExtendedAttributes::FileCrc32s) && crcs.size() > file.block()->index())
			{
				this->m_crc32Label->setEnabled(true);
				this->m_crc32Label->setText(QString::number(crcs[file.block()->index()]));
			}
			else
			{
				this->m_crc32Label->setEnabled(false);
				this->m_crc32Label->setText(tr("-"));
			}

			if ((extendedAttributes & mpq::Attributes::ExtendedAttributes::FileMd5s) && md5s.size() > file.block()->index())
			{
				this->m_md5Label->setEnabled(true);
				const mpq::MD5Checksum md5 = md5s[file.block()->index()];

				this->m_md5Label->setText(QString::fromUtf8(md5.toString().c_str()));
			}
			else
			{
				this->m_md5Label->setEnabled(false);
				this->m_md5Label->setText(tr("-"));
			}

			if ((extendedAttributes & mpq::Attributes::ExtendedAttributes::FileTimeStamps) && fileTimes.size() > file.block()->index())
			{
				this->m_fileTimeLabel->setEnabled(true);

				time_t time = 0;
				fileTimes[file.block()->index()].toTime(time);

				QDateTime dateTime = QDateTime::fromTime_t(time);
				this->m_fileTimeLabel->setText(dateTime.toString());
			}
			else
			{
				this->m_fileTimeLabel->setEnabled(false);
				this->m_fileTimeLabel->setText(tr("-"));
			}
		}
		else
		{
			this->m_extendedAttributesGroupBox->setEnabled(false);
		}
	}
	else
	{
		this->m_extendedAttributesGroupBox->setEnabled(false);
		this->m_crc32Label->setText(tr("-"));
		this->m_md5Label->setText(tr("-"));
		this->m_fileTimeLabel->setText(tr("-"));
	}

	/*
	 * Block data:
	 */
	this->m_compressedSizeLabel->setText(sizeStringDecimal(file.compressedSize()).c_str());
	this->m_uncompressedSizeLabel->setText(sizeStringDecimal(file.size()).c_str());
	const double compressionRatio = file.size() / file.compressedSize();
	this->m_compressionRatioLabel->setText(QString(tr("%1 %").arg(QString::number(compressionRatio))));
	this->m_flagsLabel->setText(mpq::flagsString(file.block()->flags()).c_str());
	this->m_fileKeyLabel->setText(QString::number(file.fileKey()));
	this->m_offsetLabel->setText(QString::number(file.block()->blockOffset()));
	this->m_extendedOffsetLabel->setText(QString::number(file.block()->extendedBlockOffset()));
	// TODO number of sectors should be calculated without reading the sectors!
	mpq::File::Sectors sectors;
	file.sectors(sectors);
	this->m_sectorsLabel->setText(QString::number(sectors.size()));
}

void FileInfoDialog::fill(const QString &dirPath)
{
	this->m_filePathLabel->setText(dirPath);
	/*
	 * Disable other groups completely.
	 */
}

QString FileInfoDialog::localeToString(mpq::File::Locale locale)
{
	switch (locale)
	{
		case mpq::File::Locale::Neutral:
		{
			return tr("Neutral");
		}
	}

	return QString();
}

QString FileInfoDialog::platformToString(mpq::File::Platform platform)
{
	switch (platform)
	{
		case mpq::File::Platform::Default:
		{
			return tr("Default");
		}
	}

	return QString();
}

}

}
