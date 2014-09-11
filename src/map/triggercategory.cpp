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

#include "triggercategory.hpp"
#include "../utilities.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace map
{

TriggerCategory::TriggerCategory() : m_index(0)
{
}

std::streamsize TriggerCategory::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_index, size);
	readString(istream, this->m_name, size);

	return size;
}

std::streamsize TriggerCategory::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->index(), size);
	writeString(ostream, this->name(), size);

	return size;
}

}

}
