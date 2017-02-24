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
#include <boost/program_options.hpp>

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
	string inputOriginal;
	string inputTranslatedOriginal;
	string inputUntranslated;
	string output;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of mpq."))
	("help,h",_("Shows this text."))
	("verbose", _("Add more text output."))
	("update", _("Drops entries which are not from the original and adds additional entries from the original which are not part of the translated."))
	("s", boost::program_options::value<string>(&inputOriginal), _("Source file."))
	("t", boost::program_options::value<string>(&inputTranslatedOriginal), _("Translated file."))
	("u", boost::program_options::value<string>(&inputUntranslated), _("Untranslated file."))
	("o", boost::program_options::value<string>(&output), _("Output file."))
	;

	boost::program_options::positional_options_description p;
	p.add("s", 1);
	p.add("t", 1);
	p.add("u", 1);
	p.add("o", 1);

	boost::program_options::variables_map vm;

	try
	{
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	}
	catch (const std::exception &exception)
	{
		std::cerr << boost::format(_("Error while parsing program options: \"%1%\"")) % exception.what() << std::endl;

		std::cerr << boost::format(_("Usage: %1% <input original war3map.wts> <input translated war3map.wts of original> <input untranslated war3map.wts of another map> <output file wts file>")) % argv[0] << std::endl;

		return EXIT_FAILURE;
	}

	boost::program_options::notify(vm);

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
				const MapStrings::Entry &untranslatedEntry = iterator->second;
				const string untranslatedValue = untranslatedEntry.value;
				const MapStrings::Entry &translation = inputTranslatedStrings.entries()[i];
				const string translatedValue = translation.value;

				// Only add it if it is different from the original value.
				if (untranslatedValue != translatedValue)
				{
					TranslationMap::const_iterator translationIterator = translations.find(untranslatedValue);

					if (translationIterator != translations.end())
					{
						const string alreadyTranslatedValue = translationIterator->second.value;

						if (alreadyTranslatedValue != translatedValue)
						{
							std::cerr << "Conflict: Multiple translations for \"" << untranslatedValue << "\": \"" << translatedValue << "\" and \"" << alreadyTranslatedValue << "\"." << std::endl;
						}
					}

					translations.insert(std::make_pair(untranslatedValue, translation));
					translationsReverse.insert(std::make_pair(translatedValue, untranslatedEntry));
				}
			}
			else
			{
				std::cerr << "Missing entry " << key << " in original file with " << originalEntries.size() << " entries (remove this entry from the translated file)." << std::endl;
			}
		}

		MapStrings inputUntranslatedStrings;

		ifstream inInputUntranslated(inputUntranslated);
		inputUntranslatedStrings.read(inInputUntranslated);
		Entries outEntries;
		std::size_t missingTranslations = 0;

		for (std::size_t i = 0; i < inputUntranslatedStrings.entries().size(); ++i)
		{
			/*
			 * Look for a translation of this exact string.
			 */
			MapStrings::Entry entry = inputUntranslatedStrings.entries()[i];
			const int32 key = inputUntranslatedStrings.entries()[i].key;

			if (vm.count("update"))
			{
				Entries::const_iterator iterator = originalEntries.find(key);

				// The key does not exist in the original file, so this entry has to go if it is updated.
				if (iterator == originalEntries.end())
				{
					std::cerr << "Skipping entry " << key << " which does not exist in the original file since --update is used." << std::endl;

					continue;
				}
				// At least update always the comment
				else
				{
					entry.comment = iterator->second.comment;
				}
			}

			const string value = entry.value;
			TranslationMap::const_iterator iterator = translations.find(value);

			// A translation does already exist of the value, so use it.
			if (iterator != translations.end())
			{
				const MapStrings::Entry &translatedEntry = iterator->second;
				entry.value = translatedEntry.value;
				entry.comment = translatedEntry.comment;
			}
			// If the already translated file is used as input, this value could already be translated. In this case don't check the keys (might differ) but check if the exact string is already translated. This is also useful when updating a translation file.
			else
			{
				TranslationMap::const_iterator iterator = translationsReverse.find(value);

				// This might happen if they are not the files from the same map but the file does already contain translations.
				if (iterator != translationsReverse.end())
				{
					const int32 translatedKey = iterator->second.key;

					if (key != translatedKey)
					{
						std::cerr << "Already translated entry " << key << " has not the same entry number as original " << translatedKey << std::endl;
					}
				}
				// Is not even an already translated string.
				else
				{
					std::cerr << "Missing translation for entry " << key << " with string \"" << value << "\" in translations with size " << translations.size() << "." << std::endl;
					++missingTranslations;
				}
			}

			outEntries.insert(std::make_pair(key, entry));
		}

		/*
		 * Add all entries from the original source which are not part of the translated document if "--update" is specified.
		 */
		if (vm.count("update"))
		{
			for (std::size_t i = 0; i < inputOriginalStrings.entries().size(); ++i)
			{
				MapStrings::Entry entry = inputOriginalStrings.entries()[i];
				const int32 key = entry.key;

				if (outEntries.find(key) == outEntries.end())
				{
					std::cerr << "Add entry " << key << " from original strings which does not exist in the translated file." << std::endl;

					// Could still exist already as translation.
					TranslationMap::const_iterator iterator = translations.find(entry.value);

					if (iterator != translations.end())
					{
						const string translatedValue = iterator->second.value;
						std::cerr << "Found a translation for the text, maybe the key has simply changed: " << translatedValue << std::endl;

						entry.value = translatedValue;
					}

					outEntries.insert(std::make_pair(key, entry));
				}
			}
		}

		typedef std::pair<int32, MapStrings::Entry> Pair;
		std::vector<Pair> entries(outEntries.begin(), outEntries.end());
		// Sort by the key.
		std::sort(entries.begin(), entries.end(), [](const Pair &a, const Pair &b) { return a.first < b.first; });

		MapStrings outputStrings;

		for (const Pair &e : entries)
		{
			outputStrings.entries().push_back(e.second);
		}

		ofstream out(output);
		outputStrings.write(out);

		std::cerr << "Missing translations: " << missingTranslations << std::endl;
	}
	catch (const Exception &e)
	{
		std::cerr << e.what() << std::endl;

		return 1;
	}

	return 0;
}
