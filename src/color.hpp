/***************************************************************************
 *   Copyright (C) 2012 by Tamino Dauth                                    *
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

#ifndef WC3LIB_COLOR_HPP
#define WC3LIB_COLOR_HPP

/**
 * \file
 * Provides basic color classes supporting I/O operations and color channels used by
 * Warcraft III formats.
 *
 * \ingroup colors
 *
 * \defgroup colors Colors
 * \brief Everything about colors in Warcraft III.
 */

#include "config.h"
#include "utilities.hpp"
#include "platform.hpp"
#include "format.hpp"

namespace wc3lib
{

/**
 * \brief Basic RGB color with storage and serialization support of three color channels.
 *
 * \ingroup colors
 */
class Rgb : public Format
{
	public:
		Rgb(uint8_t red, uint8_t green, uint8_t blue);
		Rgb();

		virtual std::streamsize read(InputStream &istream) throw (class Exception);

		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		void setRed(uint8_t red);
		uint8_t red() const;
		void setGreen(uint8_t green);
		uint8_t green() const;
		void setBlue(uint8_t blue);
		uint8_t blue() const;

		/**
		 * Computes an integer value representation containing values of all color channels.
		 * The channel values can be extracting using binary operations like AND.
		 *
		 * The first 8 bits are not used (usually alpha value for argb or red value for rgba).
		 */
		virtual uint32_t value() const;

		bool operator==(uint32_t value) const;
		bool operator==(const Rgb &other) const;

	protected:
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
};

class Bgr : public Rgb
{
	public:
		virtual std::streamsize read(InputStream &istream) throw (Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

		/**
		 * The first 8 bits are not used (usually alpha value for argb or red value for rgba).
		 */
		virtual uint32_t value() const;
};

class Rgba : public Rgb
{
	public:
		Rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
		Rgba();

		virtual std::streamsize read(InputStream &istream) throw (class Exception);

		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		void setAlpha(uint8_t alpha);
		uint8_t alpha() const;

		virtual uint32_t value() const;

	protected:
		uint8_t m_alpha;
};

class Bgra : public Rgba
{
	public:
		virtual std::streamsize read(InputStream &istream) throw (Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

		virtual uint32_t value() const;
};

inline void Rgb::setRed(uint8_t red)
{
	this->m_red = red;
}

inline uint8_t Rgb::red() const
{
	return this->m_red;
}

inline void Rgb::setGreen(uint8_t green)
{
	this->m_green = green;
}

inline uint8_t Rgb::green() const
{
	return this->m_green;
}

inline void Rgb::setBlue(uint8_t blue)
{
	this->m_blue = blue;
}

inline uint8_t Rgb::blue() const
{
	return this->m_blue;
}

inline void Rgba::setAlpha(uint8_t alpha)
{
	this->m_alpha = alpha;
}

inline uint8_t Rgba::alpha() const
{
	return this->m_alpha;
}

}

#endif
