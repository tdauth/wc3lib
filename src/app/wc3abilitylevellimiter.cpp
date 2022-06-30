/***************************************************************************
 *   Copyright (C) 2022 by Tamino Dauth                                    *
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
	bindtextdomain("wc3abilitylevellimiter", LOCALE_DIR);
	textdomain("wc3abilitylevellimiter");

	wc3lib::int32 maxLevel = 100;
    wc3lib::map::id levelFieldId = wc3lib::map::stringToId("alev");
    std::string levelFieldIdInput = "alev";
    bool verbose = false;
    bool overwrite = false;
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
    ("l", boost::program_options::value<int>(&maxLevel)->default_value(100), _("Maximum ability level."))
    ("id", boost::program_options::value<std::string>(&levelFieldIdInput)->default_value("alev"), _("Level field ID."))
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
		boost::format(_("wc3abilitylevellimiter %1%.")) % version
		<< std::endl <<
		_(
		"Copyright © 2022 Tamino Dauth\n"
		"License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law."
		) << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help"))
	{
		std::cout << _("Usage: wc3abilitylevellimiter [options] [output file/directory] [input files]") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << _("\nReport bugs to tamino@cdauth.eu or on https://wc3lib.org") << std::endl;

		return EXIT_SUCCESS;
	}
	
	verbose = vm.count("verbose");
    overwrite = vm.count("overwrite");
	
	levelFieldId = wc3lib::map::stringToId(levelFieldIdInput);

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
	
    long removedModificationsCounter = 0;
    long updateddModificationsCounter = 0;

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
                for (int i = 0; i < unit.modifications().size(); ) {
                    //  wc3lib::map::CustomObjects::Modification &modification : unit.modifications()) {
                    wc3lib::map::CustomObjects::Modification *modification = dynamic_cast<wc3lib::map::CustomObjects::Modification*>(&unit.modifications()[i]);
                    wc3lib::int32 sourceMaxLevel = modification->level();
                    
                    if (sourceMaxLevel > maxLevel) {
                        if (verbose) {
                            std::cout << boost::format(_("Removing modification %1% of custom object %2% with level %3% to keep maximum level %4% of modifications.")) % wc3lib::map::idToString(modification->valueId()) % wc3lib::map::idToString(unit.customId()) % sourceMaxLevel % maxLevel << std::endl;
                        }
                        
                        removedModificationsCounter++;
                        
                        unit.modifications().erase(std::next(unit.modifications().begin(), i));
                    } else {
                        i++;
                        
                        if (modification->valueId() == levelFieldId && modification->value().type() == wc3lib::map::Value::Type::Integer) {
                            sourceMaxLevel = modification->value().toInteger();
                            
                            if (sourceMaxLevel > maxLevel) {
                                modification->setValue(wc3lib::map::Value(maxLevel));
                                
                                if (verbose) {
                                    std::cout << boost::format(_("Updating modification %1% of custom object %2% with level %3% to keep maximum level %4% of modifications.")) % wc3lib::map::idToString(modification->valueId()) % wc3lib::map::idToString(unit.customId()) % sourceMaxLevel % maxLevel << std::endl;
                                }
                                
                                updateddModificationsCounter++;
                            }
                        }
                    }
                }
            }
            
            if (!overwrite && boost::filesystem::exists(realOutputFile)) {
                std::cerr << boost::format(_("Output file %1% does already exist. Use --overwrite to overwrite it.")) % realOutputFile << std::endl;
            
                return EXIT_FAILURE;
            } else {
                boost::filesystem::ofstream out(realOutputFile, std::ios::out | std::ios::binary);
                customObjects.write(out);
            }
        }
        catch (const wc3lib::Exception &exception)
        {
            std::cerr << boost::format(_("Error occured when converting file %1%: \"%2%\".\nSkipping file.")) % path % exception.what() << std::endl;
            
            return EXIT_FAILURE;
        }
    }
    
    std::cout << boost::format(_("Removed %1% and updated %2% modifications.")) % removedModificationsCounter % updateddModificationsCounter << std::endl;

	return EXIT_SUCCESS;
}
