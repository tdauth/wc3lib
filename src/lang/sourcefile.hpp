/***************************************************************************
 *   Copyright (C) 2008 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
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

#ifndef WC3LIB_LANG_SOURCEFILE_HPP
#define WC3LIB_LANG_SOURCEFILE_HPP

#include <map>

#include <boost/filesystem/path.hpp>

#include "position.hpp"

namespace wc3lib
{

namespace lang
{

class SourceFile
{
	public:
		SourceFile(class Parser &parser, const boost::filesystem::path &path);
		virtual ~SourceFile();
		const class Parser& parser() const;
		const boost::filesystem::path& path() const;
		/// @return Returns all syntax errors of file sorted by their line numbers.
		const std::map<class Position, class SyntaxError*> syntaxErrors() const;
		/// @return Returns all tokens of file.
		const std::map<class Position, class Token*> tokens() const;

	protected:
		class Parser *m_parser;
		boost::filesystem::path m_path;
		std::map<class Position, class SyntaxError*> m_syntaxErrors;
		std::map<class Position, class Token*> m_tokens;
};

inline const class Parser& SourceFile::parser() const
{
	return *this->m_parser;
}

inline const boost::filesystem::path& SourceFile::path() const
{
	return this->m_path;
}

}

}

#endif
