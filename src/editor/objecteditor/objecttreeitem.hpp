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

#ifndef WC3LIB_EDITOR_OBJECTTREEITEM_HPP
#define WC3LIB_EDITOR_OBJECTTREEITEM_HPP

#include <QWidget>
#include <QList>
#include <QString>
#include <QModelIndex>
#include <QIcon>

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class ObjectData;
class ObjectTreeModel;

/**
 * \brief An item representing one single object or folder in a tree model.
 *
 * \ingroup objectdata
 */
class ObjectTreeItem
{
	public:
		typedef QList<ObjectTreeItem*> Children;

		/**
		 * Creates a new object tree item for a folder.
		 * \param window The window is required to load the item's icon using \ref WarcraftIIISharedData::icon().
		 * \param text The folder's name.
		 */
		ObjectTreeItem(ObjectData *objectData, QWidget *window, const QString &text, ObjectTreeItem *parent = 0);
		/**
		 * Creates new object tree item for an object.
		 * \param originalObjectId The original object ID of the object which the item represents.
		 * \param customObjectId The custom object ID of the object which the item represents.
		 */
		ObjectTreeItem(ObjectData *objectData, QWidget *window, const QString &originalObjectId, const QString &customObjectId, ObjectTreeItem *parent = 0);
		~ObjectTreeItem();

		/**
		 * Only folders have child items and no corresponding object ID.
		 * \return Returns true if the item is a folder and therefore not an object. Otherwise it returns false.
		 */
		bool isFolder() const;

		/**
		 * Sets the text of a folder item.
		 * This only affects folder items since object items get their text depending on the object's name and raw data.
		 * \param text The new text of the folder.
		 */
		void setFolderText(const QString &text);
		QString text(bool showRawData) const;

		void setObjectData(ObjectData *objectData);
		ObjectData* objectData() const;
		void setObjectId(const QString &originalObjectId, const QString &customObjectId);
		const QString& originalObjectId() const;
		const QString& customObjectId() const;
		int index();
		int row();
		void setParent(ObjectTreeItem *parent);
		ObjectTreeItem* parent() const;
		/**
		 * If the item is folder, a folder icon will be returned. Depending on the result of \ref expanded() it might be a open folder icon.
		 * If the item is an object it will be the icon defined by the corresponding object data for the object.
		 * \return Returns the current icon of the item.
		 */
		QIcon icon() const;
		void appendChild(ObjectTreeItem *child);
		void removeChild(ObjectTreeItem *child);
		void setChildren(const Children &children);
		Children& children();
		const Children& children() const;

		/**
		 * Counts the actual objects in all sub folders as well as the current.
		 */
		int countObjects() const;
		/**
		 * Modified objects are marked with a different color.
		 *
		 * \return Returns true if any of the children objects or the object itself has modifications.
		 */
		bool hasModifiedObject() const;

		/**
		 * \return Returns 0 if child at \p row does not exist.
		 */
		ObjectTreeItem* child(int row) const;

		/**
		 * Indicates that the folder is expanded. This should change the folder's icon returned by \ref icon().
		 * \param expanded If this value is true, the folder is expanded. Otherwise it is collapsed.
		 */
		void setExpanded(bool expanded);
		/**
		 * \return Returns if the folder is expanded. If it returns false the folder is collapsed.
		 */
		bool expanded() const;

		QModelIndex modelIndex(ObjectTreeModel *model);

		/**
		 * Counts all items which aren't folders.
		 * It counts items with an infinite level so this function's performance is quite poor.
		 * Complexity is O(n).
		 */
		int countNonFolderItems() const;

	private:
		ObjectData *m_objectData;
		QWidget *m_window;
		QString m_text;
		QString m_originalObjectId;
		QString m_customObjectId;
		ObjectTreeItem *m_parent;
		Children m_children;
		bool m_expanded;
		// TODO find a better caching mechanism
		mutable QIcon m_icon;
};

inline void ObjectTreeItem::setFolderText(const QString& text)
{
	this->m_text = text;
}

inline void ObjectTreeItem::setObjectData(ObjectData* objectData)
{
	this->m_objectData = objectData;
}

inline ObjectData* ObjectTreeItem::objectData() const
{
	return this->m_objectData;
}

inline void ObjectTreeItem::setObjectId(const QString& originalObjectId, const QString& customObjectId)
{
	this->m_originalObjectId = originalObjectId;
	this->m_customObjectId = customObjectId;
}

inline const QString& ObjectTreeItem::originalObjectId() const
{
	return this->m_originalObjectId;
}

inline const QString& ObjectTreeItem::customObjectId() const
{
	return this->m_customObjectId;
}

inline int ObjectTreeItem::index()
{
	if (this->parent() == nullptr)
	{
		return -1;
	}

	return this->parent()->children().indexOf(this);
}

inline int ObjectTreeItem::row()
{
	return this->index();
}

inline void ObjectTreeItem::setParent(ObjectTreeItem* parent)
{
	this->m_parent = parent;
}

inline ObjectTreeItem* ObjectTreeItem::parent() const
{
	return this->m_parent;
}

inline void ObjectTreeItem::appendChild(ObjectTreeItem* child)
{
	this->m_children.append(child);
}

inline void ObjectTreeItem::removeChild(ObjectTreeItem* child)
{
	this->m_children.removeAll(child);
}

inline void ObjectTreeItem::setChildren(const ObjectTreeItem::Children &children)
{
	this->m_children = children;
}

inline ObjectTreeItem::Children& ObjectTreeItem::children()
{
	return this->m_children;
}

inline const ObjectTreeItem::Children& ObjectTreeItem::children() const
{
	return this->m_children;
}

inline ObjectTreeItem* ObjectTreeItem::child(int row) const
{
	if (row >= this->m_children.size() || row < 0)
	{
		return 0;
	}

	return this->m_children[row];
}

inline void ObjectTreeItem::setExpanded(bool expanded)
{
	this->m_expanded = expanded;
}

inline bool ObjectTreeItem::expanded() const
{
	return this->m_expanded;
}

}

}

#endif // WC3LIB_EDITOR_OBJECTTREEITEM_HPP
