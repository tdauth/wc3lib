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

#include "mpqtreeitem.hpp"
#include "mpqtreemodel.hpp"

namespace wc3lib
{

namespace editor
{

MpqTreeItem::MpqTreeItem(MpqTreeItem* parent) : QObject(parent), m_parent(parent), m_archive(0), m_isFolder(false), m_isArchive(false), m_isFile(false)
{
}

int MpqTreeItem::row(const MpqTreeModel *model) const
{
	if (parent() != 0)
	{
		return parent()->children().indexOf(const_cast<MpqTreeItem*>(this));
	}

	return model->topLevelItems().indexOf(const_cast<MpqTreeItem*>(this));
}


QModelIndex MpqTreeItem::index(MpqTreeModel *model)
{
	QModelIndex parentIndex;

	if (parent() != 0)
	{
		parentIndex = parent()->index(model);
	}

	return model->index(row(model), 0, parentIndex);
}

QString MpqTreeItem::dirname() const
{
	const int lastIndex = filePath().lastIndexOf('\\');

	if (lastIndex > 0)
	{
		const QString result = filePath().mid(0, lastIndex);
		const int secondLastIndex = result.lastIndexOf('\\');

		if (secondLastIndex != -1 && secondLastIndex < result.size() - 1)
		{
			return result.mid(secondLastIndex + 1);
		}
		// top level dir
		else
		{
			return result;
		}
	}

	return QString();
}

QString MpqTreeItem::basename() const
{
	QString fileName = filename();

	const int lastIndex = fileName.lastIndexOf('.');

	if (lastIndex != -1)
	{
		if (fileName.size() > lastIndex)
		{
			return fileName.mid(lastIndex + 1);
		}

		return QString();
	}

	return fileName;
}

QString MpqTreeItem::filename() const
{
	const int lastIndex = filePath().lastIndexOf('\\');

	if (lastIndex != -1)
	{
		if (filePath().size() > lastIndex)
		{
			return filePath().mid(lastIndex + 1);
		}

		return QString();
	}

	return filePath();
}

}

}
