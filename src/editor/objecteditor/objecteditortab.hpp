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

#include <QObject>
#include <QWidget>

#include <KUrl>
#include <KUrlRequester>
#include <KLineEdit>

#include "objecteditor.hpp"
#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class ObjectData;
class ObjectTreeView;
class ObjectTreeModel;
class ObjectTreeItem;
class ObjectTableWidget;
class ObjectTableWidgetPair;
class Map;

/**
 * \brief A single tab of the Object Editor which theoretically could be used as standalone application as well. It supports a specific set of predefined data and meta data (\ref metaData()) and lists the standard objects by default.
 *
 * This abstract class provides the basic interface which each tab must fulfil to provide all functionality of the object editor. A concrete tab should be realized by deriving from this class and implementing all abstract element functions.
 *
 * The tab may list custom objects as well if some are created or imported. All modifications are stored and can be accessed via \ref objectData().
 *
 * \ingroup objectdata
 */
class ObjectEditorTab : public QWidget
{
	Q_OBJECT

	public:
		/**
		 * Creates a new object editor tab using the source \p source and standard objects data/meta data \p objectData.
		 */
		ObjectEditorTab(MpqPriorityList *source, ObjectData *objectData, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~ObjectEditorTab();

		/**
		 * \return Returns the corresponding MPQ priority list which all necessary data is loaded from.
		 */
		MpqPriorityList* source() const;

		/**
		 * \return If it has an object editor (\ref hasObjectEditor()) this returns its corresponding tab index of \ref ObjectEditor::tabWidget().
		 */
		int tabIndex() const;

		/**
		 * \return Returns true if an Object Editor is associated with this tab. As the tab might be started as stand-alone application this could return false.
		 *
		 * \sa objectEditor()
		 */
		bool hasObjectEditor() const;
		/**
		 * \return Returns the associated parent Object Editor.
		 * \throws std::bad_cast This might throw an exception if no Object Editor is associated. Use \ref hasObjectEditor() to check before.
		 *
		 * \sa hasObjectEditor()
		 */
		ObjectEditor* objectEditor() const throw (std::bad_cast);
		KLineEdit* filterLineEdit() const;
		/**
		 * \return Returns the left edge tree widget which lists all objects (standard and custom).
		 *
		 * \sa tableWidget()
		 */
		ObjectTreeView* treeView() const;
		ObjectTreeModel* treeModel() const;

		/**
		 * \return Returns the right edge table widget which lists descriptions and values for all fields of the currently activated object.
		 *
		 * \sa treeView()
		 */
		ObjectTableWidget* tableWidget() const;

		/**
		 * Each tab needs underlying data for the standard objects as well as meta data of all object fields..
		 * These data is provided by the class \ref ObjectData or any derived classes.
		 *
		 * \return Returns the underlying standard objects data and meta data instance.
		 */
		ObjectData* objectData() const;

		/**
		 * Shows or hides raw data IDs for objects as well as fields.
		 * \param show If this value is true the raw data IDs will be shown. Otherwise they will be hidden.
		 *
		 * \sa showRawData()
		 */
		void setShowRawData(bool show);
		/**
		 * \return Returns true if raw data IDs are shown. Otherwise it returns false.
		 *
		 * \sa setShowRawData()
		 */
		bool showRawData() const;

		virtual QString name() const = 0;
		/**
		 * Fills the item \p item with the custom object using the ID \p originalObjectId and \p customObjectId.
		 * Usually this sets the text of the item to the name or raw data ID of the object it is refering to.
		 * All modifications can be accessed via the standard element functions for modifications and should be available at the moment when this function
		 * is being called.
		 */
		virtual void fillTreeItem(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem *item) = 0;
		virtual void fillTableRow(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, ObjectTableWidgetPair *pair) = 0;

		virtual bool clipboardIsEmpty();

		/**
		 * \return Returns the currently selected objects as table.
		 *
		 * The table can be used for export or the clipboard.
		 */
		virtual map::CustomObjects::Table selection() const;

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
		friend ObjectEditor;

		virtual void setupUi();

		virtual ObjectTreeModel* createTreeModel() = 0;
		/**
		 * Creates the table widget which lists description and values of object fields.
		 */
		virtual ObjectTableWidget* createTableWidget() = 0;

		virtual void onSwitchToMap(Map *map) = 0;

		virtual void onNewObject() = 0;
		virtual void onRenameObject() = 0;
		virtual void onDeleteObject() = 0;
		virtual void onResetObject() = 0;
		virtual void onResetAllObjects() = 0;
		virtual void onExportAllObjects() = 0;
		virtual void onImportAllObjects() = 0;
		virtual void onCopyObject() = 0;
		virtual void onPasteObject() = 0;

		virtual void onShowRawData(bool show) = 0;

		/**
		 * Activates object with ID \p originalObjectId and \p customObjectId by selection of \p item in column \p column.
		 * Usually this should display the fields of the object in the table widget.
		 */
		virtual void activateObject(ObjectTreeItem *item) = 0;
		/**
		 * Activates a folder item.
		 * Usually this should hide the columns of the table widget since no object is selected.
		 */
		virtual void activateFolder(ObjectTreeItem *item) = 0;

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

		/**
		 * The actual source which is used for loading all necessary files.
		 */
		MpqPriorityList *m_source;

		int m_tabIndex;

		KLineEdit *m_filterLineEdit;
		ObjectTreeView *m_treeView; // left side tree widget
		ObjectTreeModel *m_treeModel;

		ObjectTableWidget *m_tableWidget; // centered table widget of current selected object

		ObjectData *m_objectData;

		/**
		 * Flag which stores if raw data IDs are shown.
		 */
		bool m_showRawData;

		/**
		 * Stores the currently pasted or cut objects.
		 */
		map::CustomObjects::Table m_clipboard;

	private slots:
		void itemClicked(QModelIndex index);
		void filterTreeWidget(const QString &text);
		// TEST
		void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};

inline MpqPriorityList* ObjectEditorTab::source() const
{
	return m_source;
}

inline int ObjectEditorTab::tabIndex() const
{
	return m_tabIndex;
}

inline bool ObjectEditorTab::hasObjectEditor() const
{
	// TODO typeid comparison doesn't work, dynamic_cast is working workaround!
	// NOTE parent could be tab widget of object editor
	if (dynamic_cast<ObjectEditor*>(parent()) == 0)
	{
		if (dynamic_cast<KTabWidget*>(parent()) != 0)
		{
			return dynamic_cast<ObjectEditor*>(parentWidget()->parentWidget()->parentWidget()) != 0; // first parent is stacked widget, second tab widget and third object editor
		}
	}

	return true;
}

inline ObjectEditor* ObjectEditorTab::objectEditor() const throw (std::bad_cast)
{
	// TODO typeid comparison doesn't work, dynamic_cast is working workaround!
	// NOTE parent could be tab widget of object editor
	if (dynamic_cast<ObjectEditor*>(parent()) == 0)
	{
		return boost::polymorphic_cast<class ObjectEditor*>(parentWidget()->parentWidget()->parentWidget()); // first parent is stacked widget, second tab widget and third object editor
	}

	return boost::polymorphic_cast<class ObjectEditor*>(parent());
}

inline KLineEdit* ObjectEditorTab::filterLineEdit() const
{
	return this->m_filterLineEdit;
}

inline ObjectTreeView* ObjectEditorTab::treeView() const
{
	return m_treeView;
}

inline ObjectTreeModel* ObjectEditorTab::treeModel() const
{
	return this->m_treeModel;
}

inline ObjectTableWidget* ObjectEditorTab::tableWidget() const
{
	return m_tableWidget;
}

inline ObjectData* ObjectEditorTab::objectData() const
{
	return this->m_objectData;
}

inline bool ObjectEditorTab::showRawData() const
{
	return this->m_showRawData;
}

inline void ObjectEditorTab::newObject()
{
	onNewObject();
}

inline void ObjectEditorTab::renameObject()
{
	onRenameObject();
}

}

}

#endif
