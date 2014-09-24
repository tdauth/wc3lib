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

#include <boost/cast.hpp>

#include <QtGui>

#include <KFileDialog>

#include "listfilesdialog.hpp"
#include "platform.hpp"


namespace wc3lib
{

namespace editor
{

ListfilesDialog *ListfilesDialog::m_dialog = 0;

ListfilesDialog::ListfilesDialog(QWidget* parent, Qt::WindowFlags flags) : QDialog(parent, flags), m_fileListLayout(0)
{
	this->setWindowTitle(tr("Select Listfiles"));

	QVBoxLayout *layout = new QVBoxLayout(this);
	this->setLayout(layout);
	this->fill();
}

int ListfilesDialog::show(mpq::Listfile::Entries& entries, QWidget* parent)
{
	if (m_dialog == 0)
	{
		m_dialog = new ListfilesDialog(parent);
	}

	m_dialog->setParent(parent);
	int result = m_dialog->exec();

	if (result == Accepted)
	{
		entries = m_dialog->checkedEntries();
	}

	return result;
}

void ListfilesDialog::fill()
{
	QVBoxLayout *layout = boost::polymorphic_cast<QVBoxLayout*>(this->layout());
	m_fileListLayout = new QVBoxLayout(this);
	layout->addLayout(m_fileListLayout);
	QFileInfoList listfiles = installedListfiles();

	foreach (QFileInfo fileInfo, listfiles)
	{
		this->m_files.append(fileInfo);

		QCheckBox *checkBox = new QCheckBox(fileInfo.fileName(), this);
		m_fileListLayout->addWidget(checkBox);

		this->m_checkBoxes.append(checkBox);
	}

	QCheckBox *all = new QCheckBox(tr("Check All"), this);
	all->setChecked(false);
	connect(all, SIGNAL(clicked(bool)), this, SLOT(checkAll(bool)));
	layout->addWidget(all, Qt::AlignBottom);

	QPushButton *button = new QPushButton(tr("Add list files"), this);
	connect(button, SIGNAL(clicked()), this, SLOT(addFiles()));
	layout->addWidget(button, Qt::AlignBottom);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(accept()));
	connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
	layout->addWidget(buttonBox, Qt::AlignBottom);
}

QFileInfoList ListfilesDialog::checkedFiles() const
{
	QFileInfoList result;

	for (int i = 0; i < this->files().size() && i < this->m_checkBoxes.size(); ++i)
	{
		if (this->m_checkBoxes[i]->isChecked())
		{
			result.push_back(this->files()[i]);
		}
	}

	return result;
}

mpq::Listfile::Entries ListfilesDialog::checkedEntries() const
{
	QFileInfoList checkedFiles = this->checkedFiles();
	mpq::Listfile::Entries result;

	foreach (QFileInfoList::const_reference ref, checkedFiles)
	{
		QFile file(ref.absoluteFilePath());

		qDebug() << "Absolute file path:" << ref.absoluteFilePath();

		if (file.open(QIODevice::ReadOnly))
		{
			qDebug() << "Open!";

			QTextStream in(&file);

			while (!in.atEnd())
			{
				result.push_back(in.readLine().toStdString());
			}
		}
	}

	return result;
}

void ListfilesDialog::addFiles()
{
	KUrl::List urls = KFileDialog::getOpenUrls(this->m_addUrl, "*.txt", this, tr("Add listfile"));

	foreach (KUrl::List::const_reference ref, urls)
	{
		this->addFile(QFileInfo(ref.toLocalFile()));
		this->m_addUrl = ref.directory();
	}
}

void ListfilesDialog::addFile(const QFileInfo &fileInfo)
{
	this->m_files.append(fileInfo);

	QCheckBox *checkBox = new QCheckBox(fileInfo.fileName(), this);
	m_fileListLayout->addWidget(checkBox);
	checkBox->setChecked(true);
	this->m_checkBoxes.append(checkBox);
}

void ListfilesDialog::checkAll(bool check)
{
	foreach (QCheckBox *checkBox, this->m_checkBoxes)
	{
		checkBox->setChecked(check);
	}
}

}

}

