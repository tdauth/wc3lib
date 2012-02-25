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
#include <iostream> //debug

#include "objects.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

#ifdef SQLITE
const char *Implementation::sqlTableName = "Implementations";
std::size_t Implementation::sqlColumns;
std::string Implementation::sqlColumnStatement;

void Implementation::initClass()
{
	Implementation::sqlColumns = Object::sqlColumns + 3;
	Implementation::sqlColumnStatement = Object::sqlColumnStatement +
	",Container INT,"
	"Module INT,"
	"IsOptional BOOLEAN";
}
#endif

Implementation::Implementation(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Object *container, const std::string &moduleExpression, bool isOptional) : Object(identifier, sourceFile, line, docComment), m_container(container), m_moduleExpression(moduleExpression), m_isOptional(isOptional)
{
}

#ifdef SQLITE
Implementation::Implementation(std::vector<const unsigned char*> &columnVector) : Object(columnVector), m_container(0)
{
	this->prepareVector();
}
#endif

void Implementation::init()
{
	//std::cout << "Before search of " << this->moduleExpression() << std::endl;
	this->m_module = static_cast<class Module*>(this->searchObjectInList(this->moduleExpression(), Parser::Modules));
	//std::cout << "Searching for module " << this->moduleExpression() << std::endl;

	if (this->m_module != 0)
		this->m_moduleExpression.clear();
}

void Implementation::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Container\">"		<< _("Container") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Module\">"		<< _("Module") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Optional\">"		<< _("Optional") << "</a></li>\n"
	;
}

void Implementation::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Container\">" << _("Container") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->container()) << '\n'
	<< "\t\t<h2><a name=\"Module\">" << _("Module") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->module(), this->moduleExpression()) << '\n'
	<< "\t\t<h2><a name=\"Optional\">" << _("Optional") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isOptional()) << '\n'
	;
}

#ifdef SQLITE
std::string Implementation::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< Object::sqlStatement() << ", "
	<< "Container=" << Object::objectId(this->container()) << ", "
	<< "Module=" << Object::objectId(this->module()) << ", "
	<< "IsOptional=" << this->isOptional();

	return sstream.str();
}
#endif

class Object* Implementation::container() const
{
	return this->m_container;
}

}
