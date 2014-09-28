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

Listfile::Entries Listfile::caseSensitiveFileEntries(const Listfile::Entries& entries, const string& prefix, bool recursive)
{
	/**
	 * Uses the upper case file path as key since listfile entries are not case sensitive.
	 * The value is still case sensitive and should be the first occurence of the path in a listfile.
	 */
	typedef std::map<string, string> UniqueEntries;
	UniqueEntries uniqueEntries;
	Entries result;
	result.reserve(entries.size());

	BOOST_FOREACH(Entries::const_reference ref, entries)
	{
		if (prefix.empty() || (ref.size() > prefix.size() && boost::istarts_with(ref, prefix))) // paths are not case sensitive!
		{
			/*
			 * Split relative path up into directory paths and replace the single dirs by the alread found ones.
			 */
			std::vector<string> pathTokens;
			//string substring = ref.substr(prefix.size());
			boost::algorithm::split(pathTokens, ref, boost::algorithm::is_any_of("\\"), boost::algorithm::token_compress_on);

			/*
			 * If recursive is true all files which start with the prefix will be added.
			 * Otherwise only entries in the directory of the prefix will be added, so the file name follows the prefix.
			 */
			if (recursive || (pathTokens.back() == ref.substr(prefix.size())))
			{
				ostringstream caseSensitiveEntry;
				int i = 0;

				BOOST_FOREACH(std::vector<string>::reference ref, pathTokens)
				{
					string upperRef = ref;
					boost::to_upper(upperRef);
					UniqueEntries::iterator uniqueEntryIterator = uniqueEntries.find(upperRef);

					if (uniqueEntryIterator == uniqueEntries.end())
					{
						uniqueEntryIterator = uniqueEntries.insert(std::make_pair(upperRef, ref)).first;
					}

					/*
					 * Add the case sensitive path token.
					 */
					caseSensitiveEntry << uniqueEntryIterator->second;

					/*
					 * Add file path separator to all tokens except for the last one to reproduce the file path with the correct cases.
					 */
					if (i < pathTokens.size() - 1)
					{
						caseSensitiveEntry << '\\';
					}

					++i;
				}

				result.push_back(caseSensitiveEntry.str());
			}
		}
	}

	return result;
}

Listfile::Entries Listfile::caseSensitiveDirEntries(const Listfile::Entries& entries, const string& prefix, bool recursive)
{
	/**
	 * Uses the upper case file path as key since listfile entries are not case sensitive.
	 * The value is still case sensitive and should be the first occurence of the path in a listfile.
	 */
	typedef std::map<string, string> UniqueEntries;
	UniqueEntries uniqueEntries;
	Entries result;
	result.reserve(entries.size());

	BOOST_FOREACH(Entries::const_reference ref, entries)
	{
		if (prefix.empty() || (ref.size() > prefix.size() && boost::istarts_with(ref, prefix))) // paths are not case sensitive!
		{
			string::size_type index;
			/*
			 * Start searching for the directory separator after the prefix.
			 */
			std::size_t pos = prefix.size();

			if (ref[pos] == '\\')
			{
				pos++;

				if (pos >= ref.size())
				{
					break;
				}
			}

			ostringstream caseSensitiveEntry;
			bool isFirst = true;

			do
			{
				/*
				 * Search for a directory separator and if found we have at least one token of the resulting directory path.
				 * Other tokens will be added in the while loop using case sensitive entries from the map "uniqueEntries".
				 */
				index = ref.find('\\', pos);

				if (index != string::npos)
				{
					string token = ref.substr(pos, index - pos);
					string upperToken = boost::to_upper_copy(token);
					UniqueEntries::iterator iterator = uniqueEntries.find(upperToken);

					if (iterator == uniqueEntries.end())
					{
						iterator = uniqueEntries.insert(std::make_pair(upperToken, token)).first;
					}

					if (!isFirst)
					{
						caseSensitiveEntry << '\\';
					}

					caseSensitiveEntry << iterator->second;
					isFirst = false;

					/*
					 * Update searching start position.
					 */
					pos = index + 1;
				}
			}
			while (recursive && index != string::npos && pos < ref.size());

			result.push_back(caseSensitiveEntry.str());
		}
	}

	return result;
}

namespace
{

/**
 * In case sensitive comparator for strings.
 */
class Comparator : public std::binary_function<const string &, const string &, bool>
{
	public:
		bool operator()(const string &var1, const string &var2) const
		{
			const string var1Upper = boost::to_upper_copy(var1);
			const string var2Upper = boost::to_upper_copy(var2);

			return strcmp(var1Upper.c_str(), var2Upper.c_str());
		}
};

}

Listfile::Entries Listfile::caseSensitiveUniqueEntries(const Listfile::Entries &entries)
{
	Entries result;
	std::set<string, Comparator> uniqueEntries;

	BOOST_FOREACH(Entries::const_reference ref, entries)
	{
		if (uniqueEntries.find(ref) == uniqueEntries.end())
		{
			uniqueEntries.insert(ref);
		}
	}

	result.insert(result.begin(), uniqueEntries.begin(), uniqueEntries.end());

	return result;
}

Listfile::Entries Listfile::existingEntries(const Listfile::Entries& entries, mpq::Archive& archive, const string &prefix, bool recursive)
{
	Entries result;
	result.reserve(entries.size());

	BOOST_FOREACH(Entries::const_reference ref, entries)
	{
		if (prefix.empty() || (ref.size() > prefix.size() && boost::istarts_with(ref, prefix) && (recursive || ref.find(prefix.size(), '\\') == string::npos)))
		{
			File file = archive.findFile(ref);

			if (file.isValid())
			{
				result.push_back(ref);
			}
		}
	}

	return result;
}

string Listfile::dirPath(const string &entry)
{
	const string::size_type index = entry.find_last_of('\\');

	if (index != string::npos)
	{
		return entry.substr(0, index);
	}

	return "";
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
