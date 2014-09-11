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

#ifndef WC3LIB_FORMAT_HPP
#define WC3LIB_FORMAT_HPP

/**
 * \defgroup io I/O
 * wc3lib supports I/O operations for most of Warcraft III's file formats.
 * Base class of all formats is \ref Format.
 */

#include <istream>
#include <ostream>

#include <boost/cstdint.hpp>

#include "config.h"
#include "utilities.hpp"
#include "platform.hpp"

namespace wc3lib
{

/**
 * \brief Abstract class for serializable file formats which supports basic I/O operations "read" and "write".
 *
 * Provides abstract functions for reading and writing.
 *
 * \ingroup io
 */
class Format
{
	public:
		typedef byte CharType;
		typedef std::basic_istream<byte> InputStream;
		typedef std::basic_ostream<byte> OutputStream;

		virtual ~Format();

		/**
		 * Reads the data of a format from input stream \p istream.
		 *
		 * \return Usually this function should returns the number of read bytes. For non-binary formats or formats which won't store any byte count this value can be 0. It has been introduced for convenience for example when reading chunks of the MDX format where you do have to know how many bytes are still left.
		 *
		 * \throws Exception If an error occurs this should throw an exception.
		 */
		virtual std::streamsize read(InputStream &istream) = 0;
		/**
		 * Writes the data of a format to output stream \p ostream.
		 *
		 * \throws Exception If an error occurs this should throw an exception.
		 */
		virtual std::streamsize write(OutputStream &ostream) const = 0;

		virtual uint32_t version() const;
};

}

#endif
