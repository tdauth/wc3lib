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

#ifndef WC3LIB_LANG_JASS_FUNCTION_HPP
#define WC3LIB_LANG_JASS_FUNCTION_HPP

#include "../object.hpp"

namespace wc3lib
{

namespace lang
{

namespace jass
{

class Function : public Object
{
	public:
		Function(const std::string &identifier, class SourceFile *sourceFile, std::size_t line, const std::string &returnTypeExpression, std::list<std::string> parameterTypeExpressions, std::list<std::string> parameterIdentifiers);
#ifdef SQLITE
		Function(std::vector<Object::SqlValueDataType> &columnVector);
#endif
		virtual void init();
#ifdef SQLITE
		virtual std::string sqlValue(std::size_t column) const;
#endif
#ifdef HTML
		virtual void writeHtmlPageNavigation(std::ostream &ostream) const;
		virtual void writeHtmlPageContent(std::ostream &ostream) const;
#endif

	protected:
		std::string m_returnTypeExpression;
		class ReturnType *m_returnType;
		std::list<std::string> m_paramaterTypeExpressions;
		std::list<class Type*> m_parameterTypes;
		std::list<std::string> m_parameterIdentifiers;
};

}

}

}

#endif
