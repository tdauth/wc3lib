/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#include "campaigninfo.hpp"
#include "../utilities.hpp"
#include "../internationalisation.hpp"

namespace wc3lib
{

namespace map
{

CampaignInfo::MapTitle::MapTitle() : m_isVisibleFromTheBeginning(false)
{
}

std::streamsize CampaignInfo::MapTitle::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	int32 visible;
	wc3lib::read(istream, visible, size);
	this->m_isVisibleFromTheBeginning = visible;
	readString(istream, this->m_chapterTitle, size);
	readString(istream, this->m_mapTitle, size);
	readString(istream, this->m_path, size);

	return size;
}

std::streamsize CampaignInfo::MapTitle::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, this->isVisibleFromTheBeginning(), size);
	writeString(ostream, this->chapterTitle(), size);
	writeString(ostream, this->mapTitle(), size);
	writeString(ostream, this->path(), size);

	return size;
}

CampaignInfo::Map::Map()
{
}

std::streamsize CampaignInfo::Map::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	readString(istream, this->m_unknown, size);
	readString(istream, this->m_path, size);

	return size;
}

std::streamsize CampaignInfo::Map::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeString(ostream, this->unknown(), size);
	writeString(ostream, this->path(), size);

	return size;
}

CampaignInfo::CampaignInfo(class Campaign *campaign) : m_campaign(campaign)
{
}

std::streamsize CampaignInfo::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_version, size);
	wc3lib::read(istream, this->m_campaignVersion, size);
	wc3lib::read(istream, this->m_editorVersion, size);
	readString(istream, this->m_name, size);
	readString(istream, this->m_difficulty, size);
	readString(istream, this->m_author, size);
	readString(istream, this->m_description, size);
	int32 flag;
	wc3lib::read(istream, flag, size);

	switch (flag)
	{
		case 0: // =Fixed Diffculty, Only w3m maps
			this->m_difficultyType = DifficultyType::Fixed;
			this->m_hasW3x = false;
			
			break;
		case 1: // =Variable Difficulty, Only w3m maps
			this->m_difficultyType = DifficultyType::Variable;
			this->m_hasW3x = false;

			break;
		case 2: // =Fixed Difficulty, Contains w3x maps
			this->m_difficultyType = DifficultyType::Fixed;
			this->m_hasW3x = true;

			break;
		case 3: // =Variable Difficulty, Contains w3x maps
			this->m_difficultyType = DifficultyType::Variable;
			this->m_hasW3x = true;

			break;

		default:
			throw Exception(boost::format(_("Campaign info: Unknown flag: %1%.")) % flag);
	}

	wc3lib::read(istream, this->m_backgroundScreenIndex, size);
	readString(istream, this->m_backgroundScreen, size);
	readString(istream, this->m_minimapPicture, size);
	wc3lib::read(istream, this->m_ambientSoundIndex, size);
	readString(istream, this->m_ambientSound, size);
	wc3lib::read(istream, this->m_terrainFogIndex, size);
	wc3lib::read(istream, this->m_fogStartZHeight, size);
	wc3lib::read(istream, this->m_fogEndZHeight, size);
	wc3lib::read(istream, this->m_fogDensity, size);
	wc3lib::read(istream, this->m_fogColor, size);
	int32 race;
	wc3lib::read<int32>(istream, race, size);
	this->m_race = BOOST_SCOPED_ENUM(Race)(race);

	int32 mapTitles;
	wc3lib::read(istream, mapTitles, size);
	int32 maps;
	wc3lib::read(istream, maps, size);

	while (mapTitles > 0)
	{
		MapTitlePtr mapTitle(new MapTitle());
		mapTitle->read(istream);
		this->m_mapTitles.push_back(mapTitle);
		--mapTitles;
	}

	while (maps > 0)
	{
		MapPtr map(new Map());
		map->read(istream);
		this->m_maps.push_back(map);
		--maps;
	}

	return size;
}

std::streamsize CampaignInfo::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->version(), size);
	wc3lib::write(ostream, this->campaignVersion(), size);
	wc3lib::write(ostream, this->editorVersion(), size);
	writeString(ostream, this->name(), size);
	writeString(ostream, this->difficulty(), size);
	writeString(ostream, this->author(), size);
	writeString(ostream, this->description(), size);
	int32 flag;
	
	if (hasW3x())
	{
		if (difficultyType() == DifficultyType::Fixed)
			flag = 2;
		else
			flag = 3;
	}
	else if (difficultyType() == DifficultyType::Fixed)
		flag = 0;
	else
		flag = 1;
	
	wc3lib::write(ostream, flag, size);

	wc3lib::write(ostream, this->backgroundScreenIndex(), size);
	writeString(ostream, this->backgroundScreen(), size);
	writeString(ostream, this->minimapPicture(), size);
	wc3lib::write(ostream, this->ambientSoundIndex(), size);
	writeString(ostream, this->ambientSound(), size);
	wc3lib::write(ostream, this->terrainFogIndex(), size);
	wc3lib::write(ostream, this->fogStartZHeight(), size);
	wc3lib::write(ostream, this->fogEndZHeight(), size);
	wc3lib::write(ostream, this->fogDensity(), size);
	wc3lib::write(ostream, this->fogColor(), size);
	wc3lib::write<int32>(ostream, race(), size);

	wc3lib::write<int32>(ostream, mapTitles().size(), size);
	wc3lib::write<int32>(ostream, maps().size(), size);

	BOOST_FOREACH(const MapTitlePtr ptr, mapTitles())
		size += ptr->write(ostream);

	BOOST_FOREACH(const MapPtr ptr, maps())
		size += ptr->write(ostream);

	return size;
}

}

}
