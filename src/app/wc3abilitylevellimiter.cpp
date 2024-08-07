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
#include <fstream>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/program_options.hpp>

#include "../../config.h"

#include "../core.hpp"
#include "../map.hpp"

typedef std::list<boost::filesystem::path> FilePaths;

const char *version = "0.1";

inline void updateUnit(wc3lib::map::CustomObjects::Unit &unit, wc3lib::int32 maxLevel, wc3lib::map::id levelFieldId, bool verbose, long &removedModificationsCounter, long &updatedModificationsCounter) {
    for (std::size_t i = 0; i < unit.sets().size(); i++) {
        wc3lib::map::CustomObjects::Set *set = dynamic_cast<wc3lib::map::CustomObjects::Set*>(&unit.sets()[i]);

        for (std::size_t j = 0; j < set->modifications().size(); ) {
            //  wc3lib::map::CustomObjects::Modification &modification : unit.modifications()) {
            wc3lib::map::CustomObjects::Modification *modification = dynamic_cast<wc3lib::map::CustomObjects::Modification*>(&set->modifications()[j]);
            wc3lib::int32 sourceMaxLevel = modification->level();

            if (sourceMaxLevel > maxLevel) {
                if (verbose) {
                    std::cout << boost::format(_("Removing modification %1% of custom object %2% with level %3% to keep maximum level %4% of modifications.")) % wc3lib::map::idToString(modification->valueId()) % wc3lib::map::idToString(unit.customId()) % sourceMaxLevel % maxLevel << std::endl;
                }

                removedModificationsCounter++;

                set->modifications().erase(std::next(set->modifications().begin(), j));
            } else {
                j++;

                if (modification->valueId() == levelFieldId && modification->value().type() == wc3lib::map::Value::Type::Integer) {
                    sourceMaxLevel = modification->value().toInteger();

                    if (sourceMaxLevel > maxLevel) {
                        modification->setValue(wc3lib::map::Value(maxLevel));

                        if (verbose) {
                            std::cout << boost::format(_("Updating modification %1% of custom object %2% with level %3% to keep maximum level %4% of modifications.")) % wc3lib::map::idToString(modification->valueId()) % wc3lib::map::idToString(unit.customId()) % sourceMaxLevel % maxLevel << std::endl;
                        }

                        updatedModificationsCounter++;
                    }
                }
            }
        }
    }
}

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
	("version,V", _("Shows current version of wc3abilitylevellimiter."))
	("help,h",_("Shows this text."))
	// options
	("verbose", _("Add more text output."))
	("overwrite", _("Overwrites existing files and directories when creating or extracting files."))
    ("maxlevel,l", boost::program_options::value<int>(&maxLevel)->default_value(100), _("Maximum ability level."))
    ("id", boost::program_options::value<std::string>(&levelFieldIdInput)->default_value("alev"), _("Level field ID."))
	("i", boost::program_options::value<Strings>(&inputFiles), _("Input files."))
	("o", boost::program_options::value<boost::filesystem::path>(&outputFile), _("Output file or directory (for multiple files)."))
	;

	boost::program_options::positional_options_description p;
	p.add("o", 1); // first input is output path
	p.add("i", -1);


	boost::program_options::variables_map vm;

	try {
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	}
	catch (std::exception &exception) {
		std::cerr << boost::format(_("Error while parsing program options: \"%1%\"")) % exception.what() << std::endl;

		return EXIT_FAILURE;
	}

	boost::program_options::notify(vm);

	if (vm.count("version")) {
		std::cout <<
		boost::format(_("wc3abilitylevellimiter %1%.")) % version
		<< std::endl
		<< wc3lib::wc3libCopyright()
        << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help")) {
		std::cout << _("Usage: wc3abilitylevellimiter [options] [output file/directory] [input files]") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << wc3lib::wc3libReportBugs() << std::endl;

		return EXIT_SUCCESS;
	}

	verbose = vm.count("verbose");
    overwrite = vm.count("overwrite");

	levelFieldId = wc3lib::map::stringToId(levelFieldIdInput);

	FilePaths inputFilePaths;

	for (Strings::const_reference ref : inputFiles) {
        inputFilePaths.push_back(ref);
	}

	if (inputFilePaths.empty()) {
		std::cerr << _("No valid input files.") << std::endl;

		return EXIT_FAILURE;
	}

    long removedModificationsCounter = 0;
    long updatedModificationsCounter = 0;

	for (FilePaths::reference path : inputFilePaths) {
        try {
            boost::filesystem::path realOutputFile = outputFile;

            if (boost::filesystem::is_directory(outputFile)) {
                realOutputFile /= path.stem().string() + "." + path.extension().string();
            }

            wc3lib::map::CustomObjects customObjects(wc3lib::map::CustomObjects::Type::Abilities);
            boost::filesystem::ifstream in(path, std::ios::in | std::ios::binary);
            customObjects.read(in);

            std::cout << boost::format(_("Read ability data with %1% modified standard abilities and %2% custom abilities.")) % customObjects.originalTable().size() % customObjects.customTable().size() << std::endl;

            for (wc3lib::map::CustomObjects::Unit &unit : customObjects.originalTable()) {
                updateUnit(unit, maxLevel,levelFieldId, verbose, removedModificationsCounter, updatedModificationsCounter);
            }

            for (wc3lib::map::CustomObjects::Unit &unit : customObjects.customTable()) {
                updateUnit(unit, maxLevel,levelFieldId, verbose, removedModificationsCounter, updatedModificationsCounter);
            }

            if (!overwrite && boost::filesystem::exists(realOutputFile)) {
                std::cerr << boost::format(_("Output file %1% does already exist. Use --overwrite to overwrite it.")) % realOutputFile << std::endl;

                return EXIT_FAILURE;
            } else {
                boost::filesystem::ofstream out(realOutputFile, std::ios::out | std::ios::binary);
                customObjects.write(out);
            }
        }
        catch (const wc3lib::Exception &exception) {
            std::cerr << boost::format(_("Error occured when converting file %1%: \"%2%\".\nSkipping file.")) % path % exception.what() << std::endl;

            return EXIT_FAILURE;
        }
    }

    std::cout << boost::format(_("Removed %1% and updated %2% modifications.")) % removedModificationsCounter % updatedModificationsCounter << std::endl;

	return EXIT_SUCCESS;
}
