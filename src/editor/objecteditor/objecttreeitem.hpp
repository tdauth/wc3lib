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
#include <QIcon>

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class ObjectData;

class ObjectTreeItem
{
	public:
		typedef QList<ObjectTreeItem*> Children;

		ObjectTreeItem(ObjectData *objectData, const QString &text, ObjectTreeItem *parent = 0);
		ObjectTreeItem(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId, ObjectTreeItem *parent = 0);
		~ObjectTreeItem();

		bool isFolder() const;

		QString text(bool showRawData) const;

		ObjectData* objectData() const;
		const QString& originalObjectId() const;
		const QString& customObjectId() const;
		int index();
		int row();
		ObjectTreeItem* parent() const;
		QIcon icon() const;
		QColor foreground() const;
		void appendChild(ObjectTreeItem *child);
		const Children& children() const;
		/**
		 * \return Returns 0 if child at \p row does not exist.
		 */
		ObjectTreeItem* child(int row) const;

		void setExpanded(MpqPriorityList *source, QWidget *window);
		void setCollapsed(MpqPriorityList *source, QWidget *window);
		void setIcon(const QIcon &icon);

	private:
		ObjectData *m_objectData;
		QString m_text;
		QString m_originalObjectId;
		QString m_customObjectId;
		ObjectTreeItem *m_parent;
		QIcon m_icon;
		Children m_children;

};

inline ObjectData* ObjectTreeItem::objectData() const
{
	return this->m_objectData;
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
	if (this->parent() == 0)
	{
		return -1;
	}

	return this->parent()->children().indexOf(this);
}

inline int ObjectTreeItem::row()
{
	return this->index();
}

inline ObjectTreeItem* ObjectTreeItem::parent() const
{
	return this->m_parent;
}

inline QIcon ObjectTreeItem::icon() const
{
	return this->m_icon;
}

inline void ObjectTreeItem::appendChild(ObjectTreeItem* child)
{
	this->m_children.append(child);
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

inline void ObjectTreeItem::setIcon(const QIcon& icon)
{
	this->m_icon = icon;
}

}

}

#endif // WC3LIB_EDITOR_OBJECTTREEITEM_HPP
