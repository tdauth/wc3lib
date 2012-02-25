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

#ifndef VJASSDOC_IMPLEMENTATION_HPP
#define VJASSDOC_IMPLEMENTATION_HPP

#include "object.hpp"
#include "parser.hpp"

namespace vjassdoc
{

class Module;

class Implementation : public Object
{
	public:
		struct UsesModule : public Parser::Comparator
		{
			virtual bool operator()(const class Implementation *thisImplementation, const class Module *module) const;
		};

#ifdef SQLITE
		static const char *sqlTableName;
		static std::size_t sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		Implementation(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Object *container, const std::string &moduleExpression, bool isOptional);
#ifdef SQLITE
		Implementation(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		virtual class Object* container() const;
		std::string moduleExpression() const;
		class Module* module() const;
		bool isOptional() const;

	protected:
		class Object *m_container;
		std::string m_moduleExpression;
		class Module *m_module;
		bool m_isOptional;
};

inline bool Implementation::UsesModule::operator()(const class Implementation *thisImplementation, const class Module *module) const
{
	return thisImplementation->module() == module;
}

inline std::string Implementation::moduleExpression() const
{
	return this->m_moduleExpression;
}

inline class Module* Implementation::module() const
{
	return this->m_module;
}

inline bool Implementation::isOptional() const
{
	return this->m_isOptional;
}

}

#endif
