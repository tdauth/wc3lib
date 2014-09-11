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

// MinimumExtent, MaximumExtent and BoundsRadius only appear when their values are not 0.0.
std::streamsize Bounds::readMdl(istream &istream)
{
	/// @todo If values do not exist they're always 0.0.
	//boost::tokenizer<> tokenizer(istream);
	/*
	if ((*iterator)->minExtX() != 0.0 || (*iterator)->minExtY() != 0.0 || (*iterator)->minExtZ() != 0.0)
		ostream << "\t\tMinimumExtent { " << (*iterator)->minExtX() << ", " << (*iterator)->minExtY() << ", " << (*iterator)->minExtZ() << " },\n";

	if ((*iterator)->maxExtX() != 0.0 || (*iterator)->maxExtY() != 0.0 || (*iterator)->maxExtZ() != 0.0)
		ostream << "\t\tMaxmimumExtent { " << (*iterator)->maxExtX() << ", " << (*iterator)->maxExtY() << ", " << (*iterator)->maxExtZ() << " },\n";

	if ((*iterator)->boundsRadius() != 0.0)
		ostream << "\t\tBoundsRadius " << (*iterator)->boundsRadius() << ",\n";
	*/

	return 0;
}

// MinimumExtent, MaximumExtent and BoundsRadius only appear when their values are not 0.0.
std::streamsize Bounds::writeMdl(ostream &ostream) const
{
	std::streamsize size = 0;

	if (this->minimumExtent().x() != 0.0 || this->minimumExtent().y() != 0.0 || this->minimumExtent().z() != 0.0)
		writeMdlVectorProperty(ostream, size, "MinimumExtent", this->minimumExtent(), 1);

	if (this->maximumExtent().x() != 0.0 || this->maximumExtent().y() != 0.0 || this->maximumExtent().z() != 0.0)
		writeMdlVectorProperty(ostream, size, "MaximumExtent", this->maximumExtent(), 1);

	if (this->boundsRadius() != 0.0)
		writeMdlValueProperty(ostream, size, "BoundsRadius", this->boundsRadius(), 1);

	return size;
}

std::streamsize Bounds::readMdx(istream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_boundsRadius, size);
	size += this->minimumExtent().read(istream);
	size += this->maximumExtent().read(istream);

	return size;
}

std::streamsize Bounds::writeMdx(ostream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->boundsRadius(), size);
	size += this->minimumExtent().write(ostream);
	size += this->maximumExtent().write(ostream);

	return size;
}

}

}
