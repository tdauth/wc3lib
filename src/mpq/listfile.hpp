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

#include <boost/algorithm/string.hpp>

#include "mpqfile.hpp"

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
class Listfile : public MpqFile
{
	public:
		typedef std::vector<string> Entries;

		/**
		 * Replaces on Unix-based systems / character by \.
		 */
		static void toListfileEntry(std::string &path);
		/**
		 * Replaces on Unix-based system \ character by /.
		 */
		static void toNativePath(std::string &entry);
		/**
		 * Splits up \p content at one of the following characters "; \r \n" and returns the resulting container with all split file paths.
		 * \return Returns the container with all found file paths in \p content.
		 */
		static Entries entries(const string &content);
		
		/**
		 * Uses \ref entries(const string&) to get all "(listfile)" entries and returns all paths starting with directory path \p dirPath.
		 * Sub directory paths will be added as well having a \ character as suffix.
		 * 
		 * \param dirPath Directory path all entries should get from. If this value is empty, function uses top level directory.
		 * \param recursive If this value is true, all paths in sub directories are listed, as well.
		 * \param directories If this value is true, sub directory paths will be added as well having a \ character as suffix.
		 * 
		 * \todo Adding all directories might be a little slow (in recursive mode etc.).
		 * \note All directories will be appended to the end of the result if \p directories is set true. Their order depends on the order of the set they are stored in. The set is used since they could occur multiple times but the user usually needs them only once.
		 */
		static Entries dirEntries(const string &content, const string &dirPath, bool recursive = true, bool directories = true);

		/**
		 * Splits up file content at one of the following characters "; \r \n" and returns the resulting container with all split file paths.
		 * \return Returns the container with all found file paths in file.
		 */
		Entries entries() const;
		Entries dirEntries(const string &dirPath, bool recursive = true, bool directories = true);

		virtual const char* fileName() const;

	protected:
		friend class Mpq;

		Listfile(Mpq *mpq, Hash *hash);
};

inline void Listfile::toListfileEntry(std::string &path)
{
#ifdef UNIX
	// (listfile) entries usually have Windows path format
	boost::algorithm::replace_all(path, "/", "\\");
#else
#warning This functions is only necessary on Unix systems!
#endif
}

inline void Listfile::toNativePath(std::string &entry)
{

#ifdef UNIX
	// (listfile) entries usually have Windows path format
	boost::algorithm::replace_all(entry, "\\", "/");
#else
#warning This functions is only necessary on Unix systems!
#endif
}

inline Listfile::Entries Listfile::entries() const
{
	ostringstream stream;
	MpqFile::writeData(stream);

	return entries(stream.str());
}

inline Listfile::Entries Listfile::dirEntries(const string& dirPath, bool recursive, bool directories)
{
	ostringstream stream;
	MpqFile::writeData(stream);
	
	return dirEntries(stream.str(), dirPath, recursive, directories);
}

inline const char* Listfile::fileName() const
{
	return "(listfile)";
}

}

}

#endif
