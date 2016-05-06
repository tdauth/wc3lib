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
#include "mpqeditor.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

MpqTreeModel::MpqTreeModel(MpqPriorityList *source, QObject* parent) : QAbstractItemModel(parent), m_source(source)
{
}

MpqTreeModel::~MpqTreeModel()
{
	foreach (MpqTreeItem *item, m_topLevelItems)
	{
		delete item;
	}
}

void MpqTreeModel::addArchive(mpq::Archive *archive, const mpq::Listfile::Entries &entries)
{
	/*
	 * Create archive row at the end of the current top level items.
	 */
	const int archiveRow = m_topLevelItems.count();

	if (!insertRows(archiveRow, 1, QModelIndex()))
	{
		throw Exception(_("Unable to add archive."));
	}

	/*
	 * Store the index for all items which need it as parent when added.
	 */
	const QModelIndex archiveIndex = index(archiveRow, 0, QModelIndex());

	if (!archiveIndex.isValid())
	{
		throw Exception(_("Unable to get archive item."));
	}

	/*
	 * Store the item as parent for all items which need it as parent when added.
	 */
	MpqTreeItem *archiveItem = this->item(archiveIndex);

	if (archiveItem == nullptr)
	{
		throw Exception(_("Unable to get archive item."));
	}

	/*
	 * Mark the item as archive and set all MPQ data.
	 */
	archiveItem->setType(MpqTreeItem::Type::Archive);
	archiveItem->setArchive(archive);
	archiveItem->setEntries(entries);

	/*
	 * Set MIME type icon to show which file type it is.
	 */
	KMimeType::Ptr mimeType = KMimeType::findByPath(archive->path().c_str());

	if (!mimeType.isNull() && !mimeType->iconName().isEmpty())
	{
		archiveItem->setIcon(m_iconLoader.loadMimeTypeIcon(mimeType->iconName(), KIconLoader::Small));
	}

	/*
	 * Add all items from the archive.
	 * TODO create first directory items which should be much faster. Then add all files.
	 */
	typedef QHash<QString, MpqTreeItem*> DirectoryItems;
	// the directory items which use the complete upper case directory path as key. Paths are separated by the \ character
	DirectoryItems dirItems;
	// all unique directory paths
	const mpq::Listfile::Entries dirEntries = mpq::Listfile::caseSensitiveDirEntries(entries, "", true);

	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, dirEntries)
	{
		const QString dirPath = QString::fromUtf8(ref.c_str());

		if (!dirPath.isEmpty())
		{
			// make sure that only one directory is created even if other cases are used
			const QString dirPathKey = dirPath.toUpper();

			// check if the whole directory item has already been created
			if (dirItems.find(dirPathKey) == dirItems.end())
			{
				// TODO splitting it up and constructing it in var "dirPath" is more expensive than using an index and sub string.
				const QStringList tokens = dirPath.split('\\');
				QModelIndex parent = archiveIndex;
				MpqTreeItem *parentItem = archiveItem;
				QString dirToken;

				// check for every single sub directory if it has been created already
				for (int i = 0; i < tokens.size(); ++i)
				{
					if (i != 0)
					{
						dirToken += '\\';
					}

					dirToken += tokens[i];
					const QString dirTokenKey = dirToken.toUpper();
					DirectoryItems::iterator iterator = dirItems.find(dirTokenKey);

					// if it has not been created yet, create a new sub directory
					if (iterator == dirItems.end())
					{
						const int dirRow = parentItem->childCount();
						insertRows(dirRow, 1, parent);

						const QModelIndex dirIndex = index(dirRow, 0, parent);
						MpqTreeItem *dirItem = item(dirIndex);
						dirItem->setType(MpqTreeItem::Type::Folder);
						dirItem->setArchive(archive);
						dirItem->setFilePath(dirToken);

						// set a folder icon
						if (this->source() != nullptr && this->source()->sharedData().get() != nullptr && this->source()->sharedData()->worldEditData().get() != nullptr)
						{
							dirItem->setIcon(this->source()->sharedData()->worldEditDataIcon("UEIcon_UnitCategory", "WorldEditArt", 0));
						}

						// TODO set other icon when expanded, this has to be done by the signal slot connection of expanded() and collapsed()

						qDebug() << "Dir item:" << dirToken;
						dirItems.insert(dirTokenKey, dirItem);
						parent = dirIndex;
						parentItem = dirItem;
					}
					// otherwise pass the valid parent item
					else
					{
						parentItem = iterator.value();
						parent = parentItem->index(this);
					}
				}
			}
		}
	}

	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, entries)
	{
		const QString filePath = QString::fromUtf8(ref.c_str());

		if (!filePath.isEmpty() && !mpq::Listfile::fileName(ref).empty())
		{
			// NOTE it is important that the last \ is kept as key
			const QString dirpath = QString::fromUtf8(mpq::Listfile::dirPath(ref).c_str());
			qDebug() << "Dir path:" << dirpath;
			MpqTreeItem *parentItem = nullptr;

			if (!dirpath.isEmpty())
			{
				const QString dirPathKey = dirpath.toUpper();
				DirectoryItems::iterator iterator = dirItems.find(dirPathKey);

				if (iterator != dirItems.end())
				{
					parentItem = iterator.value();
				}
				else
				{
					qDebug() << "Missing dir:" << dirpath;
				}
			}
			/*
			* Top level file
			*/
			else
			{
				parentItem = archiveItem;
			}

			if (parentItem != nullptr)
			{
				QModelIndex parent = parentItem->index(this);
				const int fileRow =  parentItem->childCount();
				insertRows(fileRow, 1, parent);
				const QModelIndex fileIndex = this->index(fileRow, 0, parent);
				MpqTreeItem *fileItem = this->item(fileIndex);
				fileItem->setType(MpqTreeItem::Type::File);
				fileItem->setArchive(archive);
				fileItem->setFilePath(filePath);

				/*
				* Set MIME type icon to show which file type it is.
				*/
				KMimeType::Ptr mimeType = KMimeType::findByPath(fileItem->filePath());

				if (!mimeType.isNull() && !mimeType->iconName().isEmpty())
				{
					fileItem->setIcon(m_iconLoader.loadMimeTypeIcon(mimeType->iconName(), KIconLoader::Small));
				}
			}
			//qDebug() << "File" << filePath << "with parent" << parentItem->name();
		}
	}
}

bool MpqTreeModel::removeArchive(mpq::Archive *archive)
{
	bool removed = false;

	for (FileItems::iterator iterator = this->m_topLevelItems.begin(); iterator != this->m_topLevelItems.end() && !removed; ++iterator)
	{
		if ((*iterator)->archive() == archive)
		{
			MpqTreeItem *item = *iterator;
			const QModelIndex index = item->index(this);
			this->removeRows(item->row(this), 1, parent(index));

			removed = true;
		}
	}

	return removed;
}

QVariant MpqTreeModel::data(const QModelIndex &index, int role) const
{
	if (index.isValid())
	{
		MpqTreeItem *item = this->item(index);

		if (item != nullptr)
		{
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
	}
	else
	{
		qDebug() << "index is not valid row" << index.row();
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

QModelIndex MpqTreeModel::parent(const QModelIndex &child) const
{
	if (child.isValid())
	{
		MpqTreeItem *item = this->item(child);
		qDebug() << "Item: " << item;
		MpqTreeItem *parentItem = item->parent();

		if (parentItem != nullptr)
		{
			const int parentRow = parentItem->row(this);

			return createIndex(parentRow, 0, parentItem);
		}
	}

	return QModelIndex();
}

QModelIndex MpqTreeModel::index(int row, int column, const QModelIndex &parent) const
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

MpqTreeItem* MpqTreeModel::item(const QModelIndex &index) const
{
	if (index.isValid())
	{
		return static_cast<MpqTreeItem*>(index.internalPointer());
	}

	return nullptr;
}

bool MpqTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
	beginInsertRows(parent, row, row + count - 1);

	MpqTreeItem *parentItem = nullptr;

	if (parent.isValid())
	{
		parentItem = item(parent);
	}
	else
	{
		qDebug() << "Insert top level row:" << row << "with count" << count;
	}

	for (int i = row; i < row + count; ++i)
	{
		MpqTreeItem *item = new MpqTreeItem(parentItem);

		if (parentItem != nullptr)
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
			qDebug() << "No parent";

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

bool MpqTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
	qDebug() << "Removing row" << row << "with" << count << "items from parent row" << parent.row();

	beginRemoveRows(parent, row, row + count - 1);

	MpqTreeItem *parentItem = nullptr;

	if (parent.isValid())
	{
		parentItem = item(parent);
	}

	for (int i = row; i < row + count; ++i)
	{
		MpqTreeItem *item = nullptr;

		if (parentItem != nullptr)
		{
			if (i < parentItem->childCount())
			{
				item = parentItem->children().takeAt(i);
			}
			else
			{
				qDebug() << "Invalid children" << i;
			}
		}
		else
		{
			if (i < topLevelItems().count())
			{
				item = topLevelItems().takeAt(i);
				qDebug() << "Removing top level item" << item->filePath();
			}
			else
			{
				qDebug() << "Invalid top level row" << i;
			}
		}

		if (item != nullptr)
		{
			/*
			 * Should delete its children as well.
			 */
			delete item;
			item = nullptr;
		}
	}

	endRemoveRows();

	return true;
}

QVariant MpqTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
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

MpqTreeItem* MpqTreeModel::topLevelItem(const mpq::Archive* archive)
{
	foreach (MpqTreeItem *item, this->topLevelItems())
	{
		if (item->archive() == archive)
		{
			return item;
		}
	}

	return 0;
}

}

}
