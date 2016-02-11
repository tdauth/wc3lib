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

#include "resourcesdialog.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ResourcesDialog::ResourcesDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_sources(nullptr)
{
	setupUi(this);

	//connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(accept()));
	//connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
}

void ResourcesDialog::setSources(MpqPriorityList *sources)
{
	tableWidget->clear();
	tableWidget->setRowCount(sources->sources().size());

	int row = 0;

	for (MpqPriorityList::Sources::const_iterator iterator = sources->sources().begin(); iterator != sources->sources().end(); ++iterator)
	{
		QTableWidgetItem *item = new QTableWidgetItem(iterator->url().toLocalFile());
		tableWidget->setItem(row, 0, item);
		++row;
		//tableWidget->ad
		//tableWidget->
	}
}

#include "moc_resourcesdialog.cpp"

}

}
