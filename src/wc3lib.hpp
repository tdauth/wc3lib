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
#error Undefined platform.
#endif

/**
* \mainpage wc3libmainpage
* \date Version date
* 2010-12-25
* \author Tamino Dauth <tamino@cdauth.eu>
*
* \section introductionsection Introduction
* This is the API reference and documentation of the wc3lib project. wc3lib is an abbreviation which means "Warcraft 3 Library".
* It's a collection of varios libraries which allow programmers to use Blizzard's customly developed formats with an abstract C++ interface.
* All implementations are free and mainly under the GPLv2 license but there's some external code which is mostly under some similar license as well.
* As mentioned above the wc3lib is split up into some different modules. Each module consists of a single library which supports one of Blizzard's formats (except modules "Applications" and "Editor" which are extensions of the default modules).
* To use the whole library you could simply include file \ref "wc3lib.hpp" and link your program with library "wc3lib".
* Another probably faster way is to link it with the required libraries only.
* Therefore you can include all header files of your required modules since each module has one.
* Besides you should bear in mind that each module has its own namespace.
* All namespaces and other declarations of the wc3lib belong to the global namespace \ref wc3lib.
*
* \section mpqsection MPQ module
* Include file \ref "mpq.hpp" and use namspace \ref mpq to use this module.
*
* \section blpsection BLP module
* Include file \ref "blp.hpp" and use namspace \ref blp to use this module.
*
* \section mdlx MDLX module
* This module provides several classes to read and write Blizzard's 3d graphics formats MDX (binary) and MDL (human-readable).
* It does not provide any display/rendering functionality. Use module \ref editor for this.
* Include file \ref "mdlx.hpp" and use namespace \ref mdlx to use this module.
*
* \section mapsection Map module
* Use this module to read, write and modify Warcraft 3 maps and campaigns and their corresponding files.
* Warcraft 3 maps and campaigns are usual MPQ archives containing some specific files, most of them in a binary format.
* Include file \ref "map.hpp" and use namespace \ref map to use this module.
*
* \section slksection SLK module
* \deprecated This library is deprecated and will be integrated in modul editor.
* The SLK format is a very basical, human-readable table format which is used by Blizzard's game Warcraft 3 to store many meta datas (e. g. unit type and sound data).
* This module provides some classes to read, write and modify SLK files.
* Include file \ref "slk.hpp" and use namespace \ref slk to use this module.
*
* \section editorsection Editor module
* The editor module contains many classes to emulate the original World Editor created by Blizzard Entertainment.
* There are various Qt and KDE plug-ins which allow you to load and save BLP, MPQ and SLK files in an appreciated way.
* Include file \ref "editor.hpp" and use namspace \ref edtor to use this module.
*
* \section applicationssection Applications module
* Since this module only provides some useful applications there is neither any namespace to use nor any header file to include.
* Currently there are following applications available:
* <ul>
* <li>converter - console-based converting tool for all supported formats</li>
* <li>editor - improved World Editor emulation</li>
* <li>jassc - JASS parser and compiler for JASS-based dialects (vJass, Zinc, CJass, JASS++)</li>
* <li>jasstrans - internationalisation tool which parses and stores strings from JASS code files</li>
* <li>mpq - console-based, tar-like MPQ archive extractor and creator tool</li>
* <li>tilesetter - deprecated console-based tilesetting program</li>
* </ul>
*
* \namespace wc3lib
* All code elements of the wc3lib belong to this namespace!
*/
/// @todo Add all include files!

#include "core.hpp"

#include "blp.hpp"
#include "map.hpp"
#include "mdlx.hpp"
#include "mpq.hpp"

#ifdef EDITOR
#include "editor.hpp"
#endif

#endif
