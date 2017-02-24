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

#include <boost/filesystem.hpp>

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
		std::cerr << boost::format(_("Usage: %1% <input original war3map.wts> <input translated war3map.wts of original> <input untranslated war3map.wts of another map> <output file wts file>")) % argv[0] << std::endl;

		return 1;
	}

	const string inputOriginal = argv[1];
	const string inputTranslatedOriginal = argv[2];
	const string inputUntranslated = argv[3];
	const string output = argv[4];

	try
	{
		if (!boost::filesystem::exists(inputOriginal))
		{
			throw Exception(boost::format(_("Original input file %1% does not exist.")) % inputOriginal);
		}

		if (!boost::filesystem::exists(inputTranslatedOriginal))
		{
			throw Exception(boost::format(_("Input original translated file %1% does not exist.")) % inputTranslatedOriginal);
		}

		if (!boost::filesystem::exists(inputUntranslated))
		{
			throw Exception(boost::format(_("Input untranslated file %1% does not exist.")) % inputUntranslated);
		}

		MapStrings inputOriginalStrings;
		ifstream inOriginal(inputOriginal);
		inputOriginalStrings.read(inOriginal);

		/*
		 * Build up a map for faster searching for the translated entries.
		 */
		typedef std::map<int32, MapStrings::Entry> Entries;
		Entries originalEntries;

		for (std::size_t i = 0; i < inputOriginalStrings.entries().size(); ++i)
		{
			const int32 key = inputOriginalStrings.entries()[i].key;
			originalEntries.insert(std::make_pair(key, inputOriginalStrings.entries()[i]));
		}

		MapStrings inputTranslatedStrings;
		ifstream inTranslated(inputTranslatedOriginal);
		inputTranslatedStrings.read(inTranslated);

		typedef std::map<string, MapStrings::Entry> TranslationMap;
		TranslationMap translations;
		TranslationMap translationsReverse;

		for (std::size_t i = 0; i < inputTranslatedStrings.entries().size(); ++i)
		{
			/*
			 * Add the translation with the key of the untranslated string.
			 */
			const int32 key = inputTranslatedStrings.entries()[i].key;
			Entries::const_iterator iterator = originalEntries.find(key);

			if (iterator != originalEntries.end())
			{
				const string untranslatedValue = iterator->second.value;
				const MapStrings::Entry &translation = inputTranslatedStrings.entries()[i];
				translations.insert(std::make_pair(untranslatedValue, translation));

				const string translatedValue = translation.value;
				const MapStrings::Entry &untranslatedEntry = inputTranslatedStrings.entries()[i];
				translationsReverse.insert(std::make_pair(translatedValue, untranslatedEntry));
			}
			else
			{
				std::cerr << "Missing entry " << key << " in original file with " << originalEntries.size() << " entries (remove this entry from the translated file)." << std::endl;
			}
		}

		MapStrings inputUntranslatedStrings;

		ifstream inInputUntranslated(inputUntranslated);
		inputUntranslatedStrings.read(inInputUntranslated);

		for (std::size_t i = 0; i < inputUntranslatedStrings.entries().size(); ++i)
		{
			/*
			 * Look for a translation of this exact string.
			 */
			const string value = inputUntranslatedStrings.entries()[i].value;
			TranslationMap::const_iterator iterator = translations.find(value);

			if (iterator != translations.end())
			{
				const MapStrings::Entry &entry = iterator->second;
				inputUntranslatedStrings.entries()[i].value = entry.value;
				inputUntranslatedStrings.entries()[i].comment = entry.comment;
			}
			// If the already translated file is used as input, this value could already be translated. In this case don't check the keys (might differ) but check if the exact string is already translated. This is also useful when updating a translation file.
			else
			{
				const int32 untranslatedKey = inputUntranslatedStrings.entries()[i].key;
				TranslationMap::const_iterator iterator = translationsReverse.find(value);

				// This might happen if they are not the files from the same map but the file does already contain translations.
				if (iterator != translationsReverse.end())
				{
					const int32 translatedKey = iterator->second.key;

					if (untranslatedKey != translatedKey)
					{
						std::cerr << "Already translated entry " << untranslatedKey << " has not the same entry number as original " << translatedKey << std::endl;
					}
				}
				// Is not even an already translated string.
				else
				{
					std::cerr << "Missing translation for entry " << untranslatedKey << " with string \"" << value << "\" in translations with size " << translations.size() << "." << std::endl;
				}
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
