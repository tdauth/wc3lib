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

#include <KIO/NetAccess>

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

void MetaData::load()
{
	QString filePath;

	if (!this->source()->download(this->url(), filePath, 0))
	{
		throw Exception(boost::format(_("Error on downloading file %1%: %2%")) % this->url().toLocalFile().toUtf8().constData() % KIO::NetAccess::lastErrorString().toUtf8().constData());
	}

	ifstream in(filePath.toUtf8().constData());

	if (!in)
	{
		throw Exception(boost::format(_("Unable to open downloaded file %1%")) % filePath.constData());
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
			this->m_columnKeys[MetaData::fromSlkString(QString::fromUtf8(firstColumnCell.c_str()))] = column;
		}

		for (map::Slk::Table::size_type row = 0; row < this->slk().table().shape()[1]; ++row)
		{
			map::Slk::Cell &firstRowCell = this->slk().table()[0][row];
			this->m_rowKeys[MetaData::fromSlkString(QString::fromUtf8(firstRowCell.c_str()))] = row;
		}
	}
	else if (suffix == "txt")
	{
		this->m_source = map::Txt();
		this->txt().read(in);

		for (map::Txt::Sections::iterator iterator = this->txt().sections().begin(); iterator != this->txt().sections().end(); ++iterator)
		{
			map::Txt::Section *section = &(*iterator);
			this->m_sectionKeys[QString::fromUtf8(iterator->name.c_str())] = section;

			for (map::Txt::Entries::iterator entryIterator = iterator->entries.begin(); entryIterator != iterator->entries.end(); ++entryIterator)
			{
				map::Txt::Entry *entry = &(*entryIterator);
				const TxtEntryKey entryKey = TxtEntryKey(section, QString::fromUtf8(entry->first.c_str()));
				this->m_entryKeys[entryKey] = entry;
			}
		}
	}
}

void MetaData::save(const KUrl &url) const
{
	QTemporaryFile file;

	if (file.open())
	{
		ofstream out(file.fileName().toUtf8().constData());

		if (hasSlk())
		{
			this->slk().write(out);
		}
		else
		{
			this->txt().write(out);
		}

		out.close();
		file.close();

		if (!this->source()->upload(file.fileName(), url, 0))
		{
			throw Exception(KIO::NetAccess::lastErrorString().toUtf8().constData());
		}
	}

	throw Exception(boost::format(_("Error on opening temporary file %1%.")) % file.fileName().toUtf8().constData());
}

void MetaData::reload()
{
	clear();
	load();
}

QString MetaData::value(const QString &rowKey, const QString &columnKey) const
{
	if (hasSlk())
	{
		SlkKeys::const_iterator columnIterator = this->columnKeys().find(columnKey);

		if (columnIterator == this->columnKeys().end())
		{
			throw Exception(boost::format(_("Column %1% not found.")) % columnKey.toUtf8().constData());
		}

		SlkKeys::const_iterator rowIterator = this->rowKeys().find(rowKey);

		if (rowIterator == this->rowKeys().end())
		{
			throw Exception(boost::format(_("Row %1% not found.")) % rowKey.toUtf8().constData());
		}

		const map::Slk::Table::size_type column = columnIterator.value();
		const map::Slk::Table::size_type row = rowIterator.value();

		if (column < this->slk().columns() && row < this->slk().rows())
		{
			return fromSlkString(QString::fromUtf8(this->slk().table()[column][row].c_str()));
		}
		else
		{
			throw Exception(boost::format(_("%1%|%2% is out of range.")) % column % row);
		}
	}
	else if (hasTxt())
	{
		// TXT
		TxtSectionKeys::const_iterator rowIterator = this->sectionKeys().find(rowKey);

		if (rowIterator == this->sectionKeys().end())
		{
			throw Exception(boost::format(_("Row %1% not found.")) % rowKey.toUtf8().constData());
		}

		const TxtEntryKey entryKey = TxtEntryKey(rowIterator.value(), columnKey);

		TxtEntryKeys::const_iterator columnIterator = this->entryKeys().find(entryKey);

		if (columnIterator != this->entryKeys().end())
		{
			return fromSlkString(QString::fromUtf8(columnIterator.value()->second.c_str()));
		}
		else
		{
			throw Exception(boost::format(_("Column %1% not found.")) % columnKey.toUtf8().constData());
		}
	}

	throw Exception();

}

bool MetaData::hasValue(const QString &rowKey, const QString &columnKey) const
{
	if (hasSlk())
	{
		SlkKeys::const_iterator columnIterator = this->columnKeys().find(columnKey);

		if (columnIterator == this->columnKeys().end())
		{
			return false;
		}

		SlkKeys::const_iterator rowIterator = this->rowKeys().find(rowKey);

		if (rowIterator == this->rowKeys().end())
		{
			return false;
		}

		return true;
	}
	else if (hasTxt())
	{
		TxtSectionKeys::const_iterator sectionIterator = this->sectionKeys().find(rowKey);

		if (sectionIterator == this->sectionKeys().end())
		{
			return false;
		}

		const TxtEntryKey entryKey = TxtEntryKey(sectionIterator.value(), columnKey);

		TxtEntryKeys::const_iterator columnIterator = this->entryKeys().find(entryKey);

		if (columnIterator == this->entryKeys().end())
		{
			return false;
		}

		return true;
	}

	return false;
}

}

}
