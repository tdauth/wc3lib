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
#include "file.hpp"
#include "internationalisation.hpp"
#include "vjassdoc.hpp"

namespace vjassdoc
{

bool Struct::HasExtension::operator()(const class Object *thisObject, const class Object *extension) const
{
	return static_cast<const class Struct*>(thisObject)->extension() == static_cast<const class Interface*>(extension);
}

#ifdef SQLITE
const char *Struct::sqlTableName = "Structs";
unsigned int Struct::sqlColumns;
std::string Struct::sqlColumnStatement;

void Struct::initClass()
{
	Struct::sqlColumns = Interface::sqlColumns + 7;
	Struct::sqlColumnStatement = Interface::sqlColumnStatement +
	",Size INT,"
	"SizeExpression VARCHAR(255),"
	"Extension INT,"
	"ExtensionExpression VARCHAR(255),"
	"Constructor INT,"
	"Destructor INT,"
	"Initializer INT";
}
#endif

Struct::Struct(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate, const std::string &sizeExpression, const std::string &extensionExpression) : Interface(identifier, sourceFile, line, docComment, library, scope, isPrivate), m_size(0), m_sizeExpression(sizeExpression), m_extension(0), m_extensionExpression(extensionExpression), m_constructor(0), m_destructor(0), m_initializer(0)
{
}

#ifdef SQLITE
Struct::Struct(std::vector<const unsigned char*> &columnVector) : Interface(columnVector), m_size(0), m_extension(0), m_constructor(0), m_destructor(0), m_initializer(0)
{
}
#endif

void Struct::init()
{
	Interface::init();

	if (!this->sizeExpression().empty())
	{
		this->m_size = this->findValue(static_cast<class Object*>(Vjassdoc::parser()->integerType()), this->m_sizeExpression);

		if (this->m_size != 0)
			this->m_sizeExpression.clear();
	}
	else
		this->m_sizeExpression = '-';

	if (!this->m_extensionExpression.empty() && this->m_extensionExpression.find(File::expressionText[File::ArrayExpression]) != 0)
	{
		this->m_extension = static_cast<class Interface*>(this->searchObjectInList(this->m_extensionExpression, Parser::Interfaces));

		if (this->m_extension == 0)
			this->m_extension = static_cast<class Interface*>(this->searchObjectInList(this->m_extensionExpression, Parser::Structs));

		if (this->m_extension != 0)
			this->m_extensionExpression.clear();
	}

	m_container = this;
	this->m_constructor = static_cast<class Method*>(this->searchObjectInList("create", Parser::Methods, Parser::CheckContainer));
	this->m_destructor = static_cast<class Method*>(this->searchObjectInList("onDestroy", Parser::Methods, Parser::CheckContainer));
	this->m_initializer = static_cast<class Method*>(this->searchObjectInList("onInit", Parser::Methods, Parser::CheckContainer));
}

void Struct::pageNavigation(std::ofstream &file) const
{
	Interface::pageNavigation(file);
	file
	<< "\t\t\t<li><a href=\"#Size\">"			<< _("Size") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Extensions\">"			<< _("Extensions") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Child Structs\">"		<< _("Child Structs") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Constructor\">"		<< _("Constructor") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Destructor\">"			<< _("Destructor") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Initializer\">"		<< _("Initializer") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Inherited Members\">"		<< _("Inherited Members") << "</a></li>\n"
	<< "\t\t\t<li><a href=\"#Inherited Methods\">"		<< _("Inherited Methods") << "</a></li>\n"
	;
}

void Struct::page(std::ofstream &file) const
{
	Interface::page(file);
	file
	<< "\t\t<h2><a name=\"Size\">" << _("Size") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->size(), this->sizeExpression()) << '\n'
	<< "\t\t<h2><a name=\"Extensions\">" << _("Extensions") << "</a></h2>\n"
	;
	std::list<class Interface*> extensions = this->extensions();

	if (!extensions.empty())
	{
		file << "\t\t<ul>\n";

		for (std::list<class Interface*>::iterator iterator = extensions.begin(); iterator != extensions.end(); ++iterator)
		{
			file << "\t\t\t<li>" << Object::objectPageLink(*iterator) << "</li>\n";

			if (iterator != --extensions.end())
				file << "\t\t\t<li>^</li>\n";
		}

		file << "\t\t</ul>\n";
	}
	else
		file << "\t\t-\n";

	file
	<< "\t\t<h2><a name=\"Child Structs\">" << _("Child Structs") << "</a></h2>\n"
	;

	std::list<class Object*> list = Vjassdoc::parser()->getSpecificList(Parser::Structs, Struct::HasExtension(), this);

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
	<< "\t\t<h2><a name=\"Constructor\">" << _("Constructor") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->constructor()) << '\n'
	<< "\t\t<h2><a name=\"Destructor\">" << _("Destructor") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->destructor()) << '\n'
	<< "\t\t<h2><a name=\"Initializer\">" << _("Initializer") << "</a></h2>\n"
	<< "\t\t" << Object::objectPageLink(this->initializer()) << '\n'
	<< "\t\t<h2><a name=\"Inherited Members\">" << _("Inherited Members") << "</a></h2>\n"
	;

	if (this->extension() != 0 &&  dynamic_cast<class Struct*>(this->extension()) != 0)
	{
		class Struct *extension = static_cast<class Struct*>(this->extension());

		do
		{
			extension->getMemberList(file);

			if (extension->extension() != 0 && dynamic_cast<class Struct*>(extension->extension()) != 0)
				extension = static_cast<class Struct*>(extension->extension());
			else
				break;
		}
		while (true);

		file << "\t\t<h2><a name=\"Inherited Methods\">" << _("Inherited Methods") << "</a></h2>\n";

		extension = static_cast<class Struct*>(this->extension()); //do not check at twice

		do
		{
			extension->getMethodList(file);

			if (extension->extension() != 0 && dynamic_cast<class Struct*>(extension->extension()) != 0)
				extension = static_cast<class Struct*>(extension->extension());
			else
				break;
		}
		while (true);
	}
	else
	{
		file
		<< "\t\t-\n"
		<< "\t\t<h2><a name=\"Inherited Methods\">" << _("Inherited Methods") << "</a></h2>\n"
		<< "\t\t-\n"
		;
	}
}

#ifdef SQLITE
std::string Struct::sqlStatement() const
{
	std::ostringstream sstream;
	sstream
	<< Interface::sqlStatement() << ", "
	<< "Size=" << Object::objectId(this->size()) << ", "
	<< "SizeExpression=\"" << this->sizeExpression() << "\", "
	<< "Extension=" << Object::objectId(this->extension()) << ", "
	<< "ExtensionExpression=\"" << this->extensionExpression() << "\", "
	<< "Constructor=" << Object::objectId(this->constructor()) << ", "
	<< "Destructor=" << Object::objectId(this->destructor()) << ", "
	<< "Initializer=" << Object::objectId(this->initializer());

	return sstream.str();
}
#endif

std::list<class Interface*> Struct::extensions() const
{
	std::list<class Interface*> extensions;

	for (class Interface *extension = this->extension(); extension != 0; extension = static_cast<class Struct*>(extension)->extension())
	{
		extensions.push_front(extension);

		if (dynamic_cast<class Struct*>(extension) == 0)
			break;
	}

	return extensions;
}

}
