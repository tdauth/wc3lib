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

#include "environment.hpp"
#include "tilepoint.hpp"

namespace wc3lib
{

namespace map
{

const int32 Environment::maxTilesets = 16;

Environment::Environment(class W3m *w3m) : m_w3m(w3m)
{
}

std::streamsize Environment::read(InputStream &istream) throw (class Exception)
{
	int32 requiredFileId;
	std::streamsize size = 0;
	wc3lib::read(istream, requiredFileId, size);

	if (requiredFileId != fileId())
		throw Exception(boost::format(_("Environment: Expected \"%1%\" identifier. Got unknown \"%2%\".")) % fileId() % requiredFileId);

	wc3lib::read(istream, this->m_version, size);

	if (this->version() != latestFileVersion())
		std::cerr << boost::format(_("Environment: Expected version %1%. Got unknown %2%.")) % latestFileVersion() % this->version() << std::endl;

	char8 mainTileset;
	wc3lib::read(istream, mainTileset, size);
	this->m_mainTileset = Environment::convertCharToMainTileset(mainTileset);

	int32 customTilesetsFlag;
	wc3lib::read(istream, customTilesetsFlag, size);
	this->m_customized = customTilesetsFlag;

	int32 groundTilesetsNumber;
	wc3lib::read(istream, groundTilesetsNumber, size);

	if (groundTilesetsNumber > Environment::maxTilesets)
		throw Exception(boost::str(boost::format(_("Environment: Ground tilesets are limited to %1% however %2% are used.")) % Environment::maxTilesets % groundTilesetsNumber));

	this->m_groundTilesetsIds.resize(groundTilesetsNumber);
	
	for (std::size_t i = 0; i < groundTilesetsNumber; ++i)
	{
		id groundTilesetId;
		wc3lib::read(istream, groundTilesetId, size);
		this->m_groundTilesetsIds[i] = groundTilesetId;
	}

	int32 cliffTilesetsNumber;
	wc3lib::read(istream, cliffTilesetsNumber, size);

	if (cliffTilesetsNumber > Environment::maxTilesets)
		throw Exception(boost::str(boost::format(_("Environment: Cliff tilesets are limited to %1% however %2% are used.")) % Environment::maxTilesets % cliffTilesetsNumber));

	this->m_cliffTilesetsIds.resize(cliffTilesetsNumber);
	
	for (std::size_t i = 0; i < cliffTilesetsNumber; ++i)
	{
		id cliffTilesetId;
		wc3lib::read(istream, cliffTilesetId, size);
		this->m_cliffTilesetsIds[i] = cliffTilesetId;
	}

	wc3lib::read(istream, this->m_maxX, size);
	wc3lib::read(istream, this->m_maxY, size);
	wc3lib::read(istream, this->m_centerOffsetX, size);
	wc3lib::read(istream, this->m_centerOffsetY, size);

	// The first tilepoint defined in the file stands for the lower left corner of the map when looking from the top, then it goes line by line (horizontal).
	for (int32 y = 0; y < this->m_maxY; ++y)
	{
		for (int32 x = 0; x < this->m_maxX; ++x)
		{
			TilepointPtr ptr(new Tilepoint(this, x, y));
			size += ptr->read(istream);
			this->m_tilepoints.insert(ptr);
		}
	}

	return size;
}

std::streamsize Environment::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, fileId(), size);
	wc3lib::write(ostream, version(), size);

	if (this->version() != latestFileVersion())
		std::cerr << boost::format(_("Environment: Expected version %1%. Got unknown %2%.")) % latestFileVersion() % this->version() << std::endl;

	wc3lib::write<char8>(ostream, mainTileset(), size);
	wc3lib::write<int32>(ostream, customized(), size);

	if (groundTilesetsIds().size() > Environment::maxTilesets)
		throw Exception(boost::format(_("Environment: Ground tilesets are limited to %1% however %2% are used.")) % Environment::maxTilesets % groundTilesetsIds().size());

	wc3lib::write<int32>(ostream, groundTilesetsIds().size(), size);
	
	for (std::size_t i = 0; i < groundTilesetsIds().size(); ++i)
		wc3lib::write<id>(ostream, this->groundTilesetsIds()[i], size);

	
	if (cliffTilesetsIds().size() > Environment::maxTilesets)
		throw Exception(boost::format(_("Environment: Cliff tilesets are limited to %1% however %2% are used.")) % Environment::maxTilesets % cliffTilesetsIds().size());
	
	wc3lib::write(ostream, cliffTilesetsIds().size(), size);
	
	for (std::size_t i = 0; i < cliffTilesetsIds().size(); ++i)
		wc3lib::write<id>(ostream, cliffTilesetsIds()[i], size);
	
	wc3lib::write(ostream, this->maxX(), size);
	wc3lib::write(ostream, this->maxY(), size);
	wc3lib::write(ostream, this->centerOffsetX(), size);
	wc3lib::write(ostream, this->centerOffsetY(), size);
	
	// The first tilepoint defined in the file stands for the lower left corner of the map when looking from the top, then it goes line by line (horizontal).
	for (int32 y = 0; y < this->maxY(); ++y)
	{
		for (int32 x = 0; x < this->maxX(); ++x)
			size += tilepoint(Position(x, y))->write(ostream);
	}

	return size;
}

BOOST_SCOPED_ENUM(Environment::MainTileset) Environment::convertCharToMainTileset(char value) throw (class Exception)
{
	switch (value)
	{
		case 'A':
			return Environment::MainTileset::Ashenvale;

		case 'B':
			return Environment::MainTileset::Barrens;

		case 'C':
			return Environment::MainTileset::Felwood;

		case 'D':
			return Environment::MainTileset::Dungeon;

		case 'F':
			return Environment::MainTileset::LordaeronFall;

		case 'G':
			return Environment::MainTileset::Underground;

		case 'L':
			return Environment::MainTileset::LordaeronSummer;

		case 'N':
			return Environment::MainTileset::Northrend;

		case 'Q':
			return Environment::MainTileset::VillageFall;

		case 'V':
			return Environment::MainTileset::Village;

		case 'W':
			return Environment::MainTileset::LordaeronWinter;

		case 'X':
			return Environment::MainTileset::Dalaran;

		case 'Y':
			return Environment::MainTileset::Cityscape;

		case 'Z':
			return Environment::MainTileset::SunkenRuins;

		case 'I':
			return Environment::MainTileset::Icecrown;

		case 'J':
			return Environment::MainTileset::DalaranRuins;

		case 'O':
			return Environment::MainTileset::Outland;

		case 'K':
			return Environment::MainTileset::BlackCitadel;
	}

	throw Exception(boost::format(_("Environment: Character \'%1%\' does not refer to any main tileset.")) % value);

	//return Environment::Ashenvale;
}

}

}
