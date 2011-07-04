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

#ifndef WC3LIB_LANG_SYNTAXERROR_HPP
#define WC3LIB_LANG_SYNTAXERROR_HPP

#include <string>
#include <ostream>

#include "sourcefile.hpp"
#include "position.hpp"
#include "../internationalisation.hpp"

namespace wc3lib
{

namespace lang
{

class SyntaxError
{
	public:
		SyntaxError(class SourceFile *sourceFile, const class Position &position, const std::string &message);
		class SourceFile* sourceFile() const;
		const class Position& position() const;
		const std::string& message() const;

	protected:
		class SourceFile *m_sourceFile;
		class Position m_position;
		std::string m_message;
};

/// Allows formatted output of syntax errors.
inline std::ostream& operator<<(std::ostream &ostream, const class SyntaxError &syntaxError)
{
	ostream << boost::format(_("%1% (Line %2%, column %3%): %4%")) % syntaxError.sourceFile()->path().string() % syntaxError.position().line() % syntaxError.position().column() % syntaxError.message() << std::endl;

	return ostream;
}

inline class SourceFile* SyntaxError::sourceFile() const
{
	return this->m_sourceFile;
}

inline const class Position& SyntaxError::position() const
{
	return this->m_position;
}

inline const std::string& SyntaxError::message() const
{
	return this->m_message;
}

}

}

#endif
