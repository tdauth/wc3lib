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

#include <QtCore>

#include "metadata.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

MetaData::MetaData(const KUrl &url) : Resource(url, Type::MetaData), m_source(map::Slk())
{
}

void MetaData::clear() throw ()
{
	if (hasSlk())
	{
		this->slk().clear();
		this->m_rowKeys.clear();
		this->m_columnKeys.clear();
	}
	else
	{
		this->txt().sections().clear();
	}
}

void MetaData::load() throw (Exception)
{
	QString filePath;

	if (!this->source()->download(this->url(), filePath, 0))
	{
		throw Exception();
	}

	ifstream in(filePath.toUtf8().constData());

	if (!in)
	{
		throw Exception();
	}

	/*
	 * Make rows and columns accessable over their first column/row values.
	 */
	this->m_columnKeys.clear();
	this->m_rowKeys.clear();
	this->m_sectionKeys.clear();

	QString suffix = QFileInfo(this->url().fileName()).suffix().toLower();

	if (suffix == "slk")
	{
		this->m_source = map::Slk();
		this->slk().read(in);

		/*
		 * Store indices of rows and columns by the value of the first cell.
		 */
		for (map::Slk::Table::size_type column = 0; column < this->slk().table().shape()[0]; ++column)
		{
			map::Slk::Cell &firstColumnCell = this->slk().table()[column][0];
			this->m_columnKeys[QString::fromUtf8(firstColumnCell.c_str())] = column;
		}

		for (map::Slk::Table::size_type row = 0; row < this->slk().table().shape()[1]; ++row)
		{
			map::Slk::Cell &firstRowCell = this->slk().table()[0][row];
			this->m_rowKeys[QString::fromUtf8(firstRowCell.c_str())] = row;
		}
	}
	else if (suffix == "txt")
	{
		this->m_source = map::Txt();
		this->txt().read(in);

		for (map::Txt::Sections::iterator iterator = this->txt().sections().begin(); iterator != this->txt().sections().end(); ++iterator)
		{
			this->m_sectionKeys[QString::fromUtf8(iterator->name.c_str())] = &(*iterator);
		}
	}
}

void MetaData::reload() throw (Exception)
{
	clear();
	load();
}

}

}
