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
#include <optional>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/program_options.hpp>

#include "../../config.h"

#include "../core.hpp"
#include "../map.hpp"

typedef std::list<boost::filesystem::path> FilePaths;

const char *version = "0.1";

inline void updateUnit(wc3lib::map::CustomObjects::Unit &unit, wc3lib::map::id fieldIdX, wc3lib::map::id fieldIdY, wc3lib::map::id fieldIdHotkey, wc3lib::map::id fieldIdUnhotkey, wc3lib::map::id fieldIdHeroAbility, bool onlyHeroAbilities, std::string &hotkeys, bool verbose, std::map<wc3lib::map::id, bool> &metaIsHero, long &updatedModificationsCounter) {
    bool isHeroAbility = metaIsHero[unit.originalId()];
    std::optional<wc3lib::int32> x;
    std::optional<std::size_t> setIndexX;
    std::optional<std::size_t> indexX;
    std::optional<wc3lib::int32> y;
    std::optional<std::size_t> setIndexY;
    std::optional<std::size_t> indexY;
    std::optional<wc3lib::byte> hotkey;
    std::optional<std::size_t> setIndexHotkey;
    std::optional<std::size_t> indexHotkey;
    std::optional<wc3lib::byte> unhotkey;
    std::optional<std::size_t> setIndexUnhotkey;
    std::optional<std::size_t> indexUnhotkey;

    for (std::size_t i = 0; i < unit.sets().size(); i++) {
        wc3lib::map::CustomObjects::Set *set = dynamic_cast<wc3lib::map::CustomObjects::Set*>(&unit.sets()[i]);

        for (std::size_t j = 0; j < set->modifications().size(); j++) {
            wc3lib::map::CustomObjects::Modification *modification = dynamic_cast<wc3lib::map::CustomObjects::Modification*>(&set->modifications()[j]);
            wc3lib::map::id modificationId = modification->valueId();

            if (modificationId == fieldIdX && modification->value().type() == wc3lib::map::Value::Type::Integer) {
                x = modification->value().toInteger();
                indexX = j;
                setIndexX = i;

                if (verbose) {
                    //std::cout << boost::format(_("Got button position X modification %1% of object %2% with value %3%.")) % wc3lib::map::idToString(modificationId) % wc3lib::map::idToString(unit.customId()) % x.value() << std::endl;
                }
            }

            if (modificationId == fieldIdY && modification->value().type() == wc3lib::map::Value::Type::Integer) {
                y = modification->value().toInteger();
                indexY = j;
                setIndexY = i;
            }

            if (modificationId == fieldIdHotkey && modification->value().type() == wc3lib::map::Value::Type::Character) {
                hotkey = modification->value().toCharacter();
                indexHotkey = j;
                setIndexHotkey = i;
            }

            if (modificationId == fieldIdUnhotkey && modification->value().type() == wc3lib::map::Value::Type::Character) {
                unhotkey = modification->value().toCharacter();
                indexUnhotkey = j;
                setIndexUnhotkey = i;
            }

            if (modificationId == fieldIdHeroAbility && modification->value().type() == wc3lib::map::Value::Type::Boolean) {
                isHeroAbility = modification->value().toBoolean();

                if (verbose) {
                    //std::cout << boost::format(_("Got is hero ability modification %1% of object %2% with value %3%.")) % wc3lib::map::idToString(modificationId) % wc3lib::map::idToString(unit.customId()) % isHeroAbility << std::endl;
                }
            }
        }
    }

    if ((!onlyHeroAbilities || isHeroAbility) && x.has_value() && indexX.has_value() && x.value() < hotkeys.size() && x.value() >= 0) {
        if (verbose) {
            std::cout << boost::format(_("Checking hotkey for object %1%.")) % wc3lib::map::idToString(unit.customId()) << std::endl;
        }

        if (hotkey.has_value() && indexHotkey.has_value() && setIndexHotkey.has_value()) {
            unit.sets()[setIndexHotkey.value()].modifications()[indexHotkey.value()].setValue(wc3lib::byte(hotkeys[x.value()]));
            updatedModificationsCounter++;

            if (verbose) {
                std::cout << boost::format(_("Updating hotkey for object %1%.")) % wc3lib::map::idToString(unit.customId()) << std::endl;
            }
        }

        if (unhotkey.has_value() && indexUnhotkey.has_value()  && setIndexUnhotkey.has_value()) {
            unit.sets()[setIndexUnhotkey.value()].modifications()[indexUnhotkey.value()].setValue(wc3lib::byte(hotkeys[x.value()]));
            updatedModificationsCounter++;

            if (verbose) {
                std::cout << boost::format(_("Updating unhotkey for object %1%.")) % wc3lib::map::idToString(unit.customId()) << std::endl;
            }
        }
    }
}

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("wc3abilityhotkeyfixer", LOCALE_DIR);
	textdomain("wc3abilityhotkeyfixer");

	std::string hotkeys = "QWER";

    wc3lib::map::id fieldIdX = wc3lib::map::stringToId("abpx");
    wc3lib::map::id fieldIdY = wc3lib::map::stringToId("abpy");
    wc3lib::map::id fieldIdHotkey = wc3lib::map::stringToId("ahky");
    wc3lib::map::id fieldIdUnhotkey = wc3lib::map::stringToId("auhk");
    wc3lib::map::id fieldIdHeroAbility = wc3lib::map::stringToId("aher");
    std::string fieldIdXInput = "abpx";
    std::string fieldIdYInput = "abpy";
    std::string fieldIdHotkeyInput = "ahky";
    std::string fieldIdUnhotkeyInput = "auhk";
    std::string fieldIdHeroAbilityInput = "aher";
    bool onlyHeroAbilities = true;
    bool verbose = false;
    bool overwrite = false;
	typedef std::vector<std::string> Strings;
	Strings inputFiles;
    boost::filesystem::path abilityDataFile;
	boost::filesystem::path outputFile;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of wc3abilityhotkeyfixer."))
	("help,h",_("Shows this text."))
	// options
	("verbose", _("Add more text output."))
	("overwrite", _("Overwrites existing files and directories when creating or extracting files."))
    ("hotkeys", boost::program_options::value<std::string>(&hotkeys)->default_value(hotkeys), _("The hotkeys."))
    ("idx", boost::program_options::value<std::string>(&fieldIdXInput)->default_value(fieldIdXInput), _("Field ID of the icon X position."))
    ("idy", boost::program_options::value<std::string>(&fieldIdYInput)->default_value(fieldIdYInput), _("Field ID of the icon Y position."))
    ("idhotkey", boost::program_options::value<std::string>(&fieldIdHotkeyInput)->default_value(fieldIdHotkeyInput), _("Field ID of the hotkey."))
    ("idunhotkey", boost::program_options::value<std::string>(&fieldIdUnhotkeyInput)->default_value(fieldIdUnhotkeyInput), _("Field ID of the unhotkey."))
    ("idhero", boost::program_options::value<std::string>(&fieldIdHeroAbilityInput)->default_value(fieldIdHeroAbilityInput), _("Field ID of the hero ability."))
    ("onlyheroabilities", boost::program_options::value<bool>(&onlyHeroAbilities)->default_value(onlyHeroAbilities), _("Modifies only hero abilities."))
    ("abilitydata", boost::program_options::value<boost::filesystem::path>(&abilityDataFile)->required(), _("SLK file AbilityData.slk with all the standard fields."))
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
		boost::format(_("wc3abilityhotkeyfixer %1%.")) % version
		<< std::endl
		<< wc3lib::wc3libCopyright()
		<< std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help"))
	{
		std::cout << _("Usage: wc3abilityhotkeyfixer [options] [output file/directory] [input files]") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << wc3lib::wc3libReportBugs() << std::endl;

		return EXIT_SUCCESS;
	}

	verbose = vm.count("verbose");
    overwrite = vm.count("overwrite");

    fieldIdX = wc3lib::map::stringToId(fieldIdXInput);
    fieldIdY = wc3lib::map::stringToId(fieldIdYInput);
    fieldIdHotkey = wc3lib::map::stringToId(fieldIdHotkeyInput);
    fieldIdUnhotkey = wc3lib::map::stringToId(fieldIdUnhotkeyInput);
    fieldIdHeroAbility = wc3lib::map::stringToId(fieldIdHeroAbilityInput);

	FilePaths inputFilePaths;

	for (Strings::const_reference ref : inputFiles)
	{
        inputFilePaths.push_back(ref);
	}

	if (inputFilePaths.empty())
	{
		std::cerr << _("No valid input files.") << std::endl;

		return EXIT_FAILURE;
	}

	if (verbose) {
        std::cout << boost::format(_("Reading file %1%.")) % abilityDataFile << std::endl;
    }

	wc3lib::map::Slk abilityData;
    boost::filesystem::ifstream inAbilityData(abilityDataFile, std::ios::in | std::ios::binary);
    abilityData.read(inAbilityData);

    long updatedModificationsCounter = 0;

	for (FilePaths::reference path : inputFilePaths)
    {
        try
        {
            boost::filesystem::path realOutputFile = outputFile;

            if (boost::filesystem::is_directory(outputFile))
            {
                realOutputFile /= path.stem().string() + "." + path.extension().string();
            }

            if (verbose) {
                std::cout << boost::format(_("Reading file %1%.")) % path << std::endl;
            }

            std::string extension = path.extension().string();
            wc3lib::map::CustomObjects customObjects(wc3lib::map::CustomObjects::typeByExtension(extension));
            boost::filesystem::ifstream in(path, std::ios::in | std::ios::binary);
            customObjects.read(in);

            if (verbose) {
                std::cout << boost::format(_("Detected %1% standard and %2% custom objects.")) % customObjects.originalTable().size() % customObjects.customTable().size() << std::endl;
            }

            std::map<wc3lib::map::id, bool> metaIsHero;

            for (wc3lib::map::Slk::Table::size_type row = 1; row < abilityData.rows(); row++) {
                std::string cellContent = abilityData.cell(row, 5);
                bool value = false;

                if (cellContent.size() > 0 && cellContent[0] == '\"') {
                    cellContent = cellContent.substr(1, cellContent.size() - 2);
                    value = std::stoi(cellContent);
                } else if (!cellContent.empty()) {
                    value = std::stoi(cellContent);
                }

                wc3lib::string abilityIdContent = wc3lib::map::Slk::fromSlkString(abilityData.cell(row, 0));
                std::cerr << "Ability ID " << abilityIdContent << std::endl;
                wc3lib::map::id abilityId = wc3lib::map::stringToId(abilityIdContent);
                metaIsHero[abilityId] = value;

                if (verbose && value) {
                    std::cout << boost::format(_("Standard ability %1% is a hero ability.")) %abilityIdContent << std::endl;
                }
            }

            for (wc3lib::map::CustomObjects::Unit &unit : customObjects.originalTable()) {
                updateUnit(unit, fieldIdX, fieldIdY, fieldIdHotkey, fieldIdUnhotkey, fieldIdHeroAbility, onlyHeroAbilities, hotkeys, verbose, metaIsHero, updatedModificationsCounter) ;
            }

            for (wc3lib::map::CustomObjects::Unit &unit : customObjects.customTable()) {
                updateUnit(unit, fieldIdX, fieldIdY, fieldIdHotkey, fieldIdUnhotkey, fieldIdHeroAbility, onlyHeroAbilities, hotkeys, verbose, metaIsHero, updatedModificationsCounter) ;
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

    std::cout << boost::format(_("Updated %1% modifications.")) % updatedModificationsCounter << std::endl;

	return EXIT_SUCCESS;
}
