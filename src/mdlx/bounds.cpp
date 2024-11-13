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

#include "bounds.hpp"

namespace wc3lib
{

namespace mdlx
{

Bounds::Bounds()
: m_boundsRadius(0.0)
, m_minimumExtent(0.0, 0.0, 0.0)
, m_maximumExtent(0.0, 0.0, 0.0)
{
}

Bounds::~Bounds()
{
}

std::streamsize Bounds::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_boundsRadius, size);
	wc3lib::read(istream, m_minimumExtent, size);
	wc3lib::read(istream, m_maximumExtent, size);

	return size;
}

std::streamsize Bounds::write(OutputStream &ostream) const
{
    std::streamsize size = 0;
	wc3lib::write(ostream, m_boundsRadius, size);
	wc3lib::write(ostream, m_minimumExtent, size);
	wc3lib::write(ostream, m_maximumExtent, size);

	return size;
}


}

}
