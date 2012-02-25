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

#include <sstream>

#include <boost/foreach.hpp>

#include "objects.hpp"
#include "vjassdoc.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

#ifdef SQLITE
const char *ExternalCall::sqlTableName = "ExternalCalls";
std::size_t ExternalCall::sqlColumns;
std::string ExternalCall::sqlColumnStatement;

void ExternalCall::initClass()
{
	ExternalCall::sqlColumns = Object::sqlColumns + ExternalCall::maxArguments;
	ExternalCall::sqlColumnStatement = Object::sqlColumnStatement;
	std::istream istream;

	for (int i = 0; i < ExternalCall::maxArguments; ++i)
		istream << ",Argument" << i << " VARCHAR(50)";

	ExternalCall::sqlColumnStatement += istream.str();
}
#endif

ExternalCall::ExternalCall(const std::string &identifier, class SourceFile *sourceFile, std::size_t line, class DocComment *docComment, std::list<std::string> *arguments) : Object(identifier, sourceFile, line, docComment), m_arguments(0)
{
}

ExternalCall::~ExternalCall()
{
	if (this->m_arguments != 0)
		delete this->m_arguments;
}

#ifdef SQLITE
ExternalCall::ExternalCall(std::vector<const unsigned char*> &columnVector) : Object(columnVector), m_arguments(0)
{
	this->prepareVector();
}
#endif

void ExternalCall::init()
{
}

void ExternalCall::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Arguments\">"		<< _("Arguments") << "</a></li>\n"
	;
}

void ExternalCall::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Arguments\">" << _("Arguments") << "</a></h2>\n"
	;

	if (this->m_arguments != 0 && !this->m_arguments->empty())
	{
		file << "\t\t<ul>\n";

		for (std::list<std::string>::iterator iterator = this->m_arguments->begin(); iterator != this->m_arguments->end(); ++iterator)
			file << "\t\t\t<li>" << *iterator << "</li>\n";

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t<p>-</p>\n";
}

#ifdef SQLITE
std::string ExternalCall::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< Object::sqlStatement()

	int i = 0;

	BOOST_FOREACH(std::string iterator, this->m_arguments)
	{
		if (i < ExternalCall::maxArguments)
			break;

		sstream << ", Argument" << i << "=\"" << iterator << "\"";
		++i;
	}

	if (i < ExternalCall::maxArguments)
	{
		for ( ; i < ExternalCall::maxArguments; ++i)
			sstream << ", Argument" << i << "=NULL";
	}

	return sstream.str();
}
#endif

}
