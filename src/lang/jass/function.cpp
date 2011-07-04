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

#include "function.hpp"

namespace wc3lib
{
	
namespace lang
{
	
namespace jass
{

Function::Function(const std::string &identifier, class SourceFile *sourceFile, std::size_t line, const std::string &returnTypeExpression, std::list<std::string> parameterTypeExpressions, std::list<std::string> parameterIdentifiers) : Object(identifier, sourceFile, line), m_returnTypeExpression(returnTypeExpression), m_returnType(0), m_parameterTypeExpressions(parameterTypeExpressions), m_parameterIdentifiers(parameterIdentifiers)
{
}

#ifdef SQLITE		
Function::Function(std::vector<Object::SqlValueDataType> &columnVector) : Object(columnVector)
{
}
#endif

void Function::init()
{
}

#ifdef SQLITE
std::string Function::sqlValue(std::size_t column) const
{
}
#endif
#ifdef HTML
void Fnction::writeHtmlPageNavigation(std::ostream &ostream) const
{
}

void Function::writeHtmlPageContent(std::ostream &ostream) const
{
}
#endif

}

}

}
