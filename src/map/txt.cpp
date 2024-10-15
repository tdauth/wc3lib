/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include <sstream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "txt.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize Txt::read(InputStream &istream)
{
	std::streamsize size = 0;
	std::string line;
	Section section;
	bool hasSection = false;
	int l = 1;

	while (std::getline(istream, line)) {
		size += line.length();

		// remove BOM marker
		if (l == 1 && line.length() >= 3 && line[0] == '\xEF' && line[1] == '\xBB' && line[2] == '\xBF') {
			line = line.substr(3);
		}

		// remove comment
		std::size_t i = line.find("/"); // there seem to be broken comments like "/ Stuffed Penguin" in ItemFunc.txt so accept / instead of //

		if (i != std::string::npos) {
			line = line.substr(0, i);
		}

		// remove spaces after removing comment
		boost::trim_if(line, boost::is_any_of(" ,\n,\r,\t"));

		if (line.empty()) {
			// skip empty
		} else if (line.starts_with('[')) {
			if (hasSection) {
				m_sections.push_back(std::move(section));
			}

			section = Section();
			boost::trim_if(line, boost::is_any_of("[,]")); // remove the brackets
			section.name = line;
			hasSection = true;
		} else {
			if (!hasSection) { // "Purchase Medusa Pebble" appears as single line in ItemStrings.txt and should simply be ignored
				std::cerr << boost::format(_("Entry without section at line %1%: %2%.")) % l % line << std::endl;
			} else {
				std::vector<string> r;
				boost::split(r, line, boost::is_any_of("="));

				if (r.size() < 2) {
					std::cerr << boost::format(_("Invalid entry at line %1% with length %2%: %3%.")) % l % line.size() % line << std::endl;
				} else {
					string key = r[0];
					string value = r[1];
					boost::trim(key);
					boost::trim(value);

					section.entries.push_back(Entry(std::move(key), std::move(value)));
				}
			}
		}

		++l;
	}

	if (hasSection) {
		m_sections.push_back(std::move(section));
	}

	return size;
}

std::streamsize Txt::write(OutputStream &ostream, bool useSpaces) const
{
	std::streamsize size = 0;
	const string space = useSpaces ? " " : "";
	string newLine = "";

	for (auto ref : sections())
	{
		string sectionName = newLine + "[" + ref.name + "]";
		newLine = "\n";
		wc3lib::writeString(ostream, sectionName, size);

		for (auto keyValuePair : ref.entries)
		{
			string keyValueEntry = newLine + keyValuePair.key() + space + "=" + space + keyValuePair.value();
			wc3lib::writeString(ostream, keyValueEntry, size);
		}
	}

	return size;
}

}

}
