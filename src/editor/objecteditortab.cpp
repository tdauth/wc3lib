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

#include "objecteditortab.hpp"

namespace wc3lib
{

namespace editor
{

ObjectEditorTab::ObjectEditorTab(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : m_source(source), QWidget(parent, f)
{
	// TODO can not call pure virtual member function name in constructor
	//objectEditor->tabWidget()->addTab(this, name());
	if (hasObjectEditor())
		objectEditor()->tabWidget()->addTab(this, "");
}
	
void ObjectEditorTab::showEvent(QShowEvent *event)
{
	if (treeWidget() == 0)
		m_treeWidget = createTreeWidget();
	
	if (tableWidget() == 0)
		m_tableWidget = createTableWidget();
}

#include "moc_objecteditortab.cpp"

}

}
