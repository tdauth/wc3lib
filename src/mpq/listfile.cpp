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

string Listfile::content(const Listfile::Entries &entries)
{
	return boost::algorithm::join(entries, ";");
}

Listfile::CaseSensitiveEntries Listfile::caseSensitiveEntries(const Listfile::Entries &entries)
{
	CaseSensitiveEntries result;

	BOOST_FOREACH(Entries::const_reference ref, entries)
	{
		/*
		 * Split relative path up into directory paths and replace the single dirs by the alread found ones.
		 */
		std::vector<string> pathTokens;
		boost::algorithm::split(pathTokens, ref, boost::algorithm::is_any_of("\\"), boost::algorithm::token_compress_on);
		stringstream sstream;

		for (std::size_t i = 0; i < pathTokens.size(); ++i)
		{
			if (i != 0)
			{
				sstream << '\\';
			}

			const string value = pathTokens[i];
			const string key = boost::to_upper_copy(value);
			CaseSensitiveEntries::iterator iterator = result.find(key);

			if (iterator == result.end())
			{
				//std::cerr << "Key: " << key << ", Value: " << value << std::endl;
				iterator = result.insert(std::make_pair(key, value)).first;
			}

			sstream << iterator->second;
			const string longValue = sstream.str();
			const string longKey = boost::to_upper_copy(longValue);
			iterator = result.find(longKey);

			if (iterator == result.end())
			{
				//std::cerr << "Key: " << longKey << ", Value: " << longValue << std::endl;
				iterator = result.insert(std::make_pair(longKey, longValue)).first;
			}
		}
	}

	return result;
}

Listfile::Entries Listfile::caseSensitiveFileEntries(const Listfile::Entries &entries, const string &prefix, bool recursive)
{
	const Listfile::CaseSensitiveEntries uniqueEntries = caseSensitiveEntries(entries);
	Entries result;

	BOOST_FOREACH(Entries::const_reference ref, entries)
	{
		if (prefix.empty() || (ref.size() > prefix.size() && boost::istarts_with(ref, prefix))) // paths are not case sensitive!
		{
			const string::size_type index = ref.find_last_of('\\');

			if (recursive || index == string::npos)
			{
				if (index != string::npos)
				{
					const string key = boost::to_upper_copy(ref);
					const CaseSensitiveEntries::const_iterator iterator = uniqueEntries.find(key);

					if (iterator != uniqueEntries.end())
					{
						result.push_back(iterator->second);
					}
				}
				/*
				* Top level file path
				*/
				else
				{
					result.push_back(ref);
				}
			}
		}
	}

	return result;
}

Listfile::Entries Listfile::caseSensitiveDirEntries(const Listfile::Entries &entries, const string &prefix, bool recursive)
{
	const Listfile::CaseSensitiveEntries uniqueEntries = caseSensitiveEntries(entries);
	/*
	 * Store which directories have already been added to the result to keep directories unique.
	 */
	std::set<string> dirEntriesDone;
	Entries result;

	BOOST_FOREACH(Entries::const_reference ref, entries)
	{
		if (
			ref.size() > prefix.size()
			// paths are not case sensitive!
			&& (prefix.empty() || boost::istarts_with(ref, prefix))
			)
		{
			/*
			 * Now take for each dirname token the case sensitive corresponding entry.
			 */
			std::vector<string> pathTokens;
			boost::algorithm::split(pathTokens, ref, boost::algorithm::is_any_of("\\"), boost::algorithm::token_compress_on);
			stringstream sstream;
			const std::size_t dirPathTokensSize = pathTokens.size() - 1;
			/*
			 * Count the inner directory levels to make sure that none recursive entries are being added if not wished by the user.
			 */
			std::size_t countInnerDirLevels = 0;

			for (std::size_t i = 0; i < dirPathTokensSize && (recursive || countInnerDirLevels < 1); ++i)
			{
				const string key = boost::to_upper_copy(pathTokens[i]);
				const CaseSensitiveEntries::const_iterator iterator = uniqueEntries.find(key);

				if (iterator != uniqueEntries.end())
				{
					if (i > 0)
					{
						sstream << '\\';
					}

					sstream << iterator->second;

					const string doneValue = sstream.str();

					if ((doneValue.size() > prefix.size() && boost::istarts_with(doneValue, prefix)))
					{
						++countInnerDirLevels;
						const string doneKey = boost::to_upper_copy(doneValue);
						const std::set<string>::const_iterator doneIterator = dirEntriesDone.find(doneKey);

						if (doneIterator == dirEntriesDone.cend())
						{
							dirEntriesDone.insert(doneKey);
							result.push_back(doneValue);
						}
					}
				}
				else
				{
					throw Exception();
				}
			}
		}
	}

	return result;
}

Listfile::Entries Listfile::existingEntries(const Listfile::Entries& entries, mpq::Archive& archive, const string &prefix, bool recursive)
{
	Entries result;
	result.reserve(entries.size());

	BOOST_FOREACH(Entries::const_reference ref, entries)
	{
		/*
		 * If there is a prefix the entry has to start with it.
		 */
		if (prefix.empty() || (ref.size() > prefix.size() && boost::istarts_with(ref, prefix)))
		{
			/*
			 * Start searching for the directory separator after the prefix.
			 */
			const std::size_t pos = prefix.size();
			/*
			 * If the function does not list entries recursively they have to be files of the current directory level starting from the prefix.
			 * This is checked by searching for another dir separator after the one of the prefix (if there was one).
			 */
			if (recursive || ref.find(pos, '\\') == string::npos)
			{
				File file = archive.findFile(ref);

				if (file.isValid())
				{
					result.push_back(ref);
				}
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

string Listfile::fileName(const string &entry)
{
	const string::size_type index = entry.find_last_of('\\');

	if (index != string::npos && entry.size() > (index + 1))
	{
		return entry.substr(index + 1);
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
