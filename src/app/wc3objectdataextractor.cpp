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

namespace
{

inline std::vector<std::string> splitAndIgnoreEmpty(const std::string &v)
{
	std::vector<std::string> r;
	boost::split(r, v, boost::is_any_of(","), boost::token_compress_on);

	if (r.size() >= 1 && r[0] == "")
	{
		r.clear();
	}

	return r;
}

inline std::string GetCamelCase(const std::string &v)
{
	if (v.size() > 0)
	{
		return boost::to_upper_copy(v.substr(0, 1)) + v.substr(1);
	}

	return v;
}

}

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("wc3objectdataextractor", LOCALE_DIR);
	textdomain("wc3objectdataextractor");

	typedef std::vector<std::string> Strings;
	std::string fieldIds;
	std::string fieldIdTypes;
	std::string objectIds;
	int max = 20;
	Strings inputFiles;
	boost::filesystem::path outputFile;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of wc3objectdataextractor."))
	("help,h",_("Shows this text."))
	// options
	("verbose,v", _("Add more text output."))
	("force,F", _("Overwrites existing files and directories when creating or extracting files."))
    ("fieldids,f", boost::program_options::value<std::string>(&fieldIds)->default_value(""), _("Field IDs. Extracts all field IDS if none is specified. Comma-separated values: umki,ucam"))
	("fieldtypes,t", boost::program_options::value<std::string>(&fieldIdTypes)->default_value(""), _("Field types. The types of the fields in the same order as the field IDs. The default type is integer. Comma-separated values: integer,real,boolean,character,integerlist,stringlist"))
    ("objectids,d", boost::program_options::value<std::string>(&objectIds)->default_value(""), _("Object IDs. Extracts all object IDS if none is specified."))
	("max,m", boost::program_options::value<int>(&max)->default_value(max), _("Maximum number of levels."))
	("vjass,j", _("Generates a vJass library."))
	("private,p", _("Make everything private in the vJass library."))
	("getters,g", _("Generate getters."))
	("input,i", boost::program_options::value<Strings>(&inputFiles), _("Input object data files (.w3a, .w3u etc.)."))
	("output,o", boost::program_options::value<boost::filesystem::path>(&outputFile), _("Output JASS file."))
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

	if (boost::filesystem::exists(outputFile) && !vm.count("force"))
	{
		std::cerr << boost::format(_("Output file %1% does already exist. Use --force/-F if you want to replace it.")) % outputFile.string() << std::endl;

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

	std::set<wc3lib::map::id> objectIdsSet;
	std::set<wc3lib::map::id> fieldIdsSet;
	std::map<wc3lib::map::id, std::string> fieldTypes;

	std::vector<std::string> objectIdsVector = splitAndIgnoreEmpty(objectIds);

	for (const std::string &objectId : objectIdsVector)
	{
		if (objectId.size() > 0)
		{
			objectIdsSet.insert(wc3lib::map::stringToId(objectId));
		}
	}

	std::vector<std::string> fieldIdTypesVector = splitAndIgnoreEmpty(fieldIdTypes);
	std::vector<std::string> fieldIdsVector = splitAndIgnoreEmpty(fieldIds);;

	int i = 0;

	for (const std::string &fieldId : fieldIdsVector)
	{
		if (fieldId.size() > 0)
		{
			fieldIdsSet.insert(wc3lib::map::stringToId(fieldId));
			fieldTypes[wc3lib::map::stringToId(fieldId)] = fieldIdTypesVector.size() > i ? fieldIdTypesVector[i] : std::string("integer");

			++i;
		}
	}

    long counter = 0;

	boost::filesystem::ofstream out(outputFile, std::ios::out);

	if (!out)
	{
		std::cerr << _("Error on opening output file.") << std::endl;

		return EXIT_FAILURE;
	}

	if (vm.count("vjass"))
	{
		out << "library ObjectDataFields initializer InitObjectDataFields" << std::endl;
		out << std::endl;
	}

	out << "globals" << std::endl;

	out << "\t";

	if (vm.count("vjass"))
	{
		out << "public ";
	}

	out << "constant integer MAX_OBJECT_DATA_FIELD_ENTRIES = " << max << std::endl;

	for (const std::string &fieldId : fieldIdsVector)
	{
		if (fieldId.size() > 0)
		{
			std::string type = fieldTypes[wc3lib::map::stringToId(fieldId)];

			if (type == "integerlist")
			{
				type = "integer";
			}

			out << "\t";

			if (vm.count("vjass") && vm.count("private"))
			{
				out << "private ";
			}

			out << type << " array " << fieldId << std::endl;

			out << "\t";

			if (vm.count("vjass") && vm.count("private"))
			{
				out << "private ";
			}

			out << "integer array " << fieldId << "Count" << std::endl;
		}
	}

	out << "endglobals" << std::endl;

	out << std::endl;

	if (vm.count("getters"))
	{
		for (const std::string &fieldId : fieldIdsVector)
		{
			std::string type = fieldTypes[wc3lib::map::stringToId(fieldId)];

			if (type == "integerlist")
			{
				type = "integer";
			}

			out << "function Get" << GetCamelCase(fieldId) << " takes integer objectId, integer index returns " << type << std::endl;
			out << "\treturn " << fieldId << "[index * MAX_OBJECT_DATA_FIELD_ENTRIES + objectId]" << std::endl;
			out << "endfunction" << std::endl;

			out << std::endl;

			out << "function Get" << GetCamelCase(fieldId) << "Count takes integer objectId returns integer" << std::endl;
			out << "\treturn " << fieldId << "Count[objectId]" << std::endl;
			out << "endfunction" << std::endl;

			out << std::endl;
		}
	}


	if (vm.count("vjass") && vm.count("private"))
	{
		out << "private ";
	}

	out << "function InitObjectDataFields takes nothing returns nothing" << std::endl;

	for (FilePaths::reference path : inputFilePaths)
    {
        try
        {
			std::string extension = path.extension().string();
            wc3lib::map::CustomObjects customObjects(wc3lib::map::CustomObjects::typeByExtension(extension));
            boost::filesystem::ifstream in(path, std::ios::in | std::ios::binary);
            customObjects.read(in);

            for (const wc3lib::map::CustomUnits::Unit &unit : customObjects.customTable()) {
				if (objectIdsSet.empty() || objectIdsSet.contains(unit.customId())) {
					for (const wc3lib::map::CustomUnits::Set &set : unit.sets()) {
						for (const wc3lib::map::CustomUnits::Modification &modification : set.modifications()) {
							if (fieldIdsSet.empty() || fieldIdsSet.contains(modification.valueId())) {

								if (vm.count("verbose"))
								{
									std::cout << boost::format(_("Extracting field %1% from object ID %2%.")) % wc3lib::map::idToString(modification.valueId()) % wc3lib::map::idToString(unit.customId()) << std::endl;
								}

								counter++;

								const std::string type = fieldTypes[modification.valueId()];

								//std::cerr << "Field ID " << wc3lib::map::idToString(modification.valueId()) << std::endl;
								//std::cerr << "Type " << type << std::endl;

								if (type == "integerlist") {

									if (modification.value().isList())
									{
										const wc3lib::map::List &v = modification.value().toList();

										for (std::size_t i = 0; i < v.size(); i++) {
											out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "' + "  << i << " * MAX_OBJECT_DATA_FIELD_ENTRIES] = " << v[i] << std::endl;

											if (i >= max)
											{
												std::cerr << boost::format(_("Warning: Reached maximum %1% with field %2% from object ID %3%.")) % max % wc3lib::map::idToString(modification.valueId()) % wc3lib::map::idToString(unit.customId()) << std::endl;
											}
										}

										if (v.size() > 0)
										{
											out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "Count['" << wc3lib::map::idToString(unit.customId()) << "'] = " << v.size() << std::endl;
										}
									}
									else if (modification.value().isString())
									{
										wc3lib::string v = modification.value().toString();
										std::vector<std::string> valueVector = splitAndIgnoreEmpty(v);
										std::size_t i = 0;

										for (const std::string &ref : valueVector)
										{
											out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "' + "  << i << " * MAX_OBJECT_DATA_FIELD_ENTRIES] = '" << valueVector[i] << "'" << std::endl;

											++i;
										}

										if (valueVector.size() > 0)
										{
											out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "Count['" << wc3lib::map::idToString(unit.customId()) << "'] = " << valueVector.size() << std::endl;
										}
									}
									else
									{
										std::cerr << boost::format(_("Warning: Extracting field %1% from object ID %2% does not work with type %3%.")) % wc3lib::map::idToString(modification.valueId()) % wc3lib::map::idToString(unit.customId()) % type  << std::endl;
									}
								} else if (type == "stringlist") {
									if (modification.value().isList())
									{
										const wc3lib::map::List &v = modification.value().toList();

										for (std::size_t i = 0; i < v.size(); i++) {
											out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "' + "  << i << " * MAX_OBJECT_DATA_FIELD_ENTRIES] = \"" << v[i] << "\"" << std::endl;

											++i;
										}

										if (v.size() > 0)
										{
											out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "Count['" << wc3lib::map::idToString(unit.customId()) << "'] = " << v.size() << std::endl;
										}
									}
									else if (modification.value().isString())
									{
										wc3lib::string v = modification.value().toString();
										std::vector<std::string> valueVector = splitAndIgnoreEmpty(v);
										std::size_t i = 0;

										for (const std::string &ref : valueVector)
										{
											out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "' + "  << i << " * MAX_OBJECT_DATA_FIELD_ENTRIES] = \"" << valueVector[i] << "\"" << std::endl;

											++i;
										}

										if (valueVector.size() > 0)
										{
											out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "Count['" << wc3lib::map::idToString(unit.customId()) << "'] = " << valueVector.size() << std::endl;
										}
									}
									else
									{
										std::cerr << boost::format(_("Warning: Extracting field %1% from object ID %2% does not work with type %3%.")) % wc3lib::map::idToString(modification.valueId()) % wc3lib::map::idToString(unit.customId()) % type  << std::endl;
									}
								} else if (type == "integer") {
									if (modification.value().isInteger())
									{
										out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "'] = " << modification.value().toInteger() << std::endl;
										out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "Count['" << wc3lib::map::idToString(unit.customId()) << "'] = 1" << std::endl;
									}
									else
									{
										std::cerr << boost::format(_("Warning: Extracting field %1% from object ID %2% does not work with type %3%.")) % wc3lib::map::idToString(modification.valueId()) % wc3lib::map::idToString(unit.customId()) % type  << std::endl;
									}
								} else if (type == "real") {
									out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "'] = " << modification.value().toReal() << std::endl;
									out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "Count['" << wc3lib::map::idToString(unit.customId()) << "'] = 1" << std::endl;
								} else if (type == "string") {
									out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "'] = \"" << modification.value().toString() << "\"" << std::endl;
									out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "Count['" << wc3lib::map::idToString(unit.customId()) << "'] = 1" << std::endl;
								} else if (type == "boolean") {
									out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "'] = " << modification.value().toBoolean() << std::endl;
									out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "Count['" << wc3lib::map::idToString(unit.customId()) << "'] = 1" << std::endl;
								} else if (type == "character") {
									out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "['" << wc3lib::map::idToString(unit.customId()) << "'] = " << modification.value().toCharacter() << std::endl;
									out << "\tset " << wc3lib::map::idToString(modification.valueId()) << "Count['" << wc3lib::map::idToString(unit.customId()) << "'] = 1" << std::endl;
								}
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

	if (vm.count("vjass"))
	{
		out << std::endl;
		out << "endlibrary" << std::endl;
	}

    std::cout << boost::format(_("Extracted %1% object data fields.")) % counter << std::endl;

	return EXIT_SUCCESS;
}
