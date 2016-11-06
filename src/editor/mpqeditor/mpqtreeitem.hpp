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

#ifndef WC3LIB_EDITOR_MPQTREEITEM_HPP
#define WC3LIB_EDITOR_MPQTREEITEM_HPP

#include <QObject>
#include <QList>
#include <QString>
#include <QIcon>
#include <QAbstractItemModel>

#include "../../mpq.hpp"

namespace wc3lib
{

namespace editor
{

class MpqTreeModel;

/**
 * \brief One single item of a \ref MpqTreeModel which might represent a file, an archive or a folder.
 */
class MpqTreeItem : public QObject
{
	public:
		enum class Type
		{
			File,
			Folder,
			Archive
		};

		typedef QList<MpqTreeItem*> FileItems;

		MpqTreeItem(MpqTreeItem *parent);

		MpqTreeItem* parent() const;

		void setArchive(mpq::Archive *archive);
		mpq::Archive* archive() const;

		void setEntries(const mpq::Listfile::Entries &entries);
		const mpq::Listfile::Entries& entries() const;

		void setFilePath(const QString &filePath);
		const QString& filePath() const;

		void setIcon(const QIcon &icon);
		const QIcon& icon() const;

		void appendChild(MpqTreeItem *item);
		int childCount() const;
		const FileItems& children() const;
		FileItems& children();

		void setType(Type type);
		Type type() const;
		bool isFolder() const;
		bool isArchive() const;
		bool isFile() const;

		QString name() const;
		QString dirname() const;
		/**
		 * \return Returns the base name (file name without extension) of a listfile entry \p path.
		 */
		QString basename() const;
		QString filename() const;

		int row(const MpqTreeModel *model) const;
		QModelIndex index(MpqTreeModel *model);

	private:
		MpqTreeItem *m_parent;
		mpq::Archive *m_archive;
		mpq::Listfile::Entries m_entries;
		QString m_filePath;
		QIcon m_icon;
		FileItems m_children;

		Type m_type;
};

inline MpqTreeItem* MpqTreeItem::parent() const
{
	return this->m_parent;
}

inline void MpqTreeItem::setArchive(mpq::Archive *archive)
{
	this->m_archive = archive;
}

inline mpq::Archive* MpqTreeItem::archive() const
{
	return this->m_archive;
}

inline void MpqTreeItem::setEntries(const mpq::Listfile::Entries &entries)
{
	this->m_entries = entries;
}

inline const mpq::Listfile::Entries& MpqTreeItem::entries() const
{
	return this->m_entries;
}

inline void MpqTreeItem::setIcon(const QIcon &icon)
{
	this->m_icon = icon;
}

inline const QIcon& MpqTreeItem::icon() const
{
	return this->m_icon;
}

inline QString MpqTreeItem::name() const
{
	if (isFolder())
	{
		return filename();
	}
	else if (isArchive())
	{
		return QString::fromStdString(archive()->path().filename().string());
	}
	else if (isFile())
	{
		return filename();
	}

	return QString("Invalid item");
}

inline void MpqTreeItem::setFilePath(const QString &filePath)
{
	this->m_filePath = filePath;
}

inline const QString& MpqTreeItem::filePath() const
{
	return this->m_filePath;
}

inline void MpqTreeItem::appendChild(MpqTreeItem *item)
{
	this->m_children.append(item);
}

inline int MpqTreeItem::childCount() const
{
	return this->m_children.count();
}

inline MpqTreeItem::FileItems& MpqTreeItem::children()
{
	return this->m_children;
}

inline const MpqTreeItem::FileItems& MpqTreeItem::children() const
{
	return this->m_children;
}

inline void MpqTreeItem::setType(MpqTreeItem::Type type)
{
	this->m_type = type;
}

inline MpqTreeItem::Type MpqTreeItem::type() const
{
	return this->m_type;
}

inline bool MpqTreeItem::isFolder() const
{
	return this->type() == Type::Folder;
}

inline bool MpqTreeItem::isArchive() const
{
	return this->type() == Type::Archive;
}

inline bool MpqTreeItem::isFile() const
{
	return this->type() == Type::File;
}

}

}

#endif // WC3LIB_EDITOR_MPQTREEITEM_HPP
