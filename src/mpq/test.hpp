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

#ifndef WC3LIB_MPQ_TEST
#define WC3LIB_MPQ_TEST

#include "../mpq.hpp"

/**
 * \file
 * Defines test functions for debugging the MPQ format support.
 * The functions bring properties of archives, files and sectors in a readable form which could be printed
 * to the output stream of written into a file.
 */

namespace wc3lib
{

namespace mpq
{

template<typename T>
std::string sizeString(T size, bool humanReadable, bool decimal)
{
	if (humanReadable)
	{
		if (decimal)
		{
			return sizeStringDecimal<T>(size);
		}
		else
		{
			return sizeStringBinary<T>(size);
		}
	}

	std::ostringstream result;
	result << size;

	return result.str();
}

std::string flagsString(Block::Flags flags);
std::string compressionString(Sector::Compression compression);
std::string fileInfo(File &file, bool humanReadable, bool decimal);
std::string formatString(Archive::Format format);
std::string archiveInfo(Archive &archive, bool humanReadable, bool decimal);

}

}


#endif
