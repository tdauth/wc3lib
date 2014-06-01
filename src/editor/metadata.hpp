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

#include <boost/multi_array.hpp>

#include "resource.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Base class for all possible meta data formats (units, abilities, upgrades etc.).
 * \todo Add more abstract class which is the base of water data, sound data etc. as well.
 * Resource URL should refer a SYLK file.
 */
class MetaData : public Resource
{
	public:
		/**
		 * One single cell of the SYLK table can hold one single value and supports multiple types.
		 */
		typedef QString Cell;
		/**
		 * A table is a 2-dimensional array of cells.
		 * First index is column (x) and second index is row (y).
		 */
		typedef boost::multi_array<Cell, 2> Table;

		typedef Table::array_view<1>::type View;
		typedef Table::const_array_view<1>::type ConstView;

		/**
		 * Values of the first cells are used to get the corresponding number of row or column.
		 */
		typedef QHash<Cell, int> Keys;

		MetaData(const KUrl &url);

		virtual void clear() throw ();

		/**
		 * Provides simple SYLK file parser functionality.
		 */
		virtual void load() throw (Exception);
		virtual void reload() throw (Exception);
		virtual void save(const KUrl &url) const throw (Exception)
		{
			throw Exception(_("Saving meta data is not supported yet."));
		}

		const Table& table() const;
		const Keys& columnKeys() const;
		const Keys& rowKeys() const;
		ConstView row(const Cell &key) const;
		ConstView column(const Cell &key) const;
		ConstView row(int index) const;
		ConstView column(int index) const;

		const Cell& value(int row, int column) const;
		const Cell& value(int row, const Cell &columnKey) const;
		const Cell& value(const Cell &rowKey, int column) const;
		const Cell& value(const Cell &rowKey, const Cell &columnKey) const;

	protected:
		Table m_table;
		Keys m_columnKeys;
		Keys m_rowKeys;
};

inline const MetaData::Table& MetaData::table() const
{
	return this->m_table;
}

inline const MetaData::Keys& MetaData::columnKeys() const
{
	return this->m_columnKeys;
}

inline const MetaData::Keys& MetaData::rowKeys() const
{
	return this->m_rowKeys;
}

inline MetaData::ConstView MetaData::row(const Cell& key) const
{
	return this->row(this->rowKeys()[key]);
}

inline MetaData::ConstView MetaData::row(int index) const
{
	typedef Table::index_range range;
	Table::index_gen indices;
	Table::const_array_view<1>::type myview = this->table()[indices[range(0, this->table().shape()[0])][index] ];

	return myview;
}

inline MetaData::ConstView MetaData::column(const Cell& key) const
{
	return this->column(this->columnKeys()[key]);
}

inline MetaData::ConstView MetaData::column(int index) const
{
	typedef Table::index_range range;
	Table::index_gen indices;
	Table::const_array_view<1>::type myview = this->table()[indices[index][range(0, this->table().shape()[1])] ];

	return myview;
}

inline const MetaData::Cell& MetaData::value(int row, int column) const
{
	return this->table()[column][row];
}

inline const MetaData::Cell& MetaData::value(int row, const MetaData::Cell& columnKey) const
{
	return this->table()[this->columnKeys()[columnKey]][row];
}

inline const MetaData::Cell& MetaData::value(const MetaData::Cell& rowKey, int column) const
{
	return this->table()[column][this->rowKeys()[rowKey]];
}

inline const MetaData::Cell& MetaData::value(const MetaData::Cell& rowKey, const MetaData::Cell& columnKey) const
{
	return this->table()[this->columnKeys()[columnKey]][this->rowKeys()[rowKey]];
}

}

}

#endif
