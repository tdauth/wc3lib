/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_BLP_PLATFORM_HPP
#define WC3LIB_BLP_PLATFORM_HPP

#include "../color.hpp"

namespace wc3lib
{

namespace blp
{

/*
//+-----------------------------------------------------------------------------
//| Data types
//+-----------------------------------------------------------------------------
CHAR   - 8bit character
BYTE   - 8bit unsigned integer
WORD   - 16bit unsigned integer
DWORD  - 32bit unsigned integer
FLOAT  - 32bit floating point number
COLOR  - 32bit color value of type RGBA, one byte per channel
X[n]   - An n-dimensional vector of type X
*/
typedef char char8;
typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;

/**
 * \brief RGBA color with one byte per channel.
 *
 * Even although the JPEG stores it in BGRA it is converted into the common format of RGBA.
 */
typedef uint32_t color;

struct BlpHeader
{
	dword compression, flags, width, height, pictureType, pictureSubType;
	dword mipMapOffset[16], mipMapSize[16];
};

struct Blp2Header
{
	byte type;
	byte encoding;
	byte alphaDepth;
	byte alphaEncoding;
	byte hasMips;
	dword width, height; //height+7
	dword mipMapOffset[16];
	dword mipMapSize[16];
	color palette[256];
};

/**
 * \return Returns the red component of color \p c.
 */
inline byte red(color c)
{
	return (c & 0xFF000000) >> 24;
}

/**
 * \return Returns the green component of color \p c.
 */
inline byte green(color c)
{
	return (c & 0x00FF0000) >> 16;
}

/**
 * \return Returns the blue component of color \p c.
 */
inline byte blue(color c)
{
	return (c & 0x0000FF00) >> 8;
}

/**
 * 0x00 means fully transparent.
 * 0xFF means no transparency.
 *
 * \return Returns the alpha component of color \p c.
 */
inline byte alpha(color c)
{
	return (c & 0x000000FF);
}

/**
 * Converts the color \p rgba into an RGBA integer value.
 */
inline color fromRgba(const Rgba &rgba)
{
	return (color(rgba.red()) << 24) | (color(rgba.green()) << 16) | (color(rgba.blue()) << 8) | (color(rgba.alpha()));
}

/**
 * Converts the color \p value into an RGBA struct value.
 */
inline Rgba toRgba(color value)
{
	return Rgba(red(value), green(value), blue(value), alpha(value));
}

}

}

#endif
