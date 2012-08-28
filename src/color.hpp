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

#include "utilities.hpp"
#include "platform.hpp"
#include "format.hpp"

namespace wc3lib
{

class Rgb : public Format
{
	public:
		Rgb(uint8_t red, uint8_t green, uint8_t blue) : m_red(red), m_green(green), m_blue(blue)
		{
		}

		Rgb() : m_red(0), m_green(0), m_blue(0)
		{
		}

		virtual std::streamsize read(InputStream &istream) throw (class Exception)
		{
			std::streamsize size = 0;
			wc3lib::read(istream, m_red, size);
			wc3lib::read(istream, m_green, size);
			wc3lib::read(istream, m_blue, size);

			return size;
		}

		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception)
		{
			std::streamsize size = 0;
			wc3lib::write(ostream, red(), size);
			wc3lib::write(ostream, green(), size);
			wc3lib::write(ostream, blue(), size);

			return size;
		}

		void setRed(uint8_t red)
		{
			m_red = red;
		}

		uint8_t red() const
		{
			return m_red;
		}

		void setGreen(uint8_t green)
		{
			m_green = green;
		}

		uint8_t green() const
		{
			return m_green;
		}

		void setBlue(uint8_t blue)
		{
			m_blue = blue;
		}

		uint8_t blue() const
		{
			return m_blue;
		}

	protected:
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
};

class Rgba : public Rgb
{
	public:
		Rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : Rgb(red, green, blue), m_alpha(alpha)
		{
		}

		Rgba() : Rgb(), m_alpha(0)
		{
		}

		virtual std::streamsize read(InputStream &istream) throw (class Exception)
		{
			std::streamsize size = Rgb::read(istream);
			wc3lib::read(istream, m_alpha, size);

			return size;
		}

		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception)
		{
			std::streamsize size = Rgb::write(ostream);
			wc3lib::write(ostream, alpha(), size);

			return size;
		}

		void setAlpha(uint8_t alpha)
		{
			m_alpha = alpha;
		}

		uint8_t alpha() const
		{
			return m_alpha;
		}

	protected:
		uint8_t m_alpha;
};

class Bgra : public Rgba
{
	public:
		virtual std::streamsize read(InputStream &istream) throw (Exception)
		{
			std::streamsize size = 0;
			wc3lib::read(istream, m_blue, size);
			wc3lib::read(istream, m_green, size);
			wc3lib::read(istream, m_red, size);
			wc3lib::read(istream, m_alpha, size);

			return size;
		}

		virtual std::streamsize write(OutputStream &ostream) const throw (Exception)
		{
			std::streamsize size = 0;
			wc3lib::write(ostream, blue(), size);
			wc3lib::write(ostream, green(), size);
			wc3lib::write(ostream, red(), size);
			wc3lib::write(ostream, alpha(), size);

			return size;
		}
};

}

#endif
