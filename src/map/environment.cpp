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

#include "environment.hpp"
#include "tilepoint.hpp"

namespace wc3lib
{

namespace map
{

const uint32 Environment::maxTilesets = 16;

Environment::~Environment()
{
	clear();
}

std::streamsize Environment::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = FileFormat::read(istream);
	wc3lib::read<byte>(istream, (byte&)this->m_mainTileset, size);
	uint32 customTilesetsFlag;
	wc3lib::read(istream, customTilesetsFlag, size);
	this->m_customized = customTilesetsFlag;

	uint32 groundTilesetsNumber;
	wc3lib::read(istream, groundTilesetsNumber, size);

	if (groundTilesetsNumber > maxTilesets)
		std::cerr << boost::format(_("Environment (warning): Only the first %1% ground tilesets can be used, however %2% are stored.")) % maxTilesets % groundTilesetsNumber << std::endl;

	this->m_groundTilesetsIds.resize(groundTilesetsNumber);

	for (uint32 i = 0; i < groundTilesetsNumber; ++i)
	{
		id groundTilesetId;
		wc3lib::read(istream, groundTilesetId, size);
		this->m_groundTilesetsIds[i] = groundTilesetId;
	}

	uint32 cliffTilesetsNumber;
	wc3lib::read(istream, cliffTilesetsNumber, size);

	if (cliffTilesetsNumber > maxTilesets)
		std::cerr << boost::format(_("Environment (warning): Only the first %1% cliff tilesets can be used, however %2% are stored.")) % maxTilesets % groundTilesetsNumber << std::endl;

	this->m_cliffTilesetsIds.resize(cliffTilesetsNumber);

	for (uint32 i = 0; i < cliffTilesetsNumber; ++i)
	{
		id cliffTilesetId;
		wc3lib::read(istream, cliffTilesetId, size);
		this->m_cliffTilesetsIds[i] = cliffTilesetId;
	}

	wc3lib::read(istream, this->m_maxX, size);
	wc3lib::read(istream, this->m_maxY, size);
	wc3lib::read(istream, this->m_centerOffsetX, size);
	wc3lib::read(istream, this->m_centerOffsetY, size);
	clear();
	tilepoints().resize(boost::extents[this->maxX()][this->maxY()]);

	// The first tilepoint defined in the file stands for the lower left corner of the map when looking from the top, then it goes line by line (horizontal).
	for (uint32 y = 0; y < this->maxY(); ++y)
	{
		for (uint32 x = 0; x < this->maxX(); ++x)
		{
			Tilepoint *tilepoint = new Tilepoint();
			size += tilepoint->read(istream);
			tilepoints()[x][y] = tilepoint;
		}
	}

	return size;
}

std::streamsize Environment::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = FileFormat::write(ostream);
	wc3lib::write<byte>(ostream, static_cast<byte>(mainTileset()), size);
	wc3lib::write<uint32>(ostream, customized(), size);

	if (groundTilesetsIds().size() > maxTilesets)
		std::cerr << boost::format(_("Environment (warning): Only the first %1% ground tilesets can be used, however %2% are stored.")) % maxTilesets % groundTilesetsIds().size() << std::endl;

	wc3lib::write<uint32>(ostream, groundTilesetsIds().size(), size);

	for (uint32 i = 0; i < groundTilesetsIds().size(); ++i)
		wc3lib::write<id>(ostream, this->groundTilesetsIds()[i], size);


	if (cliffTilesetsIds().size() > maxTilesets)
		std::cerr << boost::format(_("Environment (warning): Only the first %1% ground tilesets can be used, however %2% are stored.")) % maxTilesets % cliffTilesetsIds().size() << std::endl;

	wc3lib::write<uint32>(ostream, cliffTilesetsIds().size(), size);

	for (uint32 i = 0; i < cliffTilesetsIds().size(); ++i)
		wc3lib::write<id>(ostream, cliffTilesetsIds()[i], size);

	wc3lib::write(ostream, this->maxX(), size);
	wc3lib::write(ostream, this->maxY(), size);
	wc3lib::write(ostream, this->centerOffsetX(), size);
	wc3lib::write(ostream, this->centerOffsetY(), size);

	// The first tilepoint defined in the file stands for the lower left corner of the map when looking from the top, then it goes line by line (horizontal).
	for (uint32 y = 0; y < this->maxY(); ++y)
	{
		for (uint32 x = 0; x < this->maxX(); ++x)
			size += tilepoints()[x][y]->write(ostream);
	}

	return size;
}

void Environment::clear()
{
	for (Tilepoints::iterator columnIterator = tilepoints().begin();
columnIterator != tilepoints().end(); ++columnIterator)
	{
		for (Tilepoints::subarray<1>::type::iterator
rowIterator = columnIterator->begin();
rowIterator != columnIterator->end(); ++rowIterator)
			delete *rowIterator;
        }
}

}

}
