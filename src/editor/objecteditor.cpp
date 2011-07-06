/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#include <QtGui>

#include <KMenu>
#include <KAction>
#include <KToolBar>

#include "objecteditor.hpp"
#include "objecteditortab.hpp"
#include "editor.hpp"
#include "uniteditor.hpp"

namespace wc3lib
{

namespace editor
{

ObjectEditor::ObjectEditor(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : Module(source, parent, f), m_tabWidget(new KTabWidget(this)), m_unitEditor(new UnitEditor(source, this, f))
{
	connect(tabWidget(), SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
	/*
	const class mpq::MpqFile *unitEditorDataFile = editor->loadMpqFile("UI/UnitEditorData.txt");

	if (unitEditorDataFile != 0)
	{
		this->m_data = new slk::Data;
		std::iostream iostream;
		unitEditorDataFile->write(iostream);
		this->m_data->read(iostream);
	}
	*/
	
	//m_unitEditor = new UnitEditor(this);
	
}

ObjectEditor::~ObjectEditor()
{
}

class ObjectEditorTab* ObjectEditor::tab(int index) const
{
	return boost::polymorphic_cast<class ObjectEditorTab*>(this->tabWidget()->widget(index));
}

void ObjectEditor::exportAll()
{
}

void ObjectEditor::importAll()
{
}

void ObjectEditor::createFileActions(class KMenu *menu)
{
	m_newObjectAction = new KAction(this);
	menu->addAction(newObjectAction());
	
	m_renameObjectAction = new KAction(this);
	menu->addAction(renameObjectAction());
	
	m_deleteObjectAction = new KAction(this);
	menu->addAction(deleteObjectAction());
	
	menu->addSeparator();
	
	m_resetObjectAction = new KAction(this);
	menu->addAction(resetObjectAction());
	
	m_resetAllObjectsAction = new KAction(this);
	menu->addAction(resetAllObjectsAction());
	
	menu->addSeparator();
	
	m_exportAllObjectsAction = new KAction(this);
	menu->addAction(exportAllObjectsAction());
	
	m_importAllObjectsAction = new KAction(this);
	menu->addAction(importAllObjectsAction());
	
	KAction *action = new KAction(editor()->tr("WESTRING_MENU_OE_EXPORTALL", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(exportAll()));
	
	action = new KAction(editor()->tr("WESTRING_MENU_OE_IMPORTALL", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(importAll()));
}

void ObjectEditor::createEditActions(class KMenu *menu)
{
	m_copyObjectAction = new KAction(this);
	menu->addAction(copyObjectAction());
	
	m_pasteObjectAction = new KAction(this);
	menu->addAction(pasteObjectAction());
	
	menu->addSeparator();
	
	KAction *action = new KAction(editor()->tr("WESTRING_MENU_VIEWINPALETTE", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(viewInPalette()));
	
	action = new KAction(editor()->tr("WESTRING_MENU_OE_FIND", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(find()));
	
	action = new KAction(editor()->tr("WESTRING_MENU_OE_FINDNEXT", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(findNext()));
	
	action = new KAction(editor()->tr("WESTRING_MENU_OE_FINDPREV", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(findPrevious()));
	
	menu->addSeparator();
	
	action = new KAction(editor()->tr("WESTRING_MENU_OE_MODIFYFIELD", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(modifyField()));
	
	action = new KAction(editor()->tr("WESTRING_MENU_OE_RESETFIELD", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(resetField()));
	
	action = new KAction(editor()->tr("WESTRING_MENU_OE_AUTOFILL", "WorldEditStrings"), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, SLOT(autoFill()));
}

void ObjectEditor::createMenus(class KMenuBar *menuBar)
{
	/// \todo Create menu "view" with meta data categories of current tab and "raw data" and sort by names actions
}

void ObjectEditor::createWindowsActions(class KMenu *menu)
{
}

void ObjectEditor::createToolButtons(class KToolBar *toolBar)
{
	toolBar->addAction(copyObjectAction());
	toolBar->addAction(pasteObjectAction());
	toolBar->addAction(newObjectAction());
}

class SettingsInterface* ObjectEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void ObjectEditor::currentChanged(int index)
{
	/*
	// refresh file actions
	foreach (QAction *action, m_currentActions)
		delete action;
	
	m_currentActions.clear();
	
	ObjectEditorTab *t = tab(index);
	/// \todo ObjectEditorTab actions have to be placed before
	m_currentActions << t->createFileActions(fileMenu());
	m_currentActions << t->createEditActions(editMenu());
	t->createToolButtons(toolBar());
	*/
	ObjectEditorTab *t = tab(index);
	QString file;
	
	if (currentTab() != 0)
		disconnect(newObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(newObject()));
	
	newObjectAction()->setText(t->newObjectText());
	
	if (editor()->download(t->newObjectIconUrl(), file, this))
		newObjectAction()->setIcon(QIcon(file));
	
	connect(newObjectAction(), SIGNAL(triggered()), t, SLOT(newObject()));
	
	if (currentTab() != 0)
		disconnect(renameObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(renameObject()));
	
	renameObjectAction()->setText(t->renameObjectText());
	connect(renameObjectAction(), SIGNAL(triggered()), t, SLOT(renameObject()));
	
	if (currentTab() != 0)
		disconnect(deleteObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(deleteObject()));
	
	deleteObjectAction()->setText(t->deleteObjectText());
	connect(deleteObjectAction(), SIGNAL(triggered()), t, SLOT(deleteObject()));
	
	if (currentTab() != 0)
		disconnect(resetObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(resetObject()));
	
	resetObjectAction()->setText(t->resetObjectText());
	connect(resetObjectAction(), SIGNAL(triggered()), t, SLOT(resetObject()));
	
	if (currentTab() != 0)
		disconnect(resetAllObjectsAction(), SIGNAL(triggered()), currentTab(), SLOT(resetAllObjects()));
	
	resetAllObjectsAction()->setText(t->resetAllObjectsText());
	connect(resetAllObjectsAction(), SIGNAL(triggered()), t, SLOT(resetAllObjects()));
	
	if (currentTab() != 0)
		disconnect(exportAllObjectsAction(), SIGNAL(triggered()), currentTab(), SLOT(exportAllObjects()));
	
	exportAllObjectsAction()->setText(t->exportAllObjectsText());
	connect(exportAllObjectsAction(), SIGNAL(triggered()), t, SLOT(exportAllObjects()));
	
	if (currentTab() != 0)
		disconnect(importAllObjectsAction(), SIGNAL(triggered()), currentTab(), SLOT(importAllObjects()));
	
	importAllObjectsAction()->setText(t->importAllObjectsText());
	connect(importAllObjectsAction(), SIGNAL(triggered()), t, SLOT(importAllObjects()));
	
	if (currentTab() != 0)
		disconnect(copyObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(copyObject()));
	
	copyObjectAction()->setText(t->copyObjectText());
	
	if (editor()->download(t->copyObjectIconUrl(), file, this))
		copyObjectAction()->setIcon(QIcon(file));
	
	connect(copyObjectAction(), SIGNAL(triggered()), t, SLOT(copyObject()));
	
	if (currentTab() != 0)
		disconnect(pasteObjectAction(), SIGNAL(triggered()), currentTab(), SLOT(pasteObject()));
	
	pasteObjectAction()->setText(t->pasteObjectText());
	
	if (editor()->download(t->pasteObjectIconUrl(), file, this))
		pasteObjectAction()->setIcon(QIcon(file));
	
	connect(pasteObjectAction(), SIGNAL(triggered()), t, SLOT(pasteObject()));
	
	m_currentTab = t;
}

#include "moc_objecteditor.cpp"

}

}
