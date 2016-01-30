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

#include <iostream>

#include <boost/cast.hpp>

#include <QtCore>

#include <KIO/NetAccess>

#include "metadata.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

TextSourceInterface::~TextSourceInterface()
{
}

bool SlkTextSource::hasValue(const QString &rowKey, const QString &columnKey) const
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

QString SlkTextSource::value(const QString &rowKey, const QString &columnKey) const
{
	SlkKeys::const_iterator rowIterator = this->rowKeys().find(rowKey);

	if (rowIterator == this->rowKeys().end())
	{
		std::cerr << boost::format(_("Row \"%1%\" not found.")) % rowKey.toUtf8().constData() << std::endl;

		return QString();
	}

	return value(rowIterator.value(), columnKey);
}

bool SlkTextSource::hasValue(int row, const QString &columnKey) const
{
	if (row >= boost::numeric_cast<int>(this->slk().rows()) || row < 0)
	{
		return false;
	}

	SlkKeys::const_iterator columnIterator = this->columnKeys().find(columnKey);

	if (columnIterator == this->columnKeys().end())
	{
		return false;
	}

	return true;
}

QString SlkTextSource::value(int row, const QString &columnKey) const
{
	SlkKeys::const_iterator columnIterator = this->columnKeys().find(columnKey);

	if (columnIterator == this->columnKeys().end())
	{
		std::cerr << boost::format(_("Column \"%1%\" not found.")) % columnKey.toUtf8().constData() << std::endl;

		return QString();
	}

	const map::Slk::Table::size_type column = columnIterator.value();
	const map::Slk::Table::size_type rowValue = boost::numeric_cast<map::Slk::Table::size_type>(row);

	if (column < this->slk().columns() && rowValue < this->slk().rows())
	{
		return MetaData::fromSlkString(QString::fromUtf8(this->slk().table()[column][rowValue].c_str()));
	}
	else
	{
		std::cerr << boost::format(_("%1%|%2% is out of range.")) % column % rowValue << std::endl;

		return QString();
	}
}

void SlkTextSource::read(istream &in)
{
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

void SlkTextSource::write(ostream &out) const
{
	this->slk().write(out);
}

bool TxtTextSource::hasValue(const QString& rowKey, const QString& columnKey) const
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

QString TxtTextSource::value(const QString& rowKey, const QString& columnKey) const
{
	// TXT
	TxtSectionKeys::const_iterator rowIterator = this->sectionKeys().find(rowKey);

	if (rowIterator == this->sectionKeys().end())
	{
		std::cerr << boost::format(_("Row \"%1%\" not found.")) % rowKey.toUtf8().constData() << std::endl;

		return QString();
	}

	const TxtEntryKey entryKey = TxtEntryKey(rowIterator.value(), columnKey);

	TxtEntryKeys::const_iterator columnIterator = this->entryKeys().find(entryKey);

	if (columnIterator != this->entryKeys().end())
	{
		return MetaData::fromSlkString(QString::fromUtf8(columnIterator.value()->second.c_str()));
	}
	else
	{
		std::cerr << boost::format(_("Column \"%1%\" not found.")) % columnKey.toUtf8().constData() << std::endl;

		return QString();
	}
}

bool TxtTextSource::hasValue(int row, const QString& columnKey) const
{
	if (row >= boost::numeric_cast<int>(this->m_txt.sections().size()) || row < 0)
	{
		return false;
	}

	const map::Txt::Section *rowKey = &this->m_txt.sections()[row];
	const TxtEntryKey entryKey = TxtEntryKey(rowKey, columnKey);

	TxtEntryKeys::const_iterator columnIterator = this->entryKeys().find(entryKey);

	if (columnIterator == this->entryKeys().end())
	{
		return false;
	}

	return true;
}

QString TxtTextSource::value(int row, const QString& columnKey) const
{
	const map::Txt::Section *rowKey = &this->m_txt.sections()[row];
	const TxtEntryKey entryKey = TxtEntryKey(rowKey, columnKey);

	TxtEntryKeys::const_iterator columnIterator = this->entryKeys().find(entryKey);

	if (columnIterator != this->entryKeys().end())
	{
		return MetaData::fromSlkString(QString::fromUtf8(columnIterator.value()->second.c_str()));
	}
	else
	{
		std::cerr << boost::format(_("Column \"%1%\" not found.")) % columnKey.toUtf8().constData() << std::endl;

		return QString();
	}
}

void TxtTextSource::read(istream &in)
{
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

void TxtTextSource::write(ostream &out) const
{
	this->txt().write(out);
}

bool MapStringsTextSource::hasValue(const QString &rowKey, const QString& /* columnKey */) const
{
	bool ok = true;
	int row = rowKey.toInt(&ok);

	if (ok)
	{
		return this->entryKeys().contains(row);
	}

	return false;
}

QString MapStringsTextSource::value(const QString &rowKey, const QString& columnKey) const
{
	bool ok = true;
	int row = rowKey.toInt(&ok);

	if (ok)
	{
		return value(row, columnKey);
	}

	throw Exception(boost::format(_("Invalid row key \"%1%\".")) % rowKey.toUtf8().constData());
}

bool MapStringsTextSource::hasValue(int row, const QString& /* columnKey */) const
{
	return m_entryKeys.contains(row);
}

QString MapStringsTextSource::value(int row, const QString& /* columnKey */) const
{
	EntryKeys::const_iterator iterator = this->entryKeys().find(row);

	if (iterator != this->entryKeys().end())
	{
		return QString::fromUtf8(iterator.value()->value.c_str());
	}
	else
	{
		std::cerr << boost::format(_("String \"%1%\" not found.")) % row << std::endl;

		return QString();
	}
}

void MapStringsTextSource::read(istream &in)
{
	this->m_mapStrings.read(in);

	BOOST_FOREACH(map::MapStrings::Entries::reference ref, this->m_mapStrings.entries())
	{
		m_entryKeys.insert(ref.key, &ref);
	}

	qDebug() << "Keys entries size:" << this->m_mapStrings.entries().size();
	qDebug() << "Keys size:" << m_entryKeys.size();
}

void MapStringsTextSource::write(ostream& out) const
{
	this->m_mapStrings.write(out);
}

MetaData::MetaData(const KUrl &url) : Resource(url, Type::MetaData), m_textSource(0)
{
}

MetaData::~MetaData()
{
	if (this->textSource() != 0)
	{
		delete this->textSource();
		m_textSource = 0;
	}
}

void MetaData::clear()
{
	if (this->textSource() != 0)
	{
		this->textSource()->clear();
	}
}

void MetaData::load()
{
	QString filePath;

	if (!this->source()->download(this->url(), filePath, 0))
	{
		throw Exception(boost::format(_("Error on downloading file \"%1%\": %2%")) % this->url().toLocalFile().toUtf8().constData() % KIO::NetAccess::lastErrorString().toUtf8().constData());
	}

	ifstream in(filePath.toUtf8().constData());

	if (!in)
	{
		throw Exception(boost::format(_("Unable to open downloaded file \"%1%\"")) % filePath.constData());
	}

	/*
	 * Make rows and columns accessable over their first column/row values.
	 */
	this->clear();

	if (this->textSource() != 0)
	{
		delete this->textSource();
		m_textSource = 0;
	}

	QString suffix = QFileInfo(this->url().fileName()).suffix().toLower();

	if (suffix == "slk")
	{
		this->m_textSource = new SlkTextSource();
	}
	else if (suffix == "txt")
	{
		this->m_textSource = new TxtTextSource();
	}
	else if (suffix == "wts")
	{
		this->m_textSource = new MapStringsTextSource();
	}
	else
	{
		throw Exception(boost::format(_("Unknown file type of file \"%1%\"")) % filePath.constData());
	}

	this->m_textSource->read(in);
}

void MetaData::save(const KUrl &url) const
{
	QTemporaryFile file;

	if (file.open())
	{
		ofstream out(file.fileName().toUtf8().constData());

		this->textSource()->write(out);

		out.close();
		file.close();

		if (!this->source()->upload(file.fileName(), url, 0))
		{
			throw Exception(KIO::NetAccess::lastErrorString().toUtf8().constData());
		}
	}

	throw Exception(boost::format(_("Error on opening temporary file \"%1%\".")) % file.fileName().toUtf8().constData());
}

void MetaData::reload()
{
	clear();
	load();
}

QString MetaData::value(const QString &rowKey, const QString &columnKey) const
{
	if (this->textSource() != 0)
	{
		return this->textSource()->value(rowKey, columnKey);
	}

	return QString();
}

bool MetaData::hasValue(const QString &rowKey, const QString &columnKey) const
{
	if (this->textSource() != 0)
	{
		return this->textSource()->hasValue(rowKey, columnKey);
	}

	return false;
}

int MetaData::rows() const
{
	if (this->textSource() != 0)
	{
		return this->textSource()->rows();
	}

	return 0;
}

bool MetaData::hasValue(int row, const QString &columnKey) const
{
	return this->textSource()->hasValue(row, columnKey);
}

QString MetaData::value(int row, const QString &columnKey) const
{
	if (this->textSource() != 0)
	{
		return this->textSource()->value(row, columnKey);
	}

	return QString();
}

}

}
