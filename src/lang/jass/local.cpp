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
#include "vjassdoc.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

#ifdef SQLITE
const char *Local::sqlTableName = "Locals";
std::size_t Local::sqlColumns;
std::string Local::sqlColumnStatement;

void Local::initClass()
{
	Local::sqlColumns = Object::sqlColumns + 5;
	Local::sqlColumnStatement = Object::sqlColumnStatement +
	",Function INT,"
	"Type INT,"
	"TypeExpression VARCHAR(50),"
	"Value INT,"
	"ValueExpression VARCHAR(50)"
	;
}
#endif

Local::Local(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Function *function, const std::string &typeExpression, const std::string &valueExpression) : Object(identifier, sourceFile, line, docComment), m_function(function), m_type(0), m_typeExpression(typeExpression), m_value(0), m_valueExpression(valueExpression)
{
}

#ifdef SQLITE
Local::Local(std::vector<const unsigned char*> &columnVector) : Object(columnVector), m_function(0), m_type(0), m_value(0)
{
}
#endif

/// @todo Value expressions can be calculations etc..
void Local::init()
{
	//Must not be empty.
	this->m_type = this->searchObjectInList(this->typeExpression(), Parser::Types);
	
	if (this->m_type == 0)
		this->m_type = this->searchObjectInList(this->typeExpression(), Parser::Interfaces);
	
	if (this->m_type == 0)
		this->m_type = this->searchObjectInList(this->typeExpression(), Parser::Structs);
	
	if (this->m_type != 0)
		this->m_typeExpression.clear();
	
	this->m_value = this->findValue(this->type(), this->m_valueExpression);
}

void Local::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Function\">"		<< _("Function") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Type\">"		<< _("Type") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Value\">"		<< _("Value") << "</a></li>\n"
	;
}

void Local::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Function\">" << _("Function") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->function()) << "\n"
	<< "\t\t<h2><a name=\"Type\">" << _("Type") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->type(), this->typeExpression()) << '\n'
	<< "\t\t<h2><a name=\"Value\">" << _("Value") << "</a></h2>\n"
	;
	
	if (this->valueExpression().empty() || this->valueExpression() == "-")
		file << "\t\t" << Object::objectPageLink(this->value(), this->valueExpression()) << '\n';
	else
	{
		file << "\t\t";
		
		if (this->value() != 0)
			file << Object::objectPageLink(this->value());
		
		file << "\t\t" << this->valueExpression() << '\n';
	}
}

#ifdef SQLITE
std::string Local::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< Object::sqlStatement() << ", "
	<< "Function=" << Object::objectId(this->function()) << ", "
	<< "Type=" << Object::objectId(this->type()) << ", "
	<< "TypeExpression=\"" << Object::sqlFilteredString(this->typeExpression()) << "\", "
	<< "Value=" << Object::objectId(this->value()) << ", "
	<< "ValueExpression=\"" << Object::sqlFilteredString(this->valueExpression()) << "\""
	;
	
	std::cout << "statement: " << sstream.str() << std::endl;
	
	return sstream.str();
}
#endif

}
