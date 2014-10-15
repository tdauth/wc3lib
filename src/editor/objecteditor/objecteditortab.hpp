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

#include <KUrl>
#include <KUrlRequester>
#include <KFilterProxySearchLine>

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
class KDE_EXPORT ObjectEditorTab : public QWidget
{
	Q_OBJECT

	public:
		/**
		 * Creates a new object editor tab using the source \p source and standard objects data/meta data \p objectData.
		 */
		ObjectEditorTab(MpqPriorityList *source, ObjectData *objectData, ObjectEditor *objectEditor = 0, QWidget *parent = 0, Qt::WindowFlags f = 0);
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
		 * \return Returns the associated parent Object Editor. Returns 0 if no object editor is associated.
		 *
		 * \sa hasObjectEditor()
		 */
		ObjectEditor* objectEditor() const;
		KFilterProxySearchLine* filterSearchLine() const;
		KFilterProxySearchLine* tableFilterSearchLine() const;
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
		 * Shows up the modification dialog for field \p fieldId after enabling the object with IDs \p originalObjectId and \p customObjectId.
		 *
		 * \return Returns false if the object does not exist.
		 */
		bool modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId);

		ObjectIdDialog* idDialog() const;

	public slots:
		void newObject();
		/**
		 * Opens the edit dialog for all selected objects names.
		 */
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

		virtual KUrl copyObjectIconUrl() const = 0;
		virtual KUrl pasteObjectIconUrl() const = 0;
		virtual KUrl newObjectIconUrl() const = 0;

		/**
		 * The actual source which is used for loading all necessary files.
		 */
		MpqPriorityList *m_source;

		int m_tabIndex;

		KFilterProxySearchLine *m_filterSearchLine;
		KFilterProxySearchLine *m_tableFilterSearchLine;
		ObjectTreeView *m_treeView; // left side tree widget
		ObjectTreeModel *m_treeModel;

		ObjectTableView *m_tableView; // centered table widget of current selected object
		ObjectTableModel *m_tableModel;

		ObjectData *m_objectData;
		ObjectEditor *m_objectEditor;

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

		KUrl m_recentImportUrl;

	private slots:
		void itemClicked(QModelIndex index);
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
	return objectEditor() != 0;
}

inline ObjectEditor* ObjectEditorTab::objectEditor() const
{
	return this->m_objectEditor;
}

inline KFilterProxySearchLine* ObjectEditorTab::filterSearchLine() const
{
	return this->m_filterSearchLine;
}

inline KFilterProxySearchLine* ObjectEditorTab::tableFilterSearchLine() const
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
