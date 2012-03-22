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
* This is the API reference and documentation of the <a url="https://gitorious.org/wc3lib">wc3lib</a> project. wc3lib is an abbreviation which means "Warcraft III Library".
* It's a collection of various libraries which allow programmers to use Blizzard's customly developed formats with an abstract C++ interface.
* All implementations are free and mainly under the GPLv2 license but there's some external code which is mostly under some similar license as well.
* As mentioned above the wc3lib is split up into some different modules. Each module consists of a single library which supports one of <a url="http://blizzard.com/">Blizzard's</a> formats (except modules for \ref applicationssection "applications" and \ref editorsection "editor" which are extensions of the default modules).
* To use the whole library you could simply include file \ref wc3lib.hpp and link your program with library "wc3lib".
* Another probably faster way is to link it with the required libraries only.
* Therefore you can include all header files of your required modules since each module has one.
* Besides you should bear in mind that each module has its own namespace.
* All namespaces and other declarations of the wc3lib belong to the global namespace \ref wc3lib.
* Each module uses a header file called "platform.hpp" for its core type definitions such as integer types with specified size. There's a global \ref platform.hpp file as well which tries to provide all shared basic types.
* Actually, the wc3lib is divided into the following parts:
* <ul>
* <li>\subpage coresection </li>
* <li>\subpage mpqsection </li>
* <li>\subpage blpsection </li>
* <li>\subpage mdlxsection </li>
* <li>\subpage mapsection </li>
* <li>\subpage editorsection </li>
* <li>\subpage applicationssection </li>
* </ul>
*
* \date Version date
* 2011-10-20
* \author Tamino Dauth <tamino@cdauth.eu>
*
* \page coresection Core
* The core of the wc3lib is shared by all of its modules. It provides some basic functions and classes for exception handling, internationalisation, runtime loading of shared objects/DLLs and binary I/O.
* Use class \ref LibraryLoader to load and unload shared objects and their symbols at runtime.
* Class \ref Exception is the base class of all exceptions thrown by functions of the wc3lib.
* Include \ref i18n.hpp to use \ref boost::format and gettext macros for internationalisation of your program.
* Class \ref Format is the base class of all format related classes of the wc3lib. It supports some basic serialization member functions.
* Include \ref utilities.hpp to use many I/O stream functions heavily used by all supported format classes. Besides it includes many default components of the STL and the Boost C++ Libraries and provides some error/info output functions with human-readable messages and class \ref Vertex.
*
* \page mpqsection MPQ module
* The MPQ format (Mo'PaQ, short for Mike O'Brien Pack) is Blizzard's archive format used by most of their games.
* It provides various features:
* <ul>
* <li>file hashing</li>
* <li>locale support</li>
* <li>file sector compression (BZip2, zlib, PKWare, wave etc.)</li>
* <li>format extensions for archives which can be larger than 2^32 bytes</li>
* </ul>
* Use class \ref wc3lib::mpq::Mpq to access or create one single MPQ archive.
* Include file \ref mpq.hpp and use namspace \ref wc3lib::mpq to use this module.
*
* \page blpsection BLP module
* This module provides functionality for reading, writing and modifying Blizzard Picture files.
* Its main class is \ref wc3lib::blp::Blp which represents one single texture.
* The BLP format supports various features:
* <ul>
* <li>MIP mapping (up to 16 MIP maps)</li>
* <li>JPEG/JFIF compression</li>
* <li>paletted compression (similar to GIF - 256 colors)</li>
* <li>RGBA color space</li>
* </ul>
* Include file \ref blp.hpp and use namspace \ref wc3lib::blp to use this module.
*
* \page mdlxsection MDLX module
* This module provides several classes to read and write Blizzard's 3d graphics formats MDX (binary) and MDL (human-readable).
* It does not provide any display/rendering functionality. Use module \ref editor for this.
* Class \ref wc3lib::mdlx::Mdlx provides all functionality which is needed to save or load one MDX or MDL file.
* It can easily be used to convert one format into the other one (application "wc3converter" provides this feature).
* Include file \ref mdlx.hpp and use namespace \ref wc3lib::mdlx to use this module.
*
* \page mapsection Map module
* Use this module to read, write and modify Warcraft III maps (*.w3m, *.w3x) and campaigns (*.w3n) and their corresponding files.
* Warcraft III maps and campaigns are usual MPQ archives containing some specific files, most of them in a binary format.
* Include file \ref map.hpp and use namespace \ref wc3lib::map to use this module.
*
* \page editorsection Editor module
* The editor module contains many classes to emulate the original World Editor created by Blizzard Entertainment.
* It's based on some plugins which can be used without the whole editor module functionality, as well.
* Include file \ref editor.hpp and use namspace \ref wc3lib::edtor to use this module.
* Resource classes derived from \ref editor::Resource can be used to load Warcraft III date like textures (\ref editor::Texture), models (\ref editor::OgreMdlx) or maps (\ref editor::Map).
* Those wrapper classes provide load and save functionality based on the KIO API and considering \ref MpqPriorityList entries.
* \section pluginssection Plugins
* There is various plug-ins which allow you to load and save BLP textures in Qt/KDE GUIs and OGRE scenes and to access MPQ archives via KDE's KIO slave module.
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
* <li>tilesetter - deprecated console-based tilesetting program</li>
* </ul>
*
* \namespace wc3lib
* All code elements of the wc3lib belong to this namespace!
*/

#include "core.hpp"

#include "blp.hpp"
#include "map.hpp"
#include "mdlx.hpp"
#include "mpq.hpp"

#ifdef EDITOR
#include "editor.hpp"
#endif

#endif
