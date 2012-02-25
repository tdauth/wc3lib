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

#include "objects.hpp"
#include "internationalisation.hpp"

namespace vjassdoc
{

#ifdef SQLITE
const char *Hook::sqlTableName = "Hooks";
std::size_t Hook::sqlColumns;
std::string Hook::sqlColumnStatement;

void Hook::initClass()
{
	Hook::sqlColumns = Object::sqlColumns + 4;
	Hook::sqlColumnStatement = Object::sqlColumnStatement +
	",Function INT"
	",FunctionExpression VARCHAR(255)"
	",HookFunction INT"
	",HookFunctionExpression VARCHAR(255)";
}
#endif

Hook::Hook(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, const std::string &functionExpression, const std::string &hookFunctionExpression) : Object(identifier, sourceFile, line, docComment), m_function(0), m_functionExpression(functionExpression), m_hookFunction(0), m_hookFunctionExpression(hookFunctionExpression)
{
}

#ifdef SQLITE
Hook::Hook(std::vector<const unsigned char*> &columnVector) : Object(columnVector), m_function(0), m_hookFunction(0)
{
	this->prepareVector();
}
#endif

Hook::~Hook()
{
}

void Hook::init()
{
	this->m_function = static_cast<class Function*>(this->searchObjectInList(this->functionExpression(), Parser::Functions));

	if (this->m_function == 0)
		this->m_function = static_cast<class Function*>(this->searchObjectInList(this->functionExpression(), Parser::Methods));

	this->m_hookFunction = static_cast<class Function*>(this->searchObjectInList(this->hookFunctionExpression(), Parser::Functions));

	if (this->m_hookFunction == 0)
		this->m_hookFunction = static_cast<class Function*>(this->searchObjectInList(this->hookFunctionExpression(), Parser::Methods));
}

void Hook::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Function\">"		<< _("Function") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Hook Function\">"		<< _("Hook Function") << "</a></li>\n"
	;
}

void Hook::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << "\n"
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Function\">" << _("Function") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->function(), this->functionExpression()) << "\n"
	<< "\t\t<h2><a name=\"Hook Function\">" << _("Hook Function") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->hookFunction(), this->hookFunctionExpression()) << "\n"
	;
}

#ifdef SQLITE
std::string Hook::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< Object::sqlStatement() << ", "
	<< "Function=" << Object::objectId(this->function()) << ", "
	<< "FunctionExpression=\"" << this->functionExpression() << "\", "
	<< "HookFunction=" << Object::objectId(this->hookFunction()) << ", "
	<< "HookFunctionExpression=\"" << this->functionExpression() << "\"";

	return sstream.str();
}
#endif

}
