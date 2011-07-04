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

#ifndef VJASSDOC_EXTERNALCALL_HPP
#define VJASSDOC_EXTERNALCALL_HPP

#include "object.hpp"

namespace vjassdoc
{

class ExternalCall : public Object
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static std::size_t sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		ExternalCall(const std::string &identifier, class SourceFile *sourceFile, std::size_t line, class DocComment *docComment, std::list<std::string> *arguments);
		~ExternalCall();
#ifdef SQLITE
		ExternalCall(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		std::list<std::string>* arguments() const;

	protected:
		static const std::size_t maxArguments;
		
		std::list<std::string> *m_arguments;
};

inline std::list<std::string>* ExternalCall::arguments() const
{
	return this->m_arguments;
}

}

#endif
