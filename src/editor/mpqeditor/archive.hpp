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

#ifndef WC3LIB_EDITOR_MPQEDITOR_ARCHIVE_HPP
#define WC3LIB_EDITOR_MPQEDITOR_ARCHIVE_HPP

#include <QTreeWidgetItem>

#include <KUrl>

#include "../../mpq.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief Stores all necessary data for an open archive in th MPQ Editor.
 */
class Archive
{
	public:
		Archive();

		/**
		 * \return Returns the corresponding actual archive.
		 */
		mpq::Archive& archive();

		/**
		 * Sets the tree widget item of the archive in the MPQ Editor.
		 */
		void setItem(QTreeWidgetItem *item);
		QTreeWidgetItem* item() const;

		void deleteItem();

		/**
		 * Sets the valid listfile entries which were used when opening the archive.
		 */
		void setEntries(const mpq::Listfile::Entries &entries);
		const mpq::Listfile::Entries& entries() const;

	private:
		mpq::Archive m_archive;
		QTreeWidgetItem *m_item;
		mpq::Listfile::Entries m_entries;
};

inline mpq::Archive& Archive::archive()
{
	return this->m_archive;
}

inline void Archive::setItem(QTreeWidgetItem *item)
{
	this->m_item = item;
}

inline QTreeWidgetItem* Archive::item() const
{
	return this->m_item;
}

inline void Archive::deleteItem()
{
	if (item() != 0)
	{
		delete m_item;
		m_item = 0;
	}
}

inline void Archive::setEntries(const mpq::Listfile::Entries &entries)
{
	this->m_entries = entries;
}

inline const mpq::Listfile::Entries& Archive::entries() const
{
	return this->m_entries;
}

}

}

#endif
