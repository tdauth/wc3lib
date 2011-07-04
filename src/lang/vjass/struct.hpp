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

#ifndef VJASSDOC_STRUCT_HPP
#define VJASSDOC_STRUCT_HPP

#include <list>

#include "interface.hpp"

namespace vjassdoc
{

class Struct : public Interface
{
	public:
		struct HasExtension : public Parser::Comparator
		{
			virtual bool operator()(const class Object *thisObject, const class Object *extension) const;
		};

#ifdef SQLITE
		static const char *sqlTableName;
		static unsigned int sqlColumns;
		static std::string sqlColumnStatement;
	
		static void initClass();
#endif
		Struct(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate, const std::string &sizeExpression, const std::string &extensionExpression);
#ifdef SQLITE
		Struct(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		class Object* size() const;
		std::string sizeExpression() const;
		class Interface* extension() const;
		std::string extensionExpression() const;
		class Method* constructor() const;
		class Method* destructor() const;
		class Method *initializer() const;
		
		std::list<class Interface*> extensions() const;
		
	protected:
		class Object *m_size;
		std::string m_sizeExpression;
		class Interface *m_extension; //Interface, Struct
		std::string m_extensionExpression;
		class Method *m_constructor;
		class Method *m_destructor;
		class Method *m_initializer;
};

inline class Object* Struct::size() const
{
	return this->m_size;
}

inline std::string Struct::sizeExpression() const
{
	return this->m_sizeExpression;
}

inline class Interface* Struct::extension() const
{
	return this->m_extension;
}

inline std::string Struct::extensionExpression() const
{
	return this->m_extensionExpression;
}

inline class Method* Struct::constructor() const
{
	return this->m_constructor;
}

inline class Method* Struct::destructor() const
{
	return this->m_destructor;
}

inline class Method *Struct::initializer() const
{
	return this->m_initializer;
}

}

#endif
