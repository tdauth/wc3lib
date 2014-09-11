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

#ifndef WC3LIB_MAP_CAMPAIGNINFO_HPP
#define WC3LIB_MAP_CAMPAIGNINFO_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"
#include "../color.hpp"

namespace wc3lib
{

namespace map
{

class Campaign;

class CampaignInfo : public FileFormat
{
	public:
		enum class DifficultyType
		{
			Fixed,
			Variable
		};

		/// \todo Get missing race indices.
		enum class Race : int32
		{
			Human = 0
		};

		class MapTitle : public Format
		{
			public:
				MapTitle();

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				bool isVisibleFromTheBeginning() const;
				const string& chapterTitle() const;
				const string& mapTitle() const;
				const string& path() const;

			protected:
				bool m_isVisibleFromTheBeginning;
				string m_chapterTitle;
				string m_mapTitle;
				string m_path; // path of the map in the campaign archive
		};

		class Map : public Format
		{
			public:
				Map();

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				const string& unknown() const;
				const string& path() const;

			protected:
				string m_unknown; // unknown (always empty, might also be a single character)
				string m_path; // String: path of the map in the campaign archive
		};

		typedef boost::ptr_vector<MapTitle> MapTitles;
		typedef boost::ptr_vector<Map> Maps;

		CampaignInfo(Campaign *campaign);

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

		virtual uint32 latestFileVersion() const override;
		virtual const byte* fileName() const override;

		virtual uint32 version() const override;

		Campaign* campaign() const;
		uint32 campaignVersion() const;
		uint32 editorVersion() const;
		const string& name() const;
		const string& difficulty() const;
		const string& author() const;
		const string& description() const;
		DifficultyType difficultyType() const;
		bool hasW3x() const;
		int32 backgroundScreenIndex() const;
		const string& backgroundScreen() const;
		const string& minimapPicture() const;
		int32 ambientSoundIndex() const;
		const string& ambientSound() const;
		int32 terrainFogIndex() const;
		float32 fogStartZHeight() const;
		float32 fogEndZHeight() const;
		float32 fogDensity() const;
		const Rgba& fogColor() const;
		Race race() const;
		const MapTitles& mapTitles() const;
		const Maps& maps() const;

	protected:
		Campaign *m_campaign;
		uint32 m_version;
		uint32 m_campaignVersion;
		uint32 m_editorVersion;
		string m_name;
		string m_difficulty;
		string m_author;
		string m_description;
		DifficultyType m_difficultyType;
		bool m_hasW3x;
		int32 m_backgroundScreenIndex; // campaign background screen index (-1 = none or custom path)
		string m_backgroundScreen; // path of custom background screen (empty if none or preset)
		string m_minimapPicture; // path of minimap picture (empty = none)
		int32 m_ambientSoundIndex; // ambient sound index (-1 = imported file, 0 = none, greater 0 = preset index)
		string m_ambientSound; // path of custom imported ambient sound mp3
		int32 m_terrainFogIndex; // (0 = not used, greater 0 = index of terrain fog style dropdown box)
		float32 m_fogStartZHeight;
		float32 m_fogEndZHeight;
		float32 m_fogDensity;
		Rgba m_fogColor;
		Race m_race;
		MapTitles m_mapTitles;
		Maps m_maps;
};

inline bool CampaignInfo::MapTitle::isVisibleFromTheBeginning() const
{
	return m_isVisibleFromTheBeginning;
}

inline const string& CampaignInfo::MapTitle::chapterTitle() const
{
	return m_chapterTitle;
}

inline const string& CampaignInfo::MapTitle::mapTitle() const
{
	return m_mapTitle;
}

inline const string& CampaignInfo::MapTitle::path() const
{
	return m_path;
}

inline const string& CampaignInfo::Map::unknown() const
{
	return m_unknown;
}

inline const string& CampaignInfo::Map::path() const
{
	return m_path;
}

inline uint32 CampaignInfo::latestFileVersion() const
{
	return 1;
}

inline const byte* CampaignInfo::fileName() const
{
	return "war3campaign.w3f";
}

inline uint32 CampaignInfo::version() const
{
	return this->m_version;
}

inline Campaign* CampaignInfo::campaign() const
{
	return m_campaign;
}

inline uint32 CampaignInfo::campaignVersion() const
{
	return m_campaignVersion;
}

inline uint32 CampaignInfo::editorVersion() const
{
	return m_editorVersion;
}

inline const string& CampaignInfo::name() const
{
	return m_name;
}

inline const string& CampaignInfo::difficulty() const
{
	return m_difficulty;
}

inline const string& CampaignInfo::author() const
{
	return m_author;
}

inline const string& CampaignInfo::description() const
{
	return m_description;
}

inline CampaignInfo::DifficultyType CampaignInfo::difficultyType() const
{
	return m_difficultyType;
}

inline bool CampaignInfo::hasW3x() const
{
	return m_hasW3x;
}

inline int32 CampaignInfo::backgroundScreenIndex() const
{
	return m_backgroundScreenIndex;
}

inline const string& CampaignInfo::backgroundScreen() const
{
	return m_backgroundScreen;
}

inline const string& CampaignInfo::minimapPicture() const
{
	return m_minimapPicture;
}

inline int32 CampaignInfo::ambientSoundIndex() const
{
	return m_ambientSoundIndex;
}

inline const string& CampaignInfo::ambientSound() const
{
	return m_ambientSound;
}

inline int32 CampaignInfo::terrainFogIndex() const
{
	return m_terrainFogIndex;
}

inline float32 CampaignInfo::fogStartZHeight() const
{
	return m_fogStartZHeight;
}

inline float32 CampaignInfo::fogEndZHeight() const
{
	return m_fogEndZHeight;
}

inline float32 CampaignInfo::fogDensity() const
{
	return m_fogDensity;
}

inline const Rgba& CampaignInfo::fogColor() const
{
	return m_fogColor;
}

inline CampaignInfo::Race CampaignInfo::race() const
{
	return m_race;
}

inline const CampaignInfo::MapTitles& CampaignInfo::mapTitles() const
{
	return m_mapTitles;
}

inline const CampaignInfo::Maps& CampaignInfo::maps() const
{
	return m_maps;
}

}

}

#endif
