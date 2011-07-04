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
const char *TextMacro::sqlTableName = "TextMacros";
unsigned int TextMacro::sqlColumns;
std::string TextMacro::sqlColumnStatement;

void TextMacro::initClass()
{
	TextMacro::sqlColumns = Object::sqlColumns + 2;
	TextMacro::sqlColumnStatement = Object::sqlColumnStatement +
	",IsOnce BOOLEAN"
	",Parameters VARCHAR(255)";
}
#endif

TextMacro::TextMacro(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, bool isOnce, const std::string &parameters) : Object(identifier, sourceFile, line, docComment), m_isOnce(isOnce), m_parameters(parameters)
{
}

#ifdef SQLITE
TextMacro::TextMacro(std::vector<const unsigned char*> &columnVector) : Object(columnVector)
{
}
#endif

void TextMacro::init()
{
}

void TextMacro::pageNavigation(std::ofstream &file) const
{
	file
	<< "\t\t\t<li><a href=\"#Description\">"	<< _("Description") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Source File\">"	<< _("Source File") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Once\">"		<< _("Once") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Parameters\">"		<< _("Parameters") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Instances\">"		<< _("Instances") << "</a></li>\n"
	;
}

void TextMacro::page(std::ofstream &file) const
{
	file
	<< "\t\t<h2><a name=\"Description\">" << _("Description") << "</a></h2>\n"
	<< "\t\t<p>\n"
	<< "\t\t" << Object::objectPageLink(this->docComment()) << "\n"
	<< "\t\t</p>\n"
	<< "\t\t<h2><a name=\"Source File\">" << _("Source File") << "</a></h2>\n"
	<< "\t\t" << SourceFile::sourceFileLineLink(this) << '\n'
	<< "\t\t<h2><a name=\"Once\">" << _("Once") << "</a></h2>\n"
	<< "\t\t" << Object::showBooleanProperty(this->isOnce()) << '\n'
	<< "\t\t<h2><a name=\"Parameters\">" << _("Parameters") << "</a></h2>\n"
	<< "\t\t" << this->parameters() << '\n'
	<< "\t\t<h2><a name=\"Instances\">" << _("Instances") << "</a></h2>\n"
	;
	std::list<class Object*> instanceList = Vjassdoc::parser()->getSpecificList(Parser::TextMacroInstances, TextMacroInstance::UsesTextMacro(), this);
	
	if (!instanceList.empty())
	{
		file << "\t\t<ul>\n";
	
		for (std::list<class Object*>::iterator iterator = instanceList.begin(); iterator != instanceList.end(); ++iterator)
		{
			class TextMacroInstance *instance = static_cast<class TextMacroInstance*>(*iterator);
		
			file << "\t\t\t<li>";
			file << Object::objectPageLink(instance->sourceFile()) << " - " << Object::objectPageLink(instance);
			file << "</li>\n";
		}
		
		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";
}

#ifdef SQLITE
std::string TextMacro::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< Object::sqlStatement() << ", "
	<< "IsOnce=" << this->isOnce() << ", "
	<< "Parameters=\"" << Object::sqlFilteredString(this->parameters()) << '\"';

	return sstream.str();
}
#endif

}
