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

#include <iostream>

#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/timer/timer.hpp>

#include <StormLib.h>

#include "../../mpq.hpp"
#include "extract.hpp"

namespace wc3lib
{

namespace test
{

void extractStormLib(TMPQArchive &archive, const boost::filesystem::path &filePath, const boost::filesystem::path &target)
{
	boost::filesystem::ofstream ofstream(target, std::ios::out | std::ios::binary);

	if (!ofstream)
	{
		std::cerr << boost::format(_("Couldn't open file %1%.")) % target << std::endl;

		return;
	}

	TMPQFile *file;

	if (SFileOpenFileEx(&archive, filePath.c_str(), SFILE_OPEN_FROM_MPQ, (void**)&file))
	{
		BYTE value;

		while (SFileReadFile(&file, &value, sizeof(value)))
			ofstream.write((const char*)&value, sizeof(value));

		SFileCloseFile(file);
	}
	else
		std::cerr << boost::format(_("Couldn't open file %1%.")) % filePath << std::endl;
}

void extract(mpq::Archive &archive, const boost::filesystem::path &filePath, const boost::filesystem::path &target)
{
	boost::filesystem::ofstream ofstream(target, std::ios::out | std::ios::binary);

	if (!ofstream)
	{
		std::cerr << boost::format(_("Couldn't open file %1%.")) % target << std::endl;

		return;
	}

	const mpq::File *file = archive.findFile(filePath);

	if (file != 0)
	{
		try
		{
			file->writeData(ofstream);
		}
		catch (Exception &exception)
		{
			std::cerr << boost::format(_("Couldn't read file %1%.")) % filePath << std::endl;
		}
	}
	else
		std::cerr << boost::format(_("Couldn't find file %1%.")) % filePath << std::endl;
}

void extractStormLib(const boost::filesystem::path &archivePath, const boost::filesystem::path &filePath, const boost::filesystem::path &target)
{
	TMPQArchive *archive;

	if (SFileOpenArchive(archivePath.c_str(), 0, MPQ_OPEN_READ_ONLY, (void**)&archive))
	{
		extractStormLib(*archive, filePath, target);

		SFileCloseArchive(archive);
	}
	else
		std::cerr << boost::format(_("Couldn't open file %1%.")) % archivePath << std::endl;
}

void extract(const boost::filesystem::path &archivePath, const boost::filesystem::path &filePath, const boost::filesystem::path &target)
{
	boost::scoped_ptr<mpq::Archive> archive(new mpq::Archive());

	if (archive->open(archivePath))
	{
		extract(*archive, filePath, target);
	}
	else
		std::cerr << boost::format(_("Couldn't open file %1%.")) % archivePath << std::endl;
}

int compareExtraction(const boost::filesystem::path &archivePath, const boost::filesystem::path &filePath, const boost::filesystem::path &target)
{
	boost::timer::cpu_timer timer;
	timer.start();
	extractStormLib(archivePath, filePath, target);
	timer.stop();
	const boost::timer::cpu_times stormTimes = timer.elapsed();

	boost::filesystem::remove(target);

	timer.start();
	extract(archivePath, filePath, target);
	timer.stop();
	const boost::timer::cpu_times wc3libTimes = timer.elapsed();

	boost::filesystem::remove(target);

	std::cout << "without open archive" << std::endl;
	std::cout << filePath << std::endl;
	std::cout << "Storm time: " << stormTimes.wall << std::endl;
	std::cout << "wc3lib time: " << wc3libTimes.wall << std::endl;
	std::cout << "-----------------" << std::endl;

	return stormTimes.wall - wc3libTimes.wall;
}

int compareExtraction(mpq::Archive &archive, TMPQArchive &stormArchive, const boost::filesystem::path &filePath, const boost::filesystem::path &target)
{
	boost::timer::cpu_timer timer;
	timer.start();
	extractStormLib(stormArchive, filePath, target);
	timer.stop();
	const boost::timer::cpu_times stormTimes = timer.elapsed();

	boost::filesystem::remove(target);

	timer.start();
	extract(archive, filePath, target);
	timer.stop();
	const boost::timer::cpu_times wc3libTimes = timer.elapsed();

	boost::filesystem::remove(target);

	std::cout << "with open archive" << std::endl;
	std::cout << filePath << std::endl;
	std::cout << "Storm time: " << stormTimes.wall << std::endl;
	std::cout << "wc3lib time: " << wc3libTimes.wall << std::endl;
	std::cout << "-----------------" << std::endl;

	return stormTimes.wall - wc3libTimes.wall;
}

}

}
