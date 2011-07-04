/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#include "comment.hpp"
#include "../../internationalisation.hpp"

namespace wc3lib
{
	
namespace lang
{
	
namespace jass
{
	
#ifdef HTML
const std::string& Comment::List::htmlCategoryName() const
{
	return _("Comments");
}

const std::string& Comment::List::htmlFolderName() const
{
	return "comments";
}
#endif

#ifdef SQLITE
const std::string& Comment::List::sqlTableName() const
{
	return "Comments";
}

std::size_t Comment::List::sqlColumns() const
{
	return Object::List::sqlColumns();
}

const std::string& Comment::List::sqlColumnDataType(std::size_t column) const throw (std::exception)
{
	return Object::List::sqlColumnDataType(column);
}

const std::string& Comment::List::sqlColumnName(std::size_t column) const throw (std::exception)
{
	return Object::List::sqlColumnName(column);
}
#endif

Comment::Comment(const std::string &identifier, class SourceFile *sourceFile, std::size_t line) : Object(identifier, sourceFile, line)
{
}

#ifdef SQLITE
Comment::Comment(std::vector<Object::SqlValueDataType> &columnVector) : Object(columnVector)
{
}
#endif

Comment::~Comment()
{
}

void Comment::init()
{
}

#ifdef SQLITE
const std::string& Comment::sqlValue(std::size_t column) const
{
	return Object::sqlValue(column);
}
#endif

void Comment::writeHtmlPageNavigation(std::ostream &ostream) const
{
	ostream
	<< "\t\t\t<li><a href=\"#Text\">"		<< _("Text") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	;
}

void Comment::writeHtmlPageContent(std::ostream &ostream) const
{
	ostream
	<< "\t\t<h2><a name=\"Text\">" << _("Text") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << this->identifier() << "\n"
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << "\n"
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	;
}

}

}

}
