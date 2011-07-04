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
const char *Scope::sqlTableName = "Scopes";
unsigned int Scope::sqlColumns;
std::string Scope::sqlColumnStatement;

void Scope::initClass()
{
	Scope::sqlColumns = Object::sqlColumns + 3;
	Scope::sqlColumnStatement = Object::sqlColumnStatement +
	",Library INT,"
	"IsPrivate BOOLEAN,"
	"Initializer INT";
}
#endif

Scope::Scope(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, bool isPrivate, const std::string initializerExpression) : Object(identifier, sourceFile, line, docComment), initializerExpression(initializerExpression), m_library(library), m_isPrivate(isPrivate), m_initializer(0)
{
}

#ifdef SQLITE
Scope::Scope(std::vector<const unsigned char*> &columnVector) : Object(columnVector), m_library(0), m_initializer(0)
{
}
#endif

void Scope::init()
{
	if (!this->initializerExpression.empty())
	{
		this->m_initializer = static_cast<Function*>(this->searchObjectInList(this->initializerExpression, Parser::Functions));
		
		if (this->m_initializer == 0)
			this->m_initializer = static_cast<Function*>(this->searchObjectInList(this->initializerExpression, Parser::Methods));
		
		if (this->m_initializer != 0)
			this->initializerExpression.clear();
	}
	else
		this->initializerExpression = '-';
}

void Scope::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"			<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"			<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Library\">"				<< _("Library") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Private\">"				<< _("Private") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Initializer\">"			<< _("Initializer") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Keywords\">"				<< _("Keywords") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Text Macros\">"			<< _("Text Macros") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Text Macro Instances\">"		<< _("Text Macro Instances") << "</a></li>"
	<< "\t\t\t<li><a href=\"#Types\">"				<< _("Types") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Globals\">"				<< _("Globals") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Function Interfaces\">"		<< _("Function Interfaces") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Functions\">"				<< _("Functions") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Interfaces\">"				<< _("Interfaces") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Structs\">"				<< _("Structs") << "</a></li>\n"
	;
}

void Scope::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Container") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << '\n'
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Library\">" << _("Library") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->library()) << '\n'
	<< "\t\t<h2><a name=\"Private\">" << _("Private") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isPrivate()) << '\n'
	<< "\t\t<h2><a name=\"Initializer\">" << _("Initializer") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->initializer(), this->initializerExpression) << '\n'
	<< "\t\t<h2><a name=\"Keywords\">" << _("Keywords") << "</a></h2>\n"
	;
	
	std::list<class Object*> list = Vjassdoc::parser()->getSpecificList(Parser::Keywords, Object::IsInScope(), this);
	
	if (!list.empty())
	{
		file << "\t\t<ul>\n";
	
		for (std::list<class Object*>::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			file << "\t\t\t<li>" << Object::objectPageLink(*iterator) << "</li>\n";
		
		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
	
	file
	<< "\t\t<h2><a name=\"Text Macros\">" << _("Text Macros") << "</a></h2>\n"
	;
	
	list = Vjassdoc::parser()->getSpecificList(Parser::TextMacros, Object::IsInScope(), this);
	
	if (!list.empty())
	{
		file << "\t\t<ul>\n";
	
		for (std::list<class Object*>::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			file << "\t\t\t<li>" << Object::objectPageLink(*iterator) << "</li>\n";
		
		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
	
	file
	<< "\t\t<h2><a name=\"Text Macro Instances\">" << _("Text Macro Instances") << "</a></h2>\n"
	;
	
	list = Vjassdoc::parser()->getSpecificList(Parser::TextMacroInstances, Object::IsInScope(), this);
	
	if (!list.empty())
	{
		file << "\t\t<ul>\n";
	
		for (std::list<class Object*>::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			file << "\t\t\t<li>" << Object::objectPageLink(*iterator) << "</li>\n";
		
		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
	
	file
	<< "\t\t<h2><a name=\"Types\">" << _("Types") << "</a></h2>\n"
	;
	
	list = Vjassdoc::parser()->getSpecificList(Parser::Types, Object::IsInScope(), this);
	
	if (!list.empty())
	{
		file << "\t\t<ul>\n";
	
		for (std::list<class Object*>::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			file << "\t\t\t<li>" << Object::objectPageLink(*iterator) << "</li>\n";
		
		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
	
	file
	<< "\t\t<h2><a name=\"Globals\">" << _("Globals") << "</a></h2>\n"
	;
	
	list = Vjassdoc::parser()->getSpecificList(Parser::Globals, Object::IsInScope(), this);
	
	if (!list.empty())
	{
		file << "\t\t<ul>\n";
	
		for (std::list<class Object*>::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			file << "\t\t\t<li>" << Object::objectPageLink(*iterator) << "</li>\n";
		
		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
	
	file
	<< "\t\t<h2><a name=\"Function Interfaces\">" << _("Function Interfaces") << "</a></h2>\n"
	;
	
	list = Vjassdoc::parser()->getSpecificList(Parser::FunctionInterfaces, Object::IsInScope(), this);
	
	if (!list.empty())
	{
		file << "\t\t<ul>\n";
	
		for (std::list<class Object*>::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			file << "\t\t\t<li>" << Object::objectPageLink(*iterator) << "</li>\n";
		
		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
	
	file
	<< "\t\t<h2><a name=\"Functions\">" << _("Functions") << "</a></h2>\n"
	;
	
	list = Vjassdoc::parser()->getSpecificList(Parser::Functions, Object::IsInScope(), this);
	
	if (!list.empty())
	{
		file << "\t\t<ul>\n";
	
		for (std::list<class Object*>::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			file << "\t\t\t<li>" << Object::objectPageLink(*iterator) << "</li>\n";
		
		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
	
	file
	<< "\t\t<h2><a name=\"Interfaces\">" << _("Interfaces") << "</a></h2>\n"
	;
	
	list = Vjassdoc::parser()->getSpecificList(Parser::Interfaces, Object::IsInScope(), this);
	
	if (!list.empty())
	{
		file << "\t\t<ul>\n";
	
		for (std::list<class Object*>::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			file << "\t\t\t<li>" << Object::objectPageLink(*iterator) << "</li>\n";
		
		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
	
	file
	<< "\t\t<h2><a name=\"Structs\">" << _("Structs") << "</a></h2>\n"
	;
	
	list = Vjassdoc::parser()->getSpecificList(Parser::Structs, Object::IsInScope(), this);
	
	if (!list.empty())
	{
		file << "\t\t<ul>\n";
	
		for (std::list<class Object*>::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			file << "\t\t\t<li>" << Object::objectPageLink(*iterator) << "</li>\n";
		
		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
}

#ifdef SQLITE
std::string Scope::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< Object::sqlStatement() << ", "
	<< "Library=" << Object::objectId(this->library()) << ", "
	<< "IsPrivate=" << this->isPrivate() << ", "
	<< "Initializer=" << Object::objectId(this->initializer());

	return sstream.str();
}
#endif


class Library* Scope::library() const
{
	return this->m_library;
}

}
