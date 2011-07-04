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
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <list>

#include <getopt.h>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "../lang/parser.hpp"
#include "../lang/compiler.hpp"
#include "../internationalisation.hpp"

using namespace wc3lib::lang;

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("jassc", LOCALE_DIR);
	textdomain("jassc");

	static struct option options[] =
	{
		{"version",                 no_argument,             0, 'V'},
		{"help",                    no_argument,             0, 'h'},
		{"recursive",               required_argument,       0, 'R'},
		{"include",                 required_argument,       0, 'I'},
		{"showerrors",              no_argument,             0, 's'},
		{"verify",                  no_argument,             0, 'v'},
#ifdef HTML
		{"html",                    required_argument,       0, 'H'},
#endif
#ifdef SQLITE
		{"sqlite",                  required_argument,       0, 'L'},
#endif
		{"map",                     required_argument,       0, 'M'},
		{"mapscript",               required_argument,       0, 'T'},
		{"script",                  required_argument,       0, 'P'},
		{"optimize",                required_argument,       0, 'O'},
		{"language",                required_argument,       0, 'A'},
		{0, 0, 0, 0}
	};

	static const char *version = "0.1";
	boost::filesystem::path recursivePath;
	std::list<boost::filesystem::path> includeDirs;
	bool showErrors = false;
	bool verify = false;
#ifdef HTML
	boost::filesystem::path htmlPath;
#endif
#ifdef SQLITE
	boost::filesystem::path sqlitePath;
#endif
	boost::filesystem::path mapPath;
	boost::filesystem::path mapScriptPath;
	boost::filesystem::path scriptPath;
	std::string optimize;
	std::string language;
	int optionShortcut;

	while (true)
	{
		int optionIndex = 0;
		optionShortcut = getopt_long(argc, argv, "VhR:I:svH:L:M:T:P:O:A:", options, &optionIndex);

		if (optionShortcut == -1)
			break;

		switch (optionShortcut)
		{
			case 'V':
			{
				std::cout << boost::format(_(
				"jassc %1%.\n"
				"Copyright © 2009 Tamino Dauth\n"
				"License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n"
				"This is free software: you are free to change and redistribute it.\n"
				"There is NO WARRANTY, to the extent permitted by law."
				)) % version << std::endl;

				return EXIT_SUCCESS;
			}

			case 'h':
			{
				std::cout <<
				boost::format(_("jassc %1%.\n\n")) % version <<
				_("Usage: jassc [Options] [Code files]\n\n") <<
				_("Options:\n") <<
				_("\t-V --version                Shows the current version of vjassdoc.\n") <<
				_("\t-h --help                   Shows this text.\n") <<
				_("\t-R --recursive <arg>        --\n") <<
				_("\t-I --include <arg>          --\n") <<
				_("\t-s --showerrors             --\n") <<
				_("\t-v --verify                 --\n") <<
#ifdef HTML
				_("\t-H --html <arg>             --\n") <<
#endif
#ifdef SQLITE
				_("\t-L --sqlite <arg>           --\n") <<
#endif
				_("\t-M --map <arg>              --\n") <<
				_("\t-T --mapscript <arg>        --\n") <<
				_("\t-P --script <arg>           --\n") <<
				_("\t-O --optimize <arg>         --\n") <<
				_("\t-A --language <arg>         --\n") <<
				std::endl <<
				_("Several arguments has to be separated by using the : character.\n") <<
				_("\nReport bugs to tamino@cdauth.de or on http://sourceforge.net/projects/vjasssdk/") <<
				std::endl;

				return EXIT_SUCCESS;
			}

			/// @todo Parse options
	}

	std::list<boost::filesystem::path> filePaths;

	if (optind < argc)
	{
		for ( ; optind < argc; ++optind)
		{
			bool found = false;

			BOOST_FPREACH(const boost::filesystem::path &path, filePaths)
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

	class Parser parser();
	parser.setCurrentLanguage( ); /// @todo Set current language by option or jass.
	std::size_t lines = parser.parse(filePaths);
	std::cout << boost::format(_("Parsed %1% files with %2% lines at all.")) % paths.size() % lines << std::endl;

	if (verify)
		parser.prepareObjects();

	if (showErrors)
		parser.showSyntaxErrors(std::cout);

	if (
#ifdef HTML
		!htmlPath.empty() ||
#endif
#ifdef SQLITE
		!sqlitePath.empty() ||
#endif
		!mapPath.empty() || !mapScriptPath.empty() || !scriptPath.empty()
	)
	{
		if (!verify)
			parser.prepareObjects();

		class Compiler compiler;

		if (
	}
	std::string optimize;
	std::string language;

	return EXIT_SUCCESS;
}
