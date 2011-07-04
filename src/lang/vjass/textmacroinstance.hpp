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

#ifndef VJASSDOC_TEXTMACROINSTANCE_HPP
#define VJASSDOC_TEXTMACROINSTANCE_HPP

#include "textmacro.hpp"
#include "parser.hpp"

namespace vjassdoc
{

class TextMacroInstance : public TextMacro
{
	public:
		struct UsesTextMacro : public Parser::Comparator
		{
			virtual bool operator()(const class TextMacroInstance *thisTextMacroInstance, const class TextMacro *textMacro) const;
		};

#ifdef SQLITE
		static const char *sqlTableName;
		static unsigned int sqlColumns;
		static std::string sqlColumnStatement;

		static void initClass();
#endif
		TextMacroInstance(const std::string &identifier, class SourceFile *sourceFile, unsigned int line, class DocComment *docComment, bool isOptional, const std::string &arguments);
#ifdef SQLITE
		TextMacroInstance(std::vector<const unsigned char*> &columnVector);
#endif
		virtual void init();
		virtual void pageNavigation(std::ofstream &file) const;
		virtual void page(std::ofstream &file) const;
#ifdef SQLITE
		virtual std::string sqlStatement() const;
#endif
		virtual class TextMacro* textMacro() const;
		bool isOptional() const;
		
	protected:
		class TextMacro *m_textMacro;
		bool m_isOptional;
};

inline bool TextMacroInstance::UsesTextMacro::operator()(const class TextMacroInstance *thisTextMacroInstance, const class TextMacro *textMacro) const
{
	return thisTextMacroInstance->textMacro() == textMacro;
}


inline class TextMacro* TextMacroInstance::textMacro() const
{
	return this->m_textMacro;
}

inline bool TextMacroInstance::isOptional() const
{
	return this->m_isOptional;
}

}

#endif
