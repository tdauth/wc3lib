/***************************************************************************
 *   Copyright (C) 2012 by Tamino Dauth                                    *
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

#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>

#include "extract.hpp"
#include "../../mpq.hpp"

using namespace wc3lib;

int main(int argc, char *argv[])
{
	if (argc < 2)
		return 1;

	boost::scoped_ptr<mpq::Mpq> archive(new mpq::Mpq());
	mpq::Listfile::Entries entries;
	const boost::filesystem::path archivePath = argv[1];

	try
	{
		archive->open(archivePath);
		entries = archive->listfileFile()->entries();

	}
	catch (Exception e)
	{
		return 1;
	}

	archive.reset();

	int counter = 0;
	int differences = 0;

	BOOST_FOREACH(mpq::Listfile::Entries::reference ref, entries)
	{
		differences += wc3lib::test::compareExtraction(argv[1], ref, "tmp");

		++counter;
	}

	differences /= counter;
	std::cout << "Average differences (with no permanent open archive): " << differences << std::endl;

	counter = 0;
	differences = 0;
	archive.reset(new mpq::Mpq());
	archive->setStoreSectors(true);

	try
	{
		archive->open(archivePath);
	}
	catch (Exception e)
	{
		return 1;
	}

	TMPQArchive *stormArchive;

	if (SFileOpenArchive(archivePath.c_str(), 0, MPQ_OPEN_READ_ONLY, (void**)&stormArchive))
	{
		BOOST_FOREACH(mpq::Listfile::Entries::reference ref, entries)
		{
			differences += wc3lib::test::compareExtraction(*archive, *stormArchive, ref, "tmp");

			++counter;
		}

		SFileCloseArchive(stormArchive);
	}

	archive.reset();

	differences /= counter;
	std::cout << "Average differences (with permanent open archive): " << differences << std::endl;

	return 0;
}
