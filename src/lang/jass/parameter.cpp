/***************************************************************************
 *   Copyright (C) 2008, 2009 by Tamino Dauth                              *
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

#include "objects.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

#ifdef SQLITE
const char *Parameter::sqlTableName = "Parameters";
unsigned int Parameter::sqlColumns;
std::string Parameter::sqlColumnStatement;

void Parameter::initClass()
{
	Parameter::sqlColumns = Object::sqlColumns + 3;
	Parameter::sqlColumnStatement = Object::sqlColumnStatement +
	",FunctionInterface INT,"
	"TypeExpression VARCHAR(50),"
	"Type INT";
}
#endif

Parameter::Parameter(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class FunctionInterface *functionInterface, const std::string &typeExpression) : Object(identifier, sourceFile, line, docComment), m_functionInterface(functionInterface), m_typeExpression(typeExpression), m_type(0)
{
}

#ifdef SQLITE
Parameter::Parameter(std::vector<const unsigned char*> &columnVector) : Object(columnVector), m_functionInterface(0), m_type(0)
{
}
#endif

void Parameter::init()
{
	//Must not be empty.
	this->m_type = this->searchObjectInList(this->typeExpression(), Parser::Types);
	
	if (this->m_type == 0)
		this->m_type = this->searchObjectInList(this->typeExpression(), Parser::Interfaces);
	
	if (this->m_type == 0)
		this->m_type = this->searchObjectInList(this->typeExpression(), Parser::Structs);
	
	if (this->m_type != 0)
		this->m_typeExpression.clear();
}

void Parameter::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Function Interface\">"	<< _("Function Interface") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Type\">"		<< _("Type") << "</a></li>\n"
	;
}

void Parameter::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Function Interface\">" << _("Function Interface") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->functionInterface()) << "\n"
	<< "\t\t<h2><a name=\"Type\">" << _("Type") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->type(), this->typeExpression()) << "\n"
	;
}

#ifdef SQLITE
std::string Parameter::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< Object::sqlStatement() << ", "
	<< "FunctionInterface=" << Object::objectId(this->functionInterface()) << ", "
	<< "TypeExpression=\"" << this->typeExpression() << "\", "
	<< "Type=" << Object::objectId(this->type());
	
	return sstream.str();
}
#endif

}
