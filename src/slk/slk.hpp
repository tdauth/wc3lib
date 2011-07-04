/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_SLK_SLK_HPP
#define WC3LIB_SLK_SLK_HPP

#include "../format.hpp"
#include "../map/platform.hpp"
#include "slkitemkey.hpp"
#include "slkitems.hpp"

namespace wc3lib
{

namespace slk
{

/**
* Default model and texture meta data files:
* <ul>
* <li>"UI/DestructableMetaData.slk"</li>
* <li>"UI/UnitMetaData.slk"</li>
* </ul>
* Default class for SLK files like CliffTypes.slk.
* "UI/SkinMetaData.slk" - model data
* \todo Should use a Bison file which defines the possible syntax for these files.
* 
* Eine SLK-Datei besteht aus einer Reihe von vordefinierten Spalten. Jede Spalte hat einen bestimmten zugehörigen Datentyp. Dieser wird in der ersten Zeile des Dokuments definiert.
* Die Datensätze existieren in Form von Zeilen.
*/
class Slk : public Format
{
	public:
		Slk();
		virtual ~Slk();

		virtual std::streamsize read(istream &istream) throw (class Exception);
		virtual std::streamsize write(ostream &ostream) const throw (class Exception);
		
		const class SlkColumns& columns() const;
		const class SlkRows& rows() const;
		const class SlkItems& items() const;
		
		const class SlkItems items(const class SlkColumn *column) const;
		const class SlkItems items(const class SlkRow *row) const;
		
		const class SlkItem* item(const SlkItemKey &key) const;
		const class SlkColumn* itemColumn(const SlkItem *item) const;
		const class SlkRow* itemRow(const SlkItem *item) const;
		const ItemKey& itemKey(const SlkItem *item) const;

	protected:
		class SlkColumns *m_columns; // header columns
		class SlkRows *m_rows; // row entries
		class SlkItems *m_items; // all items
};

inline const class SlkColumns& Slk::columns() const
{
	return *m_columns;
}

inline const class SlkRows& Slk::rows() const
{
	return *m_rows;
}

inline const class SlkItems& Slk::items() const
{
	return *m_items;
}

}

}

#endif
