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

#include "shadow.hpp"
#include "w3m.hpp"

namespace wc3lib
{

namespace map
{

const int32 Shadow::shadowPointsPerTileset = 16;

Shadow::Shadow(int32 width, int32 height) : m_width(width), m_height(height)
{
}

Shadow::~Shadow()
{
}

std::streamsize Shadow::read(InputStream &istream)
{
	std::streamsize size = 0;

	this->tilepoints().resize(boost::extents[this->width()][this->height()][Shadow::shadowPointsPerTileset]);

	for (int32 width = 0; width < this->width(); ++width)
	{
		for (int32 height = 0; height < this->height(); ++height)
		{
			for (int32 point = 0; point < Shadow::shadowPointsPerTileset; ++point)
			{
				uint8 data = 0;
				wc3lib::read(istream, data, size);
				this->tilepoints()[width][height][point] = static_cast<Shadow::Type>(data);
			}
		}
	}

	return size;
}

std::streamsize Shadow::write(OutputStream &ostream) const
{
	std::streamsize size = 0;

	for (int32 width = 0; width < this->width(); ++width)
	{
		for (int32 height = 0; height < this->height(); ++height)
		{
			for (int32 point = 0; point < Shadow::shadowPointsPerTileset; ++point)
			{
				wc3lib::write<uint8>(ostream, static_cast<uint8>(this->type(width, height, point)), size);
			}
		}
	}

	return size;
}

}

}
