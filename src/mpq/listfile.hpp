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

#ifndef WC3LIB_MPQ_LISTFILE_HPP
#define WC3LIB_MPQ_LISTFILE_HPP

#include <vector>
#include <string>
#include <set>

#include <boost/algorithm/string.hpp>

#include "file.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * Each MPQ archive can contain one single "(listfile)" file with neutral locale and default platform which contains all paths (usually in Windows style) separated by one of the following characters: ";\r\n" OR ANY COMBINATION OF THESE!
 *
 * Since only hashes of file paths, locales and platforms are stored by default you usually don't know which files do belong to the archive.
 * Therefore listfiles can help especially when extracting all files from an archive or simply to do not loose any file path information.
 *
 * On Unix platforms \ref Listfile::toNativePath() can be used to convert one single entry to a Unix path.
 *
 * Use \ref Listfile::entries() to get all entries stored as vector of strings.
 *
 * Use \ref Listfile::dirEntries() if you want filter entries by directory and identify sub directories.
 *
 * \sa Attributes
 * \sa Signature
 */
class Listfile : public File
{
	public:
		typedef std::vector<string> Entries;

		Listfile();

		/**
		 * Replaces on Unix-based systems / character by \.
		 * For Windows systems it doesn't do anything.
		 */
		static void toListfileEntry(std::string &path);
		/**
		 * Replaces on Unix-based system \ character by /.
		 * For Windows systems it doesn't do anything.
		 */
		static void toNativePath(std::string &entry);
		/**
		 * Splits up \p content at one of the following characters "; \r \n" and returns the resulting container with all split file paths.
		 * \return Returns the container with all found file paths in \p content.
		 */
		static Entries entries(const string &content);

		static Entries caseSensitiveFileEntries(const Entries &entries, const string &prefix = "", bool recursive = true);
		/**
		 * \todo Entries are not unique. Most directory entries are duplicated as often as they appear. At the moment you need to call \ref caseSensitiveUniqueEntries() afterwards.
		 */
		static Entries caseSensitiveDirEntries(const Entries &entries, const string &prefix = "", bool recursive = true);
		static Entries caseSensitiveUniqueEntries(const Entries &entries);

		static Entries existingEntries(const Entries &entries, mpq::Archive &archive, const string &prefix = "", bool recursive = true);

		/**
		 * \return Returns the directory path only of \p entry.
		 */
		static string dirPath(const string &entry);

		/**
		 * Splits up file content at one of the following characters "; \r \n" and returns the resulting container with all split file paths.
		 * \return Returns the container with all found file paths in file.
		 */
		Entries entries();

		virtual const char* fileName() const;

	protected:
		friend Archive;

		Listfile(Archive *mpq, Hash *hash);
};

inline Listfile::Entries Listfile::entries()
{
	ostringstream stream;
	File::writeData(stream);

	return entries(stream.str());
}

inline const char* Listfile::fileName() const
{
	return "(listfile)";
}

}

}

#endif
