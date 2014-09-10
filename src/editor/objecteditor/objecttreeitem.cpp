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

#include "objecttreeitem.hpp"
#include "objectdata.hpp"
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

	return this->objectData()->fieldValue(originalObjectId(), customObjectId(), "unam");
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
	const QString iconFilePath = MetaData::fromFilePath(this->objectData()->source()->sharedData()->worldEditData()->value("WorldEditArt", "UEIcon_UnitCategory"));
	QString iconFile;

	if (source->download(iconFilePath, iconFile, window))
	{
		this->m_icon = QIcon(iconFile);
	}
}

void ObjectTreeItem::setCollapsed(MpqPriorityList *source, QWidget *window)
{
	const QString iconFilePath = MetaData::fromFilePath(this->objectData()->source()->sharedData()->worldEditData()->value("WorldEditArt", "UEIcon_UnitCategory"));
	QString iconFile;

	if (source->download(iconFilePath, iconFile, window))
	{
		this->m_icon = QIcon(iconFile);
	}
}

}

}
