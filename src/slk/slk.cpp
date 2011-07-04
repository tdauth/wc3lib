/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
 *   tamino@cdauth.de                                                      *
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

#include <boost/tokenizer.hpp>

#include "slk.hpp"

namespace wc3lib
{

namespace slk
{

Slk::Slk()
{
}

Slk::~Slk()
{
}

/**
  SLK files can technically be opened by Microsoft Excel, though if you don't own Excel or a SLK editor, or want to open SLK files with your application, you might be interested in the format, so here it is. SLK files are text files and read line by line. The first two letters of each line can be used to decide what to do with it.

Format:
Line["ID"]: the first line you will have to read is the one starting with ID, this id is usually set to ID;PWXL;N;E
Line["B;"]: this line defines the number of columns and lines of the table
Example: B;Y837;X61;D0 0 836 60
This file has 837 lines and 61 columns, the rest of the line is always set to ;D0 0 Y-1 X-1 so these are probably the internal index bounds
From now on we will only look at lines that start with "C;" and are followed by "Y" or "X".
Line["C;Y"]: specifies the value for the cell with the specified line and column if the fourth token starts with a "K", otherwise we ignore it, anyway the current line is set to the y value, so the following lines that might be missing the y value refer to this line
Example: C;Y1;X1;K"unitBalanceID"
Sets the cell in line 1 and column 1 to "unitBalanceID"
Line["C;X"}: specifies the value for the cell with the specified column and the line previously specified as current line (in a "C;Y" statement) if the fourth token starts with a "K", otherwise we ignore it
Example: C;X45;K1.8
Assigns 1.8 to the cell in current line and column 45
The x value might also be omitted, so that the x value of the previous line is used.

All other lines can be ignored, they contain comments and format information. After a "K" there may either be a whole or floating point number or a string within quotes. Empty cells don't have any entries. This should be enough information to successfully parse all of Blizzard's SLK files and files edited
with Excel or its Open Office equivalent. If you want to read up on the advanced tokens, get the official specs at http://www.wotsit.org/download.asp?f=sylk
*/

std::streamsize Slk::read(std::basic_istream<map::byte> &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Slk::write(std::basic_ostream<map::byte> &ostream) const throw (class Exception)
{
	return 0;
}

}

}
