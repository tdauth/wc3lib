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

MetaData::MetaData(const KUrl &url) : Resource(url, Type::MetaData)
{
}

void MetaData::clear() throw ()
{
	m_metaDataEntries.clear();
}

void MetaData::load() throw (class Exception)
{
	QString filePath;
	this->url();
	this->source()->locale();
	source()->download(url(), filePath, 0);

	if (!source()->download(url(), filePath, 0))
		throw Exception();

	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly))
		throw Exception();


	QTextStream textStream(&file);
	/*
	 * Note that even if a SYLK file is created by an application that supports Unicode (for example Microsoft Excel), the SYLK file will be encoded in the current system's ANSI code page, not in Unicode. If the application contained characters that were displayable in Unicode but have no codepoint in the current system's code page, they will be converted to question marks ('?') in the SYLK file.
	 */
	//textStream.setCodec(QTextCodec::);
	textStream.readLine(); // skip first line which usually is "ID;PWXL;N;E"

	// read row and column number
	// Example: B;X14;Y178;D0
	int columns = 0;
	int rows = 0;
	QString line = textStream.readLine();

	if (line.isEmpty())
		throw Exception();

	QStringList lineValues = line.split(';');

	if (lineValues.size() != 4)
		throw Exception();

	if (lineValues[0][0] != 'B')
		throw Exception();

	if (lineValues[1].isEmpty())
		throw Exception();

	if (lineValues[1][0] == 'X')
		columns = lineValues[1].mid(1).toInt();
	else if (lineValues[1][0] == 'Y')
		rows = lineValues[1].mid(1).toInt();
	else
		throw Exception();

	if (lineValues[2].isEmpty())
		throw Exception();

	if (lineValues[2][0] == 'X')
		columns = lineValues[2].mid(1).toInt();
	else if (lineValues[2][0] == 'Y')
		rows = lineValues[2].mid(1).toInt();
	else
		throw Exception();

	Rows results(rows);
	//
	int currentRow = -1;

	while (!textStream.atEnd())
	{
		line = textStream.readLine();

		if (line.isEmpty())
			continue;

		lineValues = line.split(';');

		//F;Y2440;X1

		// read cell, other cells of the SYLK format can be ignored
		if (lineValues[0] == "C")
		{
			if (lineValues[1].isEmpty())
				throw Exception();

			int column = -1;

			if (lineValues[1][0] == 'X')
				column = lineValues[1].mid(1).toInt();
			else if (lineValues[1][0] == 'Y')
				currentRow = lineValues[1].mid(1).toInt();
			else
				throw Exception();

			if (lineValues[2][0] == 'X')
				column = lineValues[2].mid(1).toInt();
			else if (lineValues[2][0] == 'Y')
				currentRow = lineValues[2].mid(1).toInt();
			else
				throw Exception();

			if (currentRow == -1 || column == -1)
				throw Exception();

			if (!lineValues[3].isEmpty() && lineValues[3][0] == 'K')
				results[currentRow][column] = QVariant(lineValues[3].mid(1));
		}

		if (line == "E") // end processing
			break;
	}

	this->m_metaDataEntries.resize(rows);

	for (std::size_t i = 0; i < rows; ++i)
	{
		MetaDataPtr ptr(createMetaDataEntry());
		filleMetaDataEntry(ptr, results[i]);
		this->m_metaDataEntries[i].swap(ptr);
	}
}

void MetaData::reload() throw (Exception)
{
	clear();
	load();
}

map::MetaData* MetaData::createMetaDataEntry()
{
	return new map::MetaData();
}

void MetaData::filleMetaDataEntry(MetaDataPtr &entry, const Row &row)
{
	entry->setIndex(row[0].toInt());
	entry->setDisplayName(row[1].toString().toUtf8().constData());
	entry->setMinValue(boost::numeric_cast<map::int32>(row[2].toInt()));
	/// \todo Fill other data ...
}

}

}
