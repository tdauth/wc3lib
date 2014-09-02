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
#include <KUrlRequester>
#include <KLineEdit>

#include "objecteditor.hpp"
#include "objecttreewidget.hpp"
#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class ObjectMetaData;
class ObjectTableWidget;
class Map;

/**
 * \brief A single tab of the Object Editor which theoretically could be used as standalone application as well. It supports a specific set of predefined data and meta data (\ref metaData()) and lists the standard objects by default.
 *
 * This abstract class provides the basic interface which each tab must fulfil to provide all functionality of the object editor. A concrete tab should be realized by deriving from this class and implementing all abstract element functions.
 *
 * The tab may list custom objects as well if some are created or imported. All modifications are stored and can be accessed via different element functions:
 * <ul>
 * <li>\ref modifyField() </li>
 * <li>\ref isFieldModified() </li>
 * <li>\ref resetField() </li>
 * <li>\ref fieldModificiation() </li>
 * <li>\ref clearModifications() </li>
 * <li>\ref fieldValue() </li>
 * <li>\ref fieldReadableValue() </li>
 * </ul>
 */
class ObjectEditorTab : public QWidget
{
	Q_OBJECT

	public:
		/**
		 * \brief Hash table which stores modificiations by using the field ID as hash (such as "unam").
		 */
		typedef QHash<QString, map::CustomUnits::Modification> Modifications;
		/**
		 * \brief Stores the two object IDs. The original as well as the custom.
		 */
		typedef QPair<QString, QString> ObjectId;
		/**
		 * \brief Hash table which stores objects by using the object ID pair as hash.
		 */
		typedef QHash<ObjectId, Modifications> Objects;

		/**
		 * Creates a new object editor tab using the source \p source and meta data \p metaData.
		 */
		ObjectEditorTab(MpqPriorityList *source, ObjectMetaData *metaData, QWidget *parent = 0, Qt::WindowFlags f = 0);
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
		ObjectTreeWidget* treeWidget() const;

		QTreeWidgetItem* standardObjectsItem() const;
		QTreeWidgetItem* customObjectsItem() const;

		/**
		 * \return Returns the right edge table widget which lists descriptions and values for all fields of the currently activated object.
		 *
		 * \sa treeWidget()
		 */
		ObjectTableWidget* tableWidget() const;

		/**
		 * Each tab needs underlying meta data for the default objects.
		 * These data is provided by the class \ref ObjectMetaData or any derived classes.
		 *
		 * \return Returns the underlying meta data instance.
		 */
		ObjectMetaData* metaData() const;
		/**
		 * Custom Units are used in Warcraft III: Reign of Chaos.
		 * So this should throw an exception in other tabs than unit and items since they need additional data
		 * which can be gotten from \ref customObjects().
		 *
		 * \return Returns the custom units of the current tab.
		 *
		 * \sa customObjects() hasCustomUnits() hasCustomObjects()
		 *
		 * \throws Exception Throws an exception if custom units cannot store the object data.
		 */
		virtual map::CustomUnits customUnits() const;
		/**
		 * \return Returns the custom objects of the tab.
		 *
		 * \sa customUnits(), hasCustomObjects(), hasCustomUnits()
		 */
		virtual map::CustomObjects customObjects() const;
		virtual map::CustomUnits::Unit currentUnit() const;
		virtual map::CustomObjects::Object currentObject() const;

		/**
		 * Should return if custom units are available.
		 * Usually only either custom units or custom objects are available depending on which meta data is loaded or if it is Reign of Chaos or Frozen Throne.
		 *
		 * \sa hasCustomObjects().
		 */
		virtual bool hasCustomUnits() const = 0;
		/**
		 * Should return if custom objects are available.
		 *
		 * \sa hasCustomUnits()
		 */
		virtual bool hasCustomObjects() const = 0;

		/**
		 * Modifies a single field with ID \p fieldId (taken from meta data file) of object with IDS \p originalObjectId and \p customObjectId and sets the field's modification to \p modification.
		 * For example this happens whenever a user enters another or even the same value which overwrites the default one of the object.
		 * All modified fields are exported when the user exports object data.
		 */
		void modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, const map::CustomUnits::Modification &modification);
		void resetField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId);
		bool isFieldModified(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const;
		void clearModifications();
		bool fieldModificiation(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, map::CustomUnits::Modification &modification) const;
		QString fieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const;
		QString fieldReadableValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const;

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

		/**
		 * \return Returns the original object raw data ID of the currently selected object.
		 *
		 * \sa currentCustomObjectId()
		 */
		QString currentOriginalObjectId() const;

		/**
		 * \return Returns the custom object raw data ID of the currently selected object.
		 *
		 * \sa currentOriginalObjectId()
		 */
		QString currentCustomObjectId() const;

		virtual QString name() const = 0;

		/**
		 * Imports custom units \p units and replaces all existing custom units by immediately.
		 * It updates the modifications to the loaded custom units.
		 */
		void importCustomUnits(const map::CustomUnits &units);

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

		/**
		 * This element function has to setup the tree widget \p treeWidget which means it has to add all necessary standard items to the widget.
		 */
		virtual void setupTreeWidget(ObjectTreeWidget *treeWidget) = 0;
		/**
		 * Fills the item \p item with the custom object using the ID \p originalObjectId and \p customObjectId.
		 * Usually this sets the text of the item to the name or raw data ID of the object it is refering to.
		 * All modifications can be accessed via the standard element functions for modifications and should be available at the moment when this function
		 * is being called.
		 */
		virtual void fillTreeItem(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem *item) = 0;
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
		virtual void activateObject(QTreeWidgetItem *item, int column, const QString &originalObjectId, const QString &customObjectId) = 0;
		/**
		 * Activates a folder item.
		 * Usually this should hide the columns of the table widget since no object is selected.
		 */
		virtual void activateFolder(QTreeWidgetItem *item, int column) = 0;

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
		ObjectTreeWidget *m_treeWidget; // left side tree widget
		QTreeWidgetItem *m_standardObjectsItem;
		QTreeWidgetItem *m_customObjectsItem;

		 ObjectTableWidget *m_tableWidget; // centered table widget of current selected object

		ObjectMetaData *m_metaData;

		/**
		 * Stores all modifications for all objects, standard objects as well as user defined ones.
		 */
		Objects m_objects;

		/**
		 * Flag which stores if raw data IDs are shown.
		 */
		bool m_showRawData;

	private slots:
		void itemClicked(QTreeWidgetItem *item, int column);
		void filterTreeWidget(QString text);
};

inline class MpqPriorityList* ObjectEditorTab::source() const
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
			return dynamic_cast<ObjectEditor*>(parentWidget()->parentWidget()->parentWidget()) != 0; // first parent is stacked widget, second tab widget and third object editor
	}

	return true;
}

inline ObjectEditor* ObjectEditorTab::objectEditor() const throw (std::bad_cast)
{
	// TODO typeid comparison doesn't work, dynamic_cast is working workaround!
	// NOTE parent could be tab widget of object editor
	if (dynamic_cast<ObjectEditor*>(parent()) == 0)
		return boost::polymorphic_cast<class ObjectEditor*>(parentWidget()->parentWidget()->parentWidget()); // first parent is stacked widget, second tab widget and third object editor

	return boost::polymorphic_cast<class ObjectEditor*>(parent());
}

inline KLineEdit* ObjectEditorTab::filterLineEdit() const
{
	return this->m_filterLineEdit;
}

inline ObjectTreeWidget* ObjectEditorTab::treeWidget() const
{
	return m_treeWidget;
}

inline QTreeWidgetItem* ObjectEditorTab::standardObjectsItem() const
{
	return this->m_standardObjectsItem;
}

inline QTreeWidgetItem* ObjectEditorTab::customObjectsItem() const
{
	return this->m_customObjectsItem;
}

inline ObjectTableWidget* ObjectEditorTab::tableWidget() const
{
	return m_tableWidget;
}

inline ObjectMetaData* ObjectEditorTab::metaData() const
{
	return this->m_metaData;
}

inline bool ObjectEditorTab::showRawData() const
{
	return this->m_showRawData;
}

inline QString ObjectEditorTab::currentOriginalObjectId() const
{
	const QList<QTreeWidgetItem*> items = this->treeWidget()->selectedItems();

	if (!items.empty())
	{
		return treeWidget()->itemOriginalObjectId(*items.first());
	}

	return QString();
}

inline QString ObjectEditorTab::currentCustomObjectId() const
{
	const QList<QTreeWidgetItem*> items = this->treeWidget()->selectedItems();

	if (!items.empty())
	{
		return treeWidget()->itemCustomObjectId(*items.first());
	}

	return QString();
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
