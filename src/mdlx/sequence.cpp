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

#include "sequence.hpp"

namespace wc3lib
{

namespace mdlx
{

Sequence::Sequence()
{
}

std::streamsize Sequence::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_name, size, nameSize * sizeof(byte));
	wc3lib::read(istream, m_intervalStart, size);
	wc3lib::read(istream, m_intervalEnd, size);
	wc3lib::read(istream, m_moveSpeed, size);
	wc3lib::read(istream, m_noLooping, size);
	wc3lib::read(istream, m_rarity, size);
	wc3lib::read(istream, m_syncPoint, size);
	size += Bounds::read(istream);

	return size;
}

std::streamsize Sequence::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, m_name, size, nameSize * sizeof(byte));
	wc3lib::write(ostream, m_intervalStart, size);
	wc3lib::write(ostream, m_intervalEnd, size);
	wc3lib::write(ostream, m_moveSpeed, size);
	wc3lib::write(ostream, m_noLooping, size);
	wc3lib::write(ostream, m_rarity, size);
	wc3lib::write(ostream, m_syncPoint, size);
	size += Bounds::write(ostream);

	return size;
}

}

}
