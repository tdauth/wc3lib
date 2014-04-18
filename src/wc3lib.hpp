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

#ifndef WC3LIB_WC3LIB_HPP
#define WC3LIB_WC3LIB_HPP

#if !defined(UNIX) && !defined(WINDOWS)
#warning Undefined platform. Define either UNIX or WINDOWS please.
#endif

/**
* \mainpage Warcraft III Library
* This is the API reference and documentation of the wc3lib project. wc3lib is an abbreviation which means "Warcraft III Library".
* 
* wc3lib consists of several modules which allow strict separation of areas and a clear definition of dependencies:
* <ul>
* <li>\subpage coresection </li>
* <li>\subpage mpqsection </li>
* <li>\subpage blpsection </li>
* <li>\subpage mdlxsection </li>
* <li>\subpage mapsection </li>
* <li>\subpage jasssection </li>
* <li>\subpage editorsection </li>
* <li>\subpage applicationssection </li>
* </ul>
* 
* For detailed description see \ref intro
*
* \note Check out http://wc3lib.org for further information.
*
* \date Version date
* 2014-04-18
* \author Tamino Dauth <tamino@cdauth.eu>
*
* \copyright GNU GENERAL PUBLIC LICENSE Version 2 (except external library code)
*
* \section intro Introduction
* wc3lib is a collection of several C++ libraries which provide an API to <a href="http://blizzard.com/">Blizzard's</a> file formats of the real time strategy game <a href="blizzard.com/de-de/games/war3/">Warcraft III: Reign of Chaos</a> and its expansion Warcraft III: The Frozen Throne.
* All implementations are free and mainly under the GPLv2 license although there still is some external code which is under a different license.
* 
* The library is split up into several modules. The core modules consist of a single library which support one of Blizzard's formats.
* Additional modules provide a view layer and some tools.
* To use the library simply include headers of the required modules and link your program against the libraries.
* 
* Each module has its own namespace. All namespaces and other declarations of the wc3lib belong to the global namespace \ref wc3lib.
*
* \page applicationssection Applications module
* Since this module only provides some useful applications there is neither any namespace to use nor any header file to include.
* Currently there are following applications available:
* <ul>
* <li>wc3converter - console-based converting tool for all supported formats</li>
* <li>wc3editor - improved World Editor emulation</li>
* <li>jassc - JASS parser and compiler for JASS-based dialects (vJass, Zinc, CJass, JASS++)</li>
* <li>jasstrans - internationalisation tool which parses and stores strings from JASS code files</li>
* <li>mpq - console-based, tar-like MPQ archive extractor and creator tool</li>
* <li>tilesetter - console-based tilesetting program</li>
* </ul>
*/

#include "core.hpp"

#ifdef BLP
#include "blp.hpp"
#endif

#ifdef JASS
#include "jass.hpp"
#endif

#ifdef MAP
#include "map.hpp"
#endif

#ifdef MDLX
#include "mdlx.hpp"
#endif

#ifdef MPQ
#include "mpq.hpp"
#endif

#ifdef EDITOR
#include "editor.hpp"
#endif

#endif
