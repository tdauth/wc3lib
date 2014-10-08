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
/**
 * \defgroup parsers Parsers
 *
 * wc3lib provides several parsers for various file formats. Usually they are realised using <a href="http://boost-spirit.com/home/">Boost Spirit</a> which allows to write parser
 * generators in C++ code only.
 * Unfortunately, it is slower than for example Bison since it does not generate code which is then compiled and much faster. Besides it does not use a separate lexer by default,
 * so parsers must be written carefully unless you want to get bad performance.
 *
 * Use the header \ref spirit.hpp or \ref qi.hpp before including anything from Boost Spirit or Qi. They define important macros and the debugging output file as \ref spiritTraceLog which
 * can be useful for Unit Tests.
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
 *
 * \ingroup parsers
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
