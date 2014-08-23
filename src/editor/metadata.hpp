/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_METADATA_HPP
#define WC3LIB_EDITOR_METADATA_HPP

#include <unordered_map>

#include <kdemacros.h>

#include "resource.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief Base class for all possible meta data formats (units, abilities, upgrades etc.).
 *
 * Uses \ref map::Slk to load a SYLK file which contains all meta data.
 * In addition to \ref map::Slk it stores columns and rows by keys (their first cell values) through hashing.
 * This allows fast access of columns or rows by keys which is usually required for meta data.
 *
 * \todo Remove unnecessary wrapper methods which are duplicated from \ref map::Slk.
 */
class KDE_EXPORT MetaData : public Resource
{
	public:
		/**
		 * Values of the first cells are used to get the corresponding number of row or column.
		 */
		typedef std::unordered_map<map::Slk::Cell, int> Keys;

		MetaData(const KUrl &url);

		virtual void clear() throw ();

		virtual void load() throw (Exception);
		virtual void reload() throw (Exception);
		virtual void save(const KUrl &url) const throw (Exception)
		{
			throw Exception(_("Saving meta data is not supported yet."));
		}

		map::Slk& slk();
		const map::Slk& slk() const;
		const Keys& columnKeys() const;
		const Keys& rowKeys() const;
		map::Slk::ConstView row(const map::Slk::Cell &key) const;
		map::Slk::ConstView column(const map::Slk::Cell &key) const;

		const map::Slk::Cell& value(int row, const map::Slk::Cell &columnKey) const;
		const map::Slk::Cell& value(const map::Slk::Cell &rowKey, int column) const;
		const map::Slk:: Cell& value(const map::Slk::Cell &rowKey, const map::Slk::Cell &columnKey) const;

		bool hasValue(const map::Slk::Cell &rowKey, const map::Slk::Cell &columnKey) const;

		bool isEmpty() const;

	protected:
		map::Slk m_slk;
		Keys m_columnKeys;
		Keys m_rowKeys;
};

inline map::Slk& MetaData::slk()
{
	return this->m_slk;
}

inline const map::Slk& MetaData::slk() const
{
	return this->m_slk;
}

inline const MetaData::Keys& MetaData::columnKeys() const
{
	return this->m_columnKeys;
}

inline const MetaData::Keys& MetaData::rowKeys() const
{
	return this->m_rowKeys;
}

inline map::Slk::ConstView MetaData::row(const map::Slk::Cell &key) const
{
	Keys::const_iterator iterator = this->rowKeys().find(key);

	if (iterator == this->rowKeys().end())
	{
		throw Exception(boost::format(_("Row %1% does not exist.")) % key);
	}

	return this->slk().row(iterator->second);
}

inline map::Slk::ConstView MetaData::column(const map::Slk::Cell &key) const
{
	Keys::const_iterator iterator = this->columnKeys().find(key);

	if (iterator == this->columnKeys().end())
	{
		throw Exception();
	}

	return this->slk().column(iterator->second);
}

inline const map::Slk::Cell& MetaData::value(int row, const map::Slk::Cell &columnKey) const
{
	Keys::const_iterator iterator = this->columnKeys().find(columnKey);

	if (iterator == this->columnKeys().end())
	{
		throw Exception();
	}

	return this->slk().table()[iterator->second][row];
}

inline const map::Slk::Cell& MetaData::value(const map::Slk::Cell &rowKey, int column) const
{
	Keys::const_iterator iterator = this->rowKeys().find(rowKey);

	if (iterator == this->rowKeys().end())
	{
		throw Exception();
	}

	return this->slk().table()[column][iterator->second];
}

inline const map::Slk::Cell& MetaData::value(const map::Slk::Cell &rowKey, const map::Slk::Cell &columnKey) const
{
	Keys::const_iterator columnIterator = this->columnKeys().find(columnKey);

	if (columnIterator == this->columnKeys().end())
	{
		throw Exception();
	}

	Keys::const_iterator rowIterator = this->rowKeys().find(rowKey);

	if (rowIterator == this->rowKeys().end())
	{
		throw Exception();
	}

	return this->slk().table()[columnIterator->second][rowIterator->second];
}

inline bool MetaData::hasValue(const map::Slk::Cell &rowKey, const map::Slk::Cell &columnKey) const
{
	Keys::const_iterator columnIterator = this->columnKeys().find(columnKey);

	if (columnIterator == this->columnKeys().end())
	{
		return false;
	}

	Keys::const_iterator rowIterator = this->rowKeys().find(rowKey);

	if (rowIterator == this->rowKeys().end())
	{
		return false;
	}

	return true;
}

inline bool MetaData::isEmpty() const
{
	return this->m_slk.table().empty();
}

}

}

#endif
