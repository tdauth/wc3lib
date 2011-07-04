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

#ifndef WC3LIB_LANG_JASS_GLOBAL_HPP
#define WC3LIB_LANG_JASS_GLOBAL_HPP

#include "object.hpp"

namespace wc3lib
{
	
namespace lang
{
	
namespace jass
{

class Global : public Object
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static std::size_t sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		Global(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate, bool isPublic, bool isConstant, const std::string &typeExpression, const std::string &valueExpression, const std::string &sizeExpression);
#ifdef SQLITE
		Global(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		virtual class Library* library() const;
		virtual class Scope* scope() const;
		bool isPrivate() const;
		bool isPublic() const;
		bool isConstant() const;
		class Object* type() const; //Type, Interface, Struct
		std::string typeExpression() const; //Unknown type
		class Object* value() const; //Global, Member (static), Function, Method (static)
		std::string valueLiteral() const; //Literal
		class Object* size() const; //Global (constant), Member (static, constant), Function (constant), Method (static, constant)
		int sizeLiteral() const;

	protected:
		std::string m_typeExpression;
		std::string valueExpression;
		std::string sizeExpression;

		class Library *m_library;
		class Scope *m_scope;
		bool m_isPrivate;
		bool m_isPublic;
		bool m_isConstant;
		class Object *m_type;
		class Object *m_value;
		class Object *m_size;
};

inline bool Global::isPrivate() const
{
	return this->m_isPrivate;
}

inline bool Global::isPublic() const
{
	return this->m_isPublic;
}

inline bool Global::isConstant() const
{
	return this->m_isConstant;
}

inline class Object* Global::type() const
{
	return this->m_type;
}

inline std::string Global::typeExpression() const
{
	return this->m_typeExpression;
}

inline  class Object* Global::value() const
{
	return this->m_value;
}

inline std::string Global::valueLiteral() const
{
	return this->valueExpression;
}

inline class Object* Global::size() const
{
	return this->m_size;
}

inline int Global::sizeLiteral() const
{
	if (sizeExpression.empty() || !isdigit(sizeExpression[0])) //if it is not a literal value
		return -1;
	
	return atoi(sizeExpression.c_str());
}

}

#endif
