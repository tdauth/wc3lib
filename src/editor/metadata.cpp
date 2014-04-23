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

MetaData::MetaData(const KUrl &url) : Resource(url, Type::MetaData), m_table(boost::extents[0][0])
{
}

void MetaData::clear() throw ()
{
	this->m_table.resize(boost::extents[0][0]);
	this->m_rowKeys.clear();
	this->m_columnKeys.clear();
}

void MetaData::load() throw (class Exception)
{
	QString filePath;

	if (!source()->download(url(), filePath, 0))
	{
		throw Exception();
	}

	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly))
	{
		throw Exception();
	}


	QTextStream textStream(&file);
	/*
	 * Note that even if a SYLK file is created by an application that supports Unicode (for example Microsoft Excel), the SYLK file will be encoded in the current system's ANSI code page, not in Unicode. If the application contained characters that were displayable in Unicode but have no codepoint in the current system's code page, they will be converted to question marks ('?') in the SYLK file.
	 */
	//textStream.setCodec(QTextCodec::);
	int lineNumber = 0;
	textStream.readLine(); // skip first line which usually is "ID;PWXL;N;E"
	++lineNumber;

	// read row and column number
	// Example: B;X14;Y178;D0
	int columns = 0;
	int rows = 0;
	QString line = textStream.readLine();
	++lineNumber;

	if (line.isEmpty())
	{
		throw Exception(boost::format(_("Line %1%: Line is empty.")) % lineNumber);
	}

	QStringList lineValues = line.split(';');

	if (lineValues.size() != 4)
	{
		throw Exception(boost::format(_("Line %1%: Line should have 4 values.")) % lineNumber);
	}

	if (lineValues[0][0] != 'B')
	{
		throw Exception(boost::format(_("Line %1%: Line should start with \"B\".")) % lineNumber);
	}

	if (lineValues[1].isEmpty())
	{
		throw Exception(boost::format(_("Line %1%: Missing \"X\" and \"Y\" values.")) % lineNumber);
	}
	
	bool ok = false;

	if (lineValues[1][0] == 'X')
		columns = lineValues[1].mid(1).toInt(&ok);
	else if (lineValues[1][0] == 'Y')
		rows = lineValues[1].mid(1).toInt(&ok);
	else
	{
		throw Exception(boost::format(_("Line %1%: Invalid \"X\" and \"Y\" values.")) % lineNumber);
	}
	
	if (!ok)
	{
		throw Exception(boost::format(_("Line %1%: Invalid \"X\" and \"Y\" values.")) % lineNumber);
	}

	if (lineValues[2].isEmpty())
	{
		throw Exception(boost::format(_("Line %1%: Missing \"X\" and \"Y\" values.")) % lineNumber);
	}

	if (lineValues[2][0] == 'X')
		columns = lineValues[2].mid(1).toInt(&ok);
	else if (lineValues[2][0] == 'Y')
		rows = lineValues[2].mid(1).toInt(&ok);
	else
	{
		throw Exception(boost::format(_("Line %1%: Invalid \"X\" and \"Y\" values.")) % lineNumber);
	}
	
	if (!ok)
	{
		throw Exception(boost::format(_("Line %1%: Invalid \"X\" and \"Y\" values.")) % lineNumber);
	}

	this->m_table.resize(boost::extents[columns][rows]);
	qDebug() << "Columns: " << columns << " rows: " << rows;
	//
	int currentRow = -1;
	++lineNumber;

	while (!textStream.atEnd())
	{
		line = textStream.readLine();

		if (line.isEmpty())
			continue;

		lineValues = line.split(';');
		int index = 0;

		//F;Y2440;X1

		// read cell, other cells of the SYLK format can be ignored
		if (lineValues[index] == "C")
		{
			++index;
			
			if (lineValues.size() < index + 1 || lineValues[1].isEmpty())
				throw Exception(boost::format(_("Line %1%: Expected expression after \"C\".")) % lineNumber);

			int column = -1;

			if (lineValues[index][0] == 'X')
				column = lineValues[1].mid(1).toInt();
			else if (lineValues[index][0] == 'Y')
				currentRow = lineValues[index].mid(1).toInt();
			else
			{
				throw Exception(boost::format(_("Line %1%: Expected expression \"X\" or \"Y\" after \"C\".")) % lineNumber);
			}
			
			bool secondValueIsValid = true;
			++index;

			if (lineValues.size() >= index + 1 && !lineValues[index].isEmpty())
			{
				if (lineValues[index][0] == 'X')
				{
					column = lineValues[2].mid(1).toInt();
					++index; // continue with next value if defined
				}
				else if (lineValues[index][0] == 'Y')
				{
					currentRow = lineValues[2].mid(1).toInt();
					++index; // continue with next value if defined
				}
				else
				{
					secondValueIsValid = false;
				}
			}
			else
			{
				secondValueIsValid = false;
			}
			
			if (!secondValueIsValid)
			{
				/*
				 * The row is optional and does not have to be placed there.
				 * If column was not set it is missing so throw an exception.
				 * If row was not set keep the old row value until it is overwritten!
				 */
				if (column == -1) {
					throw Exception(boost::format(_("Line %1%: Expected expression after \"X\" or \"Y\" after \"C\".")) % lineNumber);
				}
			}

			if (currentRow == -1 || column == -1)
			{
				throw Exception(boost::format(_("Line %1%: Missing row or column number.")) % lineNumber);
			}
			else if (currentRow > this->m_table.shape()[1])
			{
				throw Exception(boost::format(_("Line %1%: Row value %2% is out of range (maximum %3%).")) % lineNumber % currentRow % this->m_table.shape()[1]);
			} else if (column > this->m_table.shape()[0])
			{
				throw Exception(boost::format(_("Line %1%: Column value %2% is out of range (maximum %3%).")) % lineNumber % column % this->m_table.shape()[0]);
			}

			if (lineValues.size() >= index + 1 && !lineValues[index].isEmpty() && lineValues[index][0] == 'K')
			{
				QString cellValue = lineValues[index].mid(1);
				
				if (cellValue.startsWith('\"') && cellValue.endsWith('\"')) {
					cellValue = cellValue.mid(1, cellValue.length() - 2);
				}
				
				// columns and rows are counted from 1
				this->m_table[column - 1][currentRow - 1] = Cell(cellValue);
			}
			// empty cell
			else
			{
				// columns and rows are counted from 1
				this->m_table[column - 1][currentRow - 1] = "";
				qDebug() << "Empty cell definition in line " << lineNumber;
			}
		}

		if (line == "E") // end processing
		{
			break;
		}
		
		++lineNumber;
	}
	
	/*
	 * Make rows and columns accessable over their first column/row values.
	 */
	this->m_columnKeys.clear();
	this->m_rowKeys.clear();
	
	/*
	 * Store indices of rows and columns by the value of the first cell.
	 */
	for (Table::index column = 0; column < this->m_table.shape()[0]; ++column)
	{
		Cell &firstColumnCell = this->m_table[column][0];
		this->m_columnKeys[firstColumnCell] = column;
	}
	
	for (Table::index row = 0; row < this->m_table.shape()[1]; ++row)
	{
		Cell &firstRowCell = this->m_table[0][row];
		this->m_rowKeys[firstRowCell] = row;
	}
}

void MetaData::reload() throw (Exception)
{
	clear();
	load();
}

}

}
