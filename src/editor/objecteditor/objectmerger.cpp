/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#include "../../editor.hpp"

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>

#include <QTemporaryFile>

using namespace wc3lib;
using namespace wc3lib::editor;

namespace
{

}

/*
 * ObjectMerger emulator which allows modifying object data of maps via command line.
 */
int main(int argc, char *argv[])
{
	MpqPriorityList source;

	if (!source.configure(nullptr, "wc3lib", "objectmerger"))
	{
		return 1;
	}


	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("objectmerger", LOCALE_DIR);
	textdomain("objectmerger");

	if (argc < 7)
	{
		std::cerr << _("Missing arguments.") << std::endl;

		return 1;
	}

	boost::filesystem::path mapPath = argv[1];
	const string type = argv[2];
	const string originalObjectId = argv[3];
	const string customObjectId = argv[4];

	if (!boost::filesystem::exists(mapPath))
	{
		std::cerr << boost::format(_("File %1% does not exist.")) % mapPath << std::endl;

		return 1;
	}

	// TODO support not only map path but also a path to a custom object collection or data
	map::W3x w3x;

	try
	{
		w3x.open(mapPath);
	}
	catch (const Exception &e)
	{
		std::cerr << e.what() << std::endl;

		return 1;
	}

	ObjectData *objectData = nullptr;
	string fileName;
	map::CustomObjects::Type customObjectsType = map::CustomObjects::Type::Abilities;

	if (type == "w3a")
	{
		fileName = "war3map.w3a";
		objectData = new AbilityData(&source);
		customObjectsType = map::CustomObjects::Type::Abilities;
	}
	/*
	 * TODO
	• w3u units
	• w3t items
	• w3b destructables
	• w3d doodads
	• w3a abilities
	• w3h buffs
	• w3q upgrades
	*/


	if (objectData == nullptr || fileName.empty())
	{
		std::cerr << _("Unknown type of object data.") << std::endl;

		return 1;
	}

	mpq::File file = w3x.findFile(fileName);

	if (!file.isValid())
	{
		std::cerr << _("File does not exist.") << std::endl;

		return 1;
	}

	QTemporaryFile tmpFile;
	std::ofstream out(tmpFile.fileName().toStdString());
	file.decompress(out);
	out.close();

	std::ifstream in(tmpFile.fileName().toStdString());
	map::CustomObjects customObjects(customObjectsType);
	customObjects.read(in);
	in.close();
	tmpFile.remove();

	objectData->importCustomObjects(customObjects);

	// TODO apply map strings!

	// TODO check if object IDs are valid
	if (!objectData->standardObjectIds().contains(originalObjectId.c_str()))
	{
		std::cerr << _("Invalid original object ID.") << std::endl;

		return 1;
	}

	std::vector<string> fieldIds;
	std::vector<string> fieldLevels;
	std::vector<string> fieldValues;

	for (int i = 5; i < argc; ++i)
	{
		const string fieldId = argv[i];

		// TODO check if fieldId is valid

		string fieldLevel = "1";

		// level is required by all fields for multiple levels
		if (objectData->repeateField(fieldId.c_str()))
		{
			++i;

			if (i >= argc)
			{
				std::cerr << _("Expected field level.") << std::endl;

				return 1;
			}

			fieldLevel = argv[i];
		}

		++i;

		if (i >= argc)
		{
			std::cerr << _("Expected field value.") << std::endl;

			return 1;
		}

		const string fieldValue = argv[i];

		fieldIds.push_back(fieldId);
		fieldLevels.push_back(fieldLevel);
		fieldValues.push_back(fieldValue);
	}

	// TODO apply fields and add strings to map strings again?
	for (std::size_t i = 0; i < fieldIds.size(); ++i)
	{
		const string fieldId = fieldIds[i];
		const string fieldLevel = fieldLevels[i];
		const string fieldValue = fieldValues[i];
		const int level = std::stoi(fieldLevel);

		objectData->modifyField(originalObjectId.c_str(), customObjectId.c_str(), fieldId.c_str(), fieldValue.c_str(), level);
	}

	stringstream bytes;
	objectData->customObjects().write(bytes);

	// TODO remove old file
	//w3x.removeFile(fileName);
	w3x.addFile(fileName, bytes.str().c_str(), bytes.str().size());

	// TODO map strings have to be applied?


	return EXIT_SUCCESS;
}
