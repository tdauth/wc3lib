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
typedef std::map<wc3lib::map::id, std::map<wc3lib::map::id, std::set<wc3lib::map::id>>> WrittenModifiedIds;

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

inline std::string getCamelCase(const std::string &v)
{
	if (v.size() > 0)
	{
		return boost::to_upper_copy(v.substr(0, 1)) + v.substr(1);
	}

	return v;
}

inline std::string getCamelCase(const wc3lib::map::id &v)
{
	return getCamelCase(wc3lib::map::idToString(v));
}

inline bool isList(const std::string &type)
{
	return boost::algorithm::ends_with(type, "List");
}

inline std::string mapType(const std::string &type)
{
	if (type == "int" || type == "bool" || type == "intList" || type == "unitList" || type == "abilityList" || type == "itemList" || type == "techList" || type == "heroAbilityList" || type == "abilityList" || type == "upgradeList" || type == "tilesetList" || type == "targetList" || "abilCode" || type == "defenseType" || type == "attributeType" || type == "regenType" || type == "unitClass" || type == "deathType" || type == "unitRace" || type == "moveType" || type == "attackType" || type == "weaponType" || type == "pathingTexture")
	{
		return "integer";
	}

	if (type == "stringList")
	{
		return "string";
	}

	if (type == "realList")
	{
		return "real";
	}

	return type;
}

inline std::string getNativeTypeName(const std::string &type)
{
	if (type == "int" || type == "intList" || type == "unitList" || type == "abilityList" || type == "itemList" || type == "techList" || type == "heroAbilityList" || type == "abilityList" || type == "upgradeList") {
		return "Integer";
	} else if (type == "string") {
		return "Str";
	} else if (type == "real" || type == "unreal") {
		return "Real";
	} else if (type == "bool") {
		return "Boolean";
	}

	return "Integer";
}

inline std::string getNativeTypeDefault(const std::string &type)
{
	std::string mapped = mapType(type);

	if (type == "integer") {
		return "0";
	} else if (type == "string") {
		return "null";
	} else if (type == "real") {
		return "0.0";
	} else if (type == "boolean") {
		return "false";
	}

	return "0";
}

inline void appendLine(std::ofstream &out, long &lineCounter, long &initCounters, int limit, bool p)
{
	lineCounter++;

	if (limit > 0 && lineCounter >= limit)
	{
		initCounters++;
		lineCounter = 0;

		out << "endfunction" << std::endl;
		out << std::endl;

		if (p)
		{
			out << "private ";
		}

		out << "function InitObjectDataFields" << initCounters << " takes nothing returns nothing" << std::endl;
	}
}

template<typename V>
inline void addCall(std::ofstream &out, const std::string &type, wc3lib::map::id objectId, wc3lib::map::id fieldId, V v, long &lineCounter, long &initCounters, int limit, bool p)
{
	if (type == "int" || type == "bool" || type == "real" || type == "unreal" || type == "defenseType" || type == "attributeType" || type == "regenType" || type == "unitClass" || type == "deathType" || type == "unitRace" || type == "moveType" || type == "attackType" || type == "weaponType")
	{
		out << "\tcall Set" << getCamelCase(fieldId) << "('" << wc3lib::map::idToString(objectId) << "', " << v << ")" << std::endl;
		appendLine(out, lineCounter, initCounters, limit, p);
	}
	else if (type == "abilCode")
	{
		out << "\tcall Set" << getCamelCase(fieldId) << "('" << wc3lib::map::idToString(objectId) << "', '" << v << "')" << std::endl;
		appendLine(out, lineCounter, initCounters, limit, p);
	}
	else if (type == "string" || type == "pathingTexture")
	{
		out << "\tcall Set" << getCamelCase(fieldId) << "('" << wc3lib::map::idToString(objectId) << "', \"" << v << "\")" << std::endl;
		appendLine(out, lineCounter, initCounters, limit, p);
	}
	else if (type == "stringList" || type == "targetList" || type == "tilesetList")
	{
		out << "\tcall Add" << getCamelCase(fieldId) << "('" << wc3lib::map::idToString(objectId) << "', \"" << v << "\")" << std::endl;
		appendLine(out, lineCounter, initCounters, limit, p);
	}
	else if (type == "unitList" || type == "abilityList" || type == "itemList" || type == "techList" || type == "heroAbilityList" || type == "upgradeList")
	{
		out << "\tcall Add" << getCamelCase(fieldId) << "('" << wc3lib::map::idToString(objectId) << "', '" << v << "')" << std::endl;
		appendLine(out, lineCounter, initCounters, limit, p);
	}
	else if (type == "intList")
	{
		out << "\tcall Add" << getCamelCase(fieldId) << "('" << wc3lib::map::idToString(objectId) << "', " << v << ")" << std::endl;
		appendLine(out, lineCounter, initCounters, limit, p);
	}
	else
	{
		std::cerr << "Unknown type " << type << " for field " << wc3lib::map::idToString(fieldId) << " in object " << wc3lib::map::idToString(objectId) << std::endl;
	}
}

inline std::list<wc3lib::map::id> getMissingObjectIds(const WrittenModifiedIds &writtenModifiedIds, wc3lib::map::id originalId, wc3lib::map::id fieldId)
{
	WrittenModifiedIds::const_iterator iterator = writtenModifiedIds.find(originalId);
	std::list<wc3lib::map::id> customIds;

	if (iterator != writtenModifiedIds.end())
	{
		for (const auto &p : iterator->second)
		{
			if (!p.second.contains(fieldId))
			{
				customIds.push_back(p.first);
			}
		}
	}

	return customIds;
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
	int limit = 0;
	int structsize = 50000;
	Strings inputFiles;
	Strings inputDirectories;
	Strings slkMetaInputFiles;
	Strings slkInputFiles;
	Strings txtInputFiles;
	boost::filesystem::path outputFile;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of wc3objectdataextractor."))
	("help,h",_("Shows this text."))
	// options
	("verbose,v", _("Add more text output."))
	("force,F", _("Overwrites existing files and directories when creating or extracting files."))
    ("fieldids,f", boost::program_options::value<std::string>(&fieldIds)->default_value(""), _("Field IDs. Extracts all field IDS if none is specified. Comma-separated values: umki,ucam"))
	("fieldtypes,t", boost::program_options::value<std::string>(&fieldIdTypes)->default_value(""), _("The types of the fields in the same order as the field IDs. If no types are passed but meta data from SLK files is read, the types will be autodetected. Pass them as comma-separated values. These are all supported types: int, real, unreal, bool, intList, stringList, unitList, abilityList, itemList, techList, heroAbilityList, upgradeList, abilCode, defenseType, attributeType, regenType, unitClass, deathType, unitRace, moveType, attackType, weaponType, pathingTexture, tilesetList, targetList"))
    ("objectids,d", boost::program_options::value<std::string>(&objectIds)->default_value(""), _("Object IDs. Extracts all object IDS if none is specified."))
	("max,m", boost::program_options::value<int>(&max), _("Maximum number of list entries for a single field."))
	("oplimitoperations,l", boost::program_options::value<int>(&limit), _("Maximum number of operations."))
	("structsize,y", boost::program_options::value<int>(&structsize)->default_value(structsize), _("Maximum number of struct instances."))
	("private,p", _("Make everything private in the vJass library which should not be accessed from outside."))
	("public,b", _("Make everything public in the vJass library which can be accessed from outside."))
	("input,i", boost::program_options::value<Strings>(&inputFiles), _("Input object data files (.w3a, .w3u etc.)."))
	("inputdirectories,c", boost::program_options::value<Strings>(&inputDirectories), _("Input directories which are scanned recursively for SLK, TXT and object data files as input."))
	("slkmetainput,k", boost::program_options::value<Strings>(&slkMetaInputFiles), _("Input SLK files which contain meta data (UnitMetaData.slk, AbilityMetaData.slk etc.)."))
	("slkinput,s", boost::program_options::value<Strings>(&slkInputFiles), _("Input SLK files which contain data (UnitData.slk, AbilityData.slk etc.)."))
	("txtinput,x", boost::program_options::value<Strings>(&txtInputFiles), _("Input TXT files which object data strings."))
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
		if (fieldId.size() == 4)
		{
			fieldIdsSet.insert(wc3lib::map::stringToId(fieldId));

			if (fieldIdTypesVector.size() > i)
			{
				fieldTypes[wc3lib::map::stringToId(fieldId)] = fieldIdTypesVector[i];
			}

			++i;
		}
		else
		{
			std::cerr << boost::format(_("Skipping invalid field ID %1%.")) % fieldId << std::endl;
		}
	}

    long counter = 0;

	boost::filesystem::ofstream out(outputFile, std::ios::out);

	if (!out)
	{
		std::cerr << _("Error on opening output file.") << std::endl;

		return EXIT_FAILURE;
	}

	// Read input directories recursively for TXT and SLK files:
	for (Strings::const_reference path : inputDirectories)
    {
		boost::filesystem::path root = path;
		std::vector<boost::filesystem::path> paths;
		boost::filesystem::path p = path;

		if (boost::filesystem::exists(root) && boost::filesystem::is_directory(root))
		{
			for (auto const & entry : boost::filesystem::recursive_directory_iterator(root))
			{
				const std::string extension = boost::algorithm::to_lower_copy(entry.path().extension().string());
				const std::string filePath = entry.path().string();

				if (boost::filesystem::is_regular_file(entry)) {
					if (extension == ".txt") {
						txtInputFiles.push_back(filePath);

						if (vm.count("verbose"))
						{
							std::cout << boost::format(_("Add input file %1%.")) % filePath << std::endl;
						}
					} else if (extension == ".slk") {
						const std::string fileName = boost::algorithm::to_lower_copy(entry.path().filename().string());

						if (fileName.find("meta") != std::string::npos) {
							slkMetaInputFiles.push_back(filePath);
						} else {
							slkInputFiles.push_back(filePath);
						}

						if (vm.count("verbose"))
						{
							std::cout << boost::format(_("Add input file %1%.")) % filePath << std::endl;
						}
					} else if (extension == ".w3u" || extension == ".w3i") {
						inputFilePaths.push_back(filePath);

						if (vm.count("verbose"))
						{
							std::cout << boost::format(_("Add input file %1%.")) % filePath << std::endl;
						}
					}
				}
			}
		}
	}

	// Meta Data
	std::map<std::string, std::string> fieldIdsToNames;
	std::map<std::string, std::string> fieldNamesToIds;

	/*
	 * Read meta data file UnitMetaData.slk. The first column is the "id" and the second column is the "field" (name).
	 * The file "UnitData.slk" has rows for the object ID and columns for the fields but with the field names.
	 */
	for (Strings::const_reference path : slkMetaInputFiles)
	{
		boost::filesystem::ifstream in(path);

		if (in)
		{
			wc3lib::map::Slk slk;

			try
			{
				slk.read(in);

				if (slk.columns() >= 2)
				{
					for (std::size_t row = 1; row < slk.rows(); ++row)
					{
						const std::string id = wc3lib::map::Slk::fromSlkString(slk.cell(row, 0));
						const std::string name = wc3lib::map::Slk::fromSlkString(slk.cell(row, 1));

						if (id.length() == 4)
						{
							const wc3lib::map::id fieldId = wc3lib::map::stringToId(id);

							if (fieldIdsSet.empty() || fieldIdsSet.contains(fieldId))
							{
								if (vm.count("verbose"))
								{
									std::cout << "Adding field name " << name << " with ID " << id << " from file " << path << std::endl;
								}

								fieldIdsToNames[id] = name;
								fieldNamesToIds[name] = id;

								std::map<wc3lib::map::id, std::string>::iterator iterator = fieldTypes.find(fieldId);

								if (iterator == fieldTypes.end())
								{
									const std::string type = wc3lib::map::Slk::fromSlkString(slk.cell(row, 7));

									fieldTypes[fieldId] = type;

									if (vm.count("verbose"))
									{
										std::cout << "Autodetected type for field name " << name << " with ID " << id << " with name " << type << " from file " << path << std::endl;
									}
								}
							}
						}
						else
						{
							std::cerr << boost::format(_("Invalid length for field ID %1% from file %2%: %3%.")) % id % path % id.length() << std::endl;
						}
					}
				}
				else
				{
					std::cerr << boost::format(_("Missing first two columns in SLK file %1%.\nSkipping file.")) % path << std::endl;
				}
			}
			catch (const wc3lib::Exception &e)
			{
				std::cerr << boost::format(_("Error occured when converting file %1%: \"%2%\".\nSkipping file.")) % path % e.what() << std::endl;
			}
		}
		else
		{
			std::cerr << boost::format(_("Error occured when converting file %1%.\nSkipping file.")) % path << std::endl;
		}
	}

	if (fieldIdsVector.size() < fieldTypes.size())
	{
		std::cerr << boost::format(_("Missing %1% field types.")) % (fieldTypes.size() - fieldIdsVector.size()) << std::endl;

		return EXIT_FAILURE;
	}

	out << "library ObjectDataFields initializer InitObjectDataFields" << std::endl;
	out << std::endl;

	out << "globals" << std::endl;

	out << "\t";

	if (vm.count("public"))
	{
		out << "public ";
	}

	out << "constant integer MAX_OBJECT_DATA_FIELD_ENTRIES = " << max << std::endl;

	out << "\t";

	if (vm.count("private"))
	{
		out << "private ";
	}

	out << "hashtable fieldsHashTable = InitHashtable()" << std::endl;

	out << "endglobals" << std::endl;

	out << std::endl;

	if (vm.count("public"))
	{
		out << "public ";
	}

	std::set<std::string> allTypes;

	for (const std::string &fieldId : fieldIdsVector)
	{
		const std::string type = mapType(fieldTypes[wc3lib::map::stringToId(fieldId)]);
		allTypes.insert(type);
	}

	for (const std::string &type : allTypes)
	{
		if (vm.count("private"))
		{
			out << "private ";
		}

		out << "struct F" << type;

		if (structsize > 0)
		{
			out << "[" << structsize << "]";
		}

		out << std::endl;
		out << "\t" << type << " array v[MAX_OBJECT_DATA_FIELD_ENTRIES]" << std::endl;
		out << "\tinteger count = 0" << std::endl;
		out << std::endl;
		out << "\tmethod add takes " << type << " x returns integer" << std::endl;
		out << "\t\tlocal integer i = count" << std::endl;
		out << "\t\tset v[i] = x" << std::endl;
		out << "\t\tset count = count + 1" << std::endl;
		out << "\t\treturn i" << std::endl;
		out << "\tendmethod" << std::endl;
		out << std::endl;
		out << "endstruct" << std::endl;
	}

	out << std::endl;

	for (const std::string &fieldId : fieldIdsVector)
	{
		const std::string fieldType = fieldTypes[wc3lib::map::stringToId(fieldId)];
		const std::string type = mapType(fieldType);


		if (isList(fieldType))
		{
			if (vm.count("public"))
			{
				out << "public ";
			}

			out << "function Get" << getCamelCase(fieldId) << " takes integer objectId, integer index returns " << type << std::endl;
			out << "\tlocal F" << type << " f = Load" << getNativeTypeName(type) << "(fieldsHashTable, objectId, '" << fieldId << "')" << std::endl;
			out << "\tif (f == 0) then" << std::endl;
			out << "\t\treturn " << getNativeTypeDefault(type) << std::endl;
			out << "\tendif" << std::endl;
			out << "\treturn f.v[index]" << std::endl;
			out << "endfunction" << std::endl;

			out << std::endl;

			out << "function Get" << getCamelCase(fieldId) << "Count takes integer objectId returns integer" << std::endl;
			out << "\tlocal F" << type << " f = Load" << getNativeTypeName(type) << "(fieldsHashTable, objectId, '" << fieldId << "')" << std::endl;
			out << "\tif (f == 0) then" << std::endl;
			out << "\t\treturn " << getNativeTypeDefault(type) << std::endl;
			out << "\tendif" << std::endl;
			out << "\treturn f.count" << std::endl;
			out << "endfunction" << std::endl;

			out << std::endl;


			if (vm.count("public"))
			{
				out << "public ";
			}

			out << "function Add" << getCamelCase(fieldId) << " takes integer objectId, " << type << " v returns integer" << std::endl;
			out << "\tlocal F" << type << " f = Load" << getNativeTypeName(type) << "(fieldsHashTable, objectId, '" << fieldId << "')" << std::endl;
			out << "\tif (f == 0) then" << std::endl;
			out << "\t\tset f = F" << type << ".create()" << std::endl;
			out << "\t\tcall Save" << getNativeTypeName(type) << "(fieldsHashTable, objectId, '" << fieldId << "', f)" << std::endl;
			out << "\tendif" << std::endl;
			out << "\treturn f.add(v)" << std::endl;
			out << "endfunction" << std::endl;

			out << std::endl;
		}
		else
		{
			if (vm.count("public"))
			{
				out << "public ";
			}

			out << "function Get" << getCamelCase(fieldId) << " takes integer objectId returns " << type << std::endl;
			out << "\treturn Load" << getNativeTypeName(type) << "(fieldsHashTable, objectId, '" << fieldId << "')" << std::endl;
			out << "endfunction" << std::endl;

			out << std::endl;

			if (vm.count("public"))
			{
				out << "public ";
			}

			out << "function Set" << getCamelCase(fieldId) << " takes integer objectId, " << type << " v returns nothing" << std::endl;
			out << "\tcall Save" << getNativeTypeName(type) << "(fieldsHashTable, objectId, '" << fieldId << "', v)" << std::endl;
			out << "endfunction" << std::endl;

			out << std::endl;
		}
	}

	if (limit > 0)
	{
		if (vm.count("private"))
		{
			out << "private ";
		}

		out << "function NewOpLimit takes code callback returns nothing" << std::endl;
		out << "\tcall ForForce(bj_FORCE_PLAYER[0], callback)" << std::endl;
		out << "endfunction" << std::endl;

		out << std::endl;
	}


	if (vm.count("private"))
	{
		out << "private ";
	}

	long initCounters = 0;
	out << "function InitObjectDataFields" << initCounters << " takes nothing returns nothing" << std::endl;

	long lineCounter = 0;

	// Store all written field IDs with base IDs to know which default values still have to be added:
	WrittenModifiedIds writtenModifiedIds;

	for (FilePaths::reference path : inputFilePaths)
    {
        try
        {
			std::string extension = path.extension().string();
            wc3lib::map::CustomObjects customObjects(wc3lib::map::CustomObjects::typeByExtension(extension));
            boost::filesystem::ifstream in(path, std::ios::in | std::ios::binary);
            customObjects.read(in);

            for (const wc3lib::map::CustomUnits::Unit &unit : customObjects.customTable()) {
				const wc3lib::map::id customId = unit.customId();

				if (objectIdsSet.empty() || objectIdsSet.contains(customId)) {
					for (const wc3lib::map::CustomUnits::Set &set : unit.sets()) {
						for (const wc3lib::map::CustomUnits::Modification &modification : set.modifications()) {
							const wc3lib::map::id fieldId = modification.valueId();

							if (fieldIdsSet.empty() || fieldIdsSet.contains(fieldId)) {

								if (vm.count("verbose"))
								{
									std::cout << boost::format(_("Extracting field %1% from object ID %2%.")) % wc3lib::map::idToString(fieldId) % wc3lib::map::idToString(customId) << std::endl;
								}

								counter++;

								const std::string type = fieldTypes[fieldId];

								//std::cerr << "Field ID " << wc3lib::map::idToString(fieldId) << std::endl;
								//std::cerr << "Type " << type << std::endl;

								wc3lib::map::id originalId = unit.originalId();
								WrittenModifiedIds::iterator iterator = writtenModifiedIds.find(originalId);

								if (iterator == writtenModifiedIds.end())
								{
									writtenModifiedIds[unit.originalId()] = std::map<wc3lib::map::id, std::set<wc3lib::map::id>>();
									writtenModifiedIds[originalId][customId] = std::set<wc3lib::map::id>();
									writtenModifiedIds[originalId][customId].insert(fieldId);
								}
								else
								{
									std::map<wc3lib::map::id, std::set<wc3lib::map::id>>::iterator iterator2 = iterator->second.find(customId);

									if (iterator2 ==  writtenModifiedIds[originalId].end())
									{
										writtenModifiedIds[unit.originalId()][customId] = std::set<wc3lib::map::id>();
										writtenModifiedIds[originalId][customId].insert(fieldId);
									}
									else
									{
										iterator2->second.insert(fieldId);
									}
								}

								if (isList(type)) {
									if (modification.value().isList()) {
										const wc3lib::map::List &v = modification.value().toList();

										for (std::size_t i = 0; i < v.size(); i++) {
											addCall(out, type, customId, fieldId, v[i], lineCounter, initCounters, limit, vm.count("private"));

											if (i >= max) {
												std::cerr << boost::format(_("Warning: Reached maximum %1% with field %2% from object ID %3%.")) % max % wc3lib::map::idToString(fieldId) % wc3lib::map::idToString(customId) << std::endl;
											}
										}
									}
									else if (modification.value().isString()) {
										wc3lib::string v = modification.value().toString();
										std::vector<std::string> valueVector = splitAndIgnoreEmpty(v);
										std::size_t i = 0;

										for (const std::string &ref : valueVector) {
											addCall(out, type, customId, fieldId, ref, lineCounter, initCounters, limit, vm.count("private"));

											++i;
										}
									} else {
										std::cerr << boost::format(_("Warning: Extracting field %1% from object ID %2% does not work with type %3%.")) % wc3lib::map::idToString(fieldId) % wc3lib::map::idToString(customId) % type  << std::endl;
									}
								} else if (type == "int" || type == "bool") {
									if (modification.value().isInteger())
									{
										addCall(out, type, customId, fieldId, modification.value().toInteger(), lineCounter, initCounters, limit, vm.count("private"));
									}
									else
									{
										std::cerr << boost::format(_("Warning: Extracting field %1% from object ID %2% is not an integer with type %3%.")) % wc3lib::map::idToString(fieldId) % wc3lib::map::idToString(customId) % type  << std::endl;
									}
								} else if (type == "real") {
									if (modification.value().isReal())
									{
										addCall(out, type, customId, fieldId, modification.value().toReal(), lineCounter, initCounters, limit, vm.count("private"));
									}
									else
									{
										std::cerr << boost::format(_("Warning: Extracting field %1% from object ID %2% is not a real with type %3%.")) % wc3lib::map::idToString(fieldId) % wc3lib::map::idToString(customId) % type  << std::endl;
									}
								} else if (type == "string") {
									if (modification.value().isString())
									{
										addCall(out, type, customId, fieldId, modification.value().toString(), lineCounter, initCounters, limit, vm.count("private"));
									}
									else
									{
										std::cerr << boost::format(_("Warning: Extracting field %1% from object ID %2% is not a string with type %3%.")) % wc3lib::map::idToString(fieldId) % wc3lib::map::idToString(customId) % type  << std::endl;
									}
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

	for (Strings::const_reference path : slkInputFiles)
	{
		boost::filesystem::ifstream in(path);

		if (in)
		{
			wc3lib::map::Slk slk;

			try
			{
				slk.read(in);

				for (std::size_t column = 1; column < slk.columns(); ++column)
				{
					const std::string fieldName = wc3lib::map::Slk::fromSlkString(slk.cell(0, column));
					std::map<std::string, std::string>::iterator iterator = fieldNamesToIds.find(fieldName);

					if (iterator != fieldNamesToIds.end())
					{
						const std::string fieldIdString = iterator->second;
						const wc3lib::map::id fieldId = wc3lib::map::stringToId(iterator->second);

						if (fieldId != 0 && (fieldIdsSet.empty() || fieldIdsSet.contains(fieldId)))
						{
							const std::string type = fieldTypes[fieldId];

							for (std::size_t row = 1; row < slk.rows(); ++row)
							{
								const std::string objectIdString = wc3lib::map::Slk::fromSlkString(slk.cell(row, 0));
								const wc3lib::map::id objectId = wc3lib::map::stringToId(objectIdString);
								const std::string v = boost::trim_copy(wc3lib::map::Slk::fromSlkString(slk.cell(row, column)));

								if (v.length() > 0 && v != "_")
								{
									std::list<wc3lib::map::id> objectIds = getMissingObjectIds(writtenModifiedIds, objectId, fieldId);
									objectIds.push_front(objectId);
									std::vector<std::string> vector = splitAndIgnoreEmpty(v);

									for (wc3lib::map::id o : objectIds)
									{
										for (const std::string &actualV : vector)
										{
											addCall(out, type, o, fieldId, actualV, lineCounter, initCounters, limit, vm.count("private"));
										}
									}

									if (vm.count("verbose"))
									{
										std::cout << "Adding field name " << fieldName << " (" << wc3lib::map::idToString(fieldId) << ") for object " << objectIdString << " from file " << path << std::endl;
									}
								}
							}
						}
						else
						{
							if (vm.count("verbose"))
							{
								std::cerr << "Field " << fieldIdString << " from file " << path << " has no valid field ID." << std::endl;
							}
						}
					}
				}
			}
			catch (const wc3lib::Exception &e)
			{
				std::cerr << boost::format(_("Error occured when converting file %1%: \"%2%\".\nSkipping file.")) % path % e.what() << std::endl;
			}
		}
		else
		{
			std::cerr << boost::format(_("Error occured when converting file %1%.\nSkipping file.")) % path << std::endl;
		}
	}

	for (Strings::const_reference path : txtInputFiles)
	{
		boost::filesystem::ifstream in(path);

		if (in)
		{
			wc3lib::map::Txt txt;

			try
			{
				txt.read(in);

				for (wc3lib::map::Txt::Sections::const_reference section : txt.sections())
				{
					const std::string objectIdString = section.name;

					if (objectIdString.size() == 4)
					{
						const wc3lib::map::id objectId = wc3lib::map::stringToId(objectIdString);

						for (wc3lib::map::Txt::Entries::const_reference entry : section.entries)
						{
							std::string fieldName = entry.first;
							std::map<std::string, std::string>::iterator iterator = fieldNamesToIds.find(fieldName);

							// If the ID is not found it might be the case that it has been excluded from the extraction.
							if (iterator != fieldNamesToIds.end())
							{
								const std::string fieldIdString = iterator->second;
								const wc3lib::map::id fieldId = wc3lib::map::stringToId(fieldIdString);

								if (fieldId != 0 && (fieldIdsSet.empty() || fieldIdsSet.contains(fieldId)))
								{
									const std::string type = fieldTypes[fieldId];
									const std::string v = boost::trim_copy(entry.second);

									if (v.length() > 0 && v != "_")
									{
										std::list<wc3lib::map::id> objectIds = getMissingObjectIds(writtenModifiedIds, objectId, fieldId);
										objectIds.push_front(objectId);
										std::vector<std::string> vector = splitAndIgnoreEmpty(v);

										for (wc3lib::map::id o : objectIds)
										{
											for (const std::string &actualV : vector)
											{
												addCall(out, type, o, fieldId, actualV, lineCounter, initCounters, limit, vm.count("private"));
											}
										}

										if (vm.count("verbose"))
										{
											std::cout << "Adding field name " << fieldName << " (" << wc3lib::map::idToString(fieldId) << ") for object " << objectIdString << " from file " << path << std::endl;
										}
									}
								}
							}
						}
					}
					else
					{
						if (vm.count("verbose"))
						{
							std::cout << "Skipping non-object TXT entry " << objectIdString << " from file " << path << std::endl;
						}
					}
				}
			}
			catch (const wc3lib::Exception &e)
			{
				std::cerr << boost::format(_("Error occured when converting file %1%: \"%2%\".\nSkipping file.")) % path % e.what() << std::endl;
			}
		}
		else
		{
			std::cerr << boost::format(_("Error occured when converting file %1%.\nSkipping file.")) % path << std::endl;
		}
	}

	out << "endfunction" << std::endl;
	out << std::endl;

	if (vm.count("private"))
	{
		out << "private ";
	}

	out << "function InitObjectDataFields takes nothing returns nothing" << std::endl;

	for (long i = 0; i <= initCounters; ++i)
	{
		out << "\tcall NewOpLimit(function InitObjectDataFields" << i << ")" << std::endl;
	}

	out << "endfunction" << std::endl;


	out << std::endl;
	out << "endlibrary" << std::endl;

    std::cout << boost::format(_("Extracted %1% object data fields.")) % counter << std::endl;

	return EXIT_SUCCESS;
}
