/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#include "../mpq.hpp"
#include "../core.hpp"

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <boost/timer.hpp>

using namespace wc3lib;
using namespace wc3lib::mpq;

namespace
{

template<typename T>
std::string sizeString(T size, bool humanReadable, bool decimal)
{
	if (humanReadable)
	{
		if (decimal)
			return sizeStringDecimal<T>(size);
		else
			return sizeStringBinary<T>(size);
	}

	std::ostringstream result;
	result << size;

	return result.str();
}

std::string flagsString(BOOST_SCOPED_ENUM(Block::Flags) flags)
{
	if (flags == Block::Flags::None)
		return _("Uncompressed");
	else
	{
		std::string result;

		if (flags & Block::Flags::IsFile)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("File")).str();

		if (flags & Block::Flags::IsSingleUnit)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Single Unit")).str();

		if (flags & Block::Flags::UsesEncryptionKey)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Uses Encryption Key")).str();

		if (flags & Block::Flags::IsEncrypted)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Encrypted")).str();

		if (flags & Block::Flags::IsCompressed)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Compressed")).str();

		if (flags & Block::Flags::IsImploded)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Imploded")).str();

		return result;
	}

	return "";
}

std::string compressionString(BOOST_SCOPED_ENUM(Sector::Compression) compression)
{
	if (compression == Sector::Compression::Uncompressed)
		return _("Uncompressed");
	else
	{
		std::string result;

		if (compression & Sector::Compression::ImaAdpcmMono)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("ImaAdpcmMono")).str();

		if (compression & Sector::Compression::ImaAdpcmStereo)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("ImaAdpcmStereo")).str();

		if (compression & Sector::Compression::Huffman)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Huffman")).str();

		if (compression & Sector::Compression::Deflated)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Deflated")).str();

		if (compression & Sector::Compression::Imploded)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Imploded")).str();

		if (compression & Sector::Compression::Bzip2Compressed)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Bzip2Compressed")).str();

		if (compression & Sector::Compression::Sparse)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Sparse")).str();

		if (compression & Sector::Compression::Lzma)
			result = ((result.empty() ? boost::format(_("%1%")) : boost::format(_("%1%, %2%")) % result) % _("Lzma")).str();

		return result;
	}

	return "";
}

std::string fileInfo(const MpqFile &file, bool humanReadable, bool decimal)
{
	MpqFile::Sectors sectors = file.realSectors();
	std::stringstream sstream;
	sstream << boost::format(_("%1%\nCompressed: %2%\nEncrypted: %3%\nImploded: %4%\nFlags: %5%\nCompressed size: %6%\nSize: %7%\nHash A: %8%\nHash B: %9%\nKey: %10%")) % file.path() % boolString(file.isCompressed()) % boolString(file.isEncrypted()) % boolString(file.isImploded()) % flagsString(file.block()->flags()) % sizeString(file.compressedSize(), humanReadable, decimal) % sizeString(file.size(), humanReadable, decimal) % file.hash()->hashData().filePathHashA() % file.hash()->hashData().filePathHashB() % file.fileKey();

	if (sectors.size() > 0)
	{
		sstream << std::endl << boost::format(_("\n%1% Sectors:")) % sectors.size();

		BOOST_FOREACH(const MpqFile::SectorPtr &sector, sectors)
				sstream << boost::format(_("\nSector %1%:\n-- Offset: %2%\n-- Size: %3%\n-- Compression: %4%")) % sector->sectorIndex() % sector->sectorOffset() % sizeString(sector->sectorSize(), humanReadable, decimal) % compressionString(sector->compression());
	}

	return sstream.str();
}

std::string formatString(BOOST_SCOPED_ENUM(Mpq::Format) format)
{
	switch (format)
	{
		case Mpq::Format::Mpq1:
			return _("MPQ1");

		case Mpq::Format::Mpq2:
			return _("MPQ2");
	}

	return _("Invalid");
}

std::string archiveInfo(const Mpq &archive, bool humanReadable, bool decimal)
{
	std::stringstream sstream;
	sstream << boost::format(_("%1%\nSize: %2%\nHashes: %3%\nBlocks: %4%\nFiles: %5%\nFormat %6%\nUsed block space: %7%\nUnused block space: %8%\nSector size: %9%\nEntire block size: %10%\nEntire file size: %11%\nHas (listfile) file: %12%\nHas (attributes) file: %13%\nHas (signature) file: %14%\nHas strong digital signature: %15%\n")) % archive.path() % sizeString(archive.size(), humanReadable, decimal) % archive.hashes().size() % archive.blocks().size() % archive.files().size() %  formatString(archive.format()) % sizeString(archive.entireUsedBlockSize(), humanReadable, decimal) % sizeString(archive.entireEmptyBlockSize(), humanReadable, decimal) % sizeString(archive.entireBlockSize(), humanReadable, decimal) % sizeString(archive.entireFileSize(), humanReadable, decimal) % boolString(archive.containsListfileFile()) % boolString(archive.containsAttributesFile()) % boolString(archive.containsSignatureFile()) % boolString(archive.hasStrongDigitalSignature());

	if (archive.containsAttributesFile() && archive.attributesFile()->extendedAttributes() != Attributes::ExtendedAttributes::None)
		sstream << boost::format(_("Extended attributes:\nHas CRC32s: %1%\nHas time stamps: %2%\nHas MD5s: %3%")) % boolString(archive.attributesFile()->extendedAttributes() & Attributes::ExtendedAttributes::FileCrc32s) % boolString(archive.attributesFile()->extendedAttributes() & Attributes::ExtendedAttributes::FileTimeStamps) % boolString(archive.attributesFile()->extendedAttributes() & Attributes::ExtendedAttributes::FileMd5s);

	return sstream.str();
}

inline bool hasListfile(const MpqFile::ListfileEntries &arg)
{
	return std::find(arg.begin(), arg.end(), "(listfile)") != arg.end();
}

void extract(const Mpq &mpq,
#ifndef UNIX
const std::string entry,
#else
std::string entry,
#endif
const boost::program_options::variables_map &vm)
{
	const MpqFile *file = mpq.findFile(entry);

	if (file == 0)
	{
		std::cerr << boost::format(_("Error occured while extracting file \"%1%\": File doesn't exist.")) % entry << std::endl;

		return;
	}

#ifdef UNIX
	// (listfile) entries usually have Windows path format
	boost::algorithm::replace_all(entry, "\\", "/");
#endif
	// output direcotry is archive's basename in its actual directory (name without extension)
	boost::filesystem::path entryPath = mpq.path().parent_path() / boost::filesystem::basename(mpq.path()) / boost::filesystem::path(entry).parent_path();

	if (!vm.count("overwrite") && boost::filesystem::exists(entryPath))
	{
		std::cerr << boost::format(_("Error occured while creating extraction directory \"%1%\": \"Directory exists already (use --overwrite to create it anyway\".")) % entry << std::endl;

		return;
	}

	if (!boost::filesystem::is_directory(entryPath) && !boost::filesystem::create_directories(entryPath))
	{
		std::cerr << boost::format(_("Error occured while extracting file \"%1%\": Unable to create output directory \"%2%\".")) % entry %
		entryPath << std::endl;

		return;
	}

	entryPath /= boost::filesystem::path(entry).filename();

	if (!vm.count("overwrite") && boost::filesystem::exists(entryPath))
	{
		std::cerr << boost::format(_("Error occured while extracting file \"%1%\": \"File exists already (use --overwrite to extract it anyway\".")) % entry << std::endl;

		return;
	}

	mpq::ofstream out(entryPath, std::ios::out | std::ios::binary);

	try
	{
		checkStream(out);
		file->writeData(out);
	}
	catch (Exception &exception)
	{
		std::cerr << boost::format(_("Error occured while extracting file \"%1%\": \"%2%\".")) % entry % exception.what() << std::endl;

		return;
	}

#ifdef DEBUG
	// TEST
	// Creates info file for each extracted file for analysing it
	mpq::ofstream infoOut(entryPath.string() + "info", std::ios::out);
	infoOut << fileInfo(*file, vm.count("human-readable"), vm.count("decimal"));
	// END TEST
#endif
}

}

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("mpq", LOCALE_DIR);
	textdomain("mpq");

	static const char *version = "0.1";
	std::string format;
	typedef std::vector<boost::filesystem::path> Paths;
	typedef std::vector<std::string> Strings;
	// TODO throws exception when using escaped white spaces! - string work around
	Strings listfileStrings;
	Strings archiveStrings;
	Strings fileStrings;
	Paths listfiles;
	Paths archivePaths;
	Paths filePaths;

	const boost::program_options::command_line_style::style_t pstyle = boost::program_options::command_line_style::style_t(
	boost::program_options::command_line_style::unix_style
	);

	// boost::format(_("mpq %1%.\n\n")) % version << _("Usage: mpq [Options] [MPQ files]\n\n") <<
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of mpq."))
	("help,h",_("Shows this text."))
	// options
	("human-readable,h", boost::program_options::value<bool>()->default_value(true), _("Shows output sizes in an human-readable format."))
	("decimal,d", _("Shows decimal sizes (factor 1000 not 1024)"))
	("format,F", boost::program_options::value<std::string>(&format)->default_value("1"), _("Selects the format of the created MPQ archive and modified files: <format:format:format>\nHere's a list of valid expressions:\n* \"1\"\n* \"2\"\n* \"listfile\"\n* \"attributes\""))
	("list-files,L", boost::program_options::value<Strings>(&listfileStrings), _("Uses given listfiles to detect file paths of MPQ archives."))
	("overwrite", _("Overwrites existing files and directories when creating or extracting files."))
	("remove-files", _("Removes files/archives after adding them to the MPQ archives."))
	("interactive", _("Asks for confirmation for every action."))
	("store-sectors,s", _("Stores sector information which increases performance when accessing sector offsets and sizes (especially for encrypted files) but increases memory usage, as well."))

	// operations
	("add,a", _("Adds files of MPQ archives or from hard disk to another archive."))
	("create,c", _("Creates new MPQ archives."))
	("diff,compare,d", _("Finds differences between archives."))
	("list,t", _("Lists all contained files of all read MPQ archives."))
	("update,u", _("Only append files that are newer than the existing archives."))
	("extract,get,x", _("Extract files from MPQ archives. If no files are specified via -f all files are extracted from given MPQ archives."))
	("delete", _("Deletes files from MPQ archives."))
	("info,i", _("Shows some basic information about all read MPQ archives. If any files are specified via -f their information will be shown as well."))

	// input
	("archives,A", boost::program_options::value<Strings>(&archiveStrings), _("Expected MPQ archives."))
	("files,f", boost::program_options::value<Strings>(&fileStrings), _("Expected MPQ archives."))
	;

	boost::program_options::positional_options_description p;
	p.add("archives", -1);

	boost::program_options::variables_map vm;

	try
	{
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).style(pstyle).options(desc).positional(p).run(), vm);
	}
	catch (std::exception &exception)
	{
		std::cerr << boost::format(_("Error while parsing program options: \"%1%\"")) % exception.what() << std::endl;

		return EXIT_FAILURE;
	}

	boost::program_options::notify(vm);

	// WORKAROUND
	listfiles.resize(listfileStrings.size());
	listfiles.assign(listfileStrings.begin(), listfileStrings.end());
	archivePaths.resize(archiveStrings.size());
	archivePaths.assign(archiveStrings.begin(), archiveStrings.end());
	filePaths.resize(fileStrings.size());
	filePaths.assign(fileStrings.begin(), fileStrings.end());

	// contains all file entries
	typedef std::list<MpqFile::ListfileEntries> Listfiles;
	Listfiles listfileEntries;

	BOOST_FOREACH(Paths::const_reference path, listfiles)
	{
		mpq::ifstream in(path, std::ios::in);

		if (!in)
		{
			std::cerr << boost::format(_("Unable to open listfile %1%.")) % path << std::endl;

			continue;
		}

		const mpq::string::size_type size = (mpq::string::size_type)(endPosition(in)) + 1;
		mpq::string content(size, '0');
		in.read(&content[0], size);
		listfileEntries.push_back(MpqFile::listfileEntries(content));
	}

	if (vm.count("version"))
	{
		std::cout << boost::format(_(
		"mpq %1%.\n"
		"Copyright © 2010 Tamino Dauth\n"
		"License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law."
		)) % version << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		std::cout << _("\nReport bugs to tamino@cdauth.eu or on http://sourceforge.net/projects/vjasssdk/") << std::endl;

		return EXIT_SUCCESS;
	}

	if (archivePaths.empty())
	{
		std::cerr << _("Missing archive arguments.") << std::endl;

		return EXIT_FAILURE;
	}

	if (vm.count("info"))
	{
		BOOST_FOREACH(const Paths::reference path, archivePaths)
		{
			if (!boost::filesystem::is_regular_file(path))
			{
				std::cerr << boost::format(_("File \"%1%\" does not seem to be a regular file and will be skipped.")) % path.string() << std::endl;

				continue;
			}

			boost::scoped_ptr<Mpq> mpq(new Mpq());

			try
			{
				std::streamsize size = mpq->open(path);
			}
			catch (wc3lib::Exception &exception)
			{
				std::cerr << boost::format(_("Error occured while opening file \"%1%\":\n\"%2%\"")) % path.string() % exception.what() << std::endl;

				continue;
			}

			std::cout
			<< archiveInfo(*mpq, vm.count("human-readable"), vm.count("decimal")) << std::endl;

			if (!filePaths.empty())
			{
				BOOST_FOREACH(Paths::const_reference path, filePaths)
				{
					const MpqFile *file = mpq->findFile(path);

					if (file != 0)
						std::cout << fileInfo(*file, vm.count("human-readable"), vm.count("decimal")) << std::endl;
					else
						std::cerr << boost::format(_("Error occured while extracting file \"%1%\": File doesn't exist.")) % path << std::endl;
				}
			}

			/*

			std::size_t invalidFiles = 0;
			BOOST_SCOPED_ENUM(Block::Flags) flags = Block::Flags::None;

			BOOST_FOREACH(const Mpq::FilePtr mpqFile, mpq->files().get<0>())
			{
				if (mpqFile->sectors().empty())
				{
					++invalidFiles;

					if (flags == Block::Flags::None || flags != mpqFile->block()->flags())
					{
						std::cout << "New flag." << std::endl;
						flags = mpqFile->block()->flags();
					}
				}
			}

			std::cout << "Invalid files: " << invalidFiles << " with flags " << std::hex << flags << std::dec << std::endl;
			*/
		}
	}

	if (vm.count("list"))
	{
		std::cout << _("Listing contained files:") << std::endl;

		BOOST_FOREACH(const Paths::reference path, archivePaths)
		{
			if (!boost::filesystem::is_regular_file(path))
			{
				std::cerr << boost::format(_("File \"%1%\" does not seem to be a regular file and will be skipped.")) % path.string() << std::endl;

				continue;
			}

			boost::scoped_ptr<Mpq> mpq(new Mpq());

			try
			{
				std::streamsize size = mpq->open(path);
			}
			catch (wc3lib::Exception &exception)
			{
				std::cerr << boost::format(_("Error occured while opening file \"%1%\": \"%2%\"")) % path.string() % exception.what() << std::endl;

				continue;
			}

			BOOST_FOREACH(const Mpq::FilePtr mpqFile, mpq->files().get<0>())
				std::cout << mpqFile->path() << std::endl;
		}
	}

	if (vm.count("extract"))
	{
		BOOST_FOREACH(const Paths::reference path, archivePaths)
		{
			if (!boost::filesystem::is_regular_file(path))
			{
				std::cerr << boost::format(_("File \"%1%\" does not seem to be a regular file and will be skipped.")) % path.string() << std::endl;

				continue;
			}

			boost::scoped_ptr<Mpq> mpq(new Mpq());
			mpq->setStoreSectors(vm.count("store-sectors"));

			try
			{
				std::streamsize size = mpq->open(path);
			}
			catch (wc3lib::Exception &exception)
			{
				std::cerr << boost::format(_("Error occured while opening file \"%1%\": \"%2%\"")) % path.string() % exception.what() << std::endl;

				continue;
			}

			/*
			boost::filesystem::path dir(path.filename());

			if (!boost::filesystem::create_directories(dir))
			{
				std::cerr << boost::format(_("Error occured while opening file \"%1%\": Could not create output directory \"%2%\".")) % path.string() % dir << std::endl;

				continue;
			}
			*/

			if (filePaths.empty())
			{
				if (mpq->listfileFile() != 0)
					listfileEntries.push_front(mpq->listfileFile()->listfileEntries());

				// usually does not list itself
				if (std::find_if(listfileEntries.begin(), listfileEntries.end(), hasListfile) == listfileEntries.end())
					listfileEntries.push_front(MpqFile::ListfileEntries(1, "(listfile)"));

				BOOST_FOREACH(Listfiles::const_reference vector, listfileEntries)
				{
					BOOST_FOREACH(Listfiles::value_type::value_type entry, vector)
						extract(*mpq, entry, vm);
				}
			}
			else
			{
				BOOST_FOREACH(Paths::const_reference entry, filePaths)
					extract(*mpq, entry.string(), vm);
			}
		}
	}

	return EXIT_SUCCESS;
}
