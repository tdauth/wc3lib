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

#ifndef WC3LIB_LANG_DOCCOMMENT_HPP
#define WC3LIB_LANG_DOCCOMMENT_HPP

#include "token.hpp"

namespace wc3lib
{

namespace lang
{

/**
* Documentation comments are allowed in all languages (including JASS itself).
* They're declared just like usual comments with an additional / or * character and can contain some special
* expressions which should by formatted when calling Compiler::generateDocumentation.
* Just visit http://www.stack.nl/~dimitri/doxygen/ to get all documentation comment rules.
* Note that they have to be generated language-specific since those different languages have different declarations which allow
* a corresponding documentation comment.
* There is still the old tool vjassdoc which is also written in C++ and allows you to generate an HTML API documentation for vJass code only.
* You can get all release packages on http://sourceforge.net/projects/vjasssdk/files/wc3sdk/vjassdoc/releases/.
*/
class DocComment : public Token
{
	public:
		virtual void init();

		/**
		* Each documentation comment belongs to another token.
		*/
		void setToken(class Token *token);
		const class Token* token() const;

	protected:
		class Token *m_token;
};

}

}

#endif
