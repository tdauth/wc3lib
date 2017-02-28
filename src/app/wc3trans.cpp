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
#include <limits>

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
 * It can also be used with "--update" which drops all entries in the third file which are not in the first one and adds all which are in the first one but not in the third one.
 *
 * This might be very useful if two maps share many strings. For example when you use the same object data for two maps.
 *
 * TODO Another idea would be to use a Google Translate API via web to find missing translations and to use them when an option is specified.
 */
int main(int argc, char *argv[])
{
	string inputOriginal;
	string inputTranslatedOriginal;
	string inputUntranslated;
	string output;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of wc3trans."))
	("help,h",_("Shows this text."))
	("verbose", _("Add more text output."))
	("conflicts", _("Check for conflicts of translations. Conflicts appear when two same source strings are translated with different strings."))
	("suggest", _("Searches for similar source strings for untranslated strings and suggests the most likeable one. This reduces the performance."))
	("update", _("Drops entries which are not from the original and adds additional entries from the original which are not part of the translated."))
	("s", boost::program_options::value<string>(&inputOriginal), _("Source file which is untranslated."))
	("t", boost::program_options::value<string>(&inputTranslatedOriginal), _("Translated file version of the source file."))
	("u", boost::program_options::value<string>(&inputUntranslated), _("An unrelated untranslated/unfinished translation file."))
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

	if (vm.count("version"))
	{
		static const char *version = "0.1";

		std::cout << boost::format(_(
		"wc3trans %1%.\n"
		"Copyright © 2010 Tamino Dauth\n"
		"License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law."
		)) % version << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help"))
	{
		std::cout << _("Usage: wc3trans [options] <untranslated source war3map.wts file> <translated version of the source file> <another war3map.wts file to be updated> <output file path>") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << _("\nReport bugs to tamino@cdauth.eu or on https://wc3lib.org") << std::endl;

		return EXIT_SUCCESS;
	}

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
		 * Build up a map for faster searching for the original entries.
		 */
		typedef std::map<int32, MapStrings::Entry> Entries;
		Entries originalEntries;

		for (std::size_t i = 0; i < inputOriginalStrings.entries().size(); ++i)
		{
			const int32 key = inputOriginalStrings.entries()[i].key;
			originalEntries.insert(std::make_pair(key, inputOriginalStrings.entries()[i]));
		}

		/*
		 * Build up a map for faster searching for the translated versions of the original entries in both directions:
		 * - From the untranslated string to the translated entry.
		 * - From the translated string to the untranslated entry (this map is required to check if an entry is already translated).
		 *
		 * Only add entries to the map which differ from the original strings. These translations are only required for values which had been translated.
		 */
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
				const MapStrings::Entry &translatedEntry = inputTranslatedStrings.entries()[i];
				const string translatedValue = translatedEntry.value;

				// Only add it if it is different from the original value.
				if (untranslatedValue != translatedValue)
				{
					TranslationMap::const_iterator translationIterator = translations.find(untranslatedValue);

					if (translationIterator != translations.end())
					{
						if (vm.count("conflicts"))
						{
							const string alreadyTranslatedValue = translationIterator->second.value;

							if (alreadyTranslatedValue != translatedValue)
							{
								std::cerr << "Conflict: Multiple translations for \"" << untranslatedValue << "\": \"" << translatedValue << "\" and \"" << alreadyTranslatedValue << "\"." << std::endl;
							}
						}
					}
					// Always use the first appearance only as translation, don't overwrite existing ones! Otherwise they will be replaced with wrongly translated values on "--update"? Always assume that the first translation is the correct one.
					else
					{
						translations.insert(std::make_pair(untranslatedValue, translatedEntry));
						translationsReverse.insert(std::make_pair(translatedValue, untranslatedEntry));
					}
				}
			}
			else
			{
				std::cerr << "Missing entry " << key << " in original file with " << originalEntries.size() << " entries (remove this entry from the translated file " << inputTranslatedOriginal << ")." << std::endl;
			}
		}

		/*
		 * Read the third input file and find untranslated strings. Translate them with the help of the maps.
		 */
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

			/*
			 * If this option is specified, all entries are skipped which do not exist in the original file.
			 * They will be removed from the new output file.
			 *
			 * For existing entries the comment is updated.
			 */
			if (vm.count("update"))
			{
				Entries::const_iterator iterator = originalEntries.find(key);

				// The key does not exist in the original file, so this entry has to go if it is updated.
				if (iterator == originalEntries.end())
				{
					std::cerr << "Skipping entry " << key << " which does not exist in the original file since --update is used." << std::endl;

					continue;
				}
				// At least update always the comment.
				else
				{
					entry.comment = iterator->second.comment;
				}
			}

			/*
			 * Find the translation of the exact string.
			 */
			const string value = entry.value;
			TranslationMap::const_iterator iterator = translations.find(value);

			// A translation does already exist of the value, so use it.
			if (iterator != translations.end())
			{
				const MapStrings::Entry &translatedEntry = iterator->second;
				entry.value = translatedEntry.value;
				// Don't update the comment, since it does not have to point to the same entry. The comment has been updated with "--update" before anyway.
			}
			// If the already translated file is used as input, this value could already be translated. In this case don't check the keys (might differ) but check if the exact string is already translated. This is also useful when updating a translation file.
			else
			{
				// Is not even an already translated string.
				if (translationsReverse.find(value) == translationsReverse.end())
				{
					string suggestionAppendix;

					/*
					 * Search for the most likeable translated entry.
					 * This is done by comparing the source string with all other source strings which are translated.
					 * Suggest the translation of the source string which has the smallest difference to the current untranslated source string.
					 * This requires iterating over translations for every untranslated string!
					 */
					if (vm.count("suggest"))
					{
						string suggestion;
						string suggestionOrigin;
						int delta = std::numeric_limits<int>::max();

						for (TranslationMap::const_iterator iterator = translations.begin(); iterator != translations.end() && delta > 1; ++iterator)
						{
							const int currentDelta = std::abs<int>(value.compare(iterator->first));

							if (currentDelta < delta)
							{
								suggestion = iterator->second.value;
								suggestionOrigin = iterator->first;
								delta = currentDelta;
							}
						}

						stringstream sstream;
						sstream << " Suggesting translation \"" << suggestion << "\" with a delta of " << delta << " (smaller values mean it is more similar) from the origin string \"" << suggestionOrigin << "\".";
						suggestionAppendix = sstream.str();
					}

					std::cerr << "Missing translation for entry " << key << " with string \"" << value << "\" in translations." << suggestionAppendix << std::endl;
					++missingTranslations;
				}
			}

			outEntries.insert(std::make_pair(key, entry));
		}

		/*
		 * Add all entries from the original source which are not part of the translated document if "--update" is specified.
		 * But already use the translations which exist for them.
		 */
		if (vm.count("update"))
		{
			for (std::size_t i = 0; i < inputOriginalStrings.entries().size(); ++i)
			{
				MapStrings::Entry entry = inputOriginalStrings.entries()[i];
				const int32 key = entry.key;
				Entries::iterator iterator = outEntries.find(key);

				if (iterator == outEntries.end())
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
				// Update the comments of existing entries since --update is used.
				else
				{
					iterator->second.comment = entry.comment;
				}
			}
		}

		/*
		 * Now create the output file with all updated and newly added entries but sort them by their key before.
		 * The order should always be:
		 * STRING 0
		 * STRING 1
		 * STRING 2
		 * etc.
		 * since the World Editor handles it in the same way.
		 */
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

		std::cerr << "Missing translations: " << missingTranslations << " from " << inputUntranslatedStrings.entries().size() << " entries." << std::endl;
	}
	catch (const Exception &e)
	{
		std::cerr << e.what() << std::endl;

		return EXIT_FAILURE;
	}

	return 0;
}
