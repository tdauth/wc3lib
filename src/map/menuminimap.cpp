/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include "menuminimap.hpp"

namespace wc3lib
{

namespace map
{

MenuMinimap::Mark::Mark() : Vertex2d<int32>()
{
}

std::streamsize MenuMinimap::Mark::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read<int32>(istream, (int32&)m_iconType, size);
	size += Vertex2d<int32>::read(istream);
	size += m_color.read(istream);

	return size;
}

std::streamsize MenuMinimap::Mark::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, (int32)iconType(), size);
	size += Vertex2d<int32>::write(ostream);
	size += m_color.write(ostream);

	return size;
}

std::streamsize MenuMinimap::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_version, size);

	if (version() != latestFileVersion())
	{
		std::cerr << boost::format(_("Menu minimap version %1% isn't equal to latest version %2%")) % version() % latestFileVersion() << std::endl;
	}

	int32 number;
	wc3lib::read(istream, number, size);
	this->m_marks.reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::unique_ptr<Mark> ptr(new Mark());
		size += ptr->read(istream);
		m_marks.push_back(std::move(ptr));
	}

	return size;
}

std::streamsize MenuMinimap::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->version(), size);

	if (version() != latestFileVersion())
	{
		std::cerr << boost::format(_("Menu minimap version %1% isn't equal to latest version %2%")) % version() % latestFileVersion() << std::endl;
	}

	const int32 number = boost::numeric_cast<int32>(marks().size());
	wc3lib::write(ostream, number, size);

	BOOST_FOREACH(Marks::const_reference ptr, marks())
	{
		size += ptr.write(ostream);
	}

	return size;
}

}

}
