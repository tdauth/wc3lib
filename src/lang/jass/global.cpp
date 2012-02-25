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

#include <cctype>
#include <sstream>

#include "objects.hpp"
#include "vjassdoc.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

#ifdef SQLITE
const char *Global::sqlTableName = "Globals";
std::size_t Global::sqlColumns;
std::string Global::sqlColumnStatement;

void Global::initClass()
{
	Global::sqlColumns = Object::sqlColumns + 10;
	Global::sqlColumnStatement = Object::sqlColumnStatement +
	",Library INT,"
	"Scope INT,"
	"IsPrivate BOOLEAN,"
	"IsPublic BOOLEAN,"
	"IsConstant BOOLEAN,"
	"Type INT,"
	"Value INT,"
	"ValueLiteral VARCHAR(50),"
	"Size INT,"
	"SizeLiteral INT";
}
#endif

Global::Global(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate, bool isPublic, bool isConstant, const std::string &typeExpression, const std::string &valueExpression, const std::string &sizeExpression) : m_library(library), m_scope(scope), m_isPrivate(isPrivate), m_isPublic(isPublic), m_isConstant(isConstant), m_typeExpression(typeExpression), valueExpression(valueExpression), sizeExpression(sizeExpression), m_type(0), m_value(0), m_size(0), Object(identifier, sourceFile, line, docComment)
{
}

#ifdef SQLITE
Global::Global(std::vector<const unsigned char*> &columnVector) : m_type(0), m_value(0), m_size(0), Object(columnVector)
{
	this->prepareVector();
}
#endif

/// @todo Value expressions can be calculations etc..
void Global::init()
{
	//Must not be empty.
	this->m_type = this->searchObjectInList(this->typeExpression(), Parser::Types);

	//std::cout << "1 ALTA with type identifier " << this->m_type->identifier() << std::endl;

	if (this->m_type == 0)
		this->m_type = this->searchObjectInList(this->typeExpression(), Parser::Interfaces);

	if (this->m_type == 0)
		this->m_type = this->searchObjectInList(this->typeExpression(), Parser::Structs);

	if (this->m_type != 0)
		this->m_typeExpression.clear();

	this->m_value = this->findValue(this->m_type, this->valueExpression);
	this->m_size = this->findValue(Vjassdoc::parser()->integerType(), this->sizeExpression);
}

void Global::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Library\">"		<< _("Library") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Scope\">"		<< _("Scope") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Private\">"		<< _("Private") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Public\">"		<< _("Public") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Constant\">"		<< _("Constant") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Type\">"		<< _("Type") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Value\">"		<< _("Value") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Size\">"		<< _("Size") << "</a></li>\n"
	;
}

void Global::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Library\">" << _("Library") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->library()) << "\n"
	<< "\t\t<h2><a name=\"Scope\">" << _("Scope") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->scope()) << '\n'
	<< "\t\t<h2><a name=\"Private\">" << _("Private") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isPrivate()) << '\n'
	<< "\t\t<h2><a name=\"Public\">" << _("Public") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isPublic()) << '\n'
	<< "\t\t<h2><a name=\"Constant\">" << _("Constant") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isConstant()) << '\n'
	<< "\t\t<h2><a name=\"Type\">" << _("Type") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->type(), this->typeExpression()) << '\n'
	<< "\t\t<h2><a name=\"Value\">" << _("Value") << "</a></h2>\n"
	;

	if (this->valueExpression.empty() || this->valueExpression == "-")
		file << "\t\t" << Object::objectPageLink(this->value(), this->valueExpression) << '\n';
	else
	{
		file << "\t\t";

		if (this->value() != 0)
			file << Object::objectPageLink(this->value());

		file << "\t\t" << this->valueExpression << '\n';
	}

	file << "\t\t<h2><a name=\"Size\">" << _("Size") << "</a></h2>\n";

	if (this->sizeExpression.empty() || this->sizeExpression == "-")
		file << "\t\t" << Object::objectPageLink(this->size(), this->sizeExpression) << '\n';
	else
	{
		file << "\t\t";

		if (this->size() != 0)
			file << Object::objectPageLink(this->size());

		file << "\t\t" << this->sizeExpression << '\n';
	}
}

#ifdef SQLITE
std::string Global::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< Object::sqlStatement() << ", "
	<< "Library=" << Object::objectId(this->library()) << ", "
	<< "Scope=" << Object::objectId(this->scope()) << ", "
	<< "IsPrivate=" << this->isPrivate() << ", "
	<< "IsPublic=" << this->isPublic() << ", "
	<< "IsConstant=" << this->isConstant() << ", "
	<< "Type=" << Object::objectId(this->type()) << ", "
	<< "Value=" << Object::objectId(this->value()) << ", "
	<< "ValueLiteral=\"" << Object::sqlFilteredString(this->valueLiteral()) << "\", "
	<< "Size=" << Object::objectId(this->size()) << ", "
	<< "SizeLiteral=" << this->sizeLiteral()
	;

	return sstream.str();
}
#endif

class Library* Global::library() const
{
	return this->m_library;
}

class Scope* Global::scope() const
{
	return this->m_scope;
}

}
