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
* <li>\subpage coresection - Basic I/O functions and shared structures for all file formats.</li>
* <li>\subpage mpqsection - MPQ archive API for reading and writing MPQ archives.</li>
* <li>\subpage blpsection - BLP API for reading and writing BLP textures.</li>
* <li>\subpage mdlxsection - MDL and MDX API for reading and writing MDL and MDX model files.</li>
* <li>\subpage mapsection - API for reading and writing Warcraft III (.w3m) and Warcraft III: The Frozen Throne (.w3x) maps and all consisting files such as object data, triggers etc.</li>
* <li>\subpage jasssection - API for parsing JASS script files and generating abstract syntax trees.</li>
* <li>\subpage editorsection - API for graphical interfaces of a World Editor emulation with modules such as the object or the trigger editor.</li>
* <li>\subpage applicationssection - Core application which work on command line such as "mpq" to extract files from MPQ archives. Note that GUI applications are part of the \ref wc3lib::editor module.</li>
* <li>\subpage libsection - API for external libraries which support compression formats required by the \ref wc3lib::mpq module.</li>
* </ul>
*
* wc3lib is a collection of several C++ libraries which provide an API to <a href="http://blizzard.com/">Blizzard's</a> file formats of the real time strategy game <a href="blizzard.com/de-de/games/war3/">Warcraft III: Reign of Chaos</a> and its expansion Warcraft III: The Frozen Throne.
* All implementations are free and mainly under the GPLv2 license although there still is some external code which is under a different license.
*
* The library is split up into several modules. The core modules consist of a single library which support one of Blizzard's formats.
* Additional modules provide a view layer and some tools.
* To use the library simply include headers of the required modules and link your program against the libraries.
*
* Each module has its own namespace. All namespaces and other declarations of the wc3lib belong to the global namespace \ref wc3lib.
*
* \note Check out http://wc3lib.org for further information.
*
* \date Version date
* \today
* \author Tamino Dauth <tamino@cdauth.eu>
*
* \copyright GNU GENERAL PUBLIC LICENSE Version 2 (except external library code)
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
*
* \page libsection External library module
* Several small libraries are required for the \ref wc3lib::mpq module to compress and decompress files in MPQ archives.
* Besides the calculation of MD5 checksums is required.
* The libraries have been copied from the StormLib project and it would be useful to update them from time to time whenever they are updated externally to reduce bugs.
* They are not licensed under the GPLv2!
*
* This might be an incomplete list of functions from the external libraries:
* <ul>
* <li>\ref CompressADPCM </li>
* <li>\ref DecompressADPCM </li>
* <li>\ref implode </li>
* <li>\ref explode </li>
* <li>\ref crc32_pklib </li>
* <li>\ref MD5 </li>
* <li>\ref huffman </li>
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
