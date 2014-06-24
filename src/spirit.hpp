/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

/**
 * \file spirit.hpp
 * This file should be included before any Boost Spirit header since it enables Boost debugging mode for Spirit
 * and defines a global output file stream for traces.
 */
#ifndef WC3LIB_SPIRIT_HPP
#define WC3LIB_SPIRIT_HPP

#include <fstream>

namespace wc3lib
{

/**
 * Define this static attribute before including Spirit stuff since we define it as our custom debugging output stream
 * for Spirit traces.
 *
 * Call spiritTraceLog.open() before running a test with the filename of your output traces XML file.
 */
extern std::ofstream spiritTraceLog;

}

/**
 * Unicode support is required for UTF-8 strings in JASS.
 */
#define BOOST_SPIRIT_UNICODE

/**
 * By default Boost Spirit selects Phoenix V2 for compatibility reasons.
 * V3 fixes many issues and is therefore enabled.
 */
#define BOOST_SPIRIT_USE_PHOENIX_V3

#ifdef DEBUG
#define BOOST_SPIRIT_DEBUG
// we use the class attribute as custom output stream for Spirit's debugging traces
// the public attribute can be opened in each unit test customly where the user wants to have the output traces stored
#define BOOST_SPIRIT_DEBUG_OUT wc3lib::spiritTraceLog
#define BOOST_SPIRIT_LEXERTL_DEBUG // enable debugging of lexer
#endif

#endif
