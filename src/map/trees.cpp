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

#include <boost/foreach.hpp>

#include "trees.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize Trees::read(InputStream &istream)
{
	std::streamsize size = FileFormat::read(istream);
	wc3lib::read(istream, this->m_subVersion, size);
	int32 number;
	wc3lib::read(istream, number, size);
	this->trees().reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::unique_ptr<Tree> tree(new Tree());
		size += tree->read(istream);
		m_trees.push_back(std::move(tree));
	}

	return size;
}

std::streamsize Trees::write(OutputStream &ostream) const
{
	std::streamsize size = FileFormat::write(ostream);
	wc3lib::write(ostream, this->subVersion(), size);
	const int32 number = boost::numeric_cast<int32>(trees().size());
	wc3lib::write(ostream, number, size);

	BOOST_FOREACH(TreeContainer::const_reference tree, trees())
	{
		size += tree.write(ostream);
	}

	return size;
}

}

}
