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
#include <map>

#include "../map.hpp"

using namespace wc3lib;
using namespace wc3lib::map;

/**
 * Loads a war3map.wts file and a translated version of it.
 * Checks in a third war3map.wts file which entries are the same as in the first one and gets the corresponding translations from the second one.
 * It generates a fourth file with the translations which could be found.
 *
 * This might be very useful if two maps share many strings. For example when you use the same object data for two maps.
 */
int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		std::cerr << boost::format(_("Usage: %1% <input original war3map.wts> <input translated war3map.wts> <input untranslated war3map.wts of another map> <output file wts file>")) % argv[0] << std::endl;

		return 1;
	}

	const string input = argv[1];
	const string inputTranslated = argv[2];
	const string inputUntranslated = argv[3];
	const string output = argv[4];

	try
	{
		MapStrings inputStrings;

		ifstream inInput(input);
		inputStrings.read(inInput);

		MapStrings inputTranslatedStrings;

		ifstream inInputTranslated(inputTranslated);
		inputTranslatedStrings.read(inInputTranslated);

		/*
		 * Build up a map for faster searching for the translated entries.
		 */
		typedef std::map<int32, MapStrings::Entry> Entries;
		Entries translatedEntries;

		for (std::size_t i = 0; i < inputTranslatedStrings.entries().size(); ++i)
		{
			const int32 translatedKey = inputTranslatedStrings.entries()[i].key;
			translatedEntries.insert(std::make_pair(translatedKey, inputTranslatedStrings.entries()[i]));
		}

		typedef std::map<string, MapStrings::Entry> TranslationMap;
		TranslationMap translations;

		for (std::size_t i = 0; i < inputStrings.entries().size(); ++i)
		{
			const int32 untranslatedKey = inputStrings.entries()[i].key;
			Entries::const_iterator iterator = translatedEntries.find(untranslatedKey);

			if (iterator != translatedEntries.end())
			{
				const string untranslated = inputStrings.entries()[i].value;
				translations.insert(std::make_pair(untranslated, iterator->second));
			}
			else
			{
				std::cerr << "Missing entry " << untranslatedKey << " in translated file with " << translatedEntries.size() << " entries" << std::endl;
			}
		}

		MapStrings inputUntranslatedStrings;

		ifstream inInputUntranslated(inputUntranslated);
		inputUntranslatedStrings.read(inInputUntranslated);

		for (std::size_t i = 0; i < inputUntranslatedStrings.entries().size(); ++i)
		{
			TranslationMap::const_iterator iterator = translations.find(inputUntranslatedStrings.entries()[i].value);

			if (iterator != translations.end())
			{
				inputUntranslatedStrings.entries()[i].value = iterator->second.value;
				inputUntranslatedStrings.entries()[i].comment = iterator->second.comment;
			}
			else
			{
				std::cerr << "Missing translation for entry " << inputUntranslatedStrings.entries()[i].key << std::endl;
			}
		}

		ofstream out(output);
		inputUntranslatedStrings.write(out);
	}
	catch (const Exception &e)
	{
		std::cerr << e.what() << std::endl;

		return 1;
	}

	return 0;
}
