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

#include "../core.hpp"

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
typedef char byte;
typedef uint16_t word;
typedef uint32_t dword;
typedef float float32;
/**
 * \brief RGBA color stored in order ARGB with one byte per channel.
 */
typedef uint32_t color;
typedef Format<byte> Format;
typedef std::basic_fstream<byte> fstream;
typedef std::basic_ifstream<byte> ifstream;
typedef std::basic_ofstream<byte> ofstream;
typedef std::basic_stringstream<byte> sstream;
typedef std::basic_istringstream<byte> isstream;
typedef std::basic_ostringstream<byte> osstream;

struct BlpHeader
{
	dword compression, flags, width, height, pictureType, pictureSubType;
	dword mipMapOffset[16], mipMapSize[16];
};

struct BlpJpegHeader : public BlpHeader
{
	dword jpegHeaderSize;
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

inline byte red(color c)
{
	return (c & 0x00FF0000) >> 16;
}

inline byte green(color c)
{
	return (c & 0x0000FF00) >> 8;
}

inline byte blue(color c)
{
	return (c & 0x000000FF);
}

inline byte alpha(color c)
{
	return byte(0xFF) - (color(c & 0xFF000000) >> 24);
}

}

}

#endif
