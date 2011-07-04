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

#include "version.hpp"

namespace wc3lib
{

namespace mdlx
{

const long32 Version::currentVersion = 800;

Version::Version(class Mdlx *mdlx) : MdxBlock("VERS", "Version", false), m_mdlx(mdlx)
{
}

Version::~Version()
{
}

std::streamsize Version::readMdl(istream &istream) throw (class Exception)
{
	/*
	std::string line;
	bool gotVersion = false;

	while (std::getline(istream, line))
	{
		if (line.empty())
			continue;

		boost::tokenizer<> tokenizer(line);
		boost::tokenizer<>::iterator iterator = tokenizer.begin();

		if (iterator == tokenizer.end())
			throw Exception(_("Version: Missing tokens."));

		if ((*iterator) == "//" || ((*iterator).length() > 2 && (*iterator).substr(0, 2) == "//"))
			continue;
		else if ((*iterator) == "Version")
		{
			std::cout << "Is version " << *iterator << std::endl;
			std::cout << "Line " << line << std::endl;
			++iterator; /// @todo Is not {?!

			if (iterator == tokenizer.end())
				throw Exception(_("Version: Missing tokens."));
			else if ((*iterator) != "{")
				throw Exception(_("Version: Syntax error."));
		}
		else if ((*iterator) == "FormatVersion")
		{
			++iterator;

			if (iterator == tokenizer.end())
				throw Exception(_("Version: Missing tokens."));

			std::stringstream sstream;
			sstream << *iterator;
			sstream >> this->m_version;
			gotVersion = true;
		}
		else if ((*iterator) == "}")
			break;
	}

	if (!gotVersion)
		throw Exception(_("Version: Missing format version number."));
	*/
	return 0;
}

std::streamsize Version::writeMdl(ostream &ostream) const throw (class Exception)
{
	const string result = boost::str(boost::format(
	"// Current FormatVersion is 800\n"
	"Version {\n"
	"\tFormatVersion %1%,\n"
	"}\n")
	% version()
	)
	;
	std::streamsize size = 0;
	wc3lib::writeString(ostream, result, size);
	
	return size;
}

std::streamsize Version::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = MdxBlock::readMdx(istream);
	long32 nbytes;
	wc3lib::read(istream, nbytes, size);

	if (nbytes != sizeof(this->m_version))
		throw Exception(boost::format(_("Versions with more than %1% bytes are not supported. Read version has %1% bytes.")) % sizeof(this->m_version) % nbytes);

	wc3lib::read(istream, this->m_version, size);

	if (this->m_version != Version::currentVersion)
		std::cerr << boost::format(_("Warning: Version %1% is probably not supported. Current version is %2$.")) % this->m_version % Version::currentVersion << std::endl;

	return size;
}

std::streamsize Version::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = MdxBlock::writeMdx(ostream);
	long32 nbytes = sizeof(this->m_version);
	wc3lib::write(ostream, nbytes, size);
	wc3lib::write(ostream, this->m_version, size);

	return size;
}

}

}
