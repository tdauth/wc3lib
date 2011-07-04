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

#ifndef VJASSDOC_TEXTMACRO_HPP
#define VJASSDOC_TEXTMACRO_HPP

#include "object.hpp"

namespace vjassdoc
{

class TextMacro : public Object
{
	public:
#ifdef SQLITE
		static const char *sqlTableName;
		static unsigned int sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		TextMacro(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, bool isOnce, const std::string &parameters);
#ifdef SQLITE
		TextMacro(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		bool isOnce() const;
		std::string parameters() const;

	protected:
		bool m_isOnce;
		std::string m_parameters;
};

inline bool TextMacro::isOnce() const
{
	return this->m_isOnce;
}

inline std::string TextMacro::parameters() const
{
	return this->m_parameters;
}

}

#endif
