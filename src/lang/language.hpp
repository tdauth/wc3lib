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

#ifndef WC3LIB_LANG_LANGUAGE_HPP
#define WC3LIB_LANG_LANGUAGE_HPP

#include <string>

#include <boost/filesystem.hpp>

#include "driver.hpp"

namespace wc3lib
{

namespace lang
{

/**
* Abstract scripting language class. Should be inheritted by other languages.
* Note that you usually create no instance of a language since there should be only one single constant accessable by a static function of the language class.
*/
class Language
{
	public:
		static const class Language* language();

		/**
		* @return Returns the real name of the language.
		*/
		virtual const std::string& name() const = 0;
		/**
		* @return Returns if the language is compatible to language @param language. Compatible means that the language can treat code of the other language as well as its own!
		*/
		virtual bool compatibleTo(const class Language &language) const = 0;
		/**
		* Creates a new Bison/Flex driver for parsing the language.
		*/
		virtual class Driver* createDriver() const = 0;

	protected:
		Language();
		Language(class Language &);
		class Language& operator=(class Language &);
		virtual ~Language();

		class Language *m_language;
};

}

}

#endif
