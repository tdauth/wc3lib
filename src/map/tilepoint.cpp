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

#include "tilepoint.hpp"

namespace wc3lib
{

namespace map
{

namespace
{

struct TilepointData
{
	uint16_t groundHeight;
	unsigned int boundaryFlag:1;
	unsigned int waterLevel:15;
	unsigned int flags:4;
	unsigned int groundTextureType:4;
	uint8_t textureDetails;
	unsigned int cliffTextureType:4;
	unsigned int layerHeight:4;
};

}

std::streamsize Tilepoint::read(InputStream &istream) throw (class Exception)
{
	struct TilepointData tilepointData;
	std::streamsize size = 0;
	wc3lib::read(istream, tilepointData, size, 7);
	//istream.read(reinterpret_cast<char*>(&tilepointData), 7); // sizeof(*tilepointData)

	if (size != 7)
		throw Exception(boost::format(_("Tilepoint: Size should be 7 but is %1%.")) % size);

	this->m_groundHeight = tilepointData.groundHeight;
	this->m_waterLevel = tilepointData.waterLevel;
	this->m_flags = static_cast<Tilepoint::Flags>(tilepointData.flags);

	if (tilepointData.boundaryFlag)
	{
		this->m_flags = this->m_flags | Tilepoint::Flags::ShadowBoundary;
	}

	this->m_groundTextureType = tilepointData.groundTextureType;
	this->m_textureDetails = tilepointData.textureDetails;
	this->m_layerHeight = tilepointData.layerHeight;

	return size;
}

std::streamsize Tilepoint::write(OutputStream &ostream) const throw (class Exception)
{
	struct TilepointData tilepointData;
	tilepointData.groundHeight = this->m_groundHeight;
	tilepointData.boundaryFlag = this->m_flags & Tilepoint::Flags::ShadowBoundary;
	tilepointData.waterLevel = this->m_waterLevel;
	tilepointData.flags = static_cast<uint16>(this->m_flags); // only the first 4 bits because ShadowBoundary has already been used
	tilepointData.groundTextureType = this->m_groundTextureType;
	tilepointData.textureDetails = this->m_textureDetails;
	tilepointData.layerHeight = this->m_layerHeight;
	std::streamsize size = 0;
	wc3lib::write(ostream, tilepointData, size, 7);

	return size;
}

}

}
