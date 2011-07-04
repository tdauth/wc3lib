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

#ifndef VJASSDOC_INTERFACE_HPP
#define VJASSDOC_INTERFACE_HPP

#include "object.hpp"

namespace vjassdoc
{

class Interface : public Object
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static std::size_t sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		Interface(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, class Library *library, class Scope *scope, bool isPrivate);
#ifdef SQLITE
		Interface(std::vector<const unsigned char*> &columnVector);
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

	protected:
		class Library *m_library;
		class Scope *m_scope;
		bool m_isPrivate;
		
		void getMemberList(std::ofstream &file) const;
		void getImplementationList(std::ofstream &file) const;
		void getMethodList(std::ofstream &file) const;
};

inline bool Interface::isPrivate() const
{
	return this->m_isPrivate;
}

}

#endif
