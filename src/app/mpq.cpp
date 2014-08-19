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
#include "../utilities.hpp"

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>

using namespace wc3lib;
using namespace wc3lib::mpq;

namespace
{

inline bool hasListfile(const Listfile::Entries &arg)
{
	return std::find(arg.begin(), arg.end(), "(listfile)") != arg.end();
}

void extract(Mpq &mpq,
#ifndef UNIX
const std::string &entry,
#else
std::string entry,
#endif
const boost::program_options::variables_map &vm)
{
	mpq::MpqFile file = mpq.findFile(entry);

	if (!file.isValid())
	{
		std::cerr << boost::format(_("Error occured while extracting file \"%1%\": File doesn't exist.")) % entry << std::endl;

		return;
	}
#ifdef DEBUG
	std::string oldEntry = entry;
#endif

#ifdef UNIX
	// (listfile) entries usually have Windows path format
	Listfile::toNativePath(entry);
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

	ofstream out(entryPath, std::ios::out | std::ios::binary);

	try
	{
		checkStream(out);
		file.writeData(out);
	}
	catch (Exception &exception)
	{
		std::cerr << boost::format(_("Error occured while extracting file \"%1%\": \"%2%\".")) % entry % exception.what() << std::endl;
	}
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
	("human-readable", boost::program_options::value<bool>()->default_value(true), _("Shows output sizes in an human-readable format."))
	("decimal,d", _("Shows decimal sizes (factor 1000 not 1024)"))
	("format,F", boost::program_options::value<std::string>(&format)->default_value("1"), _("Selects the format of the created MPQ archive and modified files: <format:format:format>\nHere's a list of valid expressions:\n* \"1\"\n* \"2\"\n* \"listfile\"\n* \"attributes\""))
	("list-files,L", boost::program_options::value<Strings>(&listfileStrings), _("Uses given listfiles to detect file paths of MPQ archives."))
	("overwrite", _("Overwrites existing files and directories when creating or extracting files."))
	("remove-files", _("Removes files/archives after adding them to the MPQ archives."))
	("interactive", _("Asks for confirmation for every action."))
#ifdef DEBUG
	("debug", _("Creates info files for each extracted file."))
#endif

	// operations
	("add,a", _("Adds files of MPQ archives or from hard disk to another archive."))
	("create,c", _("Creates new MPQ archives."))
	("diff,d", _("Finds differences between archives."))
	("list,t", _("Lists all contained files of all read MPQ archives."))
	("update,u", _("Only append files that are newer than the existing archives."))
	("extract,get,x", _("Extract files from MPQ archives. If no files are specified via -f all files are extracted from given MPQ archives."))
	("delete", _("Deletes files from MPQ archives."))
	("info,i", _("Shows some basic information about all read MPQ archives. If any files are specified via -f their information will be shown as well."))

	// input
	("archives,A", boost::program_options::value<Strings>(&archiveStrings), _("Expected MPQ archives."))
	("files,f", boost::program_options::value<Strings>(&fileStrings), _("Expected files."))
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
		std::cout << _("Usage: mpq [options] [archives]") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << _("\nReport bugs to tamino@cdauth.eu or on https://wc3lib.org") << std::endl;

		return EXIT_SUCCESS;
	}

	// WORKAROUND
	listfiles.resize(listfileStrings.size());
	listfiles.assign(listfileStrings.begin(), listfileStrings.end());
	archivePaths.resize(archiveStrings.size());
	archivePaths.assign(archiveStrings.begin(), archiveStrings.end());
	filePaths.resize(fileStrings.size());
	filePaths.assign(fileStrings.begin(), fileStrings.end());

	// contains all file entries
	typedef std::list<Listfile::Entries> Listfiles;
	Listfiles listfileEntries;

	BOOST_FOREACH(Paths::const_reference path, listfiles)
	{
		ifstream in(path, std::ios::in);

		if (!in)
		{
			std::cerr << boost::format(_("Unable to open listfile %1%.")) % path << std::endl;

			continue;
		}

		const string::size_type size = (string::size_type)(endPosition(in)) + 1;
		string content(size, '0');
		in.read(&content[0], size);
		listfileEntries.push_back(Listfile::entries(content));
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
				std::cerr << boost::format(_("File %1% does not seem to be a regular file and will be skipped.")) % path << std::endl;

				continue;
			}

			boost::scoped_ptr<Mpq> mpq(new Mpq());

			try
			{
				std::streamsize size = mpq->open(path);
			}
			catch (wc3lib::Exception &exception)
			{
				std::cerr << boost::format(_("Error occured while opening file %1%:\n\"%2%\"")) % path % exception.what() << std::endl;

				continue;
			}

			std::cout
			<< archiveInfo(*mpq, vm.count("human-readable"), vm.count("decimal")) << std::endl;

			if (!filePaths.empty())
			{
				BOOST_FOREACH(Paths::const_reference path, filePaths)
				{
					MpqFile file = mpq->findFile(path);

					if (file.isValid())
					{
						std::cout << fileInfo(file, vm.count("human-readable"), vm.count("decimal")) << std::endl;
					}
					else
					{
						std::cerr << boost::format(_("Error occured while getting info of file %1%: File doesn't exist.")) % path << std::endl;
					}
				}
			}
		}
	}

	if (vm.count("list"))
	{
		std::cout << _("Listing contained files:") << std::endl;

		BOOST_FOREACH(Paths::const_reference path, archivePaths)
		{
			if (!boost::filesystem::is_regular_file(path))
			{
				std::cerr << boost::format(_("File %1% does not seem to be a regular file and will be skipped.")) % path << std::endl;

				continue;
			}

			boost::scoped_ptr<Mpq> mpq(new Mpq());

			try
			{
				std::streamsize size = mpq->open(path);
			}
			catch (wc3lib::Exception &exception)
			{
				std::cerr << boost::format(_("Error occured while opening file %1%: \"%2%\"")) % path % exception.what() << std::endl;

				continue;
			}

			if (mpq->containsListfileFile())
			{
				Listfile listfile = mpq->listfileFile();

				if (listfile.isValid())
				{
					Listfile::Entries entries = listfile.entries();
					std::sort(entries.begin(), entries.end()); // sort alphabetically

					BOOST_FOREACH(Listfile::Entries::const_reference entry, entries)
					{
						std::cout << entry << std::endl;
					}
				}
			}
		}
	}

	if (vm.count("extract"))
	{
		BOOST_FOREACH(Paths::const_reference path, archivePaths)
		{
			if (!boost::filesystem::is_regular_file(path))
			{
				std::cerr << boost::format(_("File %1% does not seem to be a regular file and will be skipped.")) % path << std::endl;

				continue;
			}

			boost::scoped_ptr<Mpq> mpq(new Mpq());

			try
			{
				std::streamsize size = mpq->open(path);
			}
			catch (wc3lib::Exception &exception)
			{
				std::cerr << boost::format(_("Error occured while opening file %1%: \"%2%\"")) % path % exception.what() << std::endl;

				continue;
			}

			if (filePaths.empty())
			{
				if (mpq->containsListfileFile())
				{
					listfileEntries.push_front(mpq->listfileFile().entries());
				}

				// usually does not list itself, TODO performance is very poor, maybe we should just push it whenever it's not front
				if (std::find_if(listfileEntries.begin(), listfileEntries.end(), hasListfile) == listfileEntries.end())
				{
					listfileEntries.push_front(Listfile::Entries(1, "(listfile)"));
				}

				BOOST_FOREACH(Listfiles::const_reference vector, listfileEntries)
				{
#ifndef UNIX
					typedef Listfiles::value_type::const_reference valueType;
#else // on Unix systems we have to change the listfile entry to a Unix-like path
					typedef Listfiles::value_type::value_type valueType;
#endif

					BOOST_FOREACH(valueType entry, vector)
					{
						if (!entry.empty())
							extract(*mpq, entry, vm);
					}
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
