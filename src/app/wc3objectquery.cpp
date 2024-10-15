/***************************************************************************
 *   Copyright (C) 2024 by Tamino Dauth                                    *
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

const char *version = "0.1";

namespace {

struct Query {
    std::string fieldId;
    std::string op;
    std::string value;
};

typedef std::list<boost::filesystem::path> FilePaths;
typedef std::vector<Query> Queries;

inline bool query(const wc3lib::map::Value &v, const std::string &op, const std::string &value, bool verbose) {
    if (op == "=") {
        switch (v.type()) {
            case wc3lib::map::Value::Type::Integer: {
                return v.toInteger() == std::stoi(value);
            }

            case wc3lib::map::Value::Type::Real:
            case wc3lib::map::Value::Type::Unreal: {
                return v.toReal() == std::stof(value);
            }

            case wc3lib::map::Value::Type::String: {
                return v.toString() == value;
            }

            case wc3lib::map::Value::Type::Boolean: {
                bool match = value == "1" || value == "on";
                return v.toBoolean() == match;
            }

            case wc3lib::map::Value::Type::Character: {
                return v.toCharacter() == value[0];
            }

            // TODO Support all types
        }
    }

    return false;
}

inline void queryUnit(wc3lib::map::CustomObjects::Unit &unit, bool verbose, long &matchesCounter, const Queries &q) {
    for (std::size_t i = 0; i < unit.sets().size(); i++) {
        wc3lib::map::CustomObjects::Set *set = dynamic_cast<wc3lib::map::CustomObjects::Set*>(&unit.sets()[i]);
        std::size_t counter = 0;

        for (std::size_t j = 0; j < set->modifications().size(); j++) {
            //  wc3lib::map::CustomObjects::Modification &modification : unit.modifications()) {
            wc3lib::map::CustomObjects::Modification *modification = dynamic_cast<wc3lib::map::CustomObjects::Modification*>(&set->modifications()[j]);

            for (auto x : q) {
                if (modification->valueId() == wc3lib::map::stringToId(x.fieldId)) {
                    if (query(modification->value(), x.op, x.value, verbose)) {
                        counter++;
                    }
                }
            }
        }

        if (counter == q.size()) {
            if (verbose) {
                std::cout << boost::format(_("Found %1%:%2%.")) % wc3lib::map::idToString(unit.originalId()) % wc3lib::map::idToString(unit.customId()) << std::endl;
            }

            matchesCounter++;
        }
    }
}

inline void queryCustomObjects(wc3lib::map::CustomObjects &customObjects, bool verbose, long &matchesCounter, const Queries &q) {
    std::cout << boost::format(_("Read custom object data with %1% modified standard objects and %2% custom objects.")) % customObjects.originalTable().size() % customObjects.customTable().size() << std::endl;

    for (wc3lib::map::CustomObjects::Unit &unit : customObjects.originalTable()) {
        queryUnit(unit, verbose, matchesCounter, q);
    }

    for (wc3lib::map::CustomObjects::Unit &unit : customObjects.customTable()) {
        queryUnit(unit, verbose, matchesCounter, q);
    }
}

}

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("wc3objectquery", LOCALE_DIR);
	textdomain("wc3objectquery");

    bool verbose = false;
	typedef std::vector<std::string> Strings;
	Strings inputFiles;
	Strings queryExpressions;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of wc3objectcompressor."))
	("help,h",_("Shows this text."))
	// options
    ("verbose,v", _("Add more text output."))
	("i", boost::program_options::value<Strings>(&inputFiles), _("Input files."))
	("query,q", boost::program_options::value<Strings>(&queryExpressions), _("Query expressions."))
	;

	boost::program_options::positional_options_description p;
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
		boost::format(_("wc3objectquery %1%.")) % version
		<< std::endl
		<< wc3lib::wc3libCopyright()
        << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help")) {
		std::cout << _("Usage: wc3objectquery [options] [query expression] [input files]") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << wc3lib::wc3libReportBugs() << std::endl;

		return EXIT_SUCCESS;
	}

	verbose = vm.count("verbose");

	FilePaths inputFilePaths;

	for (Strings::const_reference ref : inputFiles) {
        inputFilePaths.push_back(ref);
	}

	if (inputFilePaths.empty()) {
		std::cerr << _("No valid input files.") << std::endl;

		return EXIT_FAILURE;
	}

	Queries queries;

	for (auto e : queryExpressions) {
        if (e.empty()) {
            std::cerr << _("Missing query expression.") << std::endl;

            return EXIT_FAILURE;
        }

        if (e.length() < 4) {
            std::cerr << _("Missing field ID query expression.") << std::endl;

            return EXIT_FAILURE;
        }

        std::string fieldId = e.substr(0, 4);

        if (e.length() == 4) {
            std::cerr << _("Missing operator query expression.") << std::endl;

            return EXIT_FAILURE;
        }

        std::string remaining = e.substr(4);
        std::string op = "=";

        if (boost::starts_with(remaining, "=")) {
            op = "=";
        } else if (boost::starts_with(remaining, "!=")) {
            op = "!=";
        } else if (boost::starts_with(remaining, ">")) {
            op = ">";
        } else if (boost::starts_with(remaining, ">=")) {
            op = ">=";
        } else if (boost::starts_with(remaining, "<=")) {
            op = "<=";
        } else {
            std::cerr << _("Invalid operation in query expression.") << std::endl;

            return EXIT_FAILURE;

        }

        std::string value = remaining.substr(op.length());

        Query q;
        q.fieldId = fieldId;
        q.op = op;
        q.value = value;

        if (verbose) {
            std::cout << boost::format(_("Query for field %1% with operator %2% and value %3%.")) % fieldId % op % value << std::endl;
        }

        queries.push_back(q);
    }

    long matchesCounter = 0;

	for (FilePaths::reference path : inputFilePaths) {

        try {
            if (path.extension().string() == ".w3o") {
                wc3lib::map::CustomObjectsCollection customObjects;

                boost::filesystem::ifstream in(path, std::ios::in | std::ios::binary);
                customObjects.read(in);

                if (customObjects.hasUnits()) {
                    queryCustomObjects(*customObjects.units().get(), verbose, matchesCounter, queries);
                }

                if (customObjects.hasItems()) {
                    queryCustomObjects(*customObjects.items().get(), verbose, matchesCounter, queries);
                }

                if (customObjects.hasDestructibles()) {
                    queryCustomObjects(*customObjects.destructibles().get(), verbose, matchesCounter, queries);
                }

                if (customObjects.hasDoodads()) {
                    queryCustomObjects(*customObjects.doodads().get(), verbose, matchesCounter, queries);
                }

                if (customObjects.hasAbilities()) {
                    queryCustomObjects(*customObjects.abilities().get(), verbose, matchesCounter, queries);
                }

                if (customObjects.hasBuffs()) {
                    queryCustomObjects(*customObjects.buffs().get(), verbose, matchesCounter, queries);
                }

                if (customObjects.hasUpgrades()) {
                    queryCustomObjects(*customObjects.upgrades().get(), verbose, matchesCounter, queries);
                }
            } else {
                wc3lib::map::CustomObjects customObjects(wc3lib::map::CustomObjects::typeByExtension(path.extension().string()));

                boost::filesystem::ifstream in(path, std::ios::in | std::ios::binary);
                customObjects.read(in);

                queryCustomObjects(customObjects, verbose, matchesCounter, queries);
            }
        }
        catch (const wc3lib::Exception &exception) {
            std::cerr << boost::format(_("Error occured when converting file %1%: \"%2%\".\nSkipping file.")) % path % exception.what() << std::endl;

            return EXIT_FAILURE;
        }
    }

    std::cout << boost::format(_("Matched %1% modifications.")) % matchesCounter << std::endl;

	return EXIT_SUCCESS;
}
