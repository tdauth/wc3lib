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

#include <QtGui>

#include "objecttreeitem.hpp"
#include "../objectdata.hpp"
#include "objecttreemodel.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTreeItem::ObjectTreeItem(ObjectData *objectData, QWidget *window, const QString &text, ObjectTreeItem *parent)
: m_objectData(objectData)
, m_window(window)
, m_text(text)
, m_parent(parent)
, m_expanded(false)
{
	if (parent == 0)
	{
		qDebug() << "Item:" << text << "has no parent.";
	}
}

ObjectTreeItem::ObjectTreeItem(ObjectData *objectData, QWidget *window, const QString &originalObjectId, const QString &customObjectId, ObjectTreeItem *parent)
: m_objectData(objectData)
, m_window(window)
, m_originalObjectId(originalObjectId)
, m_customObjectId(customObjectId)
, m_parent(parent)
, m_expanded(false)
{
	if (parent == 0)
	{
		qDebug() << "Item:" << originalObjectId << ":" << customObjectId << "has no parent.";
	}
}

ObjectTreeItem::~ObjectTreeItem()
{
	qDeleteAll(this->m_children);
}

bool ObjectTreeItem::isFolder() const
{
	return children().size() > 0 || this->originalObjectId().isEmpty();
}

int ObjectTreeItem::countObjects() const
{
	QStack<const ObjectTreeItem*> items;
	items.push(this);
	int result = 0;

	while (!items.isEmpty())
	{
		const ObjectTreeItem *current = items.pop();

		if (current->isFolder())
		{
			for (int i = 0; i < current->children().size(); ++i)
			{
				items.push(current->child(i));
			}
		}
		else
		{
			++result;
		}
	}

	return result;
}

bool ObjectTreeItem::hasModifiedObject() const
{
	if (this->objectData() == 0)
	{
		return false;
	}

	QStack<const ObjectTreeItem*> items;
	items.push(this);
	bool result = false;

	while (!items.isEmpty() && !result)
	{
		const ObjectTreeItem *current = items.pop();

		if (current->isFolder())
		{
			for (int i = 0; i < current->children().size(); ++i)
			{
				items.push(current->child(i));
			}
		}
		else if (!current->customObjectId().isEmpty() || this->objectData()->isObjectModified(current->originalObjectId(), current->customObjectId()))
		{
			result = true;
		}
	}

	return result;
}

QString ObjectTreeItem::text(bool showRawData) const
{
	if (isFolder())
	{
		return QObject::tr("%1 (%2)").arg(this->m_text).arg(countObjects());
	}

	const QString name = this->objectData()->objectName(originalObjectId(), customObjectId());

	if (!showRawData)
	{
		return name;
	}

	if (!customObjectId().isEmpty())
	{
		return QObject::tr("%1:%2 (%3)").arg(originalObjectId()).arg(customObjectId()).arg(name);
	}

	return QObject::tr("%1 (%2)").arg(originalObjectId()).arg(name);
}

QIcon ObjectTreeItem::icon() const
{
	if (this->objectData() != nullptr)
	{
		if (isFolder())
		{
			// TODO check if it is collapsed this->expanded()
			return this->objectData()->source()->sharedData()->worldEditDataIcon("UEIcon_UnitCategory", "WorldEditArt", m_window);
		}

		// improves the performance but has to be updated
		if (this->m_icon.isNull())
		{
			this->m_icon = this->objectData()->objectIcon(originalObjectId(), customObjectId(), m_window);
		}

		return m_icon;
	}

	return QIcon();
}

QModelIndex ObjectTreeItem::modelIndex(ObjectTreeModel *model)
{
	if (parent() != 0)
	{
		return model->index(this->row(), 0, this->parent()->modelIndex(model));
	}

	return model->index(model->topLevelRow(this), 0, QModelIndex());
}

int ObjectTreeItem::countNonFolderItems() const
{
	int result = 0;
	QStack<ObjectTreeItem*> children;

	foreach (ObjectTreeItem *child, this->children())
	{
		children.push(child);
	}

	while (!children.isEmpty())
	{
		ObjectTreeItem *item = children.pop();

		if (item->children().isEmpty())
		{
			++result;
		}
		else
		{
			foreach (ObjectTreeItem *child, item->children())
			{
				children.push(child);
			}
		}
	}

	return result;
}

}

}
