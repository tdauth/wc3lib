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

#include "trees.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize Trees::read(InputStream& istream) throw (Exception)
{
	id fileId;
	std::streamsize size = 0;
	wc3lib::read(istream, fileId, size);

	if (fileId != this->fileId())
		throw Exception();

	wc3lib::read(istream, this->m_version, size);
	wc3lib::read(istream, this->m_subVersion, size);
	int32 number;
	wc3lib::read(istream, number, size);
	m_trees.resize(number);

	for ( ; number > 0; --number)
	{
		TreePtr ptr(new Tree());
		size += ptr->read(istream); // read first, we need its id!
		m_trees.insert(ptr);
	}

	return size;
}

std::streamsize Trees::write(OutputStream& ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->fileId(), size);
	wc3lib::write(ostream, this->version(), size);
	wc3lib::write(ostream, this->subVersion(), size);
	wc3lib::write<int32>(ostream, trees().size(), size);

	BOOST_FOREACH(const TreePtr &tree, trees())
		size += tree->write(ostream);

	return size;
}

}

}
