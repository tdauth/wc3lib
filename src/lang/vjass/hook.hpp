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

#ifndef VJASSDOC_HOOK_HPP
#define VJASSDOC_HOOK_HPP

#include "object.hpp"

namespace vjassdoc
{

class Hook : public Object
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static std::size_t sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		Hook(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, const std::string &functionExpression, const std::string &hookFunctionExpression);
#ifdef SQLITE
		Hook(std::vector<const unsigned char*> &columnVector);
#endif
		virtual ~Hook();
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		class Function* function() const;
		std::string functionExpression() const;
		class Function* hookFunction() const;
		std::string hookFunctionExpression() const;

	private:
		class Function *m_function;
		std::string m_functionExpression;
		class Function *m_hookFunction;
		std::string m_hookFunctionExpression;
};

inline class Function* Hook::function() const
{
	return this->m_function;
}

inline std::string Hook::functionExpression() const
{
	return this->m_functionExpression;
}

inline class Function* Hook::hookFunction() const
{
	return this->m_hookFunction;
}

inline std::string Hook::hookFunctionExpression() const
{
	return this->m_hookFunctionExpression;
}

}

#endif
