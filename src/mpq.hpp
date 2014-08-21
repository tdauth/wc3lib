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

#ifndef WC3LIB_MPQ_HPP
#define WC3LIB_MPQ_HPP

namespace wc3lib
{
/**
 * \page mpqsection MPQ module
 * The MPQ format (Mo'PaQ, short for Mike O'Brien Pack) is Blizzard's archive format used by most of their games.
 *
 * It provides various features:
 * <ul>
 * <li>file hashing</li>
 * <li>locale support</li>
 * <li>file sector compression (BZip2, zlib, PKWare, wave etc.)</li>
 * <li>format extensions for archives which can be larger than 2^32 bytes</li>
 * </ul>
 *
 * Use class \ref wc3lib::mpq::Archive to access or create one single MPQ archive.
 *
 * Include file \ref mpq.hpp and use namspace \ref wc3lib::mpq for this module.
 *
 * \namespace wc3lib::mpq
 * \brief \ref mpqsection
 */
}

#include "mpq/algorithm.hpp"
#include "mpq/archive.hpp"
#include "mpq/attributes.hpp"
#include "mpq/block.hpp"
#include "mpq/file.hpp"
#include "mpq/hash.hpp"
#include "mpq/listfile.hpp"
#include "mpq/platform.hpp"
#include "mpq/sector.hpp"
#include "mpq/signature.hpp"
#include "mpq/test.hpp"

#endif
