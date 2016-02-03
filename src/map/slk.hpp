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

#ifndef WC3LIB_MAP_SLK_HPP
#define WC3LIB_MAP_SLK_HPP

#include <boost/multi_array.hpp>

#include "../format.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief Basic class to parse and generate .SLK (SYLK) files which are used to define meta data in Warcraft III.
 *
 * This class allows parsing as well as generating SYLK files which can be compared to any spreadsheet formats (like Excel or OpenOffice).
 * The SYLK format is heavily used for object editor data definition in Warcraft III but also for other meta data like sounds or
 * weather effects.
 * In the default Warcraft III: Reign of Chaos installation there exist several files in the "war3.mpq" archive which define
 * meta data:
 * <ul>
 * <li>Units/UnitMetaData.slk - defines all meta data for units</li>
 * <li>UI/SkinMetaData.slk - defines all selectable icons and skins</li>
 * </ul>
 *
 * For Warcraft III: The Frozen Throne additional meta data files exist in th "war3x.mpq" archive:
 * <ul>
 * <li>Units
 * </ul>
 *
 * Meta data specifies the fields of handles (like "Unit" or "Sound"). It defines which values can be assigned to those fields.
 *
 * Use \ref read() to parse data from an input stream. The resulting cells are stored in a table which can be accessed using \ref table().
 * The table is a two-dimensional array with first coordinate for columns (X) and second coordinate for rows (Y).
 *
 * Rows and columns can be extracted using \ref row() and \ref column(). They are returned as array views supported by the Boost "multiarray" library.
 *
 * Single cells can be extracted using \ref cell().
 *
 * A new SYLK file can be generated using \ref write() with an output stream.
 *
 * \note This class only provides simple read and write functionalities. For fast indexing you have to use your own containers.
 *
 * \sa http://en.wikipedia.org/wiki/SYmbolic_LinK_(SYLK)
 *
 * \ingroup objectdata
 */
class Slk : public Format
{
	public:
		/**
		 * One single cell of the SYLK table can hold one single value and supports multiple types.
		 */
		typedef string Cell;
		/**
		 * A table is a 2-dimensional array of cells.
		 * First index is column (x) and second index is row (y).
		 */
		typedef boost::multi_array<Cell, 2> Table;

		typedef Table::array_view<1>::type View;
		typedef Table::const_array_view<1>::type ConstView;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

		/**
		 * Allows access to the whole table of the SLK sheet.
		 * Cells are stored in a 2-dimensional array of cells.
		 * The first index is the column (x) and the second index is the row (y).
		 *
		 * \return Returns the whole table with all cells.
		 *
		 * @{
		 */
		Table& table();
		const Table& table() const;
		/**
		 * @}
		 */

		/**
		 * The size of an SLK table where the first value is the rows and the second value is the columns.
		 */
		typedef std::pair<Table::size_type, Table::size_type> TableSize;

		/**
		 * Resizes the table to the new size of \p size.
		 */
		void resizeTable(const TableSize &size);

		/**
		 * Clears the table.
		 */
		void clear();

		/**
		 * \return Returns the maximum number of columns of the SLK table.
		 *
		 * \note Colums are the first dimension (x) of the multi array \ref Table.
		 */
		Table::size_type columns() const;

		/**
		 * \return Returns the maximum number of rows of the SLK table.
		 *
		 * \note Rows are the second dimension (y) of the multi array \ref Table.
		 */
		Table::size_type rows() const;

		/**
		 * Extracts one single column at \p column.
		 * \return The result is returned as array view.
		 *
		 * @{
		 */
		View& column(int column);
		ConstView column(int column) const;
		/**
		 * @}
		 */

		View& row(int row);
		ConstView row(int row) const;

		Cell& cell(int row, int column);
		const Cell& cell(int row, int column) const;

	protected:
		Table m_table;
};

inline Slk::Table& Slk::table()
{
	return this->m_table;
}

inline const Slk::Table& Slk::table() const
{
	return this->m_table;
}

inline void Slk::clear()
{
	m_table.resize(boost::extents[0][0]);
}

inline Slk::Table::size_type Slk::columns() const
{
	return this->table().shape()[0];
}

inline Slk::Table::size_type Slk::rows() const
{
	return this->table().shape()[1];
}


inline Slk::ConstView Slk::column(int column) const
{
	typedef Table::index_range range;
	Table::index_gen indices;
	Table::const_array_view<1>::type myview = this->table()[indices[column][range(0, this->table().shape()[1])] ];

	return myview;
}

inline Slk::ConstView Slk::row(int row) const
{
	typedef Table::index_range range;
	Table::index_gen indices;
	Table::const_array_view<1>::type myview = this->table()[indices[range(0, this->table().shape()[0])][row] ];

	return myview;
}

inline Slk::Cell& Slk::cell(int row, int column)
{
	return this->table()[column][row];
}

inline const Slk::Cell& Slk::cell(int row, int column) const
{
	return this->table()[column][row];
}

}

}

#endif
