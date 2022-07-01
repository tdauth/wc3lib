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
#include <vector>
#include <memory>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "../i18n.hpp"
#include "../map/environment.hpp"

using namespace wc3lib;
using namespace wc3lib::map;

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("wc3tilesetter", LOCALE_DIR);
	textdomain("wc3tilesetter");

	static const char* version = "0.1";

	typedef std::vector<boost::filesystem::path> Paths;
	typedef std::vector<std::string> Strings;
	// TODO throws exception when using escaped white spaces! - string work around
	Strings environmentStrings;
	Paths environments;

	const boost::program_options::command_line_style::style_t pstyle = boost::program_options::command_line_style::style_t(
	boost::program_options::command_line_style::unix_style
	);

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of tilesetter."))
	("help,h",_("Shows this text."))
	// options

	// operations
	("reassign,r", _("Reads ground and cliff types from the first environment file and assigns them to all following."))

	// input
	("files,f", boost::program_options::value<Strings>(&environmentStrings), _("Expected environment files."))
	;

	boost::program_options::positional_options_description p;
	p.add("files", -1);

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
	environments.resize(environmentStrings.size());
	environments.assign(environmentStrings.begin(), environmentStrings.end());

	if (vm.count("version"))
	{
		std::cout << boost::format(_("wc3tilesetter %1%")) % version
		<< std::endl
		<< wc3libCopyright()
		<< std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		std::cout << wc3lib::wc3libReportBugs() << std::endl;

		return EXIT_SUCCESS;
	}

	if (environments.empty())
	{
		std::cerr << _("Missing environments arguments.") << std::endl;

		return EXIT_FAILURE;
	}

	if (vm.count("reassign"))
	{
		boost::ptr_vector<map::Environment> envs;
		envs.reserve(environments.size());

		BOOST_FOREACH(Paths::const_reference path, environments)
		{
			if (!boost::filesystem::is_regular_file(path))
			{
				std::cerr << boost::format(_("File %1% does not seem to be a regular file.")) % path << std::endl;

				return EXIT_FAILURE;
			}

			boost::filesystem::ifstream ifstream;
			ifstream.exceptions(boost::filesystem::ifstream::badbit | boost::filesystem::ifstream::failbit | boost::filesystem::ifstream::eofbit);

			try
			{
				ifstream.open(path, std::ios::in | std::ios::binary);
				std::unique_ptr<map::Environment> env(new map::Environment());
				env->read(ifstream);
				envs.push_back(std::move(env));
			}
			catch (std::exception &exception)
			{
				std::cerr << boost::format(_("Error occured while opening file %1%:\n\"%2%\"")) % path % exception.what() << std::endl;

				return EXIT_FAILURE;
			}
		}

		if (envs.size() < 2)
		{
			std::cerr << _("We need at least 2 environments.") << std::endl;

			return EXIT_FAILURE;
		}

		for (std::size_t i = 1; i < envs.size(); ++i)
		{
			envs[i].cliffTilesetsIds().assign(envs[0].cliffTilesetsIds().begin(), envs[0].cliffTilesetsIds().end());
			envs[i].groundTilesetsIds().assign(envs[0].groundTilesetsIds().begin(), envs[0].groundTilesetsIds().end());

			boost::filesystem::ofstream ofstream;
			ofstream.exceptions(boost::filesystem::ofstream::badbit | boost::filesystem::ofstream::failbit | boost::filesystem::ofstream::eofbit);

			try
			{
				ofstream.open(environments[i], std::ios::out | std::ios::binary);
				envs[i].write(ofstream);
			}
			catch (std::exception &exception)
			{
				std::cerr << boost::format(_("Error occured while saving file %1%:\n\"%2%\"")) % environments[i] % exception.what() << std::endl;

				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}
