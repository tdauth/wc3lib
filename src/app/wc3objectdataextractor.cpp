/***************************************************************************
 *   Copyright (C) 2023 by Tamino Dauth                                    *
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
	bindtextdomain("wc3objectdataextractor", LOCALE_DIR);
	textdomain("wc3objectdataextractor");

	typedef std::vector<std::string> Strings;
	Strings fieldIds;
	Strings objectIds;
	int max = 100;
	Strings inputFiles;
	boost::filesystem::path outputFile;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of wc3objectdataextractor."))
	("help,h",_("Shows this text."))
	// options
	("verbose", _("Add more text output."))
	("overwrite", _("Overwrites existing files and directories when creating or extracting files."))
    ("f", boost::program_options::value<Strings>(&fieldIds), _("Field IDs. Extracts all field IDS if none is specified."))
    ("d", boost::program_options::value<Strings>(&objectIds), _("Object IDs. Extracts all object IDS if none is specified."))
	("m", boost::program_options::value<int>(&max)->default_value(max), _("Maximum number of levels."))
	("i", boost::program_options::value<Strings>(&inputFiles), _("Input object data files (.w3a, .w3u etc.)."))
	("o", boost::program_options::value<boost::filesystem::path>(&outputFile), _("Output JASS file."))
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
		boost::format(_("wc3objectdataextractor %1%.")) % version
		<< std::endl
		<< wc3lib::wc3libCopyright()
        << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help"))
	{
		std::cout << _("Usage: wc3objectdataextractor [options] [output file/directory] [input files]") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << wc3lib::wc3libReportBugs() << std::endl;

		return EXIT_SUCCESS;
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
	
	std::set<wc3lib::map::id> objectIdsSet;
	std::set<wc3lib::map::id> fieldIdsSet;
	
	for (const std::string &objectId : objectIds) {
		objectIdsSet.insert(wc3lib::map::stringToId(objectId));
	}
	
	for (const std::string &fieldId : fieldIds) {
		fieldIdsSet.insert(wc3lib::map::stringToId(fieldId));
	}
	
    long counter = 0;
	
	boost::filesystem::ofstream out(outputFile, std::ios::out);
	
	if (!out)
	{
		std::cerr << _("Error on opening output file.") << std::endl;

		return EXIT_FAILURE;
	}
	
	out << "globals" << std::endl;
	
	for (const std::string &fieldId : fieldIds) {
		out << "integer array " << fieldId << std::endl;
		out << "integer array " << fieldId << "Count" << std::endl;
	}
	
	out << "endglobals" << std::endl;
	
	out << "function InitObjectDataFields takes nothing returns nothing" << std::endl;

	for (FilePaths::reference path : inputFilePaths)
    {
        try
        {
            // TODO Determine by file extension or header!
            wc3lib::map::CustomObjects customObjects(wc3lib::map::CustomObjects::Type::Doodads);
            boost::filesystem::ifstream in(path, std::ios::in | std::ios::binary);
            customObjects.read(in);
            
            for (const wc3lib::map::CustomUnits::Unit &unit : customObjects.customTable()) {
				if (objectIdsSet.empty() || objectIdsSet.contains(unit.customId())) {
					for (const wc3lib::map::CustomUnits::Modification &modification : unit.modifications()) {
						if (fieldIdsSet.empty() || fieldIdsSet.contains(modification.valueId())) {
							std::cout << boost::format(_("Extracting field %1% from object ID %2%.")) % wc3lib::map::idToString(unit.customId()) % wc3lib::map::idToString(modification.valueId()) << std::endl;
							counter++;
							
							if (value.isList()) {
								const wc3lib::map::List &v = value.toList();
								
								for (int i = 0; i < v.size(); i++) {
									out << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "' + "  << i << " * " << max << "] = " << v[i] << std::endl;
								}
								
								out << wc3lib::map::idToString(modification.valueId()) << "Count['" << wc3lib::map::idToString(unit.customId()) << "] = " << v.size() << std::endl;
							} else if (value.isInteger()) {
								out << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "'] = " << value.toInteger() << std::endl;
							} else if (value.isReal()) {
								out << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "'] = " << value.toReal() << std::endl;
							} else if (value.isString()) {
								out << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "'] = " << value.toString() << std::endl;
							} else if (value.isBoolean()) {
								out << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "'] = " << value.toBoolean() << std::endl;
							} else if (value.isCharacter()) {
								out << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "'] = " << value.toCharacter() << std::endl;
							}
						}
					}
				}
            }
        }
        catch (const wc3lib::Exception &exception)
        {
            std::cerr << boost::format(_("Error occured when converting file %1%: \"%2%\".\nSkipping file.")) % path % exception.what() << std::endl;
        }
    }
    
    out << "endfunction" << std::endl;
    
    std::cout << boost::format(_("Extracted %1% object data fields.")) % counter << std::endl;

	return EXIT_SUCCESS;
}
