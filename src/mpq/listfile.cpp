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

// TEST
#include <iostream>

#include <set>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>

#include "listfile.hpp"
#include "mpq.hpp"

namespace wc3lib
{

namespace mpq
{

Listfile::Listfile() : File()
{
}

Listfile::Listfile(Archive *mpq, Hash *hash) : File(mpq, hash, "(listfile)")
{
}

Listfile::Entries Listfile::entries(const string &content)
{
	Entries result;

	// specification says:
	// "and is simply a text file with file paths separated by ';', 0Dh, 0Ah, or some combination of these."
	// regex gets all combinations
	boost::algorithm::split_regex(result, content,
		boost::regex(
			"("
			"\r\n;|\r;\n|\n\r;|\n;\r|;\n\r|;\r\n|"
			"\r;|\r\n|\n\r|\n;|;\n|;\r|"
			"\r|\n|;"
			")"
		));
	// OLD
	//boost::algorithm::split(result, value, boost::algorithm::is_any_of(";\r\n"), boost::algorithm::token_compress_on);

	return result;
}

Listfile::Entries Listfile::dirEntries(const Listfile::Entries &entries, const string &dirPath, bool recursive, bool directories)
{
	std::set<string> dirs;
	Entries result;

	BOOST_FOREACH(Entries::const_reference ref, entries)
	{
		if (boost::istarts_with(ref, dirPath)) // paths are not case sensitive!
		{
			string::size_type start = dirPath.length();
			bool foundOneDir = false;
			string::size_type index = string::npos;

			// find all directorie paths!
			do
			{
				index = ref.find('\\', start);

				if (index != string::npos)
				{
					if (directories)
					{

						string dirName = ref.substr(0, index + 1); // add directories with separator at the end
						boost::to_upper(dirName); // as hash algorithm uses to_upper characters file paths are case insensitive and therefore all directories are, too
						dirs.insert(dirName);
					}

					foundOneDir = true;
					start = index + 1;
				}
			}
			// if not recursive only use the first level of directories!
			while (recursive && index != string::npos && start < ref.length());

			if (recursive || !foundOneDir)
			{
				result.push_back(ref);
			}
		}
	}

	if (directories)
	{
		BOOST_FOREACH(std::set<string>::const_reference ref, dirs) // TODO reference does not work, incompatible iterator
		{
			result.push_back(ref);
		}
	}

	return result;
}

Listfile::Entries Listfile::dirEntries(const string& content, const string& dirPath, bool recursive, bool directories)
{
	Entries entries = Listfile::entries(content);

	return dirEntries(entries, dirPath, recursive, directories);
}

void Listfile::toListfileEntry(std::string &path)
{
#ifdef UNIX
	// (listfile) entries usually have Windows path format
	boost::algorithm::replace_all(path, "/", "\\");
#else
#warning This functions is only necessary on Unix systems!
#endif
}

void Listfile::toNativePath(std::string &entry)
{

#ifdef UNIX
	// (listfile) entries usually have Windows path format
	boost::algorithm::replace_all(entry, "\\", "/");
#else
#warning This functions is only necessary on Unix systems!
#endif
}

}

}
