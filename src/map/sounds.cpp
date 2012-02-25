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

#include "sounds.hpp"

namespace wc3lib
{

namespace map
{

Sounds::Sounds(wc3lib::map::W3m* w3m): m_w3m(w3m)
{

}

std::streamsize Sounds::read(InputStream& istream) throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_version, size);

	if (version() != latestFileVersion())
		std::cerr << boost::format(_("Sounds version %1% isn't equal to latest version %2%")) % version() % latestFileVersion() << std::endl;

	int32 number;
	wc3lib::read(istream, number, size);
	this->sounds().resize(number);

	for (int32 i = 0; i < number; --i)
	{
		SoundPtr ptr(new Sound());
		size += ptr->read(istream);
		this->sounds()[i].swap(ptr);
	}

	return size;
}

std::streamsize Sounds::write(OutputStream& ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->m_version, size);

	if (version() != latestFileVersion())
		std::cerr << boost::format(_("Sounds version %1% isn't equal to latest version %2%")) % version() % latestFileVersion() << std::endl;

	const int32 number = boost::numeric_cast<int32>(sounds().size());
	wc3lib::write(ostream, number, size);

	BOOST_FOREACH(SoundVector::const_reference ptr, sounds())
		size += ptr->write(ostream);

	return size;
}

}

}
