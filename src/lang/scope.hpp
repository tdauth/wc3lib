/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_LANG_SCOPE_HPP
#define WC3LIB_LANG_SCOPE_HPP

namespace wc3lib
{

namespace lang
{

/**
* Scopes can be encapsulated. Therefore each scope instance has a queue which holds all tokens.
* The first one in queue is the outermost one.
*/
class Scope
{
	public:
		Scope(class Token *token);
		Scope(std::queue<class Token*> &tokens);
		/**
		* @return Returns the innermost token of token queue.
		*/
		const class Token* token() const;

	private:
		std::queue<class Token*> m_tokens;
};

}

}
