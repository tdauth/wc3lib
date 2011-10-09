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

#include <list>

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/spirit.hpp>

#include "strings.hpp"
#include "string.hpp"
#include "../internationalisation.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace map
{

/// Returns true if a string has been found.
static bool getNextTranslationFunctionCall(const std::string &line, const std::list<std::string> &translationFunctions, string::size_type &position, std::string::size_type &length, const bool ignoreReplacedValues = true)
{
	BOOST_FOREACH(const std::string &translationFunction, translationFunctions)
	{
		std::string searchedString = translationFunction + "(\"";
		position = line.find(searchedString, position);

		if (position == std::string::npos)
			continue;

		position += searchedString.length();

		if (position >= line.length())
			continue;

		do
		{
			length = line.find("\"", position);

			if (length == std::string::npos)
				continue;
			else if (line[length - 1] == '\\') // escape sequence
				position = length + 1;
			else
				break;
		}
		while (true);

		length = length - position;

		static const char *replacedValueSchema = "STRING_";

		if (ignoreReplacedValues && line.substr(position, strlen(replacedValueSchema)) == replacedValueSchema)
			continue;

		return true;
	}

	return false;
}

static bool checkForStringConflict(Strings::StringList &strings, class String *string, Strings::ConflictFunction conflictFunction = 0)
{
	bool result = false;

	BOOST_FOREACH(Strings::StringListValue value, strings)
	{
		if (value.second->defaultString() == string->defaultString())
		{
			// use old string
			if (conflictFunction != 0)
			{
				BOOST_SCOPED_ENUM(Strings::ConflictResult) result = conflictFunction(*value.second, *string);

				switch (result)
				{
					case Strings::ConflictResult::UseBoth:
						break;

					case Strings::ConflictResult::UseFirst:
						delete string;

						break;

					case Strings::ConflictResult::UseSecond:
						strings.erase(value.first);
						delete value.second;
						strings.insert(std::make_pair(string->id(), string));

						break;
				}
			}

			result = true;
			break;
		}
	}

	strings.insert(std::make_pair(string->id(), string));

	return false;
}

Strings::Strings()
{
}

Strings::~Strings()
{
}

std::pair<std::streamsize, std::streamsize> Strings::parse(const boost::filesystem::path &path, std::basic_istream<byte> &istream, std::basic_ostream<byte> *ostream, const bool replace, const bool fill, const bool ignoreReplacedValues, const std::list<string> &translationFunctions, ConflictFunction conflictFunction) throw (class Exception)
{
	string line;
	std::streamsize gSize = 0;
	std::streamsize pSize = 0;

	//  && !eof(istream)
	for (std::size_t i = 0; !eof(istream) && readLine(istream, line, gSize); ++i)
	{
		string::size_type position = 0;
		string::size_type length = 0;
		std::cout << "End position " << endPosition(istream) << " and current position " << istream.tellg() << std::endl;
		std::cout << "Line gna " << i << std::endl;

		while (getNextTranslationFunctionCall(line, translationFunctions, position, length, ignoreReplacedValues))
		{
			//std::cout << "TRANSLATION CALL " << i << std::endl;
			string defaultString = line.substr(position, length);
			String::IdType id = 0;
			bool exists = false;
			String::IdType j = 0;
			StringListValue existing;

			BOOST_FOREACH(StringListValue value, this->m_stringList)
			{
				if (value.second->defaultString() == defaultString)
				{
					id = j;
					exists = true;
					existing = value;

					break;
				}

				++j;
			}

			// check if id is already used
			if (!exists)
			{

				// find free id otherwise take next.
				do
				{
					if (this->m_stringList.find(id) == this->m_stringList.end())
						break;

					++id;
				}
				while (true);

				stringstream sstream;
				sstream << "STRING_" << id;
				string valueString = fill ? defaultString : "";
				class String *string = new String(sstream.str(), defaultString, valueString);
				string->addUsage(path, i);
				this->m_stringList.insert(std::make_pair(id, string));

				if (replace)
					line.replace(position, length, sstream.str());
			}
			else if (replace)
				line.replace(position, length, existing.second->idString());
		}

		if (replace && ostream)
			writeLine(*ostream, line, pSize);
	}

	return std::make_pair(gSize, pSize);
}

std::streamsize Strings::read(InputStream &istream) throw (class Exception)
{
	/*
	 * TODO finish
	InputStream::iterator first = istream.begin();
	InputStream::iterator last = istream.end();

	using namespace boost;
	using namespace boost::spirit::qi;
	using namespace boost::phoenix;
	using namespace boost::spirit::qi::standard;

	//boost::spirit::qi::grammar grammar(istream, "");
	int id = 0;
	std::string defaultString;
	std::string value;
	stream_parser parser;
	// TODO consider new lines, >> should only skip "space" characters which means tabs and other white spaces
	parser.parse(first, last,
		(
			string_("STRING_") > int_[ref(id) = _1]
			>> char_('{')
			>> string_("//") >> string_[ref(defaultString) = _1]
			>> char_('"') > string_[ref(value) = _1] > char_('"')
			>> char_('}')
		),
		space
	);

	if (first != last)
		throw Exception(_("Parsing error."));
	*/

	/*
	string line;
	std::streamsize size = 0;

	for (std::size_t i = 0; readLine(istream, line, size); ++i)
	{
		string::size_type position = line.find("STRING");

		if (line.substr(0, 6) != "STRING")
			continue;

		string idString = line.substr(0, 6) + "_" + line.substr(7);
		++i;

		if (!readLine(istream, line, size))
			throw Exception(boost::format(_("Error at line %1%.")) % i);

		while (line.empty() || line.substr(0, 2) != "//")
		{
			++i;

			if (!readLine(istream, line, size))
				throw Exception(boost::format(_("Error at line %1%.")) % i);
		}

		if (line.length() >= 2 && line.substr(0, 2) == "//")
		{
			string defaultString = line.length() < 4 ? "" : line.substr(3);

			if (readLine(istream, line, size) && ++i && line == "{" && readLine(istream, line, size) && ++i)
			{
				string valueString = line;
				class String *string = new String(idString, defaultString, line);
				checkForStringConflict(this->m_stringList, string);
			}
			else
				throw Exception(boost::format(_("Error at line %1%: Missing value string line or { character line.")) % i);
		}
		else
			throw Exception(boost::format(_("Error at line %1%: Missing default string line.")) % i);
	}

	return size;
	*/
}

std::streamsize Strings::write(std::basic_ostream<byte> &ostream) const throw (class Exception)
{
	std::streamsize size = 0;

	BOOST_FOREACH(StringListValueConst value, this->m_stringList)
	{
		ostringstream sstream;
		sstream
		<< "STRING " << value.second->id() << '\n'
		<< "// " << value.second->defaultString() << '\n'
		<< "{\n"
		<< value.second->valueString() << '\n'
		<< "}\n"
		;
		writeLine(ostream, sstream.str(), size);
	}

	return size;
}

/// @todo Add better is in comment etc. support.
std::streamsize Strings::readFdf(std::basic_istream<byte> &istream) throw (class Exception)
{
	bool foundEndCharacter = false;
	bool isInComment = false;
	string line;
	std::streamsize size = 0;

	for (std::size_t i = 0; readLine(istream, line, size); ++i)
	{
		if (line.empty())
			continue;

		boost::tokenizer<> tokenizer(line);
		boost::tokenizer<>::iterator iterator = tokenizer.begin();

		if (iterator != tokenizer.end())
		{
			if (*iterator == "StringList")
			{
				++iterator;

				/// @todo Missing { character ________|________
				if (iterator == tokenizer.end() || *iterator != "{")
					throw Exception(boost::format(_("Error at line %1%: Missing \"{\" character.\n")) % i);
			}
			else if (*iterator == "}") // end string list
			{
				foundEndCharacter = true;

				break;
			}
			else if ((*iterator).length() > 7 && (*iterator).substr(0, 7)  == "STRING_")
			{
				//std::cout << "Length is bigger than 7." << std::endl;
				string idString = *iterator;
				string::size_type index = line.find_last_of("/*");

				if (index == string::npos)
					continue;

				isInComment = true;
				index += 3;
				string::size_type length = line.find_last_of("*/", index);

				if (length == string::npos)
					continue;

				isInComment = false;
				length -= index + 3;
				string defaultString = line.substr(index, length);

				if (defaultString.empty())
					continue;

				index = line.find('"');

				if (index == string::npos)
					continue;

				index += 2;
				length = line.find("\"", index);

				if (length == string::npos)
					continue;

				length -= index + 2;
				string valueString = line.substr(index, length);
				class String *string = new String(idString, defaultString, valueString);
				checkForStringConflict(this->m_stringList, string);
			}
		}
	}

	if (!foundEndCharacter)
		throw Exception(_("Warning: Did not find closing \"}\" character."));

	return size;
}

std::streamsize Strings::writeFdf(std::basic_ostream<byte> &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeLine(ostream, "StringList {", size);

	std::size_t i = 0;

	BOOST_FOREACH(const StringListValueConst value, this->m_stringList)
	{
		string string("\t");
		string.append(value.second->idString()).append("\t\t\t\"").append(value.second->valueString()).append("\"");

		if (i != this->m_stringList.size() - 1)
			string.append(",");

		string.append(" /* ").append(value.second->defaultString()).append(" */");
		writeLine(ostream, string, size);
		++i;
	}

	writeLine(ostream, "}", size);

	return size;
}

void Strings::list(std::basic_ostream<byte> &ostream) const
{
	BOOST_FOREACH(StringListValueConst value, this->m_stringList)
		ostream << boost::format(_("- %1%\t\t%2%")) % value.second->idString() % value.second->defaultString() << std::endl;
}

}

}
