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

#ifndef VJASSDOC_METHOD_HPP
#define VJASSDOC_METHOD_HPP

#include "function.hpp"

namespace vjassdoc
{

class Method : public Function
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static std::size_t sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		/// isNative is always false.
		Method(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate, std::list<class Parameter*> parameters, const std::string &returnTypeExpression, bool isPublic, bool isConstant, class Object *container, bool isStatic, bool isStub, bool isOperator, const std::string &defaultReturnValueExpression);
#ifdef SQLITE
		Method(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		virtual class Object* container() const; //Interface, Struct
		bool isStatic() const;
		bool isStub() const;
		bool isOperator() const;
		class Object* defaultReturnValue() const; //Type, Function Interface, Interface, Struct, Literal
		std::string defaultReturnValueExpression() const;

	protected:
		bool isNative() const; //do not use

		class Object *m_container;
		bool m_isStatic;
		bool m_isStub;
		bool m_isOperator;
		class Object *m_defaultReturnValue;
		std::string m_defaultReturnValueExpression;
};

inline bool Method::isStatic() const
{
	return this->m_isStatic;
}

inline bool Method::isStub() const
{
	return this->m_isStub;
}

inline bool Method::isOperator() const
{
	return this->m_isOperator;
}

inline class Object* Method::defaultReturnValue() const
{
	return this->m_defaultReturnValue;
}

inline std::string Method::defaultReturnValueExpression() const
{
	return this->m_defaultReturnValueExpression;
}

}

#endif
