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

#include "mpqfile.hpp"

namespace wc3lib
{

namespace mpq
{

class Listfile : public MpqFile
{
	public:
		typedef std::vector<string> Entries;

		/**
		 * The following container can be used to get all listed files via their corresponding file paths.
		 * Class Mpq doesn't provide such functionality by default since listfiles are optional.
		 */
		typedef boost::multi_index_container<MpqFile*,
		boost::multi_index::indexed_by<
		// blocks are unique indices since each file must have its own block!
		boost::multi_index::ordered_unique<boost::multi_index::tag<boost::filesystem::path, std::string>, boost::multi_index::const_mem_fun<MpqFile, const boost::filesystem::path&, &MpqFile::path> >
		>
		> Files;

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

		const Files& files() const;

		/**
		 * Considers clearing files (\ref files()), as well.
		 */
		virtual void removeData();
		/**
		 * Refreshes all entries by getting already set file paths of all files.
		 * \note Clears already existing entries! Be sure to call this member function when paths of files are set already.
		 */
		void refresh();

		/**
		 * Reads data from corresponding file "(listfile)" of the archive and stores paths into container.
		 * \note Always returns 0.
		 */
		std::streamsize readData();
		/**
		 * Writes data into corresponding file "(listfile)" of the archive.
		 */
		std::streamsize writeData();

		virtual const char* fileName() const;

	protected:
		friend class Mpq;

		Listfile(Mpq *mpq, Hash *hash);

		Files& files();

	private:
		Files m_files;
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
	std::ostringstream stream;
	MpqFile::writeData(stream);

	return entries(stream.str());
}

inline const Listfile::Files& Listfile::files() const
{
	return this->m_files;
}

inline const char* Listfile::fileName() const
{
	return "(listfile)";
}

inline Listfile::Files& Listfile::files()
{
	return this->m_files;
}

}

}

#endif
