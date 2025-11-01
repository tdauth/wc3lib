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
#include <QSortFilterProxyModel>
#include <QUndoStack>
#include <QLineEdit>

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
class ObjectTableView;
class ObjectTableModel;
class Map;
class ObjectIdDialog;

/**
 * \brief A single tab of the Object Editor which theoretically could be used as standalone application as well. It supports a specific set of predefined data and meta data (\ref objectData()) and lists the standard objects by default.
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
		ObjectEditorTab(MpqPriorityList *source, ObjectData *objectData, const QString &groupName, ObjectEditor *objectEditor = nullptr, QWidget *parent = nullptr);
		virtual ~ObjectEditorTab();

		/**
		 * \return Returns the corresponding MPQ priority list which all necessary data is loaded from.
		 */
		MpqPriorityList* source() const;

		/**
		 * \return Returns true if an Object Editor is associated with this tab. As the tab might be started as stand-alone application this could return false.
		 *
		 * \sa objectEditor()
		 */
		bool hasObjectEditor() const;
		/**
		 * \return Returns the associated parent Object Editor. Returns 0 if no object editor is associated.
		 *
		 * \sa hasObjectEditor()
		 */
		ObjectEditor* objectEditor() const;
		QLineEdit* filterSearchLine() const;
		QLineEdit* tableFilterSearchLine() const;
		/**
		 * \return Returns the left edge tree widget which lists all objects (standard and custom).
		 *
		 * \sa tableWidget()
		 */
		ObjectTreeView* treeView() const;
		QSortFilterProxyModel* proxyModel() const;
		ObjectTreeModel* treeModel() const;

		/**
		 * \return Returns the right edge table widget which lists descriptions and values for all fields of the currently activated object.
		 *
		 * \sa treeView()
		 */
		ObjectTableView* tableView() const;
		QSortFilterProxyModel* tableProxyModel() const;
		ObjectTableModel* tableModel() const;

		/**
		 * Each tab needs underlying data for the standard objects as well as meta data of all object fields..
		 * These data is provided by the class \ref ObjectData or any derived classes.
		 *
		 * \return Returns the underlying standard objects data and meta data instance.
		 */
		ObjectData* objectData() const;

		/**
		 * The group name for the corresponding settings of the tab.
		 * \return Returns the group name.
		 */
		QString groupName() const;

		void setSortByName(bool sort);
		bool sortByName() const;

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

		virtual bool clipboardIsEmpty();

		/**
		 * \return Returns the currently selected objects as table.
		 *
		 * The table can be used for export or the clipboard.
		 */
		virtual map::CustomObjects::Table selection() const;

		/**
		 * Selects object with IDs \p originalObjectId and \p customObjectId in the tree view
		 * which also activates the object in the table view.
		 *
		 * \return Returns false if the object does not exist.
		 */
		bool selectObject(const QString &originalObjectId, const QString &customObjectId);

		/**
		 * Selects the row in the table view with \p fieldId if it does exist.
		 * \return Returns true if the row has been selected successfully.
		 */
		bool selectField(const QString &fieldId, int level);
		/**
		 * \return Returns the field ID of the currently selected row of the table view.
		 */
		ObjectData::FieldId selectedField() const;

		/**
		 * Shows up the modification dialog for field \p fieldId after enabling the object with IDs \p originalObjectId and \p customObjectId.
		 *
		 * \return Returns false if the object does not exist.
		 */
		bool modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level);

		/**
		 * \return Returns the ID dialog which allows the user to specify a custom ID for a newly created object.
		 */
		ObjectIdDialog* idDialog() const;

	public slots:
		/**
		 * Creates a new object in the current tab with a unique object ID.
		 */
		void newObject();
		/**
		 * Opens the edit dialog for all selected objects names.
		 */
		void renameObject();
		/**
		 * Deletes all currently selected objects.
		 */
		void deleteObject();
		/**
		 * Resets all modifications for all currently selected objects.
		 */
		void resetObject();
		/**
		 * Resets all modifications for all objects of this object editor tab.
		 */
		void resetAllObjects();
		/**
		 * Exports all objects from the current tab only.
		 * The user has to select an output file in which the object data is exported into.
		 */
		void exportAllObjects();
		void importAllObjects();
		/**
		 * Given that the file at \p url is a custom objects collection, it tries to import the corresponding objects of the collection which match
		 * to the object data of the tab and replace the current object data by it.
		 */
		void importAllObjectsFromCustomObjectsCollection(const QUrl &url);
		void copyObject();
		void pasteObject();

		/**
		 * Calls \ref ObjectData::widgetize() for a user-defined directory.
		 */
		void widgetizeAllObjects();

		/**
		 * Loads the corresponding icon of the current tab.
		 * This method has to be overriden in a derived class and implemented.
		 * The icon is shown in the tab widget of the object editor.
		 */
		virtual QIcon tabIcon(QWidget *widget) const = 0;

		/**
		 * Filters the objects in the tree view by the expression \p filter which might be a regular expression.
		 * \param filter A regular expression.
		 */
		void filterObjects(const QString &filter);
		/**
		 * Filters the modifications in the table view by the expression \p filter which might be a regular expression.
		 * \param filter A regular expression.
		 */
		void filterFields(const QString &filter);

	protected:
		friend ObjectEditor;

		virtual void setupUi();

		virtual ObjectTreeModel* createTreeModel() = 0;
		virtual void onSwitchToMap(Map *map) = 0;
		virtual void onNewObject() = 0;

		virtual QString newObjectText() const = 0;
		virtual QString renameObjectText() const = 0;
		virtual QString deleteObjectText() const = 0;
		virtual QString resetObjectText() const = 0;
		virtual QString resetAllObjectsText() const = 0;
		virtual QString exportAllObjectsText() const = 0;
		virtual QString importAllObjectsText() const = 0;
		virtual QString copyObjectText() const = 0;
		virtual QString pasteObjectText() const = 0;

		virtual QUrl copyObjectIconUrl() const = 0;
		virtual QUrl pasteObjectIconUrl() const = 0;
		virtual QUrl newObjectIconUrl() const = 0;

		/**
		 * Updates the \ref m_recentImportUrl value and sorts all entries by name.
		 */
		void updateImportUrlAndSort(const QUrl &url);

		/**
		 * The actual source which is used for loading all necessary files.
		 */
		MpqPriorityList *m_source;

		QLineEdit *m_filterSearchLine;
		QLineEdit *m_tableFilterSearchLine;
		ObjectTreeView *m_treeView; // left side tree widget
		/*
		 * Left side tree model.
		 */
		ObjectTreeModel *m_treeModel;

		ObjectTableView *m_tableView; // centered table widget of current selected object
		ObjectTableModel *m_tableModel;

		/**
		 * The corresponding object data which is shown by the tab.
		 */
		ObjectData *m_objectData;

		/**
		 * The group name for the settings of this tab.
		 */
		QString m_groupName;
		/**
		 * The parent object editor.
		 */
		ObjectEditor *m_objectEditor;

		/**
		 * Flag which stores if objects are sorted by name.
		 */
		bool m_sortByName;

		/**
		 * Flag which stores if raw data IDs are shown.
		 */
		bool m_showRawData;

		/**
		 * Stores the currently pasted or cut objects.
		 */
		map::CustomObjects::Table m_clipboard;

		/**
		 * Stores the command history of a single tab which allows undo and redo actions.
		 */
		QUndoStack m_undoStack;

		ObjectIdDialog *m_idDialog;

		QUrl m_recentExportUrl;
		QUrl m_recentImportUrl;

	private slots:
		void itemClicked(QModelIndex index);
};

inline MpqPriorityList* ObjectEditorTab::source() const
{
	return m_source;
}

inline bool ObjectEditorTab::hasObjectEditor() const
{
	return objectEditor() != 0;
}

inline ObjectEditor* ObjectEditorTab::objectEditor() const
{
	return this->m_objectEditor;
}

inline QLineEdit* ObjectEditorTab::filterSearchLine() const
{
	return this->m_filterSearchLine;
}

inline QLineEdit* ObjectEditorTab::tableFilterSearchLine() const
{
	return this->m_tableFilterSearchLine;
}

inline ObjectTreeView* ObjectEditorTab::treeView() const
{
	return m_treeView;
}

inline ObjectTreeModel* ObjectEditorTab::treeModel() const
{
	return m_treeModel;
}

inline ObjectTableView* ObjectEditorTab::tableView() const
{
	return m_tableView;
}

inline ObjectTableModel* ObjectEditorTab::tableModel() const
{
	return this->m_tableModel;
}

inline ObjectData* ObjectEditorTab::objectData() const
{
	return this->m_objectData;
}

inline QString ObjectEditorTab::groupName() const
{
	return this->m_groupName;
}

inline bool ObjectEditorTab::showRawData() const
{
	return this->m_showRawData;
}

inline void ObjectEditorTab::newObject()
{
	onNewObject();
}

inline ObjectIdDialog* ObjectEditorTab::idDialog() const
{
	return this->m_idDialog;
}

}

}

#endif
