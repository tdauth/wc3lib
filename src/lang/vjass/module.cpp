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
const char *Module::sqlTableName = "Modules";
unsigned int Module::sqlColumns;
std::string Module::sqlColumnStatement;

void Module::initClass()
{
	Module::sqlColumns = Interface::sqlColumns;
	Module::sqlColumnStatement = Interface::sqlColumnStatement;
}
#endif

Module::Module(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate) : Interface(identifier, sourceFile, line, docComment, library, scope, isPrivate)
{
}

#ifdef SQLITE
Module::Module(std::vector<const unsigned char*> &columnVector) : Interface(columnVector)
{
}
#endif

void Module::pageNavigation(std::ofstream &file) const
{
	Interface::pageNavigation(file);
}

void Module::page(std::ofstream &file) const
{
	Interface::page(file);
}

}
