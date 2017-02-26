/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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
#include <map>

#include "../map.hpp"

using namespace wc3lib;
using namespace wc3lib::map;

/**
 * Validates a single "TriggerData.txt" or "TriggerStrings.txt" file.
 * At the moment only Reign of Chaos files are supported!
 * TODO Implement TriggerDataEx and add the option --tft
 * TODO Print similar errors to the ones which the World Editor prints when entries are missing.
 */
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		return 1;
	}

	const char *path = argv[1];

	boost::filesystem::path filePath(path);
	ifstream in(filePath);

	if (!in)
	{
		std::cerr << "Error on opening file: " << path << std::endl;

		return 1;
	}

	if (filePath.filename() == "TriggerData.txt")
	{
		TriggerData triggerData;

		try
		{
			triggerData.read(in);
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error: " << e.what() << std::endl;

			return 1;
		}

		return 0;
	}
	else if (filePath.filename() == "TriggerStrings.txt")
	{
		TriggerStrings triggerStrings;

		try
		{
			triggerStrings.read(in);
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error: " << e.what() << std::endl;

			return 1;
		}

		return 0;
	}

	std::cerr << "Unknown file. Expected filenames \"TriggerData.txt\" or \"TriggerStrings.txt\"" << std::endl;

	return 1;
}
