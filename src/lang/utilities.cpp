/***************************************************************************
 *   Copyright (C) 2008 by Tamino Dauth                                    *
 *   tamino@cdauth.de                                                      *
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

#include "utilities.hpp"
#include "internationalisation.hpp"

namespace wc3lib
{

namespace lang
{

std::string getToken(const std::string &line, std::string::size_type &index, bool endOfLine)
{
	if (index >= line.length() || line.empty()) //important
		return std::string();

	while (index < line.length() && (line[index] == ' ' || line[index] == '\t'))
		++index;

	std::string::size_type position = index;
	std::string::size_type length = 1;

	if (!endOfLine)
	{
		do
		{
			++index;
		}
		while (index <= line.length() && line[index] != ' ' && line[index] != '\t');

		length = index - position;
	}
	else
	{
		index = line.length();
		length = index;
	}

	return line.substr(position, length);
}

void createHtmlHeader(std::ostream &ostream, const std::string &title, const std::string &language)
{
	ostream
	<< "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	<< "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"
	<< "\t\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
	<< "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"" << language.c_str() << "\">\n"
	<< "<html>\n"
	<< "\t<head>\n"
	<< "\t\t<meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\"/>\n"
	<< "\t\t<title>" << title.c_str() << "</title>\n"
	<< "\t\t<link rel=\"stylesheet\" href=\"style.css\" type=\"text/css\"/>\n"
	<< "\t</head>\n"
	;
}

std::string booleanToString(bool value)
{
	if (value)
		return _("Yes");

	return _("No");
}
#ifdef SQLITE
const std::string& sqlFilteredString(const std::string &value)
{
	std::string result;

	for (std::string::size_type i = 0; i < value.length(); ++i)
	{
		char character = value[i];

		switch (character)
		{
			case '\'':
				result += "\'\'";

				break;

			case '\"':
				result += "\"\"";

				break;

			default:
				result += character;

				break;
		}
	}

	return result;
}
#endif

}

}
