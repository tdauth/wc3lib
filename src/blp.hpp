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

#ifndef WC3LIB_BLP_HPP
#define WC3LIB_BLP_HPP
/**
 * \page blpsection BLP module
 * This module provides functionality for reading, writing and modifying Blizzard Picture files.
 * 
 * Its main class is \ref wc3lib::blp::Blp which represents one single texture.
 * See its documentation for accessing the format.
 * 
 * The BLP format supports various features:
 * <ul>
 * <li>MIP mapping (up to 16 MIP maps)</li>
 * <li>JPEG/JFIF compression</li>
 * <li>paletted compression (similar to GIF - 256 colors)</li>
 * <li>RGBA color space</li>
 * </ul>
 * 
 * Include file \ref blp.hpp and use namspace \ref wc3lib::blp for this module.
 * 
 * \namespace wc3lib::blp
 * \brief \ref blpsection
 */
#include "blp/blp.hpp"
#include "blp/platform.hpp"

#endif
