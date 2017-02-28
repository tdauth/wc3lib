/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "../map.hpp"

#include "../../editor.hpp"
#include "../../exception.hpp"


using namespace wc3lib;
using namespace wc3lib::map;
using namespace wc3lib::editor;

/**
 * Checks if all tooltip references are valid and prints errors for invalid ones.
 */
int main(int argc, char *argv[])
{
	string wc3Dir;
	string stringsFile;
	string input;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of wc3objectvalidate."))
	("help,h",_("Shows this text."))
	("w", boost::program_options::value<string>(&wc3Dir), _("Warcraft III directory."))
	("s", boost::program_options::value<string>(&stringsFile), _("war3map.wts strings file."))
	("i", boost::program_options::value<string>(&input), _("Input object collection data file."))
	;

	boost::program_options::positional_options_description p;
	p.add("w", 1);
	p.add("s", 1);
	p.add("i", 1);

	boost::program_options::variables_map vm;

	try
	{
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	}
	catch (const std::exception &exception)
	{
		std::cerr << boost::format(_("Error while parsing program options: \"%1%\"")) % exception.what() << std::endl;

		std::cerr << boost::format(_("Usage: %1% [options] <Warcraft III directory> <map string file> <input object collection data file>")) % argv[0] << std::endl;

		return EXIT_FAILURE;
	}

	boost::program_options::notify(vm);

	if (vm.count("version"))
	{
		static const char *version = "0.1";

		std::cout << boost::format(_(
		"wc3objectvalidate %1%.\n"
		"Copyright © 2010 Tamino Dauth\n"
		"License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law."
		)) % version << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help"))
	{
		std::cout << _("Usage: wc3objectvalidate [options] <Warcraft III directory> <map string file> <input object collection data file>") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << _("\nReport bugs to tamino@cdauth.eu or on https://wc3lib.org") << std::endl;

		return EXIT_SUCCESS;
	}

	try
	{
		if (!boost::filesystem::is_directory(wc3Dir))
		{
			throw Exception(boost::format(_("%1% is not a directory.")) % wc3Dir);
		}

		if (!boost::filesystem::exists(stringsFile))
		{
			throw Exception(boost::format(_("Strings file %1% does not exist.")) % stringsFile);
		}

		if (!boost::filesystem::exists(input))
		{
			throw Exception(boost::format(_("Input file %1% does not exist.")) % input);
		}

		std::ifstream in(input);
		CustomObjectsCollection customObjectsCollection;
		customObjectsCollection.read(in);

		MpqPriorityList source;

		if (!source.addDefaultSources(QUrl::fromLocalFile(wc3Dir.c_str())))
		{
			throw Exception(boost::format(_("Could not load the default MPQ archives of Warcraft III from the directory %1%.")) % wc3Dir);
		}

		const boost::filesystem::path stringsFilePath = boost::filesystem::canonical(stringsFile);
		const QUrl stringsUrl = "file://" + QString::fromStdString(stringsFilePath.string());
		QStringList errors;

		if (customObjectsCollection.hasUnits())
		{
			UnitData unitData(&source);
			unitData.load(nullptr);
			unitData.importCustomObjects(*customObjectsCollection.units());
			unitData.applyMapStrings(stringsUrl);
			errors << unitData.validateTooltipReferences();
		}

		for (const QString &error : errors)
		{
			std::cerr << error.toStdString() << std::endl;
		}
	}
	catch (const Exception &e)
	{
		std::cerr << e.what() << std::endl;

		return 1;
	}

	return 0;
}
