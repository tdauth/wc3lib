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
#ifdef DEBUG
#include <StormLib/stormlib/StormLib.h>
#endif

using namespace wc3lib;
using namespace wc3lib::mpq;

template<typename T>
static std::string sizeString(T size, bool humanReadable, bool decimal)
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
	// formatting options
	("human-readable,h", boost::program_options::value<bool>()->default_value(true), _("Shows output sizes in an human-readable format."))
	("decimal,d", _("Shows decimal sizes (factor 1000 not 1024)"))
	("format,F", boost::program_options::value<std::string>(&format)->default_value("1"), _("Selects the format of the created MPQ archive and modified files: <format:format:format>\nHere's a list of valid expressions:\n* \"1\"\n* \"2\"\n* \"listfile\"\n* \"attributes\""))
	("list-files,L", boost::program_options::value<Strings>(&listfileStrings), _("Uses given listfiles to detect file paths of MPQ archives."))
	("overwrite", _("Overwrites existing files and directories when creating or extracting files."))
	("remove-files", _("Removes files/archives after adding them to the MPQ archives."))
	("interactive", _("Asks for confirmation for every action."))

	// operations
	("add,a", _("Adds files of MPQ archives or from hard disk to another archive."))
	("create,c", _("Creates new MPQ archives."))
	("diff,compare,d", _("Finds differences between archives."))
	("list,t", _("Lists all contained files of all read MPQ archives."))
	("update,u", _("Only append files that are newer than the existing archives."))
	("extract,get,x", _("Extract files from MPQ archives. If no files are specified via -f all files are extracted from given MPQ archives."))
	("delete", _("Deletes files from MPQ archives."))
	("info,i", _("Shows some basic information about all read MPQ archives."))
	("benchmark,b", _("Compares various functionalities of wc3lib and StormLib."))

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
	std::list<mpq::string> listfileEntries;

	BOOST_FOREACH(Paths::const_reference path, listfiles)
	{
		mpq::ifstream in(path, std::ios::in);

		try
		{
			checkStream(in);
			mpq::string content;
			in >> content;
			checkStream(in);
			MpqFile::ListfileEntries entries = MpqFile::listfileEntries(content);

			BOOST_FOREACH(MpqFile::ListfileEntries::const_reference entry, entries)
				listfileEntries.push_back(entry);
		}
		catch (Exception &exception)
		{
			std::cerr << boost::format(_("Error occured while reading listfile \"%1%\": \"%2%\".")) % path % exception.what() << std::endl;

			continue;
		}
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
			<< mpq->path().string() << std::endl
			<< boost::format(_("Size: %1%")) % sizeString(mpq->size(), vm.count("human-readable"), vm.count("decimal")) << std::endl
			<< boost::format(_("Hashes: %1%")) % mpq->hashes().size() << std::endl
			<< boost::format(_("Blocks: %1%")) % mpq->blocks().size() << std::endl
			<< boost::format(_("Files: %1%")) % mpq->files().size() << std::endl
			<< boost::format(_("Format: %1%")) % mpq->format() << std::endl
			<< boost::format(_("Used block space: %1%")) % sizeString(mpq->entireUsedBlockSize(), vm.count("human-readable"), vm.count("decimal")) << std::endl
			<< boost::format(_("Unused block space: %1%")) % sizeString(mpq->entireEmptyBlockSize(), vm.count("human-readable"), vm.count("decimal")) << std::endl
			<< boost::format(_("Sector size: %1%")) % sizeString(mpq->sectorSize(), vm.count("human-readable"), vm.count("decimal")) << std::endl
			<< boost::format(_("Entire block size: %1%")) % sizeString(mpq->entireBlockSize(), vm.count("human-readable"), vm.count("decimal")) << std::endl
			<< boost::format(_("Entire file size: %1%")) % sizeString(mpq->entireFileSize(), vm.count("human-readable"), vm.count("decimal")) << std::endl
			<< boost::format(_("Has strong digital signature: %1%")) % boolString(mpq->hasStrongDigitalSignature()) << std::endl
			<< boost::format(_("Contains listfile file: %1%")) % boolString(mpq->containsListfileFile()) << std::endl
			<< boost::format(_("Contains attributes file: %1%")) % boolString(mpq->containsAttributesFile()) << std::endl
			<< boost::format(_("Contains signature file: %1%")) % boolString(mpq->containsSignatureFile()) << std::endl
			;

			if (mpq->extendedAttributes() != Mpq::ExtendedAttributes::None)
			{
				std::cout << _("Extended attributes:") << std::endl;

				if (mpq->extendedAttributes() & Mpq::ExtendedAttributes::FileCrc32s)
					std::cout << _("* File CRC32s") << std::endl;

				if (mpq->extendedAttributes() & Mpq::ExtendedAttributes::FileTimeStamps)
					std::cout << _("* File time stamps") << std::endl;

				if (mpq->extendedAttributes() & Mpq::ExtendedAttributes::FileMd5s)
					std::cout << _("* File MD5s") << std::endl;
			}

			/// \todo TEST
			const MpqFile *mpqFile = const_cast<const Mpq*>(mpq.get())->findFile("Detector.js");

			if (mpqFile == 0)
			{
				std::cerr << _("Error while searching for file.") << std::endl;
			}
			else
			{
				std::basic_stringstream<mpq::byte> testBuffer;
				std::cout << "Found file \"Detector.js\" and wrote " << mpqFile->writeData(testBuffer) << "bytes:" << std::endl;
				std::cout << testBuffer;
			}
			//else
			//	std::cout << "Flags: " << std::hex << mpqFile->hash()->block()->flags() << std::dec << "\nFile data: " << *mpqFile << std::endl;

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

			std::size_t i = 1;

			BOOST_FOREACH(const Mpq::FilePtr mpqFile, mpq->files().get<0>())
			{
				std::cout << boost::format(_("* File %1%: (Size: %2%) - %3%")) % i % sizeString(mpqFile->size(), vm.count("human-readable"), vm.count("decimal")) % mpqFile->path() << std::endl;
				++i;
			}

			std::cout << boost::format(_("%1% files total (Size: %2%)")) % (i + 1) % sizeString(mpq->entireFileSize(), vm.count("human-readable"), vm.count("decimal")) << std::endl;
		}
	}

	if (vm.count("benchmark"))
	{
		BOOST_FOREACH(const Paths::reference path, archivePaths)
		{
			if (!boost::filesystem::is_regular_file(path))
			{
				std::cerr << boost::format(_("File \"%1%\" does not seem to be a regular file and will be skipped.")) % path.string() << std::endl;

				continue;
			}

			boost::scoped_ptr<Mpq> mpq(new Mpq());

			std::cout << _("Detected debug mode.\nStarting benchmark (wc3lib vs. StormLib)") << std::endl;
			std::cout << _("Opening MPQ archive (wc3lib):") << std::endl;
			boost::timer timer;

			try
			{
				std::streamsize size = mpq->open(path);
			}
			catch (wc3lib::Exception &exception)
			{
				std::cerr << boost::format(_("Error occured while opening file \"%1%\": \"%2%\"")) % path.string() % exception.what() << std::endl;

				continue;
			}

			std::cout << boost::format(_("Result: %1%s")) % timer.elapsed() << std::endl;

			// TEST OUTPUT
			//std::cout << "License:\n" << *mpq->findFile("License.txt") << std::endl;

			// END TEST


			std::cout << _("Closing MPQ archive (wc3lib):") << std::endl;
			timer.restart();
			mpq->close();
			std::cout << boost::format(_("Result: %1%s")) % timer.elapsed() << std::endl;
/// \todo Add runtime linking support by using LibraryLoader.
#ifdef DEBUG
			std::cout << _("Opening MPQ archive (StormLib - read only):") << std::endl;
			HANDLE stormLibArchive;
			timer.restart();

			if (SFileOpenArchive(
				path.string().c_str(),           // Archive file name
				0,      // Archive priority
				0,            // Open flags, TODO MPQ_OPEN_READ_ONLY?
				&stormLibArchive                    // Pointer to result HANDLE
				))
			{
				std::cout << boost::format(_("Result: %1%s")) % timer.elapsed() << std::endl;
				std::cout << _("Closing MPQ archive (StormLib):") << std::endl;
				timer.restart();
				SFileCloseArchive(stormLibArchive);
				std::cout << boost::format(_("Result: %1%s")) % timer.elapsed() << std::endl;
			}
			else
				std::cerr << boost::format(_("Failed to open MPQ archive \"%1%\" by using StormLib.")) % path.string() << std::endl;

#else
			std::cerr << _("Since program has been compiled without debug option, StormLib cannot be used.") << std::endl;
#endif
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
				{
					BOOST_FOREACH(mpq::MpqFile::ListfileEntries::const_reference entry, mpq->listfileFile()->listfileEntries())
						listfileEntries.push_back(entry);
				}

				BOOST_FOREACH(mpq::string entry, listfileEntries)
				{
					MpqFile *file = mpq->findFile(entry);

					if (file == 0)
					{
						std::cerr << boost::format(_("Error occured while extracting file \"%1%\": File doesn't exist.")) % entry << std::endl;

						continue;
					}


#ifdef UNIX
					// (listfile) entries usually have Windows path format
					boost::algorithm::replace_all(entry, "\\", "/");
#endif
					// output direcotry is archive's basename in its actual directory (name without extension)
					boost::filesystem::path entryPath = mpq->path().parent_path() / boost::filesystem::basename(mpq->path()) / boost::filesystem::path(entry).parent_path();
					std::cout << "Dir string " << entryPath << std::endl;

					if (!boost::filesystem::is_directory(entryPath) && !boost::filesystem::create_directories(entryPath))
					{
						std::cerr << boost::format(_("Error occured while extracting file \"%1%\": Unable to create output directory \"%2%\".")) % entry %
						entryPath << std::endl;

						continue;
					}

					entryPath /= boost::filesystem::path(entry).filename();
					mpq::ofstream out(entryPath, std::ios::out | std::ios::binary);

					try
					{
						checkStream(out);
						file->writeData(out);
					}
					catch (Exception &exception)
					{
						std::cerr << boost::format(_("Error occured while extracting file \"%1%\": \"%2%\".")) % entry % exception.what() << std::endl;

						continue;
					}
				}
			}
		}
	}

	return EXIT_SUCCESS;
}
