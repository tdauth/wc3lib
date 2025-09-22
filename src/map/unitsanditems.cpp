/***************************************************************************
 *   Copyright (C) 2025 by Tamino Dauth                                    *
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

#include "unitsanditems.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize UnitsAndItems::read(InputStream &istream)
{
	std::streamsize size = FileFormat::read(istream);
	wc3lib::read(istream, this->m_subVersion, size);
	int32 number;
	wc3lib::read(istream, number, size);
	this->unitsAndItems().reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::auto_ptr<UnitAndItem> unitAndItem(new UnitAndItem());
		size += unitAndItem->read(istream);
		m_unitsAndItems.push_back(unitAndItem);
	}

	return size;
}

std::streamsize UnitsAndItems::write(OutputStream &ostream) const
{
	std::streamsize size = FileFormat::write(ostream);
	wc3lib::write(ostream, this->subVersion(), size);
	const int32 number = boost::numeric_cast<int32>(unitsAndItems().size());
	wc3lib::write(ostream, number, size);

	BOOST_FOREACH(UnitAndItemContainer::const_reference unitAndItem, unitsAndItems())
	{
		size += unitAndItem.write(ostream);
	}

	return size;
}

}

}
