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

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include "../jass.hpp"
#include "../i18n.hpp"

namespace
{

const char *version = "0.1";

}

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("jassc", LOCALE_DIR);
	textdomain("jassc");

	typedef std::vector<std::string> Strings;
	Strings headers;
	Strings inputFiles;
	boost::filesystem::path outputFile;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of jassc."))
	("help,h", _("Shows this text."))
	// options
	("headers,j", boost::program_options::value<Strings>(&headers), _("Includes JASS headers"))
	// warning options
	("fsyntax-only", _("Check the code for syntax errors, but don't do anything beyond that."))

	("i", boost::program_options::value<Strings>(&inputFiles), _("Input files."))
	("o", boost::program_options::value<boost::filesystem::path>(&outputFile), _("Output file."))
	;

	boost::program_options::positional_options_description p;
	p.add("i", -1);

	boost::program_options::variables_map vm;

	try
	{
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	}
	catch (std::exception &exception)
	{
		std::cerr << boost::format(_("Error while parsing program options: \"%1%\"")) % exception.what() << std::endl;

		return EXIT_FAILURE;
	}

	boost::program_options::notify(vm);

	if (vm.count("version"))
	{
		std::cout <<
		boost::format(_("jassc %1%.")) % version
		<< std::endl
		<< wc3lib::wc3libCopyright()
        << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help"))
	{
		std::cout << _("Usage: jassc [options] [input files]") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << wc3lib::wc3libReportBugs() << std::endl;

		return EXIT_SUCCESS;
	}

	if (inputFiles.empty())
	{
		std::cerr << _("Missing input files.") << std::endl;

		return EXIT_FAILURE;
	}

	wc3lib::jass::Grammar grammar;
	wc3lib::jass::jass_ast ast;
	bool error = false;

	BOOST_FOREACH(Strings::const_reference ref, inputFiles)
	{
		boost::filesystem::path file(ref);

		if (boost::filesystem::is_regular_file(file))
		{
			wc3lib::ifstream ifstream(file);

			if (ifstream)
			{

				try
				{
					if (!grammar.parse(ifstream, ast, ref))
					{
						error = true;

						std::cerr << boost::format(_("Error while parsing file \"%1%\".")) % ref << std::endl;
					}
				}
				catch (wc3lib::Exception &e)
				{
					error = true;

					std::cerr << boost::format(_("Error while parsing file \"%1%\": %2%")) % ref % e.what() << std::endl;
				}
			}
		}
		else
		{
			std::cerr << boost::format(_("File \"%1%\" is no regular file.")) % ref << std::endl;
		}
	}

	/*
	 * If an error did occur when parsing the grammar we should not check the AST since
	 * it will be corrupted anyways.
	 * So do only check the AST for types etc. if it had been parsed properly.
	 */
	if (!error)
	{

		wc3lib::jass::Analyzer analyzer(grammar.typeSymbols(), grammar.globalSymbols(), grammar.functionSymbols());
		wc3lib::jass::Analyzer::Reports reports;

		analyzer.checkAst(ast, reports);

		if (!reports.empty())
		{
			BOOST_FOREACH(wc3lib::jass::Analyzer::Reports::const_reference ref, reports)
			{
				std::cerr << ref.output() << std::endl;
			}

			if (vm.count("fsyntax-only"))
			{
				return EXIT_FAILURE;
			}
		}
		else
		{
			std::cout << _("No errors occured.") << std::endl;

			if (vm.count("fsyntax-only"))
			{
				return EXIT_SUCCESS;
			}
		}
	}

	return EXIT_SUCCESS;
}
