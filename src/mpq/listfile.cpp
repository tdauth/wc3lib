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

#include "listfile.hpp"
#include "mpq.hpp"

namespace wc3lib
{

namespace mpq
{

Listfile::Listfile(Mpq* mpq, Hash* hash): MpqFile(mpq, hash)
{
	this->m_path = fileName();
}

Listfile::Entries Listfile::dirEntries(const string& content, const string& dirPath, bool recursive)
{
	Entries entries = Listfile::entries(content);
	
	std::set<string> dirs;
	Entries result;

	BOOST_FOREACH(Entries::reference ref, entries)
	{
		bool matches = false;
		
		if (dirPath.empty()) // root dir
		{
			const string::size_type index = ref.find('\\');
			
			matches = (index == string::npos || index == dirPath.size() - 1); // no back slash or last character
		}
		else
		{
			matches = boost::starts_with(ref, dirPath);
		}
		
		if (matches)
		{
			string::size_type start = dirPath.length();
			bool foundOneDir = false;
			
			// find all directorie paths!
			for (string::size_type index = ref.find(start, '\\'); index != string::npos && index + 1 < ref.length(); start = index + 1)
			{
				dirs.insert(ref.substr(0, index + 1)); // add directories with separator at the end
				foundOneDir = true;
				
				// if not recursive only use the first level of directories!
				if (!recursive)
				{
					break;
				}
			}
			
			if (!recursive || !foundOneDir)
			{
				result.push_back(ref);
			}
		}
	}
	
	BOOST_FOREACH(std::set<string>::const_reference ref, dirs) // TODO reference does not work, incompatible iterator
	{
		result.push_back(ref);
	}
	
	return result;
}


}

}
