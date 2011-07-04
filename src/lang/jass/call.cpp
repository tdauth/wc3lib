/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#include "call.hpp"
#include "../../internationalisation.hpp"

namespace wc3lib
{
	
namespace lang
{
	
namespace jass
{
	
#ifdef HTML
const std::string& Call::List::htmlCategoryName() const
{
	return _("Calls");
}

const std::string& Call::List::htmlFolderName() const
{
	return "calls";
}
#endif
#ifdef SQLITE
const std::string& Call::List::sqlTableName() const
{
	return "Calls";
}

std::size_t Call::List::sqlColumns() const
{
	
	return Object::List::sqlColumns() + 2 + 2 * sqlMaxArguments;
}

const std::string& Call::List::sqlColumnDataType(std::size_t column) const throw (class Exception)
{
	if (column == 4)
		return "VARCHAR(256)";
	else if (column == 5)
		return "INTEGER";
	else if (column >= 6 && column <= 5 + Call::sqlMaxArguments)
		return "VARCHAR(256)";
	else if (column >= 6 + Call::sqlMaxArguments && column <= 6 + 2 * Call::sqlMaxArguments - 1)
		return "INTEGER";
	
	return Object::List::sqlColumnDataType(column);
}

const std::string& Call::List::sqlColumnName(std::size_t column) const throw (class Exception)
{
	if (column == 4)
		return "FunctionIdentifier";
	else if (column == 5)
		return "Function";
	else if (column >= 6 && column <= 5 + sqlMaxArguments)
	{
		std::istringstream isstream("ArgumentIdentifier");
		isstream << column - 5;
		
		return isstream.str();
	}
	else if (column >= 6 + sqlMaxArguments && column <= 6 + 2 * Call::sqlMaxArguments - 1)
	{
		std::istringstream isstream("Argument");
		isstream << column - (5 + sqlMaxArguments);
		
		return isstream.str();
	}
	
	return Object::List::sqlColumnName(column);
}
#endif

#ifdef SQLITE
const std::size_t Call::sqlMaxArguments = 20;
#endif

Call::Call(class Object::List *list, const std::string &identifier, class SourceFile *sourceFile, std::size_t line, const std::string &functionIdentifier, const std::list<std::string> &argumentIdentifiers) : Object(list, identifier, sourceFile, line), m_functionIdentifier(functionIdentifier), m_function(0), m_argumentIdentifiers(argumentIdentifiers)
{
	for (std::size_t i = 0; i < argumentIdentifiers.size(); ++i)
		this->m_arguments.push_back(0);
}

#ifdef SQLITE
Call::Call(std::vector<Object::SqlValueDataType> &columnVector) : Object(columnVector)
{
}
#endif

void Call::init()
{
	this->m_function = static_cast<class Function*>(this->searchObjectInList(this->m_functionIdentifier, Parser::Functions));
	
	if (this->m_function == 0)
		this->m_function = static_cast<class Function*>(this->searchObjectInList(this->m_functionIdentifier, Parser::Methods));
	
	if (this->m_function != 0)
		this->m_functionIdentifier.clear();
	
	std::list<std::string>::const_iterator argumentIdentifier = this->m_argumentIdentifiers.begin();
	std::list<class Object*>::const_iterator argument = this->m_arguments.begin();
	
	while (argumentIdentifier != this->m_argumentIdentifiers.end())
	{
		class Object *object = this->searchObjectInList(*argumentIdentifier, Parser::Locals);
		
		/// @todo Call arguments can be many different Object child classes.
		
		if (object != 0)
			*argument = object;
		
		++argumentIdentifier;
		++argument;
	}
}

#ifdef SQLITE
const std::string& Call::sqlValue(std::size_t column) const
{
	if (column == 4)
		return sqlFilteredString(this->m_functionIdentifier);
	else if (column == 5)
		return Object::sqlObjectId(this->m_function);
	else if (column >= 6 && column <= 5 + sqlMaxArguments)
	{
		std::list<std::string>::const_iterator argumentIdentifier = this->m_argumentIdentifiers.begin();
		
		for (std::size_t i = 0; argumentIdentifier != this->m_argumentIdentifiers.end() && i < Call::sqlMaxArguments; ++argumentIdentifier, ++i)
		{
			if (column == i + 6)
				return sqlFilteredString(*argumentIdentifier);
		}
		
		return "";
	}
	else if (column >= 6 + sqlMaxArguments && column <= 6 + 2 * Call::sqlMaxArguments - 1)
	{
		std::list<class Object*>::const_iterator argument = this->m_arguments.begin();
		
		for (std::size_t i = 0; argument != this->m_arguments.end() && i < Call::sqlMaxArguments; ++argument, ++i)
		{
			if (column == i + 6 + Call::sqlMaxArguments)
				return Object::sqlObjectId(*argument);
		}
		
		return "";
	}
	
	return Object::sqlValue(column);
}
#endif

#ifdef HTML
void Call::writeHtmlPageNavigation(std::ostream &ostream) const
{
	ostream
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Function\">"		<< _("Function") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Arguments\">"		<< _("Arguments") << "</a></li>\n"
	;
}

void Call::writeHtmlPageContent(std::ostream &ostream) const
{
	ostream
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << "\n"
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Function\">" << _("Function") << "</a></h2>\n"
	<< "\t\t" << Object::objectLink(this->function()) << '\n'
	;
	
	if (!this->m_argumentIdentifiers.empty())
	{
		ostream << "\t\t<ul>\n";
		std::list<std::string>::const_iterator argumentIdentifier = this->m_argumentIdentifiers.begin();
		std::list<class Object*>::const_iterator argument = this->m_arguments.begin();
		
		do
		{
			ostream << "\t\t\t<li>" << Object::objectLink(*argument, argumentIdentifier) << "</li>\n";
			
			++argumentIdentifier;
			++argument;
		}
		while (argumentIdentifier != this->m_argumentIdentifiers.end());
		
		ostream << "\t\t</ul>" << std::endl;
	}
	else
		ostream << "\t\t<p>-</p>" << std::endl;
}
#endif

}

}

}
