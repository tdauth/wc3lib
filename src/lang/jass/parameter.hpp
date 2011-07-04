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


#ifndef VJASSDOC_PARAMETER_HPP
#define VJASSDOC_PARAMETER_HPP

#include "object.hpp"

namespace vjassdoc
{

class Parameter : public Object
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static unsigned int sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		Parameter(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class FunctionInterface *functionInterface, const std::string &typeExpression);
#ifdef SQLITE
		Parameter(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		/// Used by class @class FunctionInterface.
		void setFunctionInterface(class FunctionInterface *functionInterface);
		class FunctionInterface* functionInterface() const;
		std::string typeExpression() const;
		class Object* type() const; //Type, Function Interface, Interface, Struct

	private:
		class FunctionInterface *m_functionInterface;
		std::string m_typeExpression;
		class Object *m_type;
};

inline void Parameter::setFunctionInterface(class FunctionInterface *functionInterface)
{
	this->m_functionInterface = functionInterface;
}

inline class FunctionInterface* Parameter::functionInterface() const
{
	return this->m_functionInterface;
}

inline std::string Parameter::typeExpression() const
{
	return this->m_typeExpression;
}

inline class Object* Parameter::type() const
{
	return this->m_type;
}

}

#endif
