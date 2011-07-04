/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_LANG_JASS_CALL_HPP
#define WC3LIB_LANG_JASS_CALL_HPP

#include "object.hpp"

namespace wc3lib
{
	
namespace lang
{
	
namespace jass
{

class Call : public Object
{
	public:
		class List : public Object::List
		{		
#ifdef HTML
				virtual const std::string& htmlCategoryName() const;
				virtual const std::string& htmlFolderName() const;
#endif

			protected:
#ifdef SQLITE
				virtual const std::string& sqlTableName() const;
				virtual std::size_t sqlColumns() const;
				virtual const std::string& sqlColumnDataType(std::size_t column) const throw (class Exception);
				virtual const std::string& sqlColumnName(std::size_t column) const throw (class Exception);
#endif
		};
#ifdef SQLITE
		static const std::size_t sqlMaxArguments;
#endif
		Call(class Object::List *list, const std::string &identifier, class SourceFile *sourceFile, std::size_t line, const std::string &functionIdentifier, const std::list<std::string> &argumentIdentifiers);
#ifdef SQLITE
		Call(std::vector<Object::SqlValueDataType> &columnVector);
#endif
		virtual void init();
#ifdef SQLITE
		virtual const std::string& sqlValue(std::size_t column) const;
#endif
#ifdef HTML
		virtual void writeHtmlPageNavigation(std::ostream &ostream) const;
		virtual void writeHtmlPageContent(std::ostream &ostream) const;
#endif
		
		const std::string& functionIdentifier() const;
		class Function* function() const;
		const std::list<std::string>& argumentIdentifiers() const;
		const std::list<class Object*>& arguments() const;
		
	protected:
		std::string m_functionIdentifier;
		class Function *m_function;
		std::list<std::string> m_argumentIdentifiers;
		std::list<class Object*> m_arguments;
};

inline const std::string& Call::functionIdentifier() const
{
	return this->m_functionIdentifier;
}

inline class Function* Call::function() const
{
	return this->m_function;
}

inline const std::list<std::string>& Call::argumentIdentifiers() const
{
	return this->m_argumentIdentifiers;
}

inline const std::list<class Object*>& Call::arguments() const
{
	return this->m_arguments;
}

}

}

}

#endif
