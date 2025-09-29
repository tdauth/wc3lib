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

#include <boost/foreach.hpp>

#include "rects.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize Rects::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_version, size);

	if (this->version() != latestFileVersion())
	{
		std::cerr << boost::format(_("Rects: Unknown version \"%1%\", expected \"%2%\".")) % this->m_version % latestFileVersion() << std::endl;
	}

	int32 number = 0;
	wc3lib::read(istream, number, size);
	this->rects().reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::unique_ptr<Rect> rect(new Rect());
		size += rect->read(istream);
		this->rects().push_back(std::move(rect));
	}

	return size;
}

std::streamsize Rects::write(OutputStream &ostream) const
{
	if (this->version() != latestFileVersion())
	{
		std::cerr << boost::format(_("Rects: Unknown version \"%1%\", expected \"%2%\".")) % this->m_version % latestFileVersion() << std::endl;
	}

	std::streamsize size = 0;
	wc3lib::write(ostream, this->version(), size);
	const int32 number = this->rects().size();
	wc3lib::write(ostream, number, size);

	BOOST_FOREACH(RectContainer::const_reference rect, this->rects())
	{
		size += rect.write(ostream);
	}

	return size;
}

}

}
