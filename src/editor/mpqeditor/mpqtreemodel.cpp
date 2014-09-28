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

#include <KMimeType>
#include <KIconLoader>

#include "mpqtreemodel.hpp"
#include "mpqtreeitem.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

MpqTreeModel::MpqTreeModel(QObject* parent) : QAbstractItemModel(parent)
{
}

MpqTreeModel::~MpqTreeModel()
{
	foreach (MpqTreeItem *item, m_topLevelItems)
	{
		delete item;
	}
}

void MpqTreeModel::addArchive(mpq::Archive* archive, const mpq::Listfile::Entries &entries)
{
	/*
	 * Create archive row.
	 */
	const int archiveRow = m_topLevelItems.count();

	if (!insertRows(archiveRow, 1, QModelIndex()))
	{
		throw Exception(_("Unable to add archive."));
	}

	const QModelIndex archiveIndex = index(archiveRow, 0, QModelIndex());

	if (!archiveIndex.isValid())
	{
		throw Exception(_("Unable to get archive item."));
	}

	MpqTreeItem *archiveItem = this->item(archiveIndex);

	if (archiveItem == 0)
	{
		throw Exception(_("Unable to get archive item."));
	}

	archiveItem->setIsArchive(true);
	archiveItem->setArchive(archive);
	archiveItem->setEntries(entries);

	/*
	 * Add all items from the archive.
	 */
	typedef QHash<QString, MpqTreeItem*> DirectoryItems;
	DirectoryItems dirItems;

	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, entries)
	{
		const QString entryPath = QString::fromUtf8(ref.c_str());
		const QStringList tokens = entryPath.split('\\');
		QModelIndex parent = index(archiveRow, 0);
		MpqTreeItem *parentItem = this->item(parent);

		if (parentItem == 0)
		{
			throw Exception(_("Error on getting parent item."));
		}

		QString filePath;

		for (int i = 0; i < tokens.size() - 1; ++i)
		{
			const QString key = tokens[i].toUpper();
			DirectoryItems::iterator iterator = dirItems.find(key);

			/*
			 * If directory token does not already exist create the directory row at the correct dir tree level.
			 */
			if (iterator == dirItems.end())
			{
				const int dirRow = parentItem->childCount();
				insertRows(dirRow, 1, parent);

				const QModelIndex dirIndex = index(dirRow, 0, parent);
				MpqTreeItem *dirItem = item(dirIndex);
				dirItem->setIsFolder(true);
				dirItem->setArchive(archive);
				filePath += tokens[i];
				dirItem->setFilePath(filePath);

				if (this->source()->sharedData()->worldEditData().get() != 0)
				{
					dirItem->setIcon(this->source()->sharedData()->worldEditDataIcon("UEIcon_UnitCategory", "WorldEditArt", 0));
				}

				iterator = dirItems.insert(key, dirItem);
			}
			else
			{
				filePath += iterator.value()->name();
			}

			parent = iterator.value()->index(this);
			parentItem = iterator.value();
			filePath += '\\';
		}

		filePath += tokens.back();

		const int fileRow = parentItem->childCount();
		insertRows(fileRow, 1, parent);
		const QModelIndex fileIndex = this->index(fileRow, 0, parent);
		MpqTreeItem *fileItem = this->item(fileIndex);
		fileItem->setIsFile(true);
		fileItem->setArchive(archive);
		fileItem->setFilePath(filePath);

		/*
		 * Set MIME type icon to show which file type it is.
		 */
		KMimeType::Ptr mimeType = KMimeType::findByPath(filePath);

		if (!mimeType.isNull() && !mimeType->iconName().isEmpty())
		{
			// TODO performance might be improved?
			KIconLoader iconLoader;
			//qDebug() << "Has icon:" << mimeType->iconName();
			QPixmap pixmap = iconLoader.loadMimeTypeIcon(mimeType->iconName(), KIconLoader::Small);
			fileItem->setIcon(QIcon(pixmap));
		}

		//qDebug() << "File" << filePath << "with parent" << parentItem->name();
	}
}

void editor::MpqTreeModel::removeArchive(mpq::Archive* archive)
{
	for (FileItems::iterator iterator = this->m_topLevelItems.begin(); iterator != this->m_topLevelItems.end(); ++iterator)
	{
		if ((*iterator)->archive() == archive)
		{
			MpqTreeItem *item = *iterator;
			const QModelIndex index = item->index(this);
			this->removeRows(item->row(this), 1, parent(index));

			break;
		}
	}
}

QVariant MpqTreeModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid())
	{
		MpqTreeItem *item = this->item(index);

		switch (role)
		{
			case Qt::DisplayRole:
			{
				return item->name();
			}

			case Qt::DecorationRole:
			{
				return item->icon();
			}
		}
	}

	return QVariant();
}

int MpqTreeModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

int MpqTreeModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		MpqTreeItem *item = this->item(parent);

		return item->childCount();
	}
	else
	{
		return topLevelItems().count();
	}

	return 0;
}

QModelIndex MpqTreeModel::parent(const QModelIndex& child) const
{
	if (child.isValid())
	{
		MpqTreeItem *item = this->item(child);

		if (item->parent() != 0)
		{
			return createIndex(item->row(this), 0, item->parent());
		}
	}

	return QModelIndex();
}

QModelIndex MpqTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
	{
		return QModelIndex();
	}

	if (parent.isValid())
	{
		MpqTreeItem *parentItem = this->item(parent);

		return createIndex(row, column, parentItem->children()[row]);
	}
	else if (row < topLevelItems().size())
	{
		MpqTreeItem *item = this->topLevelItems()[row];

		return createIndex(row, column, item);
	}

	return QModelIndex();
}

MpqTreeItem* MpqTreeModel::item(const QModelIndex& index) const
{
	if (index.isValid())
	{
		return (MpqTreeItem*)index.internalPointer();
	}

	return 0;
}

bool editor::MpqTreeModel::insertRows(int row, int count, const QModelIndex& parent)
{
	beginInsertRows(parent, row, row + count - 1);

	MpqTreeItem *parentItem = 0;

	if (parent.isValid())
	{
		parentItem = item(parent);
	}

	for (int i = row; i < row + count; ++i)
	{
		MpqTreeItem *item = new MpqTreeItem(parentItem);

		if (parentItem != 0)
		{
			if (i >= parentItem->children().size())
			{
				parentItem->children().push_back(item);
			}
			else
			{
				parentItem->children()[i] = item;
			}
		}
		else
		{
			if (i >= m_topLevelItems.size())
			{
				m_topLevelItems.push_back(item);
			}
			else
			{
				m_topLevelItems[i] = item;
			}
		}
	}

	endInsertRows();

	return true;
}

bool editor::MpqTreeModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows(parent, row, row + count - 1);

	MpqTreeItem *parentItem = 0;

	if (parent.isValid())
	{
		parentItem = item(parent);
	}

	for (int i = row; i < row + count; ++i)
	{
		MpqTreeItem *item = 0;

		if (parentItem != 0)
		{
			if (i < parentItem->childCount())
			{
				item = parentItem->children().takeAt(row);
			}
		}
		else
		{
			if (i < topLevelItems().count())
			{
				item = topLevelItems().takeAt(row);
			}
		}

		if (parentItem != 0)
		{
			delete parentItem;
		}
		else
		{
			return false;
		}
	}

	endRemoveRows();

	return true;
}

QVariant editor::MpqTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal)
	{
		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (section)
				{
					case 0:
					{
						return QObject::tr("File name");
					}

					case 1:
					{
						return QObject::tr("File size");
					}
				}
			}
		}
	}

	return QVariant();
}

}

}
