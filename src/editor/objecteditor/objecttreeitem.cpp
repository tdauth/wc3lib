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
#include "objectdata.hpp"
#include "objecttreemodel.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTreeItem::ObjectTreeItem(ObjectData *objectData, const QString &text, ObjectTreeItem *parent)
: m_objectData(objectData)
, m_text(text)
, m_parent(parent)
{
	if (parent == 0)
	{
		qDebug() << "Item:" << text << "has no parent.";
	}
}

ObjectTreeItem::ObjectTreeItem(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId, ObjectTreeItem *parent)
: m_objectData(objectData)
, m_originalObjectId(originalObjectId)
, m_customObjectId(customObjectId)
, m_parent(parent)
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

QString ObjectTreeItem::text(bool showRawData) const
{
	if (isFolder())
	{
		return this->m_text;
	}

	return this->objectData()->fieldValue(originalObjectId(), customObjectId(), "unam") + this->objectData()->fieldValue(originalObjectId(), customObjectId(), "unsf");
}

QColor ObjectTreeItem::foreground() const
{
	if (this->objectData()->isObjectModified(originalObjectId(), customObjectId()))
	{
		return QColor(Qt::magenta);
	}

	return QColor(Qt::black);
}

void ObjectTreeItem::setExpanded(MpqPriorityList *source, QWidget *window)
{
	this->m_icon = source->sharedData()->worldEditDataIcon("UEIcon_UnitCategory", "WorldEditArt", window);
}

void ObjectTreeItem::setCollapsed(MpqPriorityList *source, QWidget *window)
{
	this->m_icon = source->sharedData()->worldEditDataIcon("UEIcon_UnitCategory", "WorldEditArt", window);
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
