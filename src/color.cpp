/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#include "color.hpp"

namespace wc3lib
{

Rgb::Rgb(uint8_t red, uint8_t green, uint8_t blue) : m_red(red), m_green(green), m_blue(blue)
{
}

Rgb::Rgb() : m_red(0), m_green(0), m_blue(0)
{
}

std::streamsize Rgb::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_red, size);
	wc3lib::read(istream, m_green, size);
	wc3lib::read(istream, m_blue, size);

	return size;
}

std::streamsize Rgb::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, red(), size);
	wc3lib::write(ostream, green(), size);
	wc3lib::write(ostream, blue(), size);

	return size;
}
uint32_t Rgb::value() const
{
	return ((uint32_t)this->red() << 16) + ((uint32_t)this->green() << 8) + this->blue();
}

bool Rgb::operator==(uint32_t value) const
{
	return this->value() == value;
}

bool Rgb::operator==(const Rgb &other) const
{
	return this->value() == other.value();
}

std::streamsize Bgr::read(InputStream &istream) throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_blue, size);
	wc3lib::read(istream, m_green, size);
	wc3lib::read(istream, m_red, size);

	return size;
}

std::streamsize Bgr::write(OutputStream &ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, blue(), size);
	wc3lib::write(ostream, green(), size);
	wc3lib::write(ostream, red(), size);

	return size;
}

uint32_t Bgr::value() const
{
	return ((uint32_t)this->blue() << 16) + ((uint32_t)this->green() << 8) + this->red();
}

Rgba::Rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : Rgb(red, green, blue), m_alpha(alpha)
{
}

Rgba::Rgba() : Rgb(), m_alpha(0)
{
}

std::streamsize Rgba::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = Rgb::read(istream);
	wc3lib::read(istream, m_alpha, size);

	return size;
}

std::streamsize Rgba::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = Rgb::write(ostream);
	wc3lib::write(ostream, alpha(), size);

	return size;
}

uint32_t Rgba::value() const
{
	return ((uint32_t)this->red() << 24) + ((uint32_t)this->green() << 16) + ((uint32_t)this->blue() << 8) + this->alpha();
}

std::streamsize Bgra::read(InputStream &istream) throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_blue, size);
	wc3lib::read(istream, m_green, size);
	wc3lib::read(istream, m_red, size);
	wc3lib::read(istream, m_alpha, size);

	return size;
}

std::streamsize Bgra::write(OutputStream &ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, blue(), size);
	wc3lib::write(ostream, green(), size);
	wc3lib::write(ostream, red(), size);
	wc3lib::write(ostream, alpha(), size);

	return size;
}

uint32_t Bgra::value() const
{
	return ((uint32_t)this->blue() << 24) + ((uint32_t)this->green() << 16) + ((uint32_t)this->red() << 8) + this->alpha();
}

}
