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

#ifndef VJASSDOC_LOCAL_HPP
#define VJASSDOC_LOCAL_HPP

#include "object.hpp"

namespace vjassdoc
{

class Function;

class Local : public Object
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static std::size_t sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		Local(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Function *function, const std::string &typeExpression, const std::string &valueExpression);
#ifdef SQLITE
		Local(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		class Function* function() const; //Function, Method
		class Object* type() const; //Type, Interface, Struct
		std::string typeExpression() const; //Unknown type
		class Object* value() const; //Global, Member (static), Function, Method (static)
		std::string valueExpression() const; //Literal

	protected:
		class Function *m_function;
		class Object *m_type;
		std::string m_typeExpression;
		class Object *m_value;
		std::string m_valueExpression;
};

inline class Function* Local::function() const
{
	return this->m_function;
}

inline class Object* Local::type() const
{
	return this->m_type;
}

inline std::string Local::typeExpression() const
{
	return this->m_typeExpression;
}

inline  class Object* Local::value() const
{
	return this->m_value;
}

inline std::string Local::valueExpression() const
{
	return this->m_valueExpression;
}

}

#endif
