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

#include "tree.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize Tree::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_treeId, size);
	wc3lib::read(istream, m_variation, size);
	size += m_position.read(istream);
	wc3lib::read(istream, m_angle, size);
	size += m_scale.read(istream);
	wc3lib::read<byte>(istream, (byte&)m_flags, size);
	wc3lib::read(istream, m_life, size);
	wc3lib::read(istream, m_customId, size);

	return size;
}

std::streamsize Tree::write(OutputStream& ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, treeId(), size);
	wc3lib::write(ostream, variation(), size);
	size += position().write(ostream);
	wc3lib::write(ostream, angle(), size);
	size += scale().write(ostream);
	wc3lib::write<byte>(ostream, static_cast<byte>(flags()), size);
	wc3lib::write(ostream, life(), size);
	wc3lib::write(ostream, customId(), size);

	return size;
}

}

}
