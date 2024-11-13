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

#include "model.hpp"

namespace wc3lib
{

namespace mdlx
{

Model::Model()
{
}

Model::~Model()
{
}

std::streamsize Model::read(InputStream &istream)
{
	std::streamsize size = 0;
    readMdxHeader(istream, size, u8"MODL");
	wc3lib::read(istream, m_name, size, nameSize * sizeof(byte));
    wc3lib::read(istream, m_animationFileName, size, animationFileNameSize * sizeof(byte));
    size += m_bounds.read(istream);
	wc3lib::read(istream, m_blendTime, size);

	return size;
}

std::streamsize Model::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
    auto p = ostream.tellp();
    ostream.seekp(sizeof(Header), std::ios_base::cur);
	wc3lib::write(ostream, m_name, size, nameSize * sizeof(byte));
    wc3lib::write(ostream, m_animationFileName, size, animationFileNameSize * sizeof(byte));
    size += m_bounds.write(ostream);
	wc3lib::write(ostream, m_blendTime, size);
    auto p2 = ostream.tellp();
    ostream.seekp(p);
    writeMdxHeader(ostream, size, u8"MODL", size);
    ostream.seekp(p2);

	return size;
}

}

}
