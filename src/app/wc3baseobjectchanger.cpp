/***************************************************************************
 *   Copyright (C) 2021 by Tamino Dauth                                    *
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

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <list>
#include <fstream>

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
//#include <boost/thread.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/program_options.hpp>

#include "../../config.h"

#include "../core.hpp"
#include "../map.hpp"
  
typedef std::list<boost::filesystem::path> FilePaths;

const char *version = "0.1";

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("wc3baseobjectchanger", LOCALE_DIR);
	textdomain("wc3baseobjectchanger");

	std::string sourceBaseObjectId;
	std::string targetBaseObjectId;
	typedef std::vector<std::string> Strings;
	Strings inputFiles;
	boost::filesystem::path outputFile;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of mpq."))
	("help,h",_("Shows this text."))
	// options
	("verbose", _("Add more text output."))
	("overwrite", _("Overwrites existing files and directories when creating or extracting files."))
    ("s", boost::program_options::value<std::string>(&sourceBaseObjectId), _("Source base ID."))
    ("t", boost::program_options::value<std::string>(&targetBaseObjectId), _("Target base ID."))
	("i", boost::program_options::value<Strings>(&inputFiles), _("Input files."))
	("o", boost::program_options::value<boost::filesystem::path>(&outputFile), _("Output file or directory (for multiple files)."))
	;

	boost::program_options::positional_options_description p;
	p.add("o", 1); // first input is output path
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
		boost::format(_("wc3baseobjectchanger %1%.")) % version
		<< std::endl <<
		_(
		"Copyright © 2021 Tamino Dauth\n"
		"License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law."
		) << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help"))
	{
		std::cout << _("Usage: wc3converter [options] [output file/directory] [input files]") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << _("\nReport bugs to tamino@cdauth.eu or on https://wc3lib.org") << std::endl;

		return EXIT_SUCCESS;
	}

	if (sourceBaseObjectId.empty())
	{
		std::cerr <<_("Missing source base object ID!") << std::endl;

		return EXIT_FAILURE;
	}

	if (targetBaseObjectId.empty())
	{
		std::cerr <<_("Missing target base object ID!") << std::endl;

		return EXIT_FAILURE;
	}

	FilePaths inputFilePaths;

	BOOST_FOREACH(Strings::const_reference ref, inputFiles)
	{
        inputFilePaths.push_back(ref);
	}

	if (inputFilePaths.empty())
	{
		std::cerr << _("No valid input files.") << std::endl;

		return EXIT_FAILURE;
	}
	
	wc3lib::map::id sourceBaseObjectIdWc3 = wc3lib::map::stringToId(sourceBaseObjectId);
    wc3lib::map::id targetBaseObjectIdWc3 = wc3lib::map::stringToId(targetBaseObjectId);
    long counter = 0;

	BOOST_FOREACH(FilePaths::reference path, inputFilePaths)
    {
        try
        {
            boost::filesystem::path realOutputFile = outputFile;

            if (boost::filesystem::is_directory(outputFile))
            {
                realOutputFile /= path.stem().string() + "." + path.extension().string();
            }
            
            // TODO Determine by file extension or header!
            wc3lib::map::CustomObjects customObjects(wc3lib::map::CustomObjects::Type::Doodads);
            boost::filesystem::ifstream in(path, std::ios::in | std::ios::binary);
            customObjects.read(in);
            
            for (wc3lib::map::CustomObjects::Unit &unit : customObjects.customTable()) {
                if (unit.originalId() == sourceBaseObjectIdWc3) {
                    unit.setOriginalId(targetBaseObjectIdWc3);
                    std::cout << boost::format(_("Changing base object ID of custom object %1% from %2% to %3%.")) % wc3lib::map::idToString(unit.customId()) % wc3lib::map::idToString(sourceBaseObjectIdWc3) % wc3lib::map::idToString(targetBaseObjectIdWc3) << std::endl;
                    counter++;
                }
            }
            
            boost::filesystem::ofstream out(realOutputFile, std::ios::out | std::ios::binary);
            customObjects.write(out);
        }
        catch (const wc3lib::Exception &exception)
        {
            std::cerr << boost::format(_("Error occured when converting file %1%: \"%2%\".\nSkipping file.")) % path % exception.what() << std::endl;
        }
    }
    
    std::cout << boost::format(_("Converted %1% object IDs.")) % counter << std::endl;

	return EXIT_SUCCESS;
}
