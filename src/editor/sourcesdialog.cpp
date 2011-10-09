/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include <KLineEdit>
#include <KUrlCompletion>
#include <KUrlRequester>

#include "sourcesdialog.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

void SourcesDialog::update()
{
	foreach (MpqPriorityList::Source source, const_cast<const MpqPriorityList*>(source())->sources())
		m_editListBox->insertItem(source->url().toEncoded());
}

SourcesDialog::SourcesDialog(class MpqPriorityList *source, QWidget *parent, Qt::WFlags flags): m_source(source), KDialog(parent, flags)
{
	setupUi(this);
	//m_editListBox->
	// TODO use URL requester
	//KUrlRequester *requester = new KUrlRequester(this);
	//KUrlCompletion *urlCompletion = new KUrlCompletion(KUrlCompletion::DirCompletion);
	//m_editListBox->lineEdit()->setCompletionObject(urlCompletion);
	//m_editListBox->setCustomEditor(*requester);
}

void SourcesDialog::added(const QString& text)
{
}

}

}
