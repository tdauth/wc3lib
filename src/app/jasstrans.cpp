/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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
#include <string>
#include <list>
#include <cstring>
#include <cstdlib>

#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <getopt.h>

#include "../map.hpp"
#include "../i18n.hpp"

using namespace wc3lib::map;

static BOOST_SCOPED_ENUM(Strings::ConflictResult) conflictFunction(const class String &string1, const class String &string2)
{
	std::cerr << boost::format(_("Detected string conflict:\nString 1: \"%1%\"\nString 1 default string: \"%2%\"\nString 1 translation: \"%3%\"\nString 2: \"%4%\"\nString 2 default string: \"%5%\"\nString 2 translation: \"%6%\"\nEnter 1 to use string 1, 2 to use string 2 or anything else to skip (strings will be left both).\n")) % string1.idString() % string1.defaultString() % string1.valueString() % string2.idString() % string2.defaultString() % string2.valueString() << std::endl;

	char answer;
	std::cin >> answer;

	if (answer == '0')
	{
		std::cout << _("Using string 1.") << std::endl;

		return Strings::ConflictResult::UseFirst;
	}
	else if (answer == '1')
	{
		std::cout << _("Using string 2.") << std::endl;

		return Strings::ConflictResult::UseSecond;
	}

	return Strings::ConflictResult::UseBoth;
}

static void addDirectory(const boost::filesystem::path &directoryPath, std::list<boost::filesystem::path> &filePaths, bool recursive, bool verbose)
{
	if (verbose)
		std::cout << boost::format(_("Reading directory \"%1%\".")) % directoryPath.string() << std::endl;

	boost::filesystem::directory_iterator endIterator;

	/// @todo BOOST_FOREACH is not usable on directy paths?
	for (boost::filesystem::directory_iterator iterator(directoryPath); iterator != endIterator; ++iterator)
	{
		if (!boost::filesystem::exists(iterator->status()))
		{
			if (verbose)
				std::cerr << boost::format(_("Error while reading file \"%1%\".")) % iterator->path().string() << std::endl;

			continue;
		}

		if (boost::filesystem::is_directory(iterator->status()))
		{
			if (recursive)
				addDirectory(iterator->path(), filePaths, recursive, verbose);
			else if (verbose)
				std::cerr << boost::format(_("\"%1%\" is another directory and is being skipped.")) % iterator->path().string() << std::endl;
		}
		else
			filePaths.push_back(iterator->path());
	}
}

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("jasstrans", LOCALE_DIR);
	textdomain("jasstrans");

	static const char *version = "0.1";
	bool optionVerbose = false;
	bool optionFill = false;
	bool optionReplace = false;
	bool optionRecursive = false;
	bool optionOverwrite = false;
	bool optionSkipunknown = false;
	std::list<std::string> optionTrans(1, "GetLocalizedString");
	optionTrans.push_back("GetLocalizedHotkey");
	boost::filesystem::path optionWtspath;
	boost::filesystem::path optionFdfpath;

	static struct option options[] =
	{
		{"version",                 no_argument,             0, 'v'},
		{"help",                    no_argument,             0, 'h'},
		{"verbose",                 no_argument,             0, 'V'},
		{"fill",                    no_argument,             0, 'F'},
		{"replace",                 no_argument,             0, 'P'},
		{"recursive",               no_argument,             0, 'R'},
		{"overwrite",               no_argument,             0, 'O'},
		{"skipunknown",             no_argument,             0, 'S'},
		{"trans",                   required_argument,       0, 't'},
		{"wtspath",                 required_argument,       0, 'w'},
		{"fdfpath",                 required_argument,       0, 'f'},
		{0, 0, 0, 0}
	};

	while (true)
	{
		int optionIndex = 0;
		int optionShortcut = getopt_long(argc, argv, "vhVFPROSt:w:f:", options, &optionIndex);

		if (optionShortcut == -1)
			break;

		switch (optionShortcut)
		{
			case 'v':
				std::cout
				<< boost::format(_("jasstrans %1%.\n")) % version
				<< _(
				"Copyright © 2009 Tamino Dauth\n"
				"License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n"
				"This is free software: you are free to change and redistribute it.\n"
				"There is NO WARRANTY, to the extent permitted by law."
				) << std::endl;

				return EXIT_SUCCESS;

			case 'h':
				std::cout <<
				_("Usage:\n") <<
				_("jasstrans [options] <files or directories>\n\n") <<
				_("File types will be detected by file extensions. Note that WTS and FDF files will be read first and JASS scripts afterwards. This allows you to add already existing string files.\n\n") <<
				_("Options:\n") <<
				_("-v, --version                   Shows the current version of jasstrans.\n") <<
				_("-h, --help                      Shows this output.\n") <<
				_("-V, --verbose                   Shows verbose output.\n") <<
				_("-F, --fill                      Fills string values with default strings when adding them from code files.\n") <<
				_("-P, --replace                   Replaces all translated strings by STRING_<id> in code files.\n") <<
				_("-R, --recursive                 Parses all source files in sub directories.\n") <<
				_("-O, --overwrite                 Overwrites existing FDF and WTS files and does not read them first.\n") <<
				_("-S, --skipunknown               Skips files with unknown extension, otherwise they will be interpreted as WTS files.")
				<< std::endl
				;
				std::cout << boost::format(_("-t, --trans <arg>               <arg> has to be the name of the translation function or a list of names separated by : characters. Default value: %1%.")) % optionTrans.front() << std::endl;
				std::cout << boost::format(_("-w, --wtspath <arg>             <arg> has to be the file path of WTS file. Default value: %1%.")) % optionWtspath << std::endl;
				std::cout << boost::format(_("-f, --fdfpath <arg>             <arg> has to be the file path of FDF file. Default value: %1%.")) % optionFdfpath << std::endl;
				std::cout <<
				_("\nReport bugs to tamino@cdauth.eu or on http://sourceforge.net/projects/vjasssdk/") <<
				std::endl;

				return EXIT_SUCCESS;

			case 'V':
				optionVerbose = true;

				break;

			case 'F':
				optionFill = true;

				break;

			case 'P':
				optionReplace = true;

				break;

			case 'R':
				optionRecursive = true;

				break;

			case 'O':
				optionOverwrite = true;

				break;

			case 'S':
				optionSkipunknown = true;

				break;

			case 't':
				typedef std::list<std::string> SplitContainerType;
				boost::split(optionTrans, optarg, boost::is_any_of(":"), boost::token_compress_on);

				break;

			case 'w':
				optionWtspath = optarg;

				break;

			case 'f':
				optionFdfpath = optarg;

				break;
		}
	}

	std::list<boost::filesystem::path> filePaths;

	if (optind < argc)
	{
		for ( ; optind < argc; ++optind)
		{
			bool found = false;

			BOOST_FOREACH(const boost::filesystem::path &path, filePaths)
			{
				if (path == argv[optind])
				{
						std::cerr << boost::format(_("File path \"%1%\" has already been added to list.")) % argv[optind] << std::endl;
						found = true;

						break;
				}
			}

			if (!found)
				filePaths.push_back(argv[optind]);
		}

	}
	else
	{
		std::cerr << _("Missing file arguments.") << std::endl;

		return EXIT_FAILURE;
	}

	std::list<boost::filesystem::path> invalidFilePaths;

	// filtering non-existing files and directories
	BOOST_FOREACH(const boost::filesystem::path path, filePaths)
	{
		if (!boost::filesystem::exists(path))
		{
			std::cerr << boost::format(_("File or directory \"%1%\" does not exist.")) % path.string() << std::endl;

			invalidFilePaths.push_back(path);
		}
		// is directory
		else if (boost::filesystem::is_directory(path))
		{
			addDirectory(path, filePaths, optionRecursive, optionVerbose); // if option recursive is true, sub directory entries will also be added

			//filePaths.erase(iterator);
			invalidFilePaths.push_back(path);
		}
	}

	BOOST_FOREACH(boost::filesystem::path &path, invalidFilePaths)
		filePaths.remove(path);

	if (filePaths.empty())
	{
		std::cerr << _("Missing valid file paths.") << std::endl;

		return EXIT_FAILURE;
	}

	class Strings strings;

	BOOST_FOREACH(boost::filesystem::path &path, filePaths)
	{
		std::string extension(path.extension().string());
		boost::algorithm::to_lower(extension);
		boost::filesystem::ifstream ifstream;


		if (extension == ".fdf")
			strings.readFdf(ifstream);
		else if (extension == ".wts")
			strings.read(ifstream);
		else
			continue;

		filePaths.remove(path);
	}

	BOOST_FOREACH(boost::filesystem::path &path, filePaths)
	{
		std::string extension(path.extension().string());
		boost::algorithm::to_lower(extension);

		if (extension != ".j" && extension != ".ai")
		{
			if (optionSkipunknown)
			{
				if (optionVerbose)
					std::cerr << boost::format(_("Warning: File \"%1%\" has unknown extension (\"%2%\") and will be skipped.")) % path.string() % extension << std::endl;

				continue;
			}
			else if (optionVerbose)
				std::cerr << boost::format(_("Warning: File \"%1%\" has unknown extension (\"%2%\") and will be handled as Jass script file.")) % path.string() % extension << std::endl;
		}

		boost::filesystem::fstream fstream(path, std::ios_base::in | std::ios_base::out); // we need out if optionReplace is true
		std::cout << "Test 1 with path " << path << std::endl;
		strings.parse(path, fstream, &fstream, optionReplace, optionFill, true, optionTrans, conflictFunction);
	}

	if (optionOverwrite)
	{
		if (boost::filesystem::exists(optionFdfpath))
		{
			boost::filesystem::ofstream ofstream(optionFdfpath);
			strings.writeFdf(ofstream);
		}

		if (boost::filesystem::exists(optionWtspath))
		{
			boost::filesystem::ofstream ofstream(optionWtspath);
			strings.write(ofstream);
		}

	}
	else if (optionVerbose)
	{
		if (boost::filesystem::exists(optionFdfpath))
			std::cout << _("FDF file won't be overwritten.") << std::endl;

		if (boost::filesystem::exists(optionWtspath))
			std::cout << _("WTS file won't be overwritten.") << std::endl;
	}

	if (optionVerbose)
	{
		std::size_t averageLength = 0;

		BOOST_FOREACH(Strings::StringListValueConst value, strings.stringList())
			averageLength += value.second->defaultString().length();

		std::cout << "Whole length is " << averageLength << std::endl;
		if (strings.stringList().size() != 0)
			averageLength /= strings.stringList().size();

		std::cout << boost::format(_("Result: %1% strings, %2% is average length.")) % strings.stringList().size() % averageLength << std::endl;
	}

	return EXIT_SUCCESS;
}
