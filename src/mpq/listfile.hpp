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

#include <boost/algorithm/string.hpp>

#include "mpqfile.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * Each MPQ archive can contain one single "(listfile)" file with neutral locale and default platform which contains all paths (usually in Windows style) separated by one of the following characters: ";\r\n".
 * Since only hashes of file paths, locales and platforms are stored by default you usually don't know which files do belong to the archive.
 * Therefore listfiles can help especially when extracting all files from an archive or simply to do not loose any file path information.
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
		 * Splits up file content at one of the following characters "; \r \n" and returns the resulting container with all split file paths.
		 * \return Returns the container with all found file paths in file.
		 */
		Entries entries() const;

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

inline Listfile::Entries Listfile::entries(const string &content)
{
	Entries result;
	string value(content);
	boost::algorithm::split(result, value, boost::algorithm::is_any_of(";\r\n"), boost::algorithm::token_compress_on);

	return result;
}

inline Listfile::Entries Listfile::entries() const
{
	ostringstream stream;
	MpqFile::writeData(stream);

	return entries(stream.str());
}

inline const char* Listfile::fileName() const
{
	return "(listfile)";
}

}

}

#endif
