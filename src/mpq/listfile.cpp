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

#include "listfile.hpp"
#include "mpq.hpp"

namespace wc3lib
{

namespace mpq
{

void Listfile::removeData()
{
	this->entries().clear();
	MpqFile::removeData();
}

void Listfile::refresh()
{
	files().clear();

	BOOST_FOREACH(const Mpq::FilePtr &mpqFile, this->mpq()->files().get<0>())
	{
		// ignore empty entries and "(listfile)" itself
		if (!mpqFile->path().empty() && mpqFile->path().string() != fileName()) /// @todo Exclude directories and file (signature)?
			files().insert(mpqFile.get());
	}
}

std::streamsize Listfile::readData()
{
	Entries entries = this->entries();

	BOOST_FOREACH(Entries::const_reference path, entries)
	{
		if (!path.empty()) // ignore empty entries
		{
			MpqFile *file = this->mpq()->findFile(path);

			if (file != 0)
				this->files().insert(file);
			// TEST
			else
				std::cerr << boost::format(_("Invalid entry in \"(listfile)\" file: %1%")) % boost::filesystem::path(path) << std::endl;
		}
	}

	return 0;
}

std::streamsize Listfile::writeData()
{
	stringstream stream;

	BOOST_FOREACH(MpqFile *file, this->files())
	{
#ifdef UNIX
		std::string path(file->path().string());
		toListfileEntry(path);
#else
		const std::string &path(file->path());
#endif
		stream << path << '\n';
	}

	return MpqFile::readData(stream);
}

Listfile::Listfile(Mpq* mpq, Hash* hash): MpqFile(mpq, hash)
{
	this->m_path = fileName();
}

}

}
