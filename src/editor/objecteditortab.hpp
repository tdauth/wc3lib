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

#ifndef WC3LIB_EDITOR_OBJECTEDITORTAB_HPP
#define WC3LIB_EDITOR_OBJECTEDITORTAB_HPP

#include <boost/cast.hpp>

#include <QWidget>

#include <KUrl>

#include "objecteditor.hpp"

namespace wc3lib
{

namespace editor
{

class ObjectEditorTab : public QWidget
{
	Q_OBJECT

	public:
		ObjectEditorTab(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);

		class MpqPriorityList* source() const;
		bool hasObjectEditor() const;
		class ObjectEditor* objectEditor() const throw (Exception);
		class ObjectTreeWidget* treeWidget() const;
		class ObjectTableWidget* tableWidget() const;

		virtual QString name() const = 0;

	public slots:
		void newObject();
		void renameObject();
		void deleteObject();
		void resetObject();
		void resetAllObjects();
		void exportAllObjects();
		void importAllObjects();
		void copyObject();
		void pasteObject();

	protected:
		friend class ObjectEditor;

		virtual void showEvent(QShowEvent *event);

		virtual class ObjectTreeWidget* createTreeWidget() = 0;
		virtual class ObjectTableWidget* createTableWidget() = 0;

		virtual void onSwitchToMap(class Map *map) = 0;

		virtual void onNewObject() = 0;
		virtual void onRenameObject() = 0;
		virtual void onDeleteObject() = 0;
		virtual void onResetObject() = 0;
		virtual void onResetAllObjects() = 0;
		virtual void onExportAllObjects() = 0;
		virtual void onImportAllObjects() = 0;
		virtual void onCopyObject() = 0;
		virtual void onPasteObject() = 0;

		virtual QString newObjectText() const = 0;
		virtual QString renameObjectText() const = 0;
		virtual QString deleteObjectText() const = 0;
		virtual QString resetObjectText() const = 0;
		virtual QString resetAllObjectsText() const = 0;
		virtual QString exportAllObjectsText() const = 0;
		virtual QString importAllObjectsText() const = 0;
		virtual QString copyObjectText() const = 0;
		virtual QString pasteObjectText() const = 0;

		virtual KUrl copyObjectIconUrl() const = 0;
		virtual KUrl pasteObjectIconUrl() const = 0;
		virtual KUrl newObjectIconUrl() const = 0;

		class MpqPriorityList *m_source;
		class ObjectTreeWidget *m_treeWidget; // left side tree widget
		class ObjectTableWidget *m_tableWidget; // centered table widget of current selected object
};

inline class MpqPriorityList* ObjectEditorTab::source() const
{
	return m_source;
}

inline bool ObjectEditorTab::hasObjectEditor() const
{
	return typeid(parent()) == typeid(class ObjectEditor);
}

inline class ObjectEditor* ObjectEditorTab::objectEditor() const throw (Exception)
{
	try
	{
		return boost::polymorphic_cast<class ObjectEditor*>(parent());
	}
	catch (std::bad_cast &exception)
	{
		throw Exception(exception.what());
	}
}

inline class ObjectTreeWidget* ObjectEditorTab::treeWidget() const
{
	return m_treeWidget;
}

inline class ObjectTableWidget* ObjectEditorTab::tableWidget() const
{
	return m_tableWidget;
}

inline void ObjectEditorTab::newObject()
{
	onNewObject();
}

inline void ObjectEditorTab::renameObject()
{
	onRenameObject();
}

inline void ObjectEditorTab::deleteObject()
{
	onDeleteObject();
}

inline void ObjectEditorTab::resetObject()
{
	onResetObject();
}

inline void ObjectEditorTab::resetAllObjects()
{
	onResetAllObjects();
}

inline void ObjectEditorTab::exportAllObjects()
{
	onExportAllObjects();
}

inline void ObjectEditorTab::importAllObjects()
{
	onImportAllObjects();
}

inline void ObjectEditorTab::copyObject()
{
	onCopyObject();
}

inline void ObjectEditorTab::pasteObject()
{
	onPasteObject();
}

}

}

#endif
