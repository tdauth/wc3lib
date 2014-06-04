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
 * <li>Units/UnitMetaData.slk - defines all meta data</li>
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

		virtual std::streamsize read(InputStream& istream) throw(class Exception) override;
		virtual std::streamsize write(OutputStream& ostream) const throw(class Exception) override;

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
		 * Extracts one single row at \p column.
		 * \return The result is returned as array view.
		 *
		 * @{
		 */
		View& row(int column);
		ConstView row(int column) const;
		/**
		 * @}
		 */

		View& column(int row);
		ConstView column(int row) const;

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

inline Slk::ConstView Slk::row(int column) const
{
	typedef Table::index_range range;
	Table::index_gen indices;
	Table::const_array_view<1>::type myview = this->table()[indices[range(0, this->table().shape()[0])][column] ];

	return myview;
}

inline Slk::ConstView Slk::column(int row) const
{
	typedef Table::index_range range;
	Table::index_gen indices;
	Table::const_array_view<1>::type myview = this->table()[indices[row][range(0, this->table().shape()[1])] ];

	return myview;
}

inline const Slk::Cell& Slk::cell(int row, int column) const
{
	return this->table()[column][row];
}

}

}

#endif
