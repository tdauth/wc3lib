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

#ifndef VJASSDOC_TYPE_HPP
#define VJASSDOC_TYPE_HPP

#include <cstdlib>

#include "object.hpp"

namespace vjassdoc
{

class Type : public Object
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static std::size_t sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		Type(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, const std::string &typeExpression, const std::string &sizeExpression);
#ifdef SQLITE
		Type(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init(); //the size object can be defined afterwards
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		class Type* type() const;
		class Object* size() const; //Global (constant), Member (static, constant), Function (constant), Method (static, constant), Literal
		/**
		* If the size is not an object it may be an integer value.
		* @return Returns the size. If the type has no size this value will be -1.
		*/
		int sizeLiteral() const;

	protected:
		std::string typeExpression;
		std::string sizeExpression;

		class Type *m_type;
		class Object *m_size;
};

inline class Type* Type::type() const
{
	return this->m_type;
}

inline class Object* Type::size() const
{
	return this->m_size;
}

inline int Type::sizeLiteral() const
{
	if (sizeExpression.empty() || !isdigit(sizeExpression[0])) //if it is not a literal value
		return -1;

	return atoi(sizeExpression.c_str());
}

}

#endif
