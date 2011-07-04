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

#ifndef WC3LIB_LANG_UTILITIES_HPP
#define WC3LIB_LANG_UTILITIES_HPP

#include <string>
#include <ostream>

#include "../internationalisation.hpp"

namespace wc3lib
{

namespace lang
{

/**
* @file utilities.hpp Contains some deprecated utility functions.
*/

extern std::string getToken(const std::string &line, std::string::size_type &index, bool endOfLine = false);
extern void createHtmlHeader(std::ostream &ostream, const std::string &title, const std::string &language = _("en"));
/**
* Converts boolean value to a localized string.
* This method has been written for showing boolean members of classes in a HTML file.
* Instead of 0 and 1 it returns the strings 'No' and 'Yes' in the local language.
*/
extern std::string booleanToString(bool value);
#ifdef SQLITE
extern const std::string& sqlFilteredString(const std::string &value);
#endif

}

}

#endif
