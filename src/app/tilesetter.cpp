/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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
#include <fstream>
#include <cstring>

#include <boost/format.hpp>

#include "../internationalisation.hpp"
#include "../exception.hpp"
#include "../map/environment.hpp"

using namespace wc3lib;
using namespace wc3lib::map;

namespace
{

static char* version = "0.1";

struct Command
{
	std::string name;
	char shortName;
	std::string help;
	std::size_t argCount;
	bool (*callbackFunction)(const struct Command &command, const std::list<std::string> &argParameters);
};


class Environment *openedEnvironment = 0;

bool commandOpen(const struct Command &command, const std::list<std::string> &argParameters)
{
	class Environment *environment = new Environment(0);
	std::ifstream ifstream(argParameters.front().c_str(), std::ios::in | std::ios::binary);

	try
	{
		std::cout << boost::format(_("Read %1% bytes.")) % environment->read(ifstream) << std::endl;
	}
	catch (class Exception &exception)
	{
		delete environment;
		std::cerr << boost::format(_("Error while reading environment file \"%1%\".")) % argParameters.front() << std::endl;

		return false;
	}

	openedEnvironment = environment;

	return true;
}

}

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("tilesetter", LOCALE_DIR);
	textdomain("tilesetter");

	std::cout << boost::format(_("Tilesetter version %1%")) % version << std::endl;
	std::cout << _(
	"Copyright (C) 2009 by Tamino Dauth\n"
	"tamino@cdauth.de\n"
	"\n"
	"This program is free software; you can redistribute it and/or modify\n"
	"it under the terms of the GNU General Public License as published by\n"
	"the Free Software Foundation; either version 2 of the License, or\n"
	"(at your option) any later version.\n"
	"\n"
	"This program is distributed in the hope that it will be useful,\n"
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	"GNU General Public License for more details.\n"
	"\n"
	"You should have received a copy of the GNU General Public License\n"
	"along with this program; if not, write to the\n"
	"Free Software Foundation, Inc.,\n"
	"59 Temple Place - Suite 330, Boston, MA  02111-1307, USA."
	) << std::endl;

	std::list<struct Command> commands;

	struct Command openCommand =
	{
		.name = "open",
		.shortName = 'o',
		.help = _("Opens a .w3e environment file."),
		.argCount = 1,
		.call = 0
	};

	commands.push_back(openCommand);

	std::cout << _("Usage 1: <command name/command short name> <argument1> <argument2> ... <argumentn>") << std::endl;
	std::cout << _("Usage 2: <command name/command short name> help") << std::endl;

	for (std::size_t i = 0; i < commandsCount; ++i)
		std::cout << boost::format(_("\tCommand \"%1%\" (\"%2%\") - %3% arguments")) % commands[i].name % commands[i].shortName % commands[i].argCount << std::endl;

	std::string input;

	do
	{
		std::getline(std::cin, input);
		std::string commandName(strtok(input.c_str(), " "));
		bool found = false;

		BOOST_FOREACH(struct Command &command, commands)
		{
			if (commandName == command.name || (commandName.size() == 1 && commandName[0] == command.shortName))
			{
				found = true;
				bool run = true;
				std::list<std::string> arguments;

				for (std::size_t i = 0; i < command.argCount < 1 ? 1 : command.argCount; ++i)
				{
					std::string argument(strtok(input.c_str(), " ");

					if (argument.empty())
					{
						run = false;
						std::cerr << boost::format(_("Error: Missing argument %1%.")) % i + 1 << std::endl;

						break;
					}
					else if (i == 0 && argument == "help")
					{
						run = false;
						std::cout << boost::format(_("Command help:\n%1%")) % command.help << std::endl;

						break;
					}

					arguments.push_back(argument);
				}

				if (run)
				{
					if (command.callbackFunction != 0)
					{
						if (!command.callbackFunction(command, arguments))
							std::cerr << _("Error occured while program was running command.") << std::endl;
					}
					else
						std::cerr << _("Command has no function.") << std::endl;
				}

				break;
			}
		}

		if (!found)
			std::cout << _("Unknown command.") << std::endl;
	}
	while (input != "quit" && input != "q");

	if (openedEnvironment != 0)
		delete openedEnvironment;

	return EXIT_SUCCESS;
}
