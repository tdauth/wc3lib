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

#include "textureanimation.hpp"

namespace wc3lib
{

namespace mdlx
{

TextureAnimation::TextureAnimation()
{
}

std::streamsize TextureAnimation::read(InputStream &istream)
{
	std::streamsize size = 0;
	long32 inclusiveSize = 0;
	wc3lib::read(istream, inclusiveSize, size);
	size += m_translations.read(istream);
	size += m_rotations.read(istream);
	size += m_scalings.read(istream);

	return size;
}

std::streamsize TextureAnimation::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	auto p = ostream.tellp();
	size += m_translations.write(ostream);
	size += m_rotations.write(ostream);
	size += m_scalings.write(ostream);
    
	const long32 inclusiveSize = size + sizeof(long32);
	auto p2 = ostream.tellp();
	ostream.seekp(p);
	wc3lib::write(ostream, inclusiveSize, size);
	ostream.seekp(p2);
	ostream.seekp(sizeof(long32), std::ios_base::cur);

	return size;
}

}

}
